// SPDX-License-Identifier: GPL-2.0
/*
 * A V4L2 driver for Sony IMX500 cameras.
 * Copyright (C) 2024, Raspberry Pi Ltd
 */
#include <linux/unaligned.h>
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/earlycpio.h>
#include <linux/firmware.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/kernel_read_file.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/vmalloc.h>
#include <media/v4l2-cci.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-mediabus.h>

/* Chip ID */
#define IMX500_REG_CHIP_ID CCI_REG16(0x0016)
#define IMX500_CHIP_ID 0x0500

#define IMX500_REG_MODE_SELECT CCI_REG8(0x0100)
#define IMX500_MODE_STANDBY 0x00
#define IMX500_MODE_STREAMING 0x01

#define IMX500_REG_IMAGE_ONLY_MODE CCI_REG8(0xa700)
#define IMX500_IMAGE_ONLY_FALSE 0x00
#define IMX500_IMAGE_ONLY_TRUE 0x01

#define IMX500_REG_SENSOR_TEMP_CTRL CCI_REG8(0x0138)

#define IMX500_REG_ORIENTATION CCI_REG8(0x101)

#define IMX500_XCLK_FREQ 24000000

#define IMX500_DEFAULT_LINK_FREQ 444000000

#define IMX500_PIXEL_RATE 744000000

/* V_TIMING internal */
#define IMX500_REG_FRAME_LENGTH CCI_REG16(0x0340)
#define IMX500_FRAME_LENGTH_MAX 0xffdc
#define IMX500_VBLANK_MIN 1117

/* H_TIMING internal */
#define IMX500_REG_LINE_LENGTH CCI_REG16(0x0342)
#define IMX500_LINE_LENGTH_MAX 0xfff0

/* Long exposure multiplier */
#define IMX500_LONG_EXP_SHIFT_MAX 7
#define IMX500_LONG_EXP_SHIFT_REG CCI_REG8(0x3210)
#define IMX500_LONG_EXP_CIT_SHIFT_REG CCI_REG8(0x3100)

/* Exposure control */
#define IMX500_REG_EXPOSURE CCI_REG16(0x0202)
#define IMX500_EXPOSURE_OFFSET 22
#define IMX500_EXPOSURE_MIN 8
#define IMX500_EXPOSURE_STEP 1
#define IMX500_EXPOSURE_DEFAULT 0x640
#define IMX500_EXPOSURE_MAX (IMX500_FRAME_LENGTH_MAX - IMX500_EXPOSURE_OFFSET)

/* Analog gain control */
#define IMX500_REG_ANALOG_GAIN CCI_REG16(0x0204)
#define IMX500_ANA_GAIN_MIN 0
#define IMX500_ANA_GAIN_MAX 978
#define IMX500_ANA_GAIN_STEP 1
#define IMX500_ANA_GAIN_DEFAULT 0x0

/* Inference windows */
#define IMX500_REG_DWP_AP_VC_VOFF CCI_REG16(0xD500)
#define IMX500_REG_DWP_AP_VC_HOFF CCI_REG16(0xD502)
#define IMX500_REG_DWP_AP_VC_VSIZE CCI_REG16(0xD504)
#define IMX500_REG_DWP_AP_VC_HSIZE CCI_REG16(0xD506)

#define IMX500_REG_DD_CH06_X_OUT_SIZE \
	CCI_REG16(0x3054) /* Output pixel count for KPI */
#define IMX500_REG_DD_CH07_X_OUT_SIZE \
	CCI_REG16(0x3056) /* Output pixel count for Input Tensor */
#define IMX500_REG_DD_CH08_X_OUT_SIZE \
	CCI_REG16(0x3058) /* Output pixel count for Output Tensor */
#define IMX500_REG_DD_CH09_X_OUT_SIZE \
	CCI_REG16(0x305A) /* Output pixel count for PQ Settings */

#define IMX500_REG_DD_CH06_Y_OUT_SIZE \
	CCI_REG16(0x305C) /* Output line count for KPI */
#define IMX500_REG_DD_CH07_Y_OUT_SIZE \
	CCI_REG16(0x305E) /* Output line count for Input Tensor */
#define IMX500_REG_DD_CH08_Y_OUT_SIZE \
	CCI_REG16(0x3060) /* Output line count for Output Tensor */
#define IMX500_REG_DD_CH09_Y_OUT_SIZE \
	CCI_REG16(0x3062) /* Output line count for PQ Settings */

#define IMX500_REG_DD_CH06_VCID \
	CCI_REG8(0x3064) /* Virtual channel ID for KPI */
#define IMX500_REG_DD_CH07_VCID \
	CCI_REG8(0x3065) /* Virtual channel ID for Input Tensor */
#define IMX500_REG_DD_CH08_VCID \
	CCI_REG8(0x3066) /* Virtual channel ID for Output Tensor */
#define IMX500_REG_DD_CH09_VCID \
	CCI_REG8(0x3067) /* Virtual channel ID for PQ Settings */

#define IMX500_REG_DD_CH06_DT CCI_REG8(0x3068) /* Data Type for KPI */
#define IMX500_REG_DD_CH07_DT CCI_REG8(0x3069) /* Data Type for Input Tensor */
#define IMX500_REG_DD_CH08_DT CCI_REG8(0x306A) /* Data Type for Output Tensor */
#define IMX500_REG_DD_CH09_DT CCI_REG8(0x306B) /* Data Type for PQ Settings */

#define IMX500_REG_DD_CH06_PACKING \
	CCI_REG8(0x306C) /* Pixel/byte packing for KPI */
#define IMX500_REG_DD_CH07_PACKING \
	CCI_REG8(0x306D) /* Pixel/byte packing for Input Tensor */
#define IMX500_REG_DD_CH08_PACKING \
	CCI_REG8(0x306E) /* Pixel/byte packing for Output Tensor */
#define IMX500_REG_DD_CH09_PACKING \
	CCI_REG8(0x306F) /* Pixel/byte packing for PQ Settings */
#define IMX500_DD_PACKING_8BPP 2 /* 8 bits/pixel */
#define IMX500_DD_PACKING_10BPP 3 /* 10 bits/pixel */

/* Interrupt command (start processing command inside IMX500 CPU) */
#define IMX500_REG_DD_CMD_INT CCI_REG8(0x3080)
#define IMX500_DD_CMD_INT_ST_TRANS 0
#define IMX500_DD_CMD_INT_UPDATE 1
#define IMX500_DD_CMD_INT_FLASH_ERASE 2

/* State transition command type */
#define IMX500_REG_DD_ST_TRANS_CMD CCI_REG8(0xD000)
#define IMX500_DD_ST_TRANS_CMD_LOADER_FW 0
#define IMX500_DD_ST_TRANS_CMD_MAIN_FW 1
#define IMX500_DD_ST_TRANS_CMD_NW_WEIGHTS 2
#define IMX500_DD_ST_TRANS_CMD_CLEAR_WEIGHTS 3

/* Network weights update command */
#define IMX500_REG_DD_UPDATE_CMD CCI_REG8(0xD001)
#define IMX500_DD_UPDATE_CMD_SRAM 0
#define IMX500_DD_UPDATE_CMD_FLASH 1

/* Transfer source when loading into RAM */
#define IMX500_REG_DD_LOAD_MODE CCI_REG8(0xD002)
#define IMX500_DD_LOAD_MODE_AP 0
#define IMX500_DD_LOAD_MODE_FLASH 1

/* Image type to transfer */
#define IMX500_REG_DD_IMAGE_TYPE CCI_REG8(0xD003)
#define IMX500_DD_IMAGE_TYPE_LOADER_FW 0
#define IMX500_DD_IMAGE_TYPE_MAIN_FW 1
#define IMX500_DD_IMAGE_TYPE_NETWORK_WEIGHTS 2

/* Number of divisions of download image file */
#define IMX500_REG_DD_DOWNLOAD_DIV_NUM CCI_REG8(0xD004)

#define IMX500_REG_DD_FLASH_TYPE CCI_REG8(0xD005)

/* total size of download file (4-byte) */
#define IMX500_REG_DD_DOWNLOAD_FILE_SIZE CCI_REG32(0xD008)

/* Status notification (4-byte) */
#define IMX500_REG_DD_REF_STS CCI_REG32(0xD010)
#define IMX500_DD_REF_STS_FATAL 0xFF
#define IMX500_DD_REF_STS_DETECT_CNT 0xFF00
#define IMX500_DD_REF_STS_ERR_CNT 0xFF0000
#define IMX500_DD_REF_CMD_REPLY_CNT 0xFF000000

/* Command reply status */
#define IMX500_REG_DD_CMD_REPLY_STS CCI_REG8(0xD014)
#define IMX500_DD_CMD_REPLY_STS_TRANS_READY 0x00
#define IMX500_DD_CMD_REPLY_STS_TRANS_DONE 0x01
#define IMX500_DD_CMD_REPLY_STS_UPDATE_READY 0x10
#define IMX500_DD_CMD_REPLY_STS_UPDATE_DONE 0x11
#define IMX500_DD_CMD_REPLY_STS_UPDATE_CANCEL_DONE 0x12
#define IMX500_DD_CMD_REPLY_STS_STATUS_ERROR 0xFF
#define IMX500_DD_CMD_REPLY_STS_MAC_AUTH_ERROR 0xFE
#define IMX500_DD_CMD_REPLY_STS_TIMEOUT_ERROR 0xFD
#define IMX500_DD_CMD_REPLY_STS_PARAMETER_ERROR 0xFC
#define IMX500_DD_CMD_REPLY_STS_INTERNAL_ERROR 0xFB
#define IMX500_DD_CMD_REPLY_STS_PACKET_FMT_ERROR 0xFA

/* Download status */
#define IMX500_REG_DD_DOWNLOAD_STS CCI_REG8(0xD015)
#define IMX500_DD_DOWNLOAD_STS_READY 0
#define IMX500_DD_DOWNLOAD_STS_DOWNLOADING 1

/* Update cancel */
#define IMX500_REG_DD_UPDATE_CANCEL CCI_REG8(0xD016)
#define IMX500_DD_UPDATE_CANCEL_NOT_CANCEL 0
#define IMX500_DD_UPDATE_CANCEL_DO_CANCEL 1

/* Notify error status */
#define IMX500_REG_DD_ERR_STS CCI_REG8(0xD020)
#define IMX500_DD_ERR_STS_STATUS_ERROR_BIT 0x1
#define IMX500_DD_ERR_STS_INTERNAL_ERROR_BIT 0x2
#define IMX500_DD_ERR_STS_PARAMETER_ERROR_BIT 0x4

/* System state */
#define IMX500_REG_DD_SYS_STATE CCI_REG8(0xD02A)
#define IMX500_DD_SYS_STATE_STANDBY_NO_NETWORK 0
#define IMX500_DD_SYS_STATE_STEAMING_NO_NETWORK 1
#define IMX500_DD_SYS_STATE_STANDBY_WITH_NETWORK 2
#define IMX500_DD_SYS_STATE_STREAMING_WITH_NETWORK 3

#define IMX500_REG_MAIN_FW_VERSION CCI_REG32(0xD07C)

/* Colour balance controls */
#define IMX500_REG_COLOUR_BALANCE_R CCI_REG16(0xd804)
#define IMX500_REG_COLOUR_BALANCE_GR CCI_REG16(0xd806)
#define IMX500_REG_COLOUR_BALANCE_GB CCI_REG16(0xd808)
#define IMX500_REG_COLOUR_BALANCE_B CCI_REG16(0xd80a)
#define IMX500_COLOUR_BALANCE_MIN 0x0001
#define IMX500_COLOUR_BALANCE_MAX 0x0fff
#define IMX500_COLOUR_BALANCE_STEP 0x0001
#define IMX500_COLOUR_BALANCE_DEFAULT 0x0100

/* Embedded sizes */
#define IMX500_MAX_EMBEDDED_SIZE \
	(2 * ((((IMX500_PIXEL_ARRAY_WIDTH * 10) >> 3) + 15) & ~15))

/* Inference sizes */
#define IMX500_INFERENCE_LINE_WIDTH 2560
#define IMX500_NUM_KPI_LINES 1
#define IMX500_NUM_PQ_LINES 1

/* IMX500 native and active pixel array size. */
#define IMX500_NATIVE_WIDTH 4072U
#define IMX500_NATIVE_HEIGHT 3176U
#define IMX500_PIXEL_ARRAY_LEFT 8U
#define IMX500_PIXEL_ARRAY_TOP 16U
#define IMX500_PIXEL_ARRAY_WIDTH 4056U
#define IMX500_PIXEL_ARRAY_HEIGHT 3040U

enum pad_types { IMAGE_PAD, METADATA_PAD, NUM_PADS };

#define V4L2_CID_USER_IMX500_INFERENCE_WINDOW (V4L2_CID_USER_IMX500_BASE + 0)
#define V4L2_CID_USER_IMX500_NETWORK_FW_FD (V4L2_CID_USER_IMX500_BASE + 1)
#define V4L2_CID_USER_GET_IMX500_DEVICE_ID (V4L2_CID_USER_IMX500_BASE + 2)

#define ONE_MIB (1024 * 1024)

/* regulator supplies */
static const char *const imx500_supply_name[] = {
	/* Supplies can be enabled in any order */
	"vana", /* Analog (2.7V) supply */
	"vdig", /* Digital Core (0.84V) supply */
	"vif", /* Interface (1.8V) supply */
};

#define IMX500_NUM_SUPPLIES ARRAY_SIZE(imx500_supply_name)

enum imx500_image_type {
	TYPE_LOADER = 0,
	TYPE_MAIN = 1,
	TYPE_NW_WEIGHTS = 2,
	TYPE_MAX
};

struct imx500_reg_list {
	unsigned int num_of_regs;
	const struct cci_reg_sequence *regs;
};

/* Mode : resolution and related config&values */
struct imx500_mode {
	/* Frame width */
	unsigned int width;

	/* Frame height */
	unsigned int height;

	/* H-timing in pixels */
	unsigned int line_length_pix;

	/* Analog crop rectangle. */
	struct v4l2_rect crop;

	/* Default register values */
	struct imx500_reg_list reg_list;
};

static const struct cci_reg_sequence mode_common_regs[] = {
	{ CCI_REG8(0x0305), 0x02 },
	{ CCI_REG8(0x0306), 0x00 },
	{ CCI_REG8(0x030d), 0x02 },
	{ CCI_REG8(0x030e), 0x00 },
	{ CCI_REG8(0x0106), 0x01 }, /* FAST_STANDBY_CTL */
	{ CCI_REG8(0x0136), 0x1b }, /* EXCLK_FREQ */
	{ CCI_REG8(0x0137), 0x00 },
	{ CCI_REG8(0x0112), 0x0a },
	{ CCI_REG8(0x0113), 0x0a },
	{ CCI_REG8(0x0114), 0x01 }, /* CSI_LANE_MODE */
	{ CCI_REG16(0x3054), IMX500_INFERENCE_LINE_WIDTH },
	{ CCI_REG16(0x3056), IMX500_INFERENCE_LINE_WIDTH },
	{ CCI_REG16(0x3058), IMX500_INFERENCE_LINE_WIDTH },
	{ CCI_REG16(0x305A), IMX500_INFERENCE_LINE_WIDTH }, /* X_OUT */
	{ CCI_REG16(0x305C), IMX500_NUM_KPI_LINES }, /* KPI Y_OUT */
	{ CCI_REG16(0x3062), IMX500_NUM_PQ_LINES }, /* PQ Y_OUT */
	{ CCI_REG8(0x3068), 0x30 },
	{ CCI_REG8(0x3069), 0x31 },
	{ CCI_REG8(0x306A), 0x32 },
	{ CCI_REG8(0x306B), 0x33 }, /* Data Types */
};

/* 12 mpix 15fps */
static const struct cci_reg_sequence mode_4056x3040_regs[] = {
	{ CCI_REG8(0x0340), 0x12 },
	{ CCI_REG8(0x0341), 0x42 },
	{ CCI_REG8(0x0342), 0x45 },
	{ CCI_REG8(0x0343), 0xec },
	{ CCI_REG8(0x3210), 0x00 },
	{ CCI_REG8(0x0344), 0x00 },
	{ CCI_REG8(0x0345), 0x00 },
	{ CCI_REG8(0x0346), 0x00 },
	{ CCI_REG8(0x0347), 0x00 },
	{ CCI_REG8(0x0348), 0x0f },
	{ CCI_REG8(0x0349), 0xd7 },
	{ CCI_REG8(0x0350), 0x00 },
	{ CCI_REG8(0x034a), 0x0b },
	{ CCI_REG8(0x034b), 0xdf },
	{ CCI_REG8(0x3f58), 0x01 },
	{ CCI_REG8(0x0381), 0x01 },
	{ CCI_REG8(0x0383), 0x01 },
	{ CCI_REG8(0x0385), 0x01 },
	{ CCI_REG8(0x0387), 0x01 },
	{ CCI_REG8(0x0900), 0x00 },
	{ CCI_REG8(0x0901), 0x11 },
	{ CCI_REG8(0x0902), 0x00 },
	{ CCI_REG8(0x3241), 0x11 },
	{ CCI_REG8(0x3242), 0x01 },
	{ CCI_REG8(0x3250), 0x00 },
	{ CCI_REG8(0x3f0f), 0x00 },
	{ CCI_REG8(0x3f40), 0x00 },
	{ CCI_REG8(0x3f41), 0x00 },
	{ CCI_REG8(0x3f42), 0x00 },
	{ CCI_REG8(0x3f43), 0x00 },
	{ CCI_REG8(0xb34e), 0x00 },
	{ CCI_REG8(0xb351), 0x20 },
	{ CCI_REG8(0xb35c), 0x00 },
	{ CCI_REG8(0xb35e), 0x08 },
	{ CCI_REG8(0x0401), 0x00 },
	{ CCI_REG8(0x0404), 0x00 },
	{ CCI_REG8(0x0405), 0x10 },
	{ CCI_REG8(0x0408), 0x00 },
	{ CCI_REG8(0x0409), 0x00 },
	{ CCI_REG8(0x040a), 0x00 },
	{ CCI_REG8(0x040b), 0x00 },
	{ CCI_REG8(0x040c), 0x0f },
	{ CCI_REG8(0x040d), 0xd8 },
	{ CCI_REG8(0x040e), 0x0b },
	{ CCI_REG8(0x040f), 0xe0 },
	{ CCI_REG8(0x034c), 0x0f },
	{ CCI_REG8(0x034d), 0xd8 },
	{ CCI_REG8(0x034e), 0x0b },
	{ CCI_REG8(0x034f), 0xe0 },
	{ CCI_REG8(0x0301), 0x05 },
	{ CCI_REG8(0x0303), 0x02 },
	{ CCI_REG8(0x0307), 0x9b },
	{ CCI_REG8(0x0309), 0x0a },
	{ CCI_REG8(0x030b), 0x01 },
	{ CCI_REG8(0x030f), 0x4a },
	{ CCI_REG8(0x0310), 0x01 },
	{ CCI_REG8(0x0820), 0x07 },
	{ CCI_REG8(0x0821), 0xce },
	{ CCI_REG8(0x0822), 0x00 },
	{ CCI_REG8(0x0823), 0x00 },
	{ CCI_REG8(0x3e20), 0x01 },
	{ CCI_REG8(0x3e35), 0x01 },
	{ CCI_REG8(0x3e36), 0x01 },
	{ CCI_REG8(0x3e37), 0x00 },
	{ CCI_REG8(0x3e3a), 0x01 },
	{ CCI_REG8(0x3e3b), 0x00 },
	{ CCI_REG8(0x00e3), 0x00 },
	{ CCI_REG8(0x00e4), 0x00 },
	{ CCI_REG8(0x00e6), 0x00 },
	{ CCI_REG8(0x00e7), 0x00 },
	{ CCI_REG8(0x00e8), 0x00 },
	{ CCI_REG8(0x00e9), 0x00 },
	{ CCI_REG8(0x3f50), 0x00 },
	{ CCI_REG8(0x3f56), 0x02 },
	{ CCI_REG8(0x3f57), 0x42 },
	{ CCI_REG8(0x3606), 0x01 },
	{ CCI_REG8(0x3607), 0x01 },
	{ CCI_REG8(0x3f26), 0x00 },
	{ CCI_REG8(0x3f4a), 0x00 },
	{ CCI_REG8(0x3f4b), 0x00 },
	{ CCI_REG8(0x4bc0), 0x16 },
	{ CCI_REG8(0x7ba8), 0x00 },
	{ CCI_REG8(0x7ba9), 0x00 },
	{ CCI_REG8(0x886b), 0x00 },
	{ CCI_REG8(0x579a), 0x00 },
	{ CCI_REG8(0x579b), 0x0a },
	{ CCI_REG8(0x579c), 0x01 },
	{ CCI_REG8(0x579d), 0x2a },
	{ CCI_REG8(0x57ac), 0x00 },
	{ CCI_REG8(0x57ad), 0x00 },
	{ CCI_REG8(0x57ae), 0x00 },
	{ CCI_REG8(0x57af), 0x81 },
	{ CCI_REG8(0x57be), 0x00 },
	{ CCI_REG8(0x57bf), 0x00 },
	{ CCI_REG8(0x57c0), 0x00 },
	{ CCI_REG8(0x57c1), 0x81 },
	{ CCI_REG8(0x57d0), 0x00 },
	{ CCI_REG8(0x57d1), 0x00 },
	{ CCI_REG8(0x57d2), 0x00 },
	{ CCI_REG8(0x57d3), 0x81 },
	{ CCI_REG8(0x5324), 0x00 },
	{ CCI_REG8(0x5325), 0x26 },
	{ CCI_REG8(0x5326), 0x00 },
	{ CCI_REG8(0x5327), 0x6b },
	{ CCI_REG8(0xbca7), 0x00 },
	{ CCI_REG8(0x5fcc), 0x28 },
	{ CCI_REG8(0x5fd7), 0x2d },
	{ CCI_REG8(0x5fe2), 0x2d },
	{ CCI_REG8(0x5fed), 0x2d },
	{ CCI_REG8(0x5ff8), 0x2d },
	{ CCI_REG8(0x6003), 0x2d },
	{ CCI_REG8(0x5d0b), 0x01 },
	{ CCI_REG8(0x6f6d), 0x00 },
	{ CCI_REG8(0x61c9), 0x00 },
	{ CCI_REG8(0x5352), 0x00 },
	{ CCI_REG8(0x5353), 0x49 },
	{ CCI_REG8(0x5356), 0x00 },
	{ CCI_REG8(0x5357), 0x30 },
	{ CCI_REG8(0x5358), 0x00 },
	{ CCI_REG8(0x5359), 0x3b },
	{ CCI_REG8(0x535c), 0x00 },
	{ CCI_REG8(0x535d), 0xb0 },
	{ CCI_REG8(0x6187), 0x18 },
	{ CCI_REG8(0x6189), 0x18 },
	{ CCI_REG8(0x618b), 0x18 },
	{ CCI_REG8(0x618d), 0x1d },
	{ CCI_REG8(0x618f), 0x1d },
	{ CCI_REG8(0x5414), 0x01 },
	{ CCI_REG8(0x5415), 0x0c },
	{ CCI_REG8(0xbca8), 0x0a },
	{ CCI_REG8(0x5fcf), 0x1e },
	{ CCI_REG8(0x5fda), 0x1e },
	{ CCI_REG8(0x5fe5), 0x1e },
	{ CCI_REG8(0x5ff0), 0x1e },
	{ CCI_REG8(0x5ffb), 0x1e },
	{ CCI_REG8(0x6006), 0x1e },
	{ CCI_REG8(0x616e), 0x04 },
	{ CCI_REG8(0x616f), 0x04 },
	{ CCI_REG8(0x6170), 0x04 },
	{ CCI_REG8(0x6171), 0x06 },
	{ CCI_REG8(0x6172), 0x06 },
	{ CCI_REG8(0x6173), 0x0c },
	{ CCI_REG8(0x6174), 0x0c },
	{ CCI_REG8(0x6175), 0x0c },
	{ CCI_REG8(0x6176), 0x00 },
	{ CCI_REG8(0x6177), 0x10 },
	{ CCI_REG8(0x6178), 0x00 },
	{ CCI_REG8(0x6179), 0x1a },
	{ CCI_REG8(0x617a), 0x00 },
	{ CCI_REG8(0x617b), 0x1a },
	{ CCI_REG8(0x617c), 0x00 },
	{ CCI_REG8(0x617d), 0x27 },
	{ CCI_REG8(0x617e), 0x00 },
	{ CCI_REG8(0x617f), 0x27 },
	{ CCI_REG8(0x6180), 0x00 },
	{ CCI_REG8(0x6181), 0x44 },
	{ CCI_REG8(0x6182), 0x00 },
	{ CCI_REG8(0x6183), 0x44 },
	{ CCI_REG8(0x6184), 0x00 },
	{ CCI_REG8(0x6185), 0x44 },
	{ CCI_REG8(0x5dfc), 0x0a },
	{ CCI_REG8(0x5e00), 0x0a },
	{ CCI_REG8(0x5e04), 0x0a },
	{ CCI_REG8(0x5e08), 0x0a },
	{ CCI_REG8(0x5dfd), 0x0a },
	{ CCI_REG8(0x5e01), 0x0a },
	{ CCI_REG8(0x5e05), 0x0a },
	{ CCI_REG8(0x5e09), 0x0a },
	{ CCI_REG8(0x5dfe), 0x0a },
	{ CCI_REG8(0x5e02), 0x0a },
	{ CCI_REG8(0x5e06), 0x0a },
	{ CCI_REG8(0x5e0a), 0x0a },
	{ CCI_REG8(0x5dff), 0x0a },
	{ CCI_REG8(0x5e03), 0x0a },
	{ CCI_REG8(0x5e07), 0x0a },
	{ CCI_REG8(0x5e0b), 0x0a },
	{ CCI_REG8(0x5dec), 0x12 },
	{ CCI_REG8(0x5df0), 0x12 },
	{ CCI_REG8(0x5df4), 0x21 },
	{ CCI_REG8(0x5df8), 0x31 },
	{ CCI_REG8(0x5ded), 0x12 },
	{ CCI_REG8(0x5df1), 0x12 },
	{ CCI_REG8(0x5df5), 0x21 },
	{ CCI_REG8(0x5df9), 0x31 },
	{ CCI_REG8(0x5dee), 0x12 },
	{ CCI_REG8(0x5df2), 0x12 },
	{ CCI_REG8(0x5df6), 0x21 },
	{ CCI_REG8(0x5dfa), 0x31 },
	{ CCI_REG8(0x5def), 0x12 },
	{ CCI_REG8(0x5df3), 0x12 },
	{ CCI_REG8(0x5df7), 0x21 },
	{ CCI_REG8(0x5dfb), 0x31 },
	{ CCI_REG8(0x5ddc), 0x0d },
	{ CCI_REG8(0x5de0), 0x0d },
	{ CCI_REG8(0x5de4), 0x0d },
	{ CCI_REG8(0x5de8), 0x0d },
	{ CCI_REG8(0x5ddd), 0x0d },
	{ CCI_REG8(0x5de1), 0x0d },
	{ CCI_REG8(0x5de5), 0x0d },
	{ CCI_REG8(0x5de9), 0x0d },
	{ CCI_REG8(0x5dde), 0x0d },
	{ CCI_REG8(0x5de2), 0x0d },
	{ CCI_REG8(0x5de6), 0x0d },
	{ CCI_REG8(0x5dea), 0x0d },
	{ CCI_REG8(0x5ddf), 0x0d },
	{ CCI_REG8(0x5de3), 0x0d },
	{ CCI_REG8(0x5de7), 0x0d },
	{ CCI_REG8(0x5deb), 0x0d },
	{ CCI_REG8(0x5dcc), 0x55 },
	{ CCI_REG8(0x5dd0), 0x50 },
	{ CCI_REG8(0x5dd4), 0x4b },
	{ CCI_REG8(0x5dd8), 0x4b },
	{ CCI_REG8(0x5dcd), 0x55 },
	{ CCI_REG8(0x5dd1), 0x50 },
	{ CCI_REG8(0x5dd5), 0x4b },
	{ CCI_REG8(0x5dd9), 0x4b },
	{ CCI_REG8(0x5dce), 0x55 },
	{ CCI_REG8(0x5dd2), 0x50 },
	{ CCI_REG8(0x5dd6), 0x4b },
	{ CCI_REG8(0x5dda), 0x4b },
	{ CCI_REG8(0x5dcf), 0x55 },
	{ CCI_REG8(0x5dd3), 0x50 },
	{ CCI_REG8(0x5dd7), 0x4b },
	{ CCI_REG8(0x5ddb), 0x4b },
	{ CCI_REG8(0x0202), 0x12 },
	{ CCI_REG8(0x0203), 0x2c },
	{ CCI_REG8(0x0204), 0x00 },
	{ CCI_REG8(0x0205), 0x00 },
	{ CCI_REG8(0x020e), 0x01 },
	{ CCI_REG8(0x020f), 0x00 },
	{ CCI_REG8(0x0210), 0x01 },
	{ CCI_REG8(0x0211), 0x00 },
	{ CCI_REG8(0x0212), 0x01 },
	{ CCI_REG8(0x0213), 0x00 },
	{ CCI_REG8(0x0214), 0x01 },
	{ CCI_REG8(0x0215), 0x00 },
};

/* 2x2 binned. 56fps */
static const struct cci_reg_sequence mode_2028x1520_regs[] = {
	{ CCI_REG8(0x0112), 0x0a },
	{ CCI_REG8(0x0113), 0x0a },
	{ CCI_REG8(0x0114), 0x01 },
	{ CCI_REG8(0x0342), 0x24 },
	{ CCI_REG8(0x0343), 0xb6 },
	{ CCI_REG8(0x0340), 0x0b },
	{ CCI_REG8(0x0341), 0x9c },
	{ CCI_REG8(0x3210), 0x00 },
	{ CCI_REG8(0x0344), 0x00 },
	{ CCI_REG8(0x0345), 0x00 },
	{ CCI_REG8(0x0346), 0x00 },
	{ CCI_REG8(0x0347), 0x00 },
	{ CCI_REG8(0x0348), 0x0f },
	{ CCI_REG8(0x0349), 0xd7 },
	{ CCI_REG8(0x0350), 0x00 },
	{ CCI_REG8(0x034a), 0x0b },
	{ CCI_REG8(0x034b), 0xdf },
	{ CCI_REG8(0x3f58), 0x01 },
	{ CCI_REG8(0x0381), 0x01 },
	{ CCI_REG8(0x0383), 0x01 },
	{ CCI_REG8(0x0385), 0x01 },
	{ CCI_REG8(0x0387), 0x01 },
	{ CCI_REG8(0x0900), 0x01 },
	{ CCI_REG8(0x0901), 0x22 },
	{ CCI_REG8(0x0902), 0x02 },
	{ CCI_REG8(0x3241), 0x11 },
	{ CCI_REG8(0x3242), 0x01 },
	{ CCI_REG8(0x3250), 0x03 },
	{ CCI_REG8(0x3f0f), 0x00 },
	{ CCI_REG8(0x3f40), 0x00 },
	{ CCI_REG8(0x3f41), 0x00 },
	{ CCI_REG8(0x3f42), 0x00 },
	{ CCI_REG8(0x3f43), 0x00 },
	{ CCI_REG8(0xb34e), 0x00 },
	{ CCI_REG8(0xb351), 0x20 },
	{ CCI_REG8(0xb35c), 0x00 },
	{ CCI_REG8(0xb35e), 0x08 },
	{ CCI_REG8(0x0401), 0x00 },
	{ CCI_REG8(0x0404), 0x00 },
	{ CCI_REG8(0x0405), 0x10 },
	{ CCI_REG8(0x0408), 0x00 },
	{ CCI_REG8(0x0409), 0x00 },
	{ CCI_REG8(0x040a), 0x00 },
	{ CCI_REG8(0x040b), 0x00 },
	{ CCI_REG8(0x040c), 0x07 },
	{ CCI_REG8(0x040d), 0xec },
	{ CCI_REG8(0x040e), 0x05 },
	{ CCI_REG8(0x040f), 0xf0 },
	{ CCI_REG8(0x034c), 0x07 },
	{ CCI_REG8(0x034d), 0xec },
	{ CCI_REG8(0x034e), 0x05 },
	{ CCI_REG8(0x034f), 0xf0 },
	{ CCI_REG8(0x0301), 0x05 },
	{ CCI_REG8(0x0303), 0x02 },
	{ CCI_REG8(0x0307), 0x9b },
	{ CCI_REG8(0x0309), 0x0a },
	{ CCI_REG8(0x030b), 0x01 },
	{ CCI_REG8(0x030f), 0x4a },
	{ CCI_REG8(0x0310), 0x01 },
	{ CCI_REG8(0x0820), 0x07 },
	{ CCI_REG8(0x0821), 0xce },
	{ CCI_REG8(0x0822), 0x00 },
	{ CCI_REG8(0x0823), 0x00 },
	{ CCI_REG8(0x3e20), 0x01 },
	{ CCI_REG8(0x3e35), 0x01 },
	{ CCI_REG8(0x3e36), 0x01 },
	{ CCI_REG8(0x3e37), 0x00 },
	{ CCI_REG8(0x3e3a), 0x01 },
	{ CCI_REG8(0x3e3b), 0x00 },
	{ CCI_REG8(0x00e3), 0x00 },
	{ CCI_REG8(0x00e4), 0x00 },
	{ CCI_REG8(0x00e6), 0x00 },
	{ CCI_REG8(0x00e7), 0x00 },
	{ CCI_REG8(0x00e8), 0x00 },
	{ CCI_REG8(0x00e9), 0x00 },
	{ CCI_REG8(0x3f50), 0x00 },
	{ CCI_REG8(0x3f56), 0x01 },
	{ CCI_REG8(0x3f57), 0x30 },
	{ CCI_REG8(0x3606), 0x01 },
	{ CCI_REG8(0x3607), 0x01 },
	{ CCI_REG8(0x3f26), 0x00 },
	{ CCI_REG8(0x3f4a), 0x00 },
	{ CCI_REG8(0x3f4b), 0x00 },
	{ CCI_REG8(0x4bc0), 0x16 },
	{ CCI_REG8(0x7ba8), 0x00 },
	{ CCI_REG8(0x7ba9), 0x00 },
	{ CCI_REG8(0x886b), 0x00 },
	{ CCI_REG8(0x579a), 0x00 },
	{ CCI_REG8(0x579b), 0x0a },
	{ CCI_REG8(0x579c), 0x01 },
	{ CCI_REG8(0x579d), 0x2a },
	{ CCI_REG8(0x57ac), 0x00 },
	{ CCI_REG8(0x57ad), 0x00 },
	{ CCI_REG8(0x57ae), 0x00 },
	{ CCI_REG8(0x57af), 0x81 },
	{ CCI_REG8(0x57be), 0x00 },
	{ CCI_REG8(0x57bf), 0x00 },
	{ CCI_REG8(0x57c0), 0x00 },
	{ CCI_REG8(0x57c1), 0x81 },
	{ CCI_REG8(0x57d0), 0x00 },
	{ CCI_REG8(0x57d1), 0x00 },
	{ CCI_REG8(0x57d2), 0x00 },
	{ CCI_REG8(0x57d3), 0x81 },
	{ CCI_REG8(0x5324), 0x00 },
	{ CCI_REG8(0x5325), 0x31 },
	{ CCI_REG8(0x5326), 0x00 },
	{ CCI_REG8(0x5327), 0x60 },
	{ CCI_REG8(0xbca7), 0x08 },
	{ CCI_REG8(0x5fcc), 0x1e },
	{ CCI_REG8(0x5fd7), 0x1e },
	{ CCI_REG8(0x5fe2), 0x1e },
	{ CCI_REG8(0x5fed), 0x1e },
	{ CCI_REG8(0x5ff8), 0x1e },
	{ CCI_REG8(0x6003), 0x1e },
	{ CCI_REG8(0x5d0b), 0x02 },
	{ CCI_REG8(0x6f6d), 0x01 },
	{ CCI_REG8(0x61c9), 0x68 },
	{ CCI_REG8(0x5352), 0x00 },
	{ CCI_REG8(0x5353), 0x3f },
	{ CCI_REG8(0x5356), 0x00 },
	{ CCI_REG8(0x5357), 0x1c },
	{ CCI_REG8(0x5358), 0x00 },
	{ CCI_REG8(0x5359), 0x3d },
	{ CCI_REG8(0x535c), 0x00 },
	{ CCI_REG8(0x535d), 0xa6 },
	{ CCI_REG8(0x6187), 0x1d },
	{ CCI_REG8(0x6189), 0x1d },
	{ CCI_REG8(0x618b), 0x1d },
	{ CCI_REG8(0x618d), 0x23 },
	{ CCI_REG8(0x618f), 0x23 },
	{ CCI_REG8(0x5414), 0x01 },
	{ CCI_REG8(0x5415), 0x12 },
	{ CCI_REG8(0xbca8), 0x00 },
	{ CCI_REG8(0x5fcf), 0x28 },
	{ CCI_REG8(0x5fda), 0x2d },
	{ CCI_REG8(0x5fe5), 0x2d },
	{ CCI_REG8(0x5ff0), 0x2d },
	{ CCI_REG8(0x5ffb), 0x2d },
	{ CCI_REG8(0x6006), 0x2d },
	{ CCI_REG8(0x616e), 0x04 },
	{ CCI_REG8(0x616f), 0x04 },
	{ CCI_REG8(0x6170), 0x04 },
	{ CCI_REG8(0x6171), 0x06 },
	{ CCI_REG8(0x6172), 0x06 },
	{ CCI_REG8(0x6173), 0x0c },
	{ CCI_REG8(0x6174), 0x0c },
	{ CCI_REG8(0x6175), 0x0c },
	{ CCI_REG8(0x6176), 0x00 },
	{ CCI_REG8(0x6177), 0x10 },
	{ CCI_REG8(0x6178), 0x00 },
	{ CCI_REG8(0x6179), 0x1a },
	{ CCI_REG8(0x617a), 0x00 },
	{ CCI_REG8(0x617b), 0x1a },
	{ CCI_REG8(0x617c), 0x00 },
	{ CCI_REG8(0x617d), 0x27 },
	{ CCI_REG8(0x617e), 0x00 },
	{ CCI_REG8(0x617f), 0x27 },
	{ CCI_REG8(0x6180), 0x00 },
	{ CCI_REG8(0x6181), 0x44 },
	{ CCI_REG8(0x6182), 0x00 },
	{ CCI_REG8(0x6183), 0x44 },
	{ CCI_REG8(0x6184), 0x00 },
	{ CCI_REG8(0x6185), 0x44 },
	{ CCI_REG8(0x5dfc), 0x0a },
	{ CCI_REG8(0x5e00), 0x0a },
	{ CCI_REG8(0x5e04), 0x0a },
	{ CCI_REG8(0x5e08), 0x0a },
	{ CCI_REG8(0x5dfd), 0x0a },
	{ CCI_REG8(0x5e01), 0x0a },
	{ CCI_REG8(0x5e05), 0x0a },
	{ CCI_REG8(0x5e09), 0x0a },
	{ CCI_REG8(0x5dfe), 0x0a },
	{ CCI_REG8(0x5e02), 0x0a },
	{ CCI_REG8(0x5e06), 0x0a },
	{ CCI_REG8(0x5e0a), 0x0a },
	{ CCI_REG8(0x5dff), 0x0a },
	{ CCI_REG8(0x5e03), 0x0a },
	{ CCI_REG8(0x5e07), 0x0a },
	{ CCI_REG8(0x5e0b), 0x0a },
	{ CCI_REG8(0x5dec), 0x12 },
	{ CCI_REG8(0x5df0), 0x12 },
	{ CCI_REG8(0x5df4), 0x21 },
	{ CCI_REG8(0x5df8), 0x31 },
	{ CCI_REG8(0x5ded), 0x12 },
	{ CCI_REG8(0x5df1), 0x12 },
	{ CCI_REG8(0x5df5), 0x21 },
	{ CCI_REG8(0x5df9), 0x31 },
	{ CCI_REG8(0x5dee), 0x12 },
	{ CCI_REG8(0x5df2), 0x12 },
	{ CCI_REG8(0x5df6), 0x21 },
	{ CCI_REG8(0x5dfa), 0x31 },
	{ CCI_REG8(0x5def), 0x12 },
	{ CCI_REG8(0x5df3), 0x12 },
	{ CCI_REG8(0x5df7), 0x21 },
	{ CCI_REG8(0x5dfb), 0x31 },
	{ CCI_REG8(0x5ddc), 0x0d },
	{ CCI_REG8(0x5de0), 0x0d },
	{ CCI_REG8(0x5de4), 0x0d },
	{ CCI_REG8(0x5de8), 0x0d },
	{ CCI_REG8(0x5ddd), 0x0d },
	{ CCI_REG8(0x5de1), 0x0d },
	{ CCI_REG8(0x5de5), 0x0d },
	{ CCI_REG8(0x5de9), 0x0d },
	{ CCI_REG8(0x5dde), 0x0d },
	{ CCI_REG8(0x5de2), 0x0d },
	{ CCI_REG8(0x5de6), 0x0d },
	{ CCI_REG8(0x5dea), 0x0d },
	{ CCI_REG8(0x5ddf), 0x0d },
	{ CCI_REG8(0x5de3), 0x0d },
	{ CCI_REG8(0x5de7), 0x0d },
	{ CCI_REG8(0x5deb), 0x0d },
	{ CCI_REG8(0x5dcc), 0x55 },
	{ CCI_REG8(0x5dd0), 0x50 },
	{ CCI_REG8(0x5dd4), 0x4b },
	{ CCI_REG8(0x5dd8), 0x4b },
	{ CCI_REG8(0x5dcd), 0x55 },
	{ CCI_REG8(0x5dd1), 0x50 },
	{ CCI_REG8(0x5dd5), 0x4b },
	{ CCI_REG8(0x5dd9), 0x4b },
	{ CCI_REG8(0x5dce), 0x55 },
	{ CCI_REG8(0x5dd2), 0x50 },
	{ CCI_REG8(0x5dd6), 0x4b },
	{ CCI_REG8(0x5dda), 0x4b },
	{ CCI_REG8(0x5dcf), 0x55 },
	{ CCI_REG8(0x5dd3), 0x50 },
	{ CCI_REG8(0x5dd7), 0x4b },
	{ CCI_REG8(0x5ddb), 0x4b },
	{ CCI_REG8(0x0202), 0x0b },
	{ CCI_REG8(0x0203), 0x86 },
	{ CCI_REG8(0x0204), 0x00 },
	{ CCI_REG8(0x0205), 0x00 },
	{ CCI_REG8(0x020e), 0x01 },
	{ CCI_REG8(0x020f), 0x00 },
	{ CCI_REG8(0x0210), 0x01 },
	{ CCI_REG8(0x0211), 0x00 },
	{ CCI_REG8(0x0212), 0x01 },
	{ CCI_REG8(0x0213), 0x00 },
	{ CCI_REG8(0x0214), 0x01 },
	{ CCI_REG8(0x0215), 0x00 },
};

static const struct cci_reg_sequence metadata_output[] = {
	{ CCI_REG8(0x3050), 1 }, /* MIPI Output enabled */
	{ CCI_REG8(0x3051), 1 }, /* MIPI output frame includes pixels data */
	{ CCI_REG8(0x3052), 1 }, /* MIPI output frame includes meta data */
	{ IMX500_REG_DD_CH06_VCID, 0 },
	{ IMX500_REG_DD_CH07_VCID, 0 },
	{ IMX500_REG_DD_CH08_VCID, 0 },
	{ IMX500_REG_DD_CH09_VCID, 0 },
	{ IMX500_REG_DD_CH06_DT,
	  0x12 }, /* KPI - User Defined 8-bit Data Type 1 */
	{ IMX500_REG_DD_CH07_DT, 0x12 }, /* Input Tensor - U.D. 8-bit type 2 */
	{ IMX500_REG_DD_CH08_DT, 0x12 }, /* Output Tensor - U.D. 8-bit type 3 */
	{ IMX500_REG_DD_CH09_DT, 0x12 }, /* PQ - U.D. 8-bit type 4 */
	{ IMX500_REG_DD_CH06_PACKING, IMX500_DD_PACKING_8BPP },
	{ IMX500_REG_DD_CH07_PACKING, IMX500_DD_PACKING_8BPP },
	{ IMX500_REG_DD_CH08_PACKING, IMX500_DD_PACKING_8BPP },
	{ IMX500_REG_DD_CH09_PACKING, IMX500_DD_PACKING_8BPP },
};

static const struct cci_reg_sequence dnn_regs[] = {
	{ CCI_REG8(0xd960), 0x52 },
	{ CCI_REG8(0xd961), 0x52 },
	{ CCI_REG8(0xd962), 0x52 },
	{ CCI_REG8(0xd963), 0x52 },
	{ CCI_REG8(0xd96c), 0x44 },
	{ CCI_REG8(0xd96d), 0x44 },
	{ CCI_REG8(0xd96e), 0x44 },
	{ CCI_REG8(0xd96f), 0x44 },
	{ CCI_REG8(0xd600), 0x20 },
	/* Black level */
	{ CCI_REG16(0xd80c), 0x100 },
	{ CCI_REG16(0xd80e), 0x100 },
	{ CCI_REG16(0xd810), 0x100 },
	{ CCI_REG16(0xd812), 0x100 },
	/* Gamma */
	{ CCI_REG8(0xd814), 1 },
	{ CCI_REG32(0xd850), 0x10000 },
	{ CCI_REG32(0xd854), 0x40002 },
	{ CCI_REG32(0xd858), 0x60005 },
	{ CCI_REG32(0xd85c), 0x90008 },
	{ CCI_REG32(0xd860), 0xc000a },
	{ CCI_REG32(0xd864), 0x12000f },
	{ CCI_REG32(0xd868), 0x1c0014 },
	{ CCI_REG32(0xd86c), 0x2a0024 },
	{ CCI_REG32(0xd870), 0x360030 },
	{ CCI_REG32(0xd874), 0x46003c },
	{ CCI_REG32(0xd878), 0x5a0051 },
	{ CCI_REG32(0xd87c), 0x750064 },
	{ CCI_REG32(0xd880), 0x920084 },
	{ CCI_REG32(0xd884), 0xa9009e },
	{ CCI_REG32(0xd888), 0xba00b2 },
	{ CCI_REG32(0xd88c), 0xc700c1 },
	{ CCI_REG32(0xd890), 0xd100cd },
	{ CCI_REG32(0xd894), 0xde00d6 },
	{ CCI_REG32(0xd898), 0xe900e4 },
	{ CCI_REG32(0xd89c), 0xf300ee },
	{ CCI_REG32(0xd8a0), 0xfb00f7 },
	{ CCI_REG16(0xd8a4), 0xff },
	{ CCI_REG32(0xd8a8), 0x10000 },
	{ CCI_REG32(0xd8ac), 0x40002 },
	{ CCI_REG32(0xd8b0), 0x60005 },
	{ CCI_REG32(0xd8b4), 0x90008 },
	{ CCI_REG32(0xd8b8), 0xc000a },
	{ CCI_REG32(0xd8bc), 0x12000f },
	{ CCI_REG32(0xd8c0), 0x1c0014 },
	{ CCI_REG32(0xd8c4), 0x2a0024 },
	{ CCI_REG32(0xd8c8), 0x360030 },
	{ CCI_REG32(0xd8cc), 0x46003c },
	{ CCI_REG32(0xd8d0), 0x5a0051 },
	{ CCI_REG32(0xd8d4), 0x750064 },
	{ CCI_REG32(0xd8d8), 0x920084 },
	{ CCI_REG32(0xd8dc), 0xa9009e },
	{ CCI_REG32(0xd8e0), 0xba00b2 },
	{ CCI_REG32(0xd8e4), 0xc700c1 },
	{ CCI_REG32(0xd8e8), 0xd100cd },
	{ CCI_REG32(0xd8ec), 0xde00d6 },
	{ CCI_REG32(0xd8f0), 0xe900e4 },
	{ CCI_REG32(0xd8f4), 0xf300ee },
	{ CCI_REG32(0xd8f8), 0xfb00f7 },
	{ CCI_REG16(0xd8fc), 0xff },
	{ CCI_REG32(0xd900), 0x10000 },
	{ CCI_REG32(0xd904), 0x40002 },
	{ CCI_REG32(0xd908), 0x60005 },
	{ CCI_REG32(0xd90c), 0x90008 },
	{ CCI_REG32(0xd910), 0xc000a },
	{ CCI_REG32(0xd914), 0x12000f },
	{ CCI_REG32(0xd918), 0x1c0014 },
	{ CCI_REG32(0xd91c), 0x2a0024 },
	{ CCI_REG32(0xd920), 0x360030 },
	{ CCI_REG32(0xd924), 0x46003c },
	{ CCI_REG32(0xd928), 0x5a0051 },
	{ CCI_REG32(0xd92c), 0x750064 },
	{ CCI_REG32(0xd930), 0x920084 },
	{ CCI_REG32(0xd934), 0xa9009e },
	{ CCI_REG32(0xd938), 0xba00b2 },
	{ CCI_REG32(0xd93c), 0xc700c1 },
	{ CCI_REG32(0xd940), 0xd100cd },
	{ CCI_REG32(0xd944), 0xde00d6 },
	{ CCI_REG32(0xd948), 0xe900e4 },
	{ CCI_REG32(0xd94c), 0xf300ee },
	{ CCI_REG32(0xd950), 0xfb00f7 },
	{ CCI_REG16(0xd954), 0xff },
	{ CCI_REG8(0xd826), 1 },
	/* LSC */
	{ CCI_REG32(0xe000), 0x2e502a0 },
	{ CCI_REG32(0xe004), 0x2c80283 },
	{ CCI_REG32(0xe008), 0x2700233 },
	{ CCI_REG32(0xe00c), 0x22d01f6 },
	{ CCI_REG32(0xe010), 0x1f401c3 },
	{ CCI_REG32(0xe014), 0x1c5019c },
	{ CCI_REG32(0xe018), 0x1bb0192 },
	{ CCI_REG32(0xe01c), 0x1ba0192 },
	{ CCI_REG32(0xe020), 0x1b90192 },
	{ CCI_REG32(0xe024), 0x1ba0192 },
	{ CCI_REG32(0xe028), 0x1ca019f },
	{ CCI_REG32(0xe02c), 0x1fb01c8 },
	{ CCI_REG32(0xe030), 0x23601fb },
	{ CCI_REG32(0xe034), 0x27a0239 },
	{ CCI_REG32(0xe038), 0x2d5028a },
	{ CCI_REG32(0xe03c), 0x2f302a8 },
	{ CCI_REG32(0xe040), 0x2c60283 },
	{ CCI_REG32(0xe044), 0x27c0240 },
	{ CCI_REG32(0xe048), 0x22d01f6 },
	{ CCI_REG32(0xe04c), 0x1fd01cd },
	{ CCI_REG32(0xe050), 0x1c4019c },
	{ CCI_REG32(0xe054), 0x19c017b },
	{ CCI_REG32(0xe058), 0x1810165 },
	{ CCI_REG32(0xe05c), 0x175015c },
	{ CCI_REG32(0xe060), 0x175015c },
	{ CCI_REG32(0xe064), 0x1840167 },
	{ CCI_REG32(0xe068), 0x1a0017e },
	{ CCI_REG32(0xe06c), 0x1cc01a1 },
	{ CCI_REG32(0xe070), 0x20501d1 },
	{ CCI_REG32(0xe074), 0x23601fc },
	{ CCI_REG32(0xe078), 0x2890246 },
	{ CCI_REG32(0xe07c), 0x2d3028a },
	{ CCI_REG32(0xe080), 0x2800243 },
	{ CCI_REG32(0xe084), 0x245020e },
	{ CCI_REG32(0xe088), 0x1ff01ce },
	{ CCI_REG32(0xe08c), 0x1c4019c },
	{ CCI_REG32(0xe090), 0x19a017b },
	{ CCI_REG32(0xe094), 0x1650150 },
	{ CCI_REG32(0xe098), 0x14a013a },
	{ CCI_REG32(0xe09c), 0x13f0131 },
	{ CCI_REG32(0xe0a0), 0x1400131 },
	{ CCI_REG32(0xe0a4), 0x14d013c },
	{ CCI_REG32(0xe0a8), 0x16a0154 },
	{ CCI_REG32(0xe0ac), 0x1a1017e },
	{ CCI_REG32(0xe0b0), 0x1cc01a1 },
	{ CCI_REG32(0xe0b4), 0x20801d3 },
	{ CCI_REG32(0xe0b8), 0x2510214 },
	{ CCI_REG32(0xe0bc), 0x28b0249 },
	{ CCI_REG32(0xe0c0), 0x2640229 },
	{ CCI_REG32(0xe0c4), 0x22101ed },
	{ CCI_REG32(0xe0c8), 0x1dc01b0 },
	{ CCI_REG32(0xe0cc), 0x19c017c },
	{ CCI_REG32(0xe0d0), 0x1650150 },
	{ CCI_REG32(0xe0d4), 0x148013a },
	{ CCI_REG32(0xe0d8), 0x123011c },
	{ CCI_REG32(0xe0dc), 0x1190113 },
	{ CCI_REG32(0xe0e0), 0x1190113 },
	{ CCI_REG32(0xe0e4), 0x1280120 },
	{ CCI_REG32(0xe0e8), 0x14c013c },
	{ CCI_REG32(0xe0ec), 0x16b0154 },
	{ CCI_REG32(0xe0f0), 0x1a30181 },
	{ CCI_REG32(0xe0f4), 0x1e601b6 },
	{ CCI_REG32(0xe0f8), 0x22c01f3 },
	{ CCI_REG32(0xe0fc), 0x2700230 },
	{ CCI_REG32(0xe100), 0x257021d },
	{ CCI_REG32(0xe104), 0x20901d8 },
	{ CCI_REG32(0xe108), 0x1c4019d },
	{ CCI_REG32(0xe10c), 0x1820167 },
	{ CCI_REG32(0xe110), 0x14b013b },
	{ CCI_REG32(0xe114), 0x124011c },
	{ CCI_REG32(0xe118), 0x1170113 },
	{ CCI_REG32(0xe11c), 0x1010101 },
	{ CCI_REG32(0xe120), 0x1030102 },
	{ CCI_REG32(0xe124), 0x1190113 },
	{ CCI_REG32(0xe128), 0x1280120 },
	{ CCI_REG32(0xe12c), 0x14f013f },
	{ CCI_REG32(0xe130), 0x189016c },
	{ CCI_REG32(0xe134), 0x1ce01a3 },
	{ CCI_REG32(0xe138), 0x21601df },
	{ CCI_REG32(0xe13c), 0x2630224 },
	{ CCI_REG32(0xe140), 0x257021d },
	{ CCI_REG32(0xe144), 0x20101d0 },
	{ CCI_REG32(0xe148), 0x1ba0194 },
	{ CCI_REG32(0xe14c), 0x176015d },
	{ CCI_REG32(0xe150), 0x13e0132 },
	{ CCI_REG32(0xe154), 0x1190114 },
	{ CCI_REG32(0xe158), 0x1010101 },
	{ CCI_REG32(0xe15c), 0x1000100 },
	{ CCI_REG32(0xe160), 0x1010100 },
	{ CCI_REG32(0xe164), 0x1040103 },
	{ CCI_REG32(0xe168), 0x11d0118 },
	{ CCI_REG32(0xe16c), 0x1450136 },
	{ CCI_REG32(0xe170), 0x17d0163 },
	{ CCI_REG32(0xe174), 0x1c4019a },
	{ CCI_REG32(0xe178), 0x20d01d6 },
	{ CCI_REG32(0xe17c), 0x2630224 },
	{ CCI_REG32(0xe180), 0x257021d },
	{ CCI_REG32(0xe184), 0x20001d0 },
	{ CCI_REG32(0xe188), 0x1b90194 },
	{ CCI_REG32(0xe18c), 0x175015d },
	{ CCI_REG32(0xe190), 0x13e0132 },
	{ CCI_REG32(0xe194), 0x1180114 },
	{ CCI_REG32(0xe198), 0x1040103 },
	{ CCI_REG32(0xe19c), 0x1000100 },
	{ CCI_REG32(0xe1a0), 0x1030102 },
	{ CCI_REG32(0xe1a4), 0x1050103 },
	{ CCI_REG32(0xe1a8), 0x11d0118 },
	{ CCI_REG32(0xe1ac), 0x1450136 },
	{ CCI_REG32(0xe1b0), 0x17d0163 },
	{ CCI_REG32(0xe1b4), 0x1c4019a },
	{ CCI_REG32(0xe1b8), 0x20d01d6 },
	{ CCI_REG32(0xe1bc), 0x2640224 },
	{ CCI_REG32(0xe1c0), 0x258021f },
	{ CCI_REG32(0xe1c4), 0x20e01db },
	{ CCI_REG32(0xe1c8), 0x1c7019f },
	{ CCI_REG32(0xe1cc), 0x1840169 },
	{ CCI_REG32(0xe1d0), 0x14d013e },
	{ CCI_REG32(0xe1d4), 0x1290120 },
	{ CCI_REG32(0xe1d8), 0x1180114 },
	{ CCI_REG32(0xe1dc), 0x1050103 },
	{ CCI_REG32(0xe1e0), 0x1050103 },
	{ CCI_REG32(0xe1e4), 0x11e0117 },
	{ CCI_REG32(0xe1e8), 0x12c0123 },
	{ CCI_REG32(0xe1ec), 0x1530142 },
	{ CCI_REG32(0xe1f0), 0x18d016f },
	{ CCI_REG32(0xe1f4), 0x1d201a6 },
	{ CCI_REG32(0xe1f8), 0x21a01e2 },
	{ CCI_REG32(0xe1fc), 0x2640225 },
	{ CCI_REG32(0xe200), 0x269022d },
	{ CCI_REG32(0xe204), 0x22601f1 },
	{ CCI_REG32(0xe208), 0x1e101b4 },
	{ CCI_REG32(0xe20c), 0x1a10181 },
	{ CCI_REG32(0xe210), 0x16c0156 },
	{ CCI_REG32(0xe214), 0x14d013e },
	{ CCI_REG32(0xe218), 0x1290120 },
	{ CCI_REG32(0xe21c), 0x11f0118 },
	{ CCI_REG32(0xe220), 0x11f0118 },
	{ CCI_REG32(0xe224), 0x12b0123 },
	{ CCI_REG32(0xe228), 0x1530142 },
	{ CCI_REG32(0xe22c), 0x172015a },
	{ CCI_REG32(0xe230), 0x1aa0187 },
	{ CCI_REG32(0xe234), 0x1ec01bb },
	{ CCI_REG32(0xe238), 0x23301f8 },
	{ CCI_REG32(0xe23c), 0x2750233 },
	{ CCI_REG32(0xe240), 0x28b024c },
	{ CCI_REG32(0xe244), 0x24f0216 },
	{ CCI_REG32(0xe248), 0x20701d4 },
	{ CCI_REG32(0xe24c), 0x1ce01a4 },
	{ CCI_REG32(0xe250), 0x1a10181 },
	{ CCI_REG32(0xe254), 0x16c0156 },
	{ CCI_REG32(0xe258), 0x1520141 },
	{ CCI_REG32(0xe25c), 0x1480138 },
	{ CCI_REG32(0xe260), 0x1480138 },
	{ CCI_REG32(0xe264), 0x1550143 },
	{ CCI_REG32(0xe268), 0x172015a },
	{ CCI_REG32(0xe26c), 0x1aa0187 },
	{ CCI_REG32(0xe270), 0x1d701a9 },
	{ CCI_REG32(0xe274), 0x21201db },
	{ CCI_REG32(0xe278), 0x25d021d },
	{ CCI_REG32(0xe27c), 0x2990254 },
	{ CCI_REG32(0xe280), 0x2d70291 },
	{ CCI_REG32(0xe284), 0x28c024c },
	{ CCI_REG32(0xe288), 0x2390201 },
	{ CCI_REG32(0xe28c), 0x20701d4 },
	{ CCI_REG32(0xe290), 0x1ce01a4 },
	{ CCI_REG32(0xe294), 0x1a70184 },
	{ CCI_REG32(0xe298), 0x18c016e },
	{ CCI_REG32(0xe29c), 0x1810164 },
	{ CCI_REG32(0xe2a0), 0x1810164 },
	{ CCI_REG32(0xe2a4), 0x1900170 },
	{ CCI_REG32(0xe2a8), 0x1ad0188 },
	{ CCI_REG32(0xe2ac), 0x1d601a9 },
	{ CCI_REG32(0xe2b0), 0x21201da },
	{ CCI_REG32(0xe2b4), 0x2450207 },
	{ CCI_REG32(0xe2b8), 0x29a0254 },
	{ CCI_REG32(0xe2bc), 0x2ea029d },
	{ CCI_REG32(0xe2c0), 0x2f602ae },
	{ CCI_REG32(0xe2c4), 0x2d80291 },
	{ CCI_REG32(0xe2c8), 0x280023f },
	{ CCI_REG32(0xe2cc), 0x2390200 },
	{ CCI_REG32(0xe2d0), 0x1fe01cc },
	{ CCI_REG32(0xe2d4), 0x1d201a4 },
	{ CCI_REG32(0xe2d8), 0x1c6019b },
	{ CCI_REG32(0xe2dc), 0x1c6019b },
	{ CCI_REG32(0xe2e0), 0x1c6019b },
	{ CCI_REG32(0xe2e4), 0x1c8019b },
	{ CCI_REG32(0xe2e8), 0x1d701a9 },
	{ CCI_REG32(0xe2ec), 0x20801d1 },
	{ CCI_REG32(0xe2f0), 0x2450206 },
	{ CCI_REG32(0xe2f4), 0x28e0248 },
	{ CCI_REG32(0xe2f8), 0x2ec029d },
	{ CCI_REG32(0xe2fc), 0x30902b9 },
	{ CCI_REG32(0xe300), 0x2a002a4 },
	{ CCI_REG32(0xe304), 0x2830286 },
	{ CCI_REG32(0xe308), 0x2330234 },
	{ CCI_REG32(0xe30c), 0x1f601f7 },
	{ CCI_REG32(0xe310), 0x1c301c4 },
	{ CCI_REG32(0xe314), 0x19c019c },
	{ CCI_REG32(0xe318), 0x1920193 },
	{ CCI_REG32(0xe31c), 0x1920193 },
	{ CCI_REG32(0xe320), 0x1920192 },
	{ CCI_REG32(0xe324), 0x1920193 },
	{ CCI_REG32(0xe328), 0x19f01a1 },
	{ CCI_REG32(0xe32c), 0x1c801ca },
	{ CCI_REG32(0xe330), 0x1fb01fe },
	{ CCI_REG32(0xe334), 0x239023e },
	{ CCI_REG32(0xe338), 0x28a0292 },
	{ CCI_REG32(0xe33c), 0x2a802b0 },
	{ CCI_REG32(0xe340), 0x2830287 },
	{ CCI_REG32(0xe344), 0x2400242 },
	{ CCI_REG32(0xe348), 0x1f601f8 },
	{ CCI_REG32(0xe34c), 0x1cd01ce },
	{ CCI_REG32(0xe350), 0x19c019d },
	{ CCI_REG32(0xe354), 0x17b017d },
	{ CCI_REG32(0xe358), 0x1650166 },
	{ CCI_REG32(0xe35c), 0x15c015d },
	{ CCI_REG32(0xe360), 0x15c015d },
	{ CCI_REG32(0xe364), 0x1670168 },
	{ CCI_REG32(0xe368), 0x17e0180 },
	{ CCI_REG32(0xe36c), 0x1a101a3 },
	{ CCI_REG32(0xe370), 0x1d101d3 },
	{ CCI_REG32(0xe374), 0x1fc0200 },
	{ CCI_REG32(0xe378), 0x246024c },
	{ CCI_REG32(0xe37c), 0x28a0291 },
	{ CCI_REG32(0xe380), 0x2430245 },
	{ CCI_REG32(0xe384), 0x20e0211 },
	{ CCI_REG32(0xe388), 0x1ce01d0 },
	{ CCI_REG32(0xe38c), 0x19c019e },
	{ CCI_REG32(0xe390), 0x17b017c },
	{ CCI_REG32(0xe394), 0x1500152 },
	{ CCI_REG32(0xe398), 0x13a013c },
	{ CCI_REG32(0xe39c), 0x1310134 },
	{ CCI_REG32(0xe3a0), 0x1310134 },
	{ CCI_REG32(0xe3a4), 0x13c013f },
	{ CCI_REG32(0xe3a8), 0x1540156 },
	{ CCI_REG32(0xe3ac), 0x17e0180 },
	{ CCI_REG32(0xe3b0), 0x1a101a4 },
	{ CCI_REG32(0xe3b4), 0x1d301d8 },
	{ CCI_REG32(0xe3b8), 0x2140219 },
	{ CCI_REG32(0xe3bc), 0x249024e },
	{ CCI_REG32(0xe3c0), 0x229022b },
	{ CCI_REG32(0xe3c4), 0x1ed01ef },
	{ CCI_REG32(0xe3c8), 0x1b001b2 },
	{ CCI_REG32(0xe3cc), 0x17c017e },
	{ CCI_REG32(0xe3d0), 0x1500151 },
	{ CCI_REG32(0xe3d4), 0x13a013c },
	{ CCI_REG32(0xe3d8), 0x11c011f },
	{ CCI_REG32(0xe3dc), 0x1130117 },
	{ CCI_REG32(0xe3e0), 0x1130117 },
	{ CCI_REG32(0xe3e4), 0x1200123 },
	{ CCI_REG32(0xe3e8), 0x13c013f },
	{ CCI_REG32(0xe3ec), 0x1540156 },
	{ CCI_REG32(0xe3f0), 0x1810183 },
	{ CCI_REG32(0xe3f4), 0x1b601ba },
	{ CCI_REG32(0xe3f8), 0x1f301f6 },
	{ CCI_REG32(0xe3fc), 0x2300234 },
	{ CCI_REG32(0xe400), 0x21d0221 },
	{ CCI_REG32(0xe404), 0x1d801db },
	{ CCI_REG32(0xe408), 0x19d019f },
	{ CCI_REG32(0xe40c), 0x1670169 },
	{ CCI_REG32(0xe410), 0x13b013d },
	{ CCI_REG32(0xe414), 0x11c011f },
	{ CCI_REG32(0xe418), 0x1130117 },
	{ CCI_REG32(0xe41c), 0x1010106 },
	{ CCI_REG32(0xe420), 0x1020108 },
	{ CCI_REG32(0xe424), 0x1130117 },
	{ CCI_REG32(0xe428), 0x1200123 },
	{ CCI_REG32(0xe42c), 0x13f0142 },
	{ CCI_REG32(0xe430), 0x16c016f },
	{ CCI_REG32(0xe434), 0x1a301a6 },
	{ CCI_REG32(0xe438), 0x1df01e2 },
	{ CCI_REG32(0xe43c), 0x2240228 },
	{ CCI_REG32(0xe440), 0x21d0220 },
	{ CCI_REG32(0xe444), 0x1d001d3 },
	{ CCI_REG32(0xe448), 0x1940196 },
	{ CCI_REG32(0xe44c), 0x15d0160 },
	{ CCI_REG32(0xe450), 0x1320135 },
	{ CCI_REG32(0xe454), 0x1140118 },
	{ CCI_REG32(0xe458), 0x1010106 },
	{ CCI_REG32(0xe45c), 0x1000106 },
	{ CCI_REG32(0xe460), 0x1000106 },
	{ CCI_REG32(0xe464), 0x1030109 },
	{ CCI_REG32(0xe468), 0x118011b },
	{ CCI_REG32(0xe46c), 0x136013a },
	{ CCI_REG32(0xe470), 0x1630165 },
	{ CCI_REG32(0xe474), 0x19a019c },
	{ CCI_REG32(0xe478), 0x1d601d9 },
	{ CCI_REG32(0xe47c), 0x2240227 },
	{ CCI_REG32(0xe480), 0x21d0220 },
	{ CCI_REG32(0xe484), 0x1d001d3 },
	{ CCI_REG32(0xe488), 0x1940196 },
	{ CCI_REG32(0xe48c), 0x15d0160 },
	{ CCI_REG32(0xe490), 0x1320135 },
	{ CCI_REG32(0xe494), 0x1140118 },
	{ CCI_REG32(0xe498), 0x1030109 },
	{ CCI_REG32(0xe49c), 0x1000106 },
	{ CCI_REG32(0xe4a0), 0x1020108 },
	{ CCI_REG32(0xe4a4), 0x1030109 },
	{ CCI_REG32(0xe4a8), 0x118011b },
	{ CCI_REG32(0xe4ac), 0x1360139 },
	{ CCI_REG32(0xe4b0), 0x1630165 },
	{ CCI_REG32(0xe4b4), 0x19a019c },
	{ CCI_REG32(0xe4b8), 0x1d601d9 },
	{ CCI_REG32(0xe4bc), 0x2240227 },
	{ CCI_REG32(0xe4c0), 0x21f0221 },
	{ CCI_REG32(0xe4c4), 0x1db01de },
	{ CCI_REG32(0xe4c8), 0x19f01a2 },
	{ CCI_REG32(0xe4cc), 0x169016c },
	{ CCI_REG32(0xe4d0), 0x13e0141 },
	{ CCI_REG32(0xe4d4), 0x1200124 },
	{ CCI_REG32(0xe4d8), 0x1140119 },
	{ CCI_REG32(0xe4dc), 0x1030109 },
	{ CCI_REG32(0xe4e0), 0x1030109 },
	{ CCI_REG32(0xe4e4), 0x117011c },
	{ CCI_REG32(0xe4e8), 0x1230126 },
	{ CCI_REG32(0xe4ec), 0x1420145 },
	{ CCI_REG32(0xe4f0), 0x16f0171 },
	{ CCI_REG32(0xe4f4), 0x1a601a8 },
	{ CCI_REG32(0xe4f8), 0x1e201e4 },
	{ CCI_REG32(0xe4fc), 0x2250227 },
	{ CCI_REG32(0xe500), 0x22d0231 },
	{ CCI_REG32(0xe504), 0x1f101f4 },
	{ CCI_REG32(0xe508), 0x1b401b7 },
	{ CCI_REG32(0xe50c), 0x1810183 },
	{ CCI_REG32(0xe510), 0x1560159 },
	{ CCI_REG32(0xe514), 0x13e0141 },
	{ CCI_REG32(0xe518), 0x1200124 },
	{ CCI_REG32(0xe51c), 0x118011c },
	{ CCI_REG32(0xe520), 0x118011c },
	{ CCI_REG32(0xe524), 0x1230126 },
	{ CCI_REG32(0xe528), 0x1420145 },
	{ CCI_REG32(0xe52c), 0x15a015c },
	{ CCI_REG32(0xe530), 0x1870188 },
	{ CCI_REG32(0xe534), 0x1bb01bd },
	{ CCI_REG32(0xe538), 0x1f801fb },
	{ CCI_REG32(0xe53c), 0x2330236 },
	{ CCI_REG32(0xe540), 0x24c0250 },
	{ CCI_REG32(0xe544), 0x2160219 },
	{ CCI_REG32(0xe548), 0x1d401d7 },
	{ CCI_REG32(0xe54c), 0x1a401a6 },
	{ CCI_REG32(0xe550), 0x1810183 },
	{ CCI_REG32(0xe554), 0x1560158 },
	{ CCI_REG32(0xe558), 0x1410144 },
	{ CCI_REG32(0xe55c), 0x138013b },
	{ CCI_REG32(0xe560), 0x138013b },
	{ CCI_REG32(0xe564), 0x1430146 },
	{ CCI_REG32(0xe568), 0x15a015c },
	{ CCI_REG32(0xe56c), 0x1870188 },
	{ CCI_REG32(0xe570), 0x1a901ab },
	{ CCI_REG32(0xe574), 0x1db01dd },
	{ CCI_REG32(0xe578), 0x21d0221 },
	{ CCI_REG32(0xe57c), 0x2540259 },
	{ CCI_REG32(0xe580), 0x2910296 },
	{ CCI_REG32(0xe584), 0x24c0251 },
	{ CCI_REG32(0xe588), 0x2010204 },
	{ CCI_REG32(0xe58c), 0x1d401d6 },
	{ CCI_REG32(0xe590), 0x1a401a5 },
	{ CCI_REG32(0xe594), 0x1840186 },
	{ CCI_REG32(0xe598), 0x16e0170 },
	{ CCI_REG32(0xe59c), 0x1640167 },
	{ CCI_REG32(0xe5a0), 0x1640167 },
	{ CCI_REG32(0xe5a4), 0x1700173 },
	{ CCI_REG32(0xe5a8), 0x188018a },
	{ CCI_REG32(0xe5ac), 0x1a901ab },
	{ CCI_REG32(0xe5b0), 0x1da01dd },
	{ CCI_REG32(0xe5b4), 0x207020a },
	{ CCI_REG32(0xe5b8), 0x2540259 },
	{ CCI_REG32(0xe5bc), 0x29d02a3 },
	{ CCI_REG32(0xe5c0), 0x2ae02b4 },
	{ CCI_REG32(0xe5c4), 0x2910297 },
	{ CCI_REG32(0xe5c8), 0x23f0243 },
	{ CCI_REG32(0xe5cc), 0x2000201 },
	{ CCI_REG32(0xe5d0), 0x1cc01cd },
	{ CCI_REG32(0xe5d4), 0x1a401a6 },
	{ CCI_REG32(0xe5d8), 0x19b019d },
	{ CCI_REG32(0xe5dc), 0x19b019d },
	{ CCI_REG32(0xe5e0), 0x19b019d },
	{ CCI_REG32(0xe5e4), 0x19b019e },
	{ CCI_REG32(0xe5e8), 0x1a901ab },
	{ CCI_REG32(0xe5ec), 0x1d101d3 },
	{ CCI_REG32(0xe5f0), 0x2060209 },
	{ CCI_REG32(0xe5f4), 0x248024b },
	{ CCI_REG32(0xe5f8), 0x29d02a3 },
	{ CCI_REG32(0xe5fc), 0x2b902c0 },
	{ CCI_REG8(0xd822), 0x01 },
	{ CCI_REG8(0xd823), 0x0f },
};

/* Mode configs */
static const struct imx500_mode imx500_supported_modes[] = {
	{
		/* 12MPix 10fps mode */
		.width = 4056,
		.height = 3040,
		.line_length_pix = 17900,
		.crop = {
			.left = IMX500_PIXEL_ARRAY_LEFT,
			.top = IMX500_PIXEL_ARRAY_TOP,
			.width = 4056,
			.height = 3040,
		},
		.reg_list = {
			.num_of_regs = ARRAY_SIZE(mode_4056x3040_regs),
			.regs = mode_4056x3040_regs,
		},
	},
	{
		/* 2x2 binned 40fps mode */
		.width = 2028,
		.height = 1520,
		.line_length_pix = 9398,
		.crop = {
			.left = IMX500_PIXEL_ARRAY_LEFT,
			.top = IMX500_PIXEL_ARRAY_TOP,
			.width = 4056,
			.height = 3040,
		},
		.reg_list = {
			.num_of_regs = ARRAY_SIZE(mode_2028x1520_regs),
			.regs = mode_2028x1520_regs,
		},
	},
};

/*
 * The supported formats.
 * This table MUST contain 4 entries per format, to cover the various flip
 * combinations in the order
 * - no flip
 * - h flip
 * - v flip
 * - h&v flips
 */
static const u32 codes[] = {
	/* 10-bit modes. */
	MEDIA_BUS_FMT_SRGGB10_1X10,
	MEDIA_BUS_FMT_SGRBG10_1X10,
	MEDIA_BUS_FMT_SGBRG10_1X10,
	MEDIA_BUS_FMT_SBGGR10_1X10,
};

enum imx500_state {
	IMX500_STATE_RESET = 0,
	IMX500_STATE_PROGRAM_EMPTY,
	IMX500_STATE_WITHOUT_NETWORK,
	IMX500_STATE_WITH_NETWORK,
};

struct imx500 {
	struct dentry *debugfs;
	struct v4l2_subdev sd;
	struct media_pad pad[NUM_PADS];
	struct regmap *regmap;

	unsigned int fmt_code;

	struct clk *xclk;
	u32 xclk_freq;

	struct gpio_desc *led_gpio;
	struct gpio_desc *reset_gpio;
	struct regulator_bulk_data supplies[IMX500_NUM_SUPPLIES];

	struct v4l2_ctrl_handler ctrl_handler;
	/* V4L2 Controls */
	struct v4l2_ctrl *pixel_rate;
	struct v4l2_ctrl *link_freq;
	struct v4l2_ctrl *exposure;
	struct v4l2_ctrl *vflip;
	struct v4l2_ctrl *hflip;
	struct v4l2_ctrl *vblank;
	struct v4l2_ctrl *hblank;
	struct v4l2_ctrl *network_fw_ctrl;
	struct v4l2_ctrl *device_id;

	struct v4l2_rect inference_window;

	/* Current mode */
	const struct imx500_mode *mode;

	/*
	 * Mutex for serialized access:
	 * Protect sensor module set pad format and start/stop streaming safely.
	 */
	struct mutex mutex;

	/* Streaming on/off */
	bool streaming;

	/* Rewrite common registers on stream on? */
	bool common_regs_written;

	bool loader_and_main_written;
	bool network_written;

	/* Current long exposure factor in use. Set through V4L2_CID_VBLANK */
	unsigned int long_exp_shift;

	struct spi_device *spi_device;

	const struct firmware *fw_loader;
	const struct firmware *fw_main;
	const u8 *fw_network;
	size_t fw_network_size;
	size_t fw_progress;
	unsigned int fw_stage;

	enum imx500_state fsm_state;

	u32 num_inference_lines;
};

static inline struct imx500 *to_imx500(struct v4l2_subdev *_sd)
{
	return container_of(_sd, struct imx500, sd);
}

static bool validate_normalization_yuv(u16 reg, uint8_t size,
				       uint32_t value)
{
	/* Some regs are 9-bit, some 8-bit, some 1-bit */
	switch (reg) {
	case 0xD62A:
	case 0xD632:
	case 0xD63A:
	case 0xD644:
	case 0xD648:
	case 0xD64C:
	case 0xD650:
	case 0xD654:
	case 0xD658:
		return size == 2 && !(value & ~0x1FF);
	case 0xD600:
	case 0xD601:
	case 0xD602:
		return size == 1 && !(value & ~0xFF);
	case 0xD629:
	case 0xD630:
	case 0xD638:
	case 0xD643:
	case 0xD647:
	case 0xD64B:
	case 0xD64F:
	case 0xD653:
	case 0xD657:
		return size == 1 && !(value & ~0x01);
	default:
		return false;
	}
}

/* Common function as bayer rgb + normalization use the same repeating register
 * layout
 */
static bool validate_bit_pattern(u8 offset, uint8_t size, uint32_t value)
{
	/* There are no odd register addresses */
	if (offset & 1)
		return false;

	/* Valid register sizes/patterns repeat every 4 */
	offset = (offset >> 1) & 3;

	if (offset == 1)
		return size == 1 && !(value & ~1);
	else
		return size == 2 && !(value & ~0x1FF);
}

static bool validate_bayer_rgb_normalization(u16 reg, uint8_t size,
					     uint32_t value)
{
	if (reg < 0xD684 || reg >= 0xD6E4)
		return false;
	return validate_bit_pattern(reg - 0xD684, size, value);
}

static bool validate_normalization_registers(u16 reg, uint8_t size,
					     uint32_t value)
{
	if (reg < 0xD708 || reg >= 0xD750)
		return false;
	return validate_bit_pattern(reg - 0xD708, size, value);
}

static bool validate_image_format_selection(u16 reg, uint8_t size,
					    uint32_t value)
{
	if (size != 1 || value > 5)
		return false;
	if (reg < 0xD750 || reg > 0xd752)
		return false;
	return true;
}

static bool validate_yc_conversion_factor(u16 reg, uint8_t size,
					  uint32_t value)
{
	static const u32 allowed[9] = {
		0x0FFF0FFF, 0x0FFF1FFF, 0x0FFF0FFF, 0x0FFF1FFF, 0x0FFF0FFF,
		0x0FFF1FFF, 0x01FF01FF, 0x01FF01FF, 0x01FF01FF,
	};

	if (size > 4 || size & 1 || reg & 1 || reg < 0x76C || reg > 0xD7FA)
		return false;

	if (size == 2) {
		if (reg & 2)
			reg -= 2;
		else
			value <<= 16;
	}

	/* High registers (clip values) are all 2x 9-bit */
	if (reg >= 0xD7D8)
		return !(value & ~0x01FF01FF);

	/* Early registers follow a repeating pattern */
	reg -= 0xD76C;
	reg >>= 2;
	return !(value & ~allowed[reg % sizeof(allowed)]);
}

static bool validate_dnn_output_setting(u16 reg, uint8_t size,
					uint32_t value)
{
	/* Only Y_OUT_SIZE for Input Tensor / Output Tensor is configurable from
	 * userspace
	 */
	return (size == 2) && (value < 2046) &&
	       ((reg == CCI_REG_ADDR(IMX500_REG_DD_CH07_Y_OUT_SIZE)) ||
		(reg == CCI_REG_ADDR(IMX500_REG_DD_CH08_Y_OUT_SIZE)));
}

static bool __must_check
imx500_validate_inference_register(const struct cci_reg_sequence *reg)
{
	unsigned int i;

	static bool (*const checks[])(uint16_t, uint8_t, uint32_t) = {
		validate_normalization_yuv,
		validate_bayer_rgb_normalization,
		validate_normalization_registers,
		validate_image_format_selection,
		validate_yc_conversion_factor,
		validate_dnn_output_setting,
	};

	if (!reg)
		return false;

	for (i = 0; i < ARRAY_SIZE(checks); i++) {
		if (checks[i](CCI_REG_ADDR(reg->reg),
			      CCI_REG_WIDTH_BYTES(reg->reg), reg->val))
			return true;
	}

	return false;
}

static int imx500_set_inference_window(struct imx500 *imx500)
{
	u16 left, top, width, height;

	if (!imx500->inference_window.width ||
	    !imx500->inference_window.height) {
		width = 4056;
		height = 3040;
		left = 0;
		top = 0;
	} else {
		width = min_t(u16, imx500->inference_window.width, 4056);
		height = min_t(u16, imx500->inference_window.height, 3040);
		left = min_t(u16, imx500->inference_window.left, 4056);
		top = min_t(u16, imx500->inference_window.top, 3040);
	}

	const struct cci_reg_sequence window_regs[] = {
		{ IMX500_REG_DWP_AP_VC_HOFF, left },
		{ IMX500_REG_DWP_AP_VC_VOFF, top },
		{ IMX500_REG_DWP_AP_VC_HSIZE, width },
		{ IMX500_REG_DWP_AP_VC_VSIZE, height },
	};

	return cci_multi_reg_write(imx500->regmap, window_regs,
				   ARRAY_SIZE(window_regs), NULL);
}

static int imx500_get_device_id(struct imx500 *imx500, u32 *device_id)
{
	const u32 addr = 0xd040;
	unsigned int i;
	int ret = 0;
	u64 tmp, data;

	for (i = 0; i < 4; i++) {
		ret = cci_read(imx500->regmap, CCI_REG32(addr + i * 4), &tmp,
			       NULL);
		if (ret)
			return -EREMOTEIO;
		data = tmp & 0xffffffff;
		device_id[i] = data;
	}

	return ret;
}

static int imx500_reg_val_write_cbk(void *arg,
				    const struct cci_reg_sequence *reg)
{
	struct imx500 *imx500 = arg;

	if (!imx500_validate_inference_register(reg))
		return -EINVAL;

	return cci_write(imx500->regmap, reg->reg, reg->val, NULL);
}

/* Get bayer order based on flip setting. */
static u32 imx500_get_format_code(struct imx500 *imx500)
{
	unsigned int i;

	lockdep_assert_held(&imx500->mutex);

	i = (imx500->vflip->val ? 2 : 0) | (imx500->hflip->val ? 1 : 0);

	return codes[i];
}

static void imx500_set_default_format(struct imx500 *imx500)
{
	/* Set default mode to max resolution */
	imx500->mode = &imx500_supported_modes[0];
	imx500->fmt_code = MEDIA_BUS_FMT_SRGGB10_1X10;
}

/* -1 on fail, block size on success */
static int imx500_validate_fw_block(const char *data, size_t maxlen)
{
	const size_t header_size = 32;
	static const char header_id[] = { '9', '4', '6', '4' };

	const size_t footer_size = 64;
	static const char footer_id[] = { '3', '6', '9', '5' };

	u32 data_size;
	u32 extra_bytes_size = 0;

	const char *end = data + maxlen;

	if (!data)
		return -1;

	if (maxlen < header_size)
		return -1;

	if (memcmp(data, &header_id, sizeof(header_id)))
		return -1;

	/* data_size is size of header + body */
	memcpy(&data_size, data + sizeof(header_id), sizeof(data_size));
	data_size = ___constant_swab32(data_size);

	/* check the device_lock flag */
	extra_bytes_size = *((u8 *)(data + 0x0e)) & 0x01 ? 32 : 0;

	if (end - data_size - footer_size - extra_bytes_size < data)
		return -1;
	if (memcmp(data + data_size + footer_size - sizeof(footer_id),
		   &footer_id, sizeof(footer_id)))
		return -1;

	return data_size + footer_size + extra_bytes_size;
}

/* Parse fw block by block, returning total valid fw size */
static size_t imx500_valid_fw_bytes(const u8 *fw,
				    const size_t fw_size)
{
	int i;
	size_t bytes = 0;

	const u8 *data = fw;
	size_t size = fw_size;

	while ((i = imx500_validate_fw_block(data, size)) > 0) {
		bytes += i;
		data += i;
		size -= i;
	}

	return bytes;
}

static int imx500_iterate_nw_regs(
	const u8 *fw, size_t fw_size, void *arg,
	int (*cbk)(void *arg, const struct cci_reg_sequence *reg))
{
	struct cpio_data cd = { NULL, 0, "" };
	const u8 *read_pos;
	size_t entries;
	size_t size;

	if (!fw || !cbk)
		return -EINVAL;

	size = imx500_valid_fw_bytes(fw, fw_size);
	cd = find_cpio_data("imx500_regs", (void *)(fw + size),
			    fw_size - size, NULL);
	if (!cd.data || cd.size % 7)
		return -EINVAL;

	read_pos = cd.data;
	entries = cd.size / 7;

	while (entries--) {
		struct cci_reg_sequence reg = { 0, 0 };
		u16 addr;
		u8 len;
		u32 val;
		int ret;

		memcpy(&addr, read_pos, sizeof(addr));
		read_pos += sizeof(addr);
		memcpy(&len, read_pos, sizeof(len));
		read_pos += sizeof(len);
		memcpy(&val, read_pos, sizeof(val));
		read_pos += sizeof(val);

		reg.reg = ((len << CCI_REG_WIDTH_SHIFT) | addr);
		reg.val = val;

		ret = cbk(arg, &reg);
		if (ret)
			return ret;
	}
	return 0;
}

static int imx500_reg_tensor_lines_cbk(void *arg,
				       const struct cci_reg_sequence *reg)
{
	u16 *tensor_lines = arg;

	if (reg->val < 2046) {
		switch (reg->reg) {
		case IMX500_REG_DD_CH07_Y_OUT_SIZE:
			tensor_lines[0] = reg->val;
			break;
		case IMX500_REG_DD_CH08_Y_OUT_SIZE:
			tensor_lines[1] = reg->val;
			break;
		}
	}

	return 0;
}

static void imx500_calc_inference_lines(struct imx500 *imx500)
{
	u16 tensor_lines[2] = { 0, 0 };

	if (!imx500->fw_network) {
		imx500->num_inference_lines = 0;
		return;
	}

	imx500_iterate_nw_regs(imx500->fw_network, imx500->fw_network_size,
			       tensor_lines, imx500_reg_tensor_lines_cbk);

	/* Full-res mode, embedded lines are actually slightly shorter than inference
	 * lines 2544 vs 2560 (over-allocate with inf. width)
	 */
	imx500->num_inference_lines = IMX500_NUM_KPI_LINES +
				      IMX500_NUM_PQ_LINES + tensor_lines[0] +
				      tensor_lines[1];
}

static void imx500_adjust_exposure_range(struct imx500 *imx500)
{
	int exposure_max, exposure_def;

	/* Honour the VBLANK limits when setting exposure. */
	exposure_max = imx500->mode->height + imx500->vblank->val -
		       IMX500_EXPOSURE_OFFSET;
	exposure_def = min(exposure_max, imx500->exposure->val);
	__v4l2_ctrl_modify_range(imx500->exposure, imx500->exposure->minimum,
				 exposure_max, imx500->exposure->step,
				 exposure_def);
}

static int imx500_set_frame_length(struct imx500 *imx500, unsigned int val)
{
	int ret = 0;

	imx500->long_exp_shift = 0;

	while (val > IMX500_FRAME_LENGTH_MAX) {
		imx500->long_exp_shift++;
		val >>= 1;
	}

	ret = cci_write(imx500->regmap, IMX500_REG_FRAME_LENGTH, val, NULL);
	if (ret)
		return ret;

	ret = cci_write(imx500->regmap, IMX500_LONG_EXP_CIT_SHIFT_REG,
			imx500->long_exp_shift, NULL);
	if (ret)
		return ret;

	return cci_write(imx500->regmap, IMX500_LONG_EXP_SHIFT_REG,
			 imx500->long_exp_shift, NULL);
}

/* reg is both input and output:
 * reg->val is the value we're polling until we're NEQ to
 * It is then populated with the updated value.
 */
static int __must_check imx500_poll_status_reg(struct imx500 *state,
					       struct cci_reg_sequence *reg,
					       u8 timeout)
{
	u64 read_value;
	int ret;

	while (timeout) {
		ret = cci_read(state->regmap, reg->reg, &read_value, NULL);
		if (ret)
			return ret;

		if (read_value != reg->val) {
			reg->val = read_value;
			return 0;
		}

		timeout--;
		mdelay(50);
	}
	return -EAGAIN;
}

static int imx500_prepare_poll_cmd_reply_sts(struct imx500 *imx500,
					     struct cci_reg_sequence *cmd_reply)
{
	/* Perform single-byte read of 4-byte IMX500_REG_DD_REF_STS register to
	 * target CMD_REPLY_STS_CNT sub-register
	 */
	cmd_reply->reg = CCI_REG8(CCI_REG_ADDR(IMX500_REG_DD_REF_STS));

	return cci_read(imx500->regmap, cmd_reply->reg, &cmd_reply->val, NULL);
}

static int imx500_clear_weights(struct imx500 *imx500)
{
	struct cci_reg_sequence cmd_reply_sts_cnt_reg;
	u64 imx500_fsm_state;
	u64 cmd_reply;
	int ret;

	static const struct cci_reg_sequence request_clear[] = {
		{ IMX500_REG_DD_ST_TRANS_CMD,
		  IMX500_DD_ST_TRANS_CMD_CLEAR_WEIGHTS },
		{ IMX500_REG_DD_CMD_INT, IMX500_DD_CMD_INT_ST_TRANS },
	};

	if (imx500->fsm_state != IMX500_STATE_WITH_NETWORK)
		return -EINVAL;

	ret = cci_read(imx500->regmap, IMX500_REG_DD_SYS_STATE,
		       &imx500_fsm_state, NULL);
	if (ret || imx500_fsm_state != IMX500_DD_SYS_STATE_STANDBY_WITH_NETWORK)
		return ret ? ret : -EREMOTEIO;

	ret = imx500_prepare_poll_cmd_reply_sts(imx500, &cmd_reply_sts_cnt_reg);
	if (ret)
		return ret;

	ret = cci_multi_reg_write(imx500->regmap, request_clear,
				  ARRAY_SIZE(request_clear), NULL);
	if (ret)
		return ret;

	ret = imx500_poll_status_reg(imx500, &cmd_reply_sts_cnt_reg, 5);
	if (ret)
		return ret;

	ret = cci_read(imx500->regmap, IMX500_REG_DD_CMD_REPLY_STS, &cmd_reply,
		       NULL);
	if (ret || cmd_reply != IMX500_DD_CMD_REPLY_STS_TRANS_DONE)
		return ret ? ret : -EREMOTEIO;

	imx500->fsm_state = IMX500_STATE_WITHOUT_NETWORK;
	imx500->network_written = false;
	return 0;
}

static void imx500_clear_fw_network(struct imx500 *imx500)
{
	/* Remove any previous firmware blob. */
	if (imx500->fw_network)
		vfree(imx500->fw_network);

	imx500->fw_network = NULL;
	imx500->network_written = false;
	imx500->fw_progress = 0;
	v4l2_ctrl_activate(imx500->device_id, false);
}

static int imx500_set_ctrl(struct v4l2_ctrl *ctrl)
{
	struct imx500 *imx500 =
		container_of(ctrl->handler, struct imx500, ctrl_handler);
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	int ret = 0;

	if (ctrl->id == V4L2_CID_USER_IMX500_NETWORK_FW_FD) {
		/* Reset state of the control. */
		if (ctrl->val < 0) {
			return 0;
		} else if (ctrl->val == S32_MAX) {
			ctrl->val = -1;
			if (pm_runtime_get_if_in_use(&client->dev) == 0)
				return 0;

			if (imx500->network_written)
				ret = imx500_clear_weights(imx500);
			imx500_clear_fw_network(imx500);

			pm_runtime_mark_last_busy(&client->dev);
			pm_runtime_put_autosuspend(&client->dev);

			return ret;
		}

		imx500_clear_fw_network(imx500);
		ret = kernel_read_file_from_fd(ctrl->val, 0,
					       (void **)&imx500->fw_network, INT_MAX,
					       &imx500->fw_network_size,
					       1);
		/*
		 * Back to reset state, the FD cannot be considered valid after
		 * this IOCTL completes.
		 */
		ctrl->val = -1;

		if (ret < 0) {
			dev_err(&client->dev, "%s failed to read fw image: %d\n",
				__func__, ret);
			imx500_clear_fw_network(imx500);
			return ret;
		}
		if (ret != imx500->fw_network_size) {
			dev_err(&client->dev, "%s read fw image size mismatich: got %u, expected %zu\n",
				__func__, ret, imx500->fw_network_size);
			imx500_clear_fw_network(imx500);
			return -EIO;
		}

		imx500_calc_inference_lines(imx500);
		return 0;
	}

	/*
	 * The VBLANK control may change the limits of usable exposure, so check
	 * and adjust if necessary.
	 */
	if (ctrl->id == V4L2_CID_VBLANK)
		imx500_adjust_exposure_range(imx500);

	/*
	 * Applying V4L2 control value only happens
	 * when power is up for streaming
	 */
	if (pm_runtime_get_if_in_use(&client->dev) == 0)
		return 0;

	switch (ctrl->id) {
	case V4L2_CID_ANALOGUE_GAIN:
		ret = cci_write(imx500->regmap, IMX500_REG_ANALOG_GAIN,
				ctrl->val, NULL);
		break;
	case V4L2_CID_EXPOSURE:
		ret = cci_write(imx500->regmap, IMX500_REG_EXPOSURE,
				ctrl->val >> imx500->long_exp_shift, NULL);
		break;
	case V4L2_CID_HFLIP:
	case V4L2_CID_VFLIP:
		ret = cci_write(imx500->regmap, IMX500_REG_ORIENTATION,
				imx500->hflip->val | imx500->vflip->val << 1,
				NULL);
		break;
	case V4L2_CID_VBLANK:
		ret = imx500_set_frame_length(imx500,
					      imx500->mode->height + ctrl->val);
		break;
	case V4L2_CID_HBLANK:
		ret = cci_write(imx500->regmap, IMX500_REG_LINE_LENGTH,
				imx500->mode->width + ctrl->val, NULL);
		break;
	case V4L2_CID_NOTIFY_GAINS:
		ret = cci_write(imx500->regmap, IMX500_REG_COLOUR_BALANCE_B,
				ctrl->p_new.p_u32[0], NULL);
		cci_write(imx500->regmap, IMX500_REG_COLOUR_BALANCE_GB,
			  ctrl->p_new.p_u32[1], &ret);
		cci_write(imx500->regmap, IMX500_REG_COLOUR_BALANCE_GR,
			  ctrl->p_new.p_u32[2], &ret);
		cci_write(imx500->regmap, IMX500_REG_COLOUR_BALANCE_R,
			  ctrl->p_new.p_u32[3], &ret);
		break;
	case V4L2_CID_USER_IMX500_INFERENCE_WINDOW:
		memcpy(&imx500->inference_window, ctrl->p_new.p_u32,
		       sizeof(struct v4l2_rect));
		ret = imx500_set_inference_window(imx500);
		break;
	default:
		dev_info(&client->dev,
			 "ctrl(id:0x%x,val:0x%x) is not handled\n", ctrl->id,
			 ctrl->val);
		ret = -EINVAL;
		break;
	}

	pm_runtime_mark_last_busy(&client->dev);
	pm_runtime_put_autosuspend(&client->dev);

	return ret;
}

static int imx500_get_ctrl(struct v4l2_ctrl *ctrl)
{
	struct imx500 *imx500 = container_of(ctrl->handler, struct imx500,
					     ctrl_handler);
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	u32 device_id[4] = {0};
	int ret;

	switch (ctrl->id) {
	case V4L2_CID_USER_GET_IMX500_DEVICE_ID:
		ret = imx500_get_device_id(imx500, device_id);
		memcpy(ctrl->p_new.p_u32, device_id, sizeof(device_id));
		break;
	default:
		dev_info(&client->dev, "ctrl(id:0x%x,val:0x%x) is not handled\n",
			 ctrl->id, ctrl->val);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static const struct v4l2_ctrl_ops imx500_ctrl_ops = {
	.g_volatile_ctrl = imx500_get_ctrl,
	.s_ctrl = imx500_set_ctrl,
};

static const struct v4l2_ctrl_config imx500_notify_gains_ctrl = {
	.ops = &imx500_ctrl_ops,
	.id = V4L2_CID_NOTIFY_GAINS,
	.type = V4L2_CTRL_TYPE_U32,
	.min = IMX500_COLOUR_BALANCE_MIN,
	.max = IMX500_COLOUR_BALANCE_MAX,
	.step = IMX500_COLOUR_BALANCE_STEP,
	.def = IMX500_COLOUR_BALANCE_DEFAULT,
	.dims = { 4 },
	.elem_size = sizeof(u32),
};

static int imx500_enum_mbus_code(struct v4l2_subdev *sd,
				 struct v4l2_subdev_state *sd_state,
				 struct v4l2_subdev_mbus_code_enum *code)
{
	struct imx500 *imx500 = to_imx500(sd);

	if (code->pad >= NUM_PADS)
		return -EINVAL;

	if (code->pad == IMAGE_PAD) {
		if (code->index != 0)
			return -EINVAL;

		code->code = imx500_get_format_code(imx500);
	} else {
		if (code->index > 0)
			return -EINVAL;

		code->code = MEDIA_BUS_FMT_SENSOR_DATA;
	}

	return 0;
}

static int imx500_enum_frame_size(struct v4l2_subdev *sd,
				  struct v4l2_subdev_state *sd_state,
				  struct v4l2_subdev_frame_size_enum *fse)
{
	struct imx500 *imx500 = to_imx500(sd);

	if (fse->pad >= NUM_PADS)
		return -EINVAL;

	if (fse->pad == IMAGE_PAD) {
		const struct imx500_mode *mode_list = imx500_supported_modes;
		unsigned int num_modes = ARRAY_SIZE(imx500_supported_modes);

		if (fse->index >= num_modes)
			return -EINVAL;

		if (fse->code != imx500_get_format_code(imx500))
			return -EINVAL;

		fse->min_width = mode_list[fse->index].width;
		fse->max_width = fse->min_width;
		fse->min_height = mode_list[fse->index].height;
		fse->max_height = fse->min_height;
	} else {
		if (fse->code != MEDIA_BUS_FMT_SENSOR_DATA || fse->index > 0)
			return -EINVAL;

		fse->min_width = IMX500_MAX_EMBEDDED_SIZE +
				 imx500->num_inference_lines *
					 IMX500_INFERENCE_LINE_WIDTH;
		fse->max_width = fse->min_width;
		fse->min_height = 1;
		fse->max_height = fse->min_height;
	}

	return 0;
}

static void imx500_update_image_pad_format(struct imx500 *imx500,
					   const struct imx500_mode *mode,
					   struct v4l2_subdev_format *fmt)
{
	fmt->format.width = mode->width;
	fmt->format.height = mode->height;
	fmt->format.field = V4L2_FIELD_NONE;
	fmt->format.colorspace = V4L2_COLORSPACE_RAW;
	fmt->format.ycbcr_enc =
		V4L2_MAP_YCBCR_ENC_DEFAULT(fmt->format.colorspace);
	fmt->format.quantization = V4L2_MAP_QUANTIZATION_DEFAULT(
		true, fmt->format.colorspace, fmt->format.ycbcr_enc);
	fmt->format.xfer_func =
		V4L2_MAP_XFER_FUNC_DEFAULT(fmt->format.colorspace);
}

static void imx500_update_metadata_pad_format(const struct imx500 *imx500,
					      struct v4l2_subdev_format *fmt)
{
	fmt->format.width =
		IMX500_MAX_EMBEDDED_SIZE +
		imx500->num_inference_lines * IMX500_INFERENCE_LINE_WIDTH;
	fmt->format.height = 1;
	fmt->format.code = MEDIA_BUS_FMT_SENSOR_DATA;
	fmt->format.field = V4L2_FIELD_NONE;
}

static int imx500_get_pad_format(struct v4l2_subdev *sd,
				 struct v4l2_subdev_state *sd_state,
				 struct v4l2_subdev_format *fmt)
{
	struct imx500 *imx500 = to_imx500(sd);

	if (fmt->pad >= NUM_PADS)
		return -EINVAL;

	mutex_lock(&imx500->mutex);

	if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
		struct v4l2_mbus_framefmt *try_fmt = v4l2_subdev_state_get_format(
			sd_state, fmt->pad);
		/* update the code which could change due to vflip or hflip */
		try_fmt->code = fmt->pad == IMAGE_PAD ?
					imx500_get_format_code(imx500) :
					MEDIA_BUS_FMT_SENSOR_DATA;
		fmt->format = *try_fmt;
	} else {
		if (fmt->pad == IMAGE_PAD) {
			imx500_update_image_pad_format(imx500, imx500->mode,
						       fmt);
			fmt->format.code = imx500_get_format_code(imx500);
		} else {
			imx500_update_metadata_pad_format(imx500, fmt);
		}
	}

	mutex_unlock(&imx500->mutex);
	return 0;
}

static void imx500_set_framing_limits(struct imx500 *imx500)
{
	unsigned int hblank_min;
	const struct imx500_mode *mode = imx500->mode;

	/* Default to no long exposure multiplier. */
	imx500->long_exp_shift = 0;

	/* Update limits and set FPS to default */
	__v4l2_ctrl_modify_range(
		imx500->vblank, IMX500_VBLANK_MIN,
		((1 << IMX500_LONG_EXP_SHIFT_MAX) * IMX500_FRAME_LENGTH_MAX) -
			mode->height, 1, IMX500_VBLANK_MIN);

	/* Setting this will adjust the exposure limits as well. */
	__v4l2_ctrl_s_ctrl(imx500->vblank, IMX500_VBLANK_MIN);

	hblank_min = mode->line_length_pix - mode->width;
	__v4l2_ctrl_modify_range(imx500->hblank, hblank_min, hblank_min, 1,
				 hblank_min);
	__v4l2_ctrl_s_ctrl(imx500->hblank, hblank_min);
}

static int imx500_set_pad_format(struct v4l2_subdev *sd,
				 struct v4l2_subdev_state *sd_state,
				 struct v4l2_subdev_format *fmt)
{
	struct v4l2_mbus_framefmt *framefmt;
	const struct imx500_mode *mode;
	struct imx500 *imx500 = to_imx500(sd);

	if (fmt->pad >= NUM_PADS)
		return -EINVAL;

	mutex_lock(&imx500->mutex);

	if (fmt->pad == IMAGE_PAD) {
		const struct imx500_mode *mode_list = imx500_supported_modes;
		unsigned int num_modes = ARRAY_SIZE(imx500_supported_modes);

		/* Bayer order varies with flips */
		fmt->format.code = imx500_get_format_code(imx500);

		mode = v4l2_find_nearest_size(mode_list, num_modes, width,
					      height, fmt->format.width,
					      fmt->format.height);
		imx500_update_image_pad_format(imx500, mode, fmt);
		if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
			framefmt = v4l2_subdev_state_get_format(sd_state, fmt->pad);
			*framefmt = fmt->format;
		} else if (imx500->mode != mode) {
			imx500->mode = mode;
			imx500->fmt_code = fmt->format.code;
			imx500_set_framing_limits(imx500);
		}
	} else {
		if (fmt->which == V4L2_SUBDEV_FORMAT_TRY) {
			framefmt = v4l2_subdev_state_get_format(sd_state, fmt->pad);
			*framefmt = fmt->format;
		} else {
			/* Only one embedded data mode is supported */
			imx500_update_metadata_pad_format(imx500, fmt);
		}
	}

	mutex_unlock(&imx500->mutex);

	return 0;
}

static const struct v4l2_rect *
__imx500_get_pad_crop(struct imx500 *imx500, struct v4l2_subdev_state *sd_state,
		      unsigned int pad, enum v4l2_subdev_format_whence which)
{
	switch (which) {
	case V4L2_SUBDEV_FORMAT_TRY:
		return v4l2_subdev_state_get_crop(sd_state, pad);
	case V4L2_SUBDEV_FORMAT_ACTIVE:
		return &imx500->mode->crop;
	}

	return NULL;
}

static int imx500_get_selection(struct v4l2_subdev *sd,
				struct v4l2_subdev_state *sd_state,
				struct v4l2_subdev_selection *sel)
{
	switch (sel->target) {
	case V4L2_SEL_TGT_CROP: {
		struct imx500 *imx500 = to_imx500(sd);

		mutex_lock(&imx500->mutex);
		sel->r = *__imx500_get_pad_crop(imx500, sd_state, sel->pad,
						sel->which);
		mutex_unlock(&imx500->mutex);

		return 0;
	}

	case V4L2_SEL_TGT_NATIVE_SIZE:
		sel->r.left = 0;
		sel->r.top = 0;
		sel->r.width = IMX500_NATIVE_WIDTH;
		sel->r.height = IMX500_NATIVE_HEIGHT;

		return 0;

	case V4L2_SEL_TGT_CROP_DEFAULT:
	case V4L2_SEL_TGT_CROP_BOUNDS:
		sel->r.left = IMX500_PIXEL_ARRAY_LEFT;
		sel->r.top = IMX500_PIXEL_ARRAY_TOP;
		sel->r.width = IMX500_PIXEL_ARRAY_WIDTH;
		sel->r.height = IMX500_PIXEL_ARRAY_HEIGHT;

		return 0;
	}

	return -EINVAL;
}

static int __must_check imx500_spi_write(struct imx500 *state, const u8 *data,
					 size_t size)
{
	if (size % 4 || size > ONE_MIB)
		return -EINVAL;

	if (!state->spi_device)
		return -ENODEV;

	return spi_write(state->spi_device, data, size);
}

/* Moves the IMX500 internal state machine between states or updates.
 *
 * Prerequisites: Sensor is powered on and not currently streaming
 */
static int imx500_state_transition(struct imx500 *imx500, const u8 *fw,
				   size_t fw_size, enum imx500_image_type type,
				   bool update)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	struct cci_reg_sequence cmd_reply_sts_cnt_reg;
	size_t valid_size;
	int ret;
	u64 tmp;

	if (!imx500 || !fw || type >= TYPE_MAX)
		return -EINVAL;

	if (!update && (int)type != (int)imx500->fsm_state)
		return -EINVAL;

	/* Validate firmware */
	valid_size = imx500_valid_fw_bytes(fw, fw_size);
	if (!valid_size)
		return -EINVAL;

	ret = imx500_prepare_poll_cmd_reply_sts(imx500, &cmd_reply_sts_cnt_reg);
	if (ret)
		return ret;

	struct cci_reg_sequence common_regs[] = {
		{ IMX500_REG_DD_FLASH_TYPE, 0x02 },
		{ IMX500_REG_DD_LOAD_MODE, IMX500_DD_LOAD_MODE_AP },
		{ IMX500_REG_DD_IMAGE_TYPE, type },
		{ IMX500_REG_DD_DOWNLOAD_DIV_NUM, (valid_size - 1) / ONE_MIB },
		{ IMX500_REG_DD_DOWNLOAD_FILE_SIZE, valid_size },
	};

	struct cci_reg_sequence state_transition_regs[] = {
		{ IMX500_REG_DD_ST_TRANS_CMD, type },
		{ IMX500_REG_DD_CMD_INT, IMX500_DD_CMD_INT_ST_TRANS },
	};

	struct cci_reg_sequence update_regs[] = {
		{ IMX500_REG_DD_UPDATE_CMD, IMX500_DD_UPDATE_CMD_SRAM },
		{ IMX500_REG_DD_CMD_INT, IMX500_DD_CMD_INT_UPDATE },
	};

	ret = cci_multi_reg_write(imx500->regmap, common_regs,
				  ARRAY_SIZE(common_regs), NULL);

	cci_multi_reg_write(imx500->regmap,
			    update ? update_regs : state_transition_regs, 2,
			    &ret);
	if (ret)
		return ret;

	/* Poll CMD_REPLY_STS_CNT until a response is available */
	ret = imx500_poll_status_reg(imx500, &cmd_reply_sts_cnt_reg, 5);
	if (ret) {
		dev_err(&client->dev, "DD_REF_STS register did not update\n");
		return ret;
	}

	/* Read response to state transition / update request */
	ret = cci_read(imx500->regmap, IMX500_REG_DD_CMD_REPLY_STS, &tmp, NULL);
	if (ret || tmp != (update ? IMX500_DD_CMD_REPLY_STS_UPDATE_READY :
				    IMX500_DD_CMD_REPLY_STS_TRANS_READY))
		return ret ? ret : -EBUSY;

	imx500->fw_stage = type;
	imx500->fw_progress = 0;

	for (size_t i = 0; i <= valid_size / ONE_MIB; i++) {
		const u8 *data = fw + (i * ONE_MIB);
		size_t size = valid_size - (i * ONE_MIB);
		struct cci_reg_sequence download_sts_reg = {
			IMX500_REG_DD_DOWNLOAD_STS,
			IMX500_DD_DOWNLOAD_STS_DOWNLOADING,
		};

		/* Calculate SPI xfer size avoiding 0-sized TXNs */
		size = min_t(size_t, size, ONE_MIB);
		if (!size)
			break;

		/* Poll until device is ready for download */
		ret = imx500_poll_status_reg(imx500, &download_sts_reg, 100);
		if (ret) {
			dev_err(&client->dev,
				"DD_DOWNLOAD_STS was never ready\n");
			return ret;
		}

		/* Do SPI transfer */
		if (imx500->led_gpio)
			gpiod_set_value_cansleep(imx500->led_gpio, 1);
		ret = imx500_spi_write(imx500, data, size);
		if (imx500->led_gpio)
			gpiod_set_value_cansleep(imx500->led_gpio, 0);

		imx500->fw_progress += size;

		if (ret < 0)
			return ret;
	}

	/* Poll until another response is available */
	ret = imx500_poll_status_reg(imx500, &cmd_reply_sts_cnt_reg, 5);
	if (ret) {
		dev_err(&client->dev,
			"DD_REF_STS register did not update after SPI write(s)\n");
		return ret;
	}

	/* Verify that state transition / update completed successfully */
	ret = cci_read(imx500->regmap, IMX500_REG_DD_CMD_REPLY_STS, &tmp, NULL);
	if (ret || tmp != (update ? IMX500_DD_CMD_REPLY_STS_UPDATE_DONE :
				    IMX500_DD_CMD_REPLY_STS_TRANS_DONE))
		return ret ? ret : -EREMOTEIO;

	if (!update && imx500->fsm_state < IMX500_STATE_WITH_NETWORK)
		imx500->fsm_state++;

	imx500->fw_progress = fw_size;

	return 0;
}

static int imx500_transition_to_standby_wo_network(struct imx500 *imx500)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	const struct firmware *firmware;
	u64 fw_ver;
	int ret;

	firmware = imx500->fw_loader;
	ret = imx500_state_transition(imx500, firmware->data, firmware->size,
				      TYPE_LOADER, false);
	if (ret) {
		dev_err(&client->dev, "%s: failed to load loader firmware\n",
			__func__);
		return ret;
	}

	firmware = imx500->fw_main;
	ret = imx500_state_transition(imx500, firmware->data, firmware->size,
				      TYPE_MAIN, false);
	if (ret) {
		dev_err(&client->dev, "%s: failed to load main firmware\n",
			__func__);
		return ret;
	}

	ret = cci_read(imx500->regmap, IMX500_REG_MAIN_FW_VERSION, &fw_ver,
		       NULL);
	if (ret) {
		dev_err(&client->dev,
			"%s: could not read main firmware version\n", __func__);
		return ret;
	}

	dev_info(&client->dev,
		 "main firmware version: %llu%llu.%llu%llu.%llu%llu\n",
		 (fw_ver >> 20) & 0xF, (fw_ver >> 16) & 0xF,
		 (fw_ver >> 12) & 0xF, (fw_ver >> 8) & 0xF, (fw_ver >> 4) & 0xF,
		 fw_ver & 0xF);

	ret = cci_multi_reg_write(imx500->regmap, metadata_output,
				  ARRAY_SIZE(metadata_output), NULL);
	if (ret) {
		dev_err(&client->dev,
			"%s: failed to configure MIPI output for DNN\n",
			__func__);
		return ret;
	}

	ret = cci_multi_reg_write(imx500->regmap, dnn_regs,
				  ARRAY_SIZE(dnn_regs), NULL);
	if (ret) {
		dev_err(&client->dev, "%s: unable to write DNN regs\n",
			__func__);
		return ret;
	}

	return 0;
}

static int imx500_transition_to_network(struct imx500 *imx500)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	u64 imx500_fsm_state;
	int ret;

	ret = imx500_iterate_nw_regs(imx500->fw_network,
				     imx500->fw_network_size, imx500,
				     imx500_reg_val_write_cbk);
	if (ret) {
		dev_err(&client->dev,
			"%s: unable to apply register writes from firmware\n",
			__func__);
		return ret;
	}

	/* Read IMX500 state to determine whether transition or update is required */
	ret = cci_read(imx500->regmap, IMX500_REG_DD_SYS_STATE,
		       &imx500_fsm_state, NULL);
	if (ret || imx500_fsm_state & 1)
		return ret ? ret : -EREMOTEIO;

	ret = imx500_state_transition(
		imx500, imx500->fw_network, imx500->fw_network_size,
		TYPE_NW_WEIGHTS,
		imx500_fsm_state == IMX500_DD_SYS_STATE_STANDBY_WITH_NETWORK);
	if (ret) {
		dev_err(&client->dev, "%s: failed to load network weights\n",
			__func__);
		return ret;
	}

	/* Select network 0 */
	ret = cci_write(imx500->regmap, CCI_REG8(0xD701), 0, NULL);
	if (ret) {
		dev_err(&client->dev, "%s: failed to select network 0\n",
			__func__);
		return ret;
	}

	return ret;
}

/* Start streaming */
static int imx500_start_streaming(struct imx500 *imx500)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	const struct imx500_reg_list *reg_list;
	int ret;

	ret = pm_runtime_resume_and_get(&client->dev);
	if (ret < 0)
		return ret;

	/*
	 * Disable the temperature sensor here - must be done else loading any
	 * firmware fails...
	 *
	 * Re-enable before stream-on below.
	 */
	cci_write(imx500->regmap, IMX500_REG_SENSOR_TEMP_CTRL, 0, &ret);

	ret = cci_write(imx500->regmap, IMX500_REG_IMAGE_ONLY_MODE,
			imx500->fw_network ? IMX500_IMAGE_ONLY_FALSE :
					     IMX500_IMAGE_ONLY_TRUE,
			NULL);
	if (ret) {
		dev_err(&client->dev, "%s failed to set image mode\n",
			__func__);
		goto err_runtime_put;
	}

	/* Acquire loader and main firmware if needed */
	if (imx500->fw_network) {
		if (!imx500->fw_loader) {
			ret = request_firmware(&imx500->fw_loader,
					       "imx500_loader.fpk",
					       &client->dev);
			if (ret) {
				dev_err(&client->dev,
					"Unable to acquire firmware loader\n");
				goto err_runtime_put;
			}
		}
		if (!imx500->fw_main) {
			ret = request_firmware(&imx500->fw_main,
					       "imx500_firmware.fpk",
					       &client->dev);
			if (ret) {
				dev_err(&client->dev,
					"Unable to acquire main firmware\n");
				goto err_runtime_put;
			}
		}
	}

	if (!imx500->common_regs_written) {
		ret = cci_multi_reg_write(imx500->regmap, mode_common_regs,
					  ARRAY_SIZE(mode_common_regs), NULL);

		if (ret) {
			dev_err(&client->dev,
				"%s failed to set common settings\n", __func__);
			goto err_runtime_put;
		}

		imx500->common_regs_written = true;
	}

	if (imx500->fw_network && !imx500->loader_and_main_written) {
		ret = imx500_transition_to_standby_wo_network(imx500);
		if (ret) {
			dev_err(&client->dev,
				"%s failed to transition from program empty state\n",
				__func__);
			goto err_runtime_put;
		}
		imx500->loader_and_main_written = true;
	}

	if (imx500->fw_network && !imx500->network_written) {
		ret = imx500_transition_to_network(imx500);
		if (ret) {
			dev_err(&client->dev,
				"%s failed to transition to network loaded\n",
				__func__);
			goto err_runtime_put;
		}
		imx500->network_written = true;
	}

	/* Enable DNN */
	if (imx500->fw_network) {
		ret = cci_write(imx500->regmap, CCI_REG8(0xD100), 4, NULL);
		if (ret) {
			dev_err(&client->dev, "%s failed to enable DNN\n",
				__func__);
			goto err_runtime_put;
		}

		v4l2_ctrl_activate(imx500->device_id, true);
	}

	/* Apply default values of current mode */
	reg_list = &imx500->mode->reg_list;
	ret = cci_multi_reg_write(imx500->regmap, reg_list->regs,
				  reg_list->num_of_regs, NULL);
	if (ret) {
		dev_err(&client->dev, "%s failed to set mode\n", __func__);
		goto err_runtime_put;
	}

	/* Apply customized values from user */
	ret = __v4l2_ctrl_handler_setup(imx500->sd.ctrl_handler);

	/* Disable any sensor startup frame drops. This must be written here! */
	cci_write(imx500->regmap, CCI_REG8(0xD405), 0, &ret);

	/* Re-enable the temperature sensor. */
	cci_write(imx500->regmap, IMX500_REG_SENSOR_TEMP_CTRL, 1, &ret);

	/* set stream on register */
	cci_write(imx500->regmap, IMX500_REG_MODE_SELECT, IMX500_MODE_STREAMING,
		  &ret);

	if (ret)
		goto err_runtime_put;

	return 0;

err_runtime_put:
	pm_runtime_mark_last_busy(&client->dev);
	pm_runtime_put_autosuspend(&client->dev);
	return ret;
}

/* Stop streaming */
static void imx500_stop_streaming(struct imx500 *imx500)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	int ret;

	/* set stream off register */
	ret = cci_write(imx500->regmap, IMX500_REG_MODE_SELECT,
			IMX500_MODE_STANDBY, NULL);
	if (ret)
		dev_err(&client->dev, "%s failed to set stream\n", __func__);

	/* Disable DNN */
	ret = cci_write(imx500->regmap, CCI_REG8(0xD100), 0, NULL);
	if (ret)
		dev_err(&client->dev, "%s failed to disable DNN\n", __func__);

	pm_runtime_mark_last_busy(&client->dev);
	pm_runtime_put_autosuspend(&client->dev);
}

static int imx500_set_stream(struct v4l2_subdev *sd, int enable)
{
	struct imx500 *imx500 = to_imx500(sd);
	int ret = 0;

	mutex_lock(&imx500->mutex);
	if (imx500->streaming == enable) {
		mutex_unlock(&imx500->mutex);
		return 0;
	}

	if (enable) {
		/*
		 * Apply default & customized values
		 * and then start streaming.
		 */
		ret = imx500_start_streaming(imx500);
		if (ret)
			goto err_start_streaming;
	} else {
		imx500_stop_streaming(imx500);
	}

	imx500->streaming = enable;

	/* vflip and hflip cannot change during streaming */
	__v4l2_ctrl_grab(imx500->vflip, enable);
	__v4l2_ctrl_grab(imx500->hflip, enable);
	__v4l2_ctrl_grab(imx500->network_fw_ctrl, enable);

	mutex_unlock(&imx500->mutex);

	return ret;

err_start_streaming:
	mutex_unlock(&imx500->mutex);

	return ret;
}

/* Power/clock management functions */
static int imx500_power_on(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx500 *imx500 = to_imx500(sd);
	int ret;

	/* Acquire GPIOs first to ensure reset is asserted before power is applied */
	imx500->led_gpio = devm_gpiod_get_optional(dev, "led", GPIOD_OUT_LOW);
	if (IS_ERR(imx500->led_gpio)) {
		ret = PTR_ERR(imx500->led_gpio);
		dev_err(&client->dev, "%s: failed to get led gpio\n", __func__);
		return ret;
	}

	imx500->reset_gpio =
		devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(imx500->reset_gpio)) {
		ret = PTR_ERR(imx500->reset_gpio);
		dev_err(&client->dev, "%s: failed to get reset gpio\n",
			__func__);
		goto gpio_led_put;
	}

	ret = regulator_bulk_enable(IMX500_NUM_SUPPLIES, imx500->supplies);
	if (ret) {
		dev_err(&client->dev, "%s: failed to enable regulators\n",
			__func__);
		goto gpio_reset_put;
	}

	/* T4 - 1us
	 * Ambiguous: Regulators rising to INCK start is specified by the datasheet
	 * but also "Presence of INCK during Power off is acceptable"
	 */
	udelay(2);

	ret = clk_prepare_enable(imx500->xclk);
	if (ret) {
		dev_err(&client->dev, "%s: failed to enable clock\n", __func__);
		goto reg_off;
	}

	/* T5 - 0ms
	 * Ambiguous: Regulators rising to XCLR rising is specified by the datasheet
	 * as 0ms but also "XCLR pin should be set to 'High' after INCK supplied.".
	 * T4 and T5 are shown as overlapping.
	 */
	if (imx500->reset_gpio)
		gpiod_set_value_cansleep(imx500->reset_gpio, 1);

	/* T7 - 9ms
	 * "INCK start and CXLR rising till Send Streaming Command wait time"
	 */
	usleep_range(9000, 12000);

	return 0;

reg_off:
	regulator_bulk_disable(IMX500_NUM_SUPPLIES, imx500->supplies);
gpio_reset_put:
	if (imx500->reset_gpio) {
		devm_gpiod_put(dev, imx500->reset_gpio);
		imx500->reset_gpio = NULL;
	}
gpio_led_put:
	if (imx500->led_gpio) {
		devm_gpiod_put(dev, imx500->led_gpio);
		imx500->led_gpio = NULL;
	}
	return ret;
}

static int imx500_power_off(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx500 *imx500 = to_imx500(sd);

	/* Datasheet specifies power down sequence as INCK disable, XCLR low,
	 * regulator disable.  T1 (XCLR neg-edge to regulator disable) is specified
	 * as 0us.
	 *
	 * Note, this is not the reverse order of power up.
	 */
	clk_disable_unprepare(imx500->xclk);
	if (imx500->reset_gpio)
		gpiod_set_value_cansleep(imx500->reset_gpio, 0);

	/* Release GPIOs before disabling regulators */
	if (imx500->reset_gpio) {
		devm_gpiod_put(&client->dev, imx500->reset_gpio);
		imx500->reset_gpio = NULL;
	}
	if (imx500->led_gpio) {
		devm_gpiod_put(&client->dev, imx500->led_gpio);
		imx500->led_gpio = NULL;
	}

	regulator_bulk_disable(IMX500_NUM_SUPPLIES, imx500->supplies);

	/* Force reprogramming of the common registers when powered up again. */
	imx500->fsm_state = IMX500_STATE_RESET;
	imx500->common_regs_written = false;
	imx500->loader_and_main_written = false;
	imx500_clear_fw_network(imx500);

	return 0;
}

static int imx500_get_regulators(struct imx500 *imx500)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	unsigned int i;

	for (i = 0; i < IMX500_NUM_SUPPLIES; i++)
		imx500->supplies[i].supply = imx500_supply_name[i];

	return devm_regulator_bulk_get(&client->dev, IMX500_NUM_SUPPLIES,
				       imx500->supplies);
}

/* Verify chip ID */
static int imx500_identify_module(struct imx500 *imx500)
{
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	int ret;
	u64 val;

	ret = cci_read(imx500->regmap, IMX500_REG_CHIP_ID, &val, NULL);
	if (ret) {
		dev_err(&client->dev,
			"failed to read chip id %x, with error %d\n",
			IMX500_CHIP_ID, ret);
		return ret;
	}

	if (val != IMX500_CHIP_ID) {
		dev_err(&client->dev, "chip id mismatch: %x!=%llx\n",
			IMX500_CHIP_ID, val);
		return -EIO;
	}

	dev_info(&client->dev, "Device found is imx%llx\n", val);

	return 0;
}

static const struct v4l2_subdev_core_ops imx500_core_ops = {
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops imx500_video_ops = {
	.s_stream = imx500_set_stream,
};

static const struct v4l2_subdev_pad_ops imx500_pad_ops = {
	.enum_mbus_code = imx500_enum_mbus_code,
	.get_fmt = imx500_get_pad_format,
	.set_fmt = imx500_set_pad_format,
	.get_selection = imx500_get_selection,
	.enum_frame_size = imx500_enum_frame_size,
};

static const struct v4l2_subdev_ops imx500_subdev_ops = {
	.core = &imx500_core_ops,
	.video = &imx500_video_ops,
	.pad = &imx500_pad_ops,
};

static const s64 imx500_link_freq_menu[] = {
	IMX500_DEFAULT_LINK_FREQ,
};

/* Custom control for inference window */
static const struct v4l2_ctrl_config inf_window_ctrl = {
	.name		= "IMX500 Inference Windows",
	.id		= V4L2_CID_USER_IMX500_INFERENCE_WINDOW,
	.dims[0]	= 4,
	.ops		= &imx500_ctrl_ops,
	.type		= V4L2_CTRL_TYPE_U32,
	.elem_size	= sizeof(u32),
	.flags		= V4L2_CTRL_FLAG_EXECUTE_ON_WRITE |
			  V4L2_CTRL_FLAG_HAS_PAYLOAD,
	.def		= 0,
	.min		= 0x00,
	.max		= 4032,
	.step		= 1,
};

/* Custom control for network firmware file FD */
static const struct v4l2_ctrl_config network_fw_fd = {
	.name		= "IMX500 Network Firmware File FD",
	.id		= V4L2_CID_USER_IMX500_NETWORK_FW_FD,
	.ops		= &imx500_ctrl_ops,
	.type		= V4L2_CTRL_TYPE_INTEGER,
	.flags		= V4L2_CTRL_FLAG_EXECUTE_ON_WRITE |
			  V4L2_CTRL_FLAG_WRITE_ONLY,
	.min		= -1,
	.max		= S32_MAX,
	.step		= 1,
	.def		= -1,
};

/* Custom control to get camera device id */
static const struct v4l2_ctrl_config cam_get_device_id = {
	.name		= "Get IMX500 Device ID",
	.id		= V4L2_CID_USER_GET_IMX500_DEVICE_ID,
	.dims[0]	= 4,
	.ops		= &imx500_ctrl_ops,
	.type		= V4L2_CTRL_TYPE_U32,
	.flags		= V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_VOLATILE |
			  V4L2_CTRL_FLAG_INACTIVE,
	.elem_size	= sizeof(u32),
	.min		= 0x00,
	.max		= U32_MAX,
	.step		= 1,
	.def		= 0,
};

/* Initialize control handlers */
static int imx500_init_controls(struct imx500 *imx500)
{
	struct v4l2_ctrl_handler *ctrl_hdlr;
	struct i2c_client *client = v4l2_get_subdevdata(&imx500->sd);
	struct v4l2_fwnode_device_properties props;
	int ret;

	ctrl_hdlr = &imx500->ctrl_handler;
	ret = v4l2_ctrl_handler_init(ctrl_hdlr, 16);
	if (ret)
		return ret;

	mutex_init(&imx500->mutex);
	ctrl_hdlr->lock = &imx500->mutex;

	/* By default, PIXEL_RATE is read only */
	imx500->pixel_rate = v4l2_ctrl_new_std(
		ctrl_hdlr, &imx500_ctrl_ops, V4L2_CID_PIXEL_RATE,
		IMX500_PIXEL_RATE, IMX500_PIXEL_RATE, 1, IMX500_PIXEL_RATE);

	/* LINK_FREQ is also read only */
	imx500->link_freq =
		v4l2_ctrl_new_int_menu(ctrl_hdlr, &imx500_ctrl_ops,
				       V4L2_CID_LINK_FREQ,
				       ARRAY_SIZE(imx500_link_freq_menu) - 1, 0,
				       imx500_link_freq_menu);
	if (imx500->link_freq)
		imx500->link_freq->flags |= V4L2_CTRL_FLAG_READ_ONLY;

	/*
	 * Create the controls here, but mode specific limits are setup
	 * in the imx500_set_framing_limits() call below.
	 */
	imx500->vblank = v4l2_ctrl_new_std(ctrl_hdlr, &imx500_ctrl_ops,
					   V4L2_CID_VBLANK, IMX500_VBLANK_MIN,
					   0xffff, 1, IMX500_VBLANK_MIN);
	imx500->hblank = v4l2_ctrl_new_std(ctrl_hdlr, &imx500_ctrl_ops,
					   V4L2_CID_HBLANK, 0, 0xffff, 1, 0);

	imx500->exposure = v4l2_ctrl_new_std(
		ctrl_hdlr, &imx500_ctrl_ops, V4L2_CID_EXPOSURE,
		IMX500_EXPOSURE_MIN, IMX500_EXPOSURE_MAX, IMX500_EXPOSURE_STEP,
		IMX500_EXPOSURE_DEFAULT);

	v4l2_ctrl_new_std(ctrl_hdlr, &imx500_ctrl_ops, V4L2_CID_ANALOGUE_GAIN,
			  IMX500_ANA_GAIN_MIN, IMX500_ANA_GAIN_MAX,
			  IMX500_ANA_GAIN_STEP, IMX500_ANA_GAIN_DEFAULT);

	imx500->hflip = v4l2_ctrl_new_std(ctrl_hdlr, &imx500_ctrl_ops,
					  V4L2_CID_HFLIP, 0, 1, 1, 0);
	if (imx500->hflip)
		imx500->hflip->flags |= V4L2_CTRL_FLAG_MODIFY_LAYOUT;

	imx500->vflip = v4l2_ctrl_new_std(ctrl_hdlr, &imx500_ctrl_ops,
					  V4L2_CID_VFLIP, 0, 1, 1, 0);
	if (imx500->vflip)
		imx500->vflip->flags |= V4L2_CTRL_FLAG_MODIFY_LAYOUT;

	v4l2_ctrl_new_custom(ctrl_hdlr, &imx500_notify_gains_ctrl, NULL);
	v4l2_ctrl_new_custom(ctrl_hdlr, &inf_window_ctrl, NULL);
	imx500->network_fw_ctrl =
		v4l2_ctrl_new_custom(ctrl_hdlr, &network_fw_fd, NULL);
	imx500->device_id =
		v4l2_ctrl_new_custom(ctrl_hdlr, &cam_get_device_id, NULL);

	if (ctrl_hdlr->error) {
		ret = ctrl_hdlr->error;
		dev_err(&client->dev, "%s control init failed (%d)\n", __func__,
			ret);
		goto error;
	}

	ret = v4l2_fwnode_device_parse(&client->dev, &props);
	if (ret)
		goto error;

	ret = v4l2_ctrl_new_fwnode_properties(ctrl_hdlr, &imx500_ctrl_ops,
					      &props);
	if (ret)
		goto error;

	imx500->sd.ctrl_handler = ctrl_hdlr;

	/* Setup exposure and frame/line length limits. */
	imx500_set_framing_limits(imx500);

	return 0;

error:
	v4l2_ctrl_handler_free(ctrl_hdlr);
	mutex_destroy(&imx500->mutex);

	return ret;
}

static void imx500_free_controls(struct imx500 *imx500)
{
	v4l2_ctrl_handler_free(imx500->sd.ctrl_handler);
	mutex_destroy(&imx500->mutex);
}

static int imx500_check_hwcfg(struct device *dev)
{
	struct fwnode_handle *endpoint;
	struct v4l2_fwnode_endpoint ep_cfg = { .bus_type =
						       V4L2_MBUS_CSI2_DPHY };
	int ret = -EINVAL;

	endpoint = fwnode_graph_get_next_endpoint(dev_fwnode(dev), NULL);
	if (!endpoint) {
		dev_err(dev, "endpoint node not found\n");
		return -EINVAL;
	}

	if (v4l2_fwnode_endpoint_alloc_parse(endpoint, &ep_cfg)) {
		dev_err(dev, "could not parse endpoint\n");
		goto error_out;
	}

	/* Check the number of MIPI CSI2 data lanes */
	if (ep_cfg.bus.mipi_csi2.num_data_lanes != 2) {
		dev_err(dev, "only 2 data lanes are currently supported\n");
		goto error_out;
	}

	/* Check the link frequency set in device tree */
	if (!ep_cfg.nr_of_link_frequencies) {
		dev_err(dev, "link-frequency property not found in DT\n");
		goto error_out;
	}

	if (ep_cfg.nr_of_link_frequencies != 1 ||
	    ep_cfg.link_frequencies[0] != IMX500_DEFAULT_LINK_FREQ) {
		dev_err(dev, "Link frequency not supported: %lld\n",
			ep_cfg.link_frequencies[0]);
		goto error_out;
	}

	ret = 0;

error_out:
	v4l2_fwnode_endpoint_free(&ep_cfg);
	fwnode_handle_put(endpoint);

	return ret;
}

static int fw_progress_show(struct seq_file *s, void *data)
{
	struct imx500 *imx500 = s->private;

	seq_printf(s, "%d %zu %zu\n", imx500->fw_stage, imx500->fw_progress,
		   imx500->fw_network_size);
	return 0;
}
DEFINE_SHOW_ATTRIBUTE(fw_progress);

static int imx500_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct spi_device *spi = NULL;
	char debugfs_name[128];
	struct imx500 *imx500;
	int ret;

	struct device_node *spi_node = of_parse_phandle(dev->of_node, "spi", 0);

	if (spi_node) {
		struct device *tmp =
			bus_find_device_by_of_node(&spi_bus_type, spi_node);
		of_node_put(spi_node);
		spi = tmp ? to_spi_device(tmp) : NULL;
		if (!spi)
			return -EPROBE_DEFER;
	}

	imx500 = devm_kzalloc(&client->dev, sizeof(*imx500), GFP_KERNEL);
	if (!imx500)
		return -ENOMEM;

	imx500->regmap = devm_cci_regmap_init_i2c(client, 16);
	if (IS_ERR(imx500->regmap))
		return dev_err_probe(dev, PTR_ERR(imx500->regmap),
				     "failed to initialise CCI\n");

	imx500->spi_device = spi;

	v4l2_i2c_subdev_init(&imx500->sd, client, &imx500_subdev_ops);

	/* Check the hardware configuration in device tree */
	if (imx500_check_hwcfg(dev))
		return -EINVAL;

	/* Get system clock (xclk) */
	imx500->xclk = devm_clk_get(dev, NULL);
	if (IS_ERR(imx500->xclk))
		return dev_err_probe(dev, PTR_ERR(imx500->xclk),
				     "failed to get xclk\n");

	imx500->xclk_freq = clk_get_rate(imx500->xclk);
	if (imx500->xclk_freq != IMX500_XCLK_FREQ) {
		dev_err(dev, "xclk frequency not supported: %d Hz\n",
			imx500->xclk_freq);
		return -EINVAL;
	}

	ret = imx500_get_regulators(imx500);
	if (ret) {
		dev_err(dev, "failed to get regulators\n");
		return ret;
	}

	/* GPIOs are acquired in imx500_power_on() to avoid preventing
	 * regulator power down when shared with other drivers.
	 */

	/*
	 * The sensor must be powered for imx500_identify_module()
	 * to be able to read the CHIP_ID register. This also ensures
	 * GPIOs are available.
	 */
	ret = imx500_power_on(dev);
	if (ret)
		return ret;

	pm_runtime_set_active(dev);
	pm_runtime_get_noresume(dev);
	pm_runtime_enable(dev);
	pm_runtime_set_autosuspend_delay(dev, 5000);
	pm_runtime_use_autosuspend(dev);

	ret = imx500_identify_module(imx500);
	if (ret)
		goto error_power_off;

	/* Initialize default format */
	imx500_set_default_format(imx500);

	/* This needs the pm runtime to be registered. */
	ret = imx500_init_controls(imx500);
	if (ret)
		goto error_power_off;

	/* Initialize subdev */
	imx500->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE |
			    V4L2_SUBDEV_FL_HAS_EVENTS;
	imx500->sd.entity.function = MEDIA_ENT_F_CAM_SENSOR;

	/* Initialize source pads */
	imx500->pad[IMAGE_PAD].flags = MEDIA_PAD_FL_SOURCE;
	imx500->pad[METADATA_PAD].flags = MEDIA_PAD_FL_SOURCE;

	ret = media_entity_pads_init(&imx500->sd.entity, NUM_PADS, imx500->pad);
	if (ret) {
		dev_err(dev, "failed to init entity pads: %d\n", ret);
		goto error_handler_free;
	}

	ret = v4l2_async_register_subdev_sensor(&imx500->sd);
	if (ret < 0) {
		dev_err(dev, "failed to register sensor sub-device: %d\n", ret);
		goto error_media_entity;
	}

	snprintf(debugfs_name, sizeof(debugfs_name), "imx500-fw:%s",
		 dev_name(dev));
	imx500->debugfs = debugfs_create_dir(debugfs_name, NULL);
	debugfs_create_file("fw_progress", 0444, imx500->debugfs, imx500,
			    &fw_progress_fops);

	pm_runtime_mark_last_busy(&client->dev);
	pm_runtime_put_autosuspend(&client->dev);

	return 0;

error_media_entity:
	media_entity_cleanup(&imx500->sd.entity);

error_handler_free:
	imx500_free_controls(imx500);

error_power_off:
	pm_runtime_disable(&client->dev);
	pm_runtime_put_noidle(&client->dev);
	imx500_power_off(&client->dev);

	return ret;
}

static void imx500_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct imx500 *imx500 = to_imx500(sd);

	if (imx500->spi_device)
		put_device(&imx500->spi_device->dev);

	v4l2_async_unregister_subdev(sd);
	media_entity_cleanup(&sd->entity);
	imx500_free_controls(imx500);

	if (imx500->fw_loader)
		release_firmware(imx500->fw_loader);

	if (imx500->fw_main)
		release_firmware(imx500->fw_main);

	imx500->fw_loader = NULL;
	imx500->fw_main = NULL;
	imx500_clear_fw_network(imx500);

	pm_runtime_disable(&client->dev);
	if (!pm_runtime_status_suspended(&client->dev))
		imx500_power_off(&client->dev);
	pm_runtime_set_suspended(&client->dev);
}

static const struct of_device_id imx500_dt_ids[] = {
	{ .compatible = "sony,imx500" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, imx500_dt_ids);

static const struct dev_pm_ops imx500_pm_ops = { SET_RUNTIME_PM_OPS(
	imx500_power_off, imx500_power_on, NULL) };

static struct i2c_driver imx500_i2c_driver = {
	.driver = {
		.name = "imx500",
		.of_match_table	= imx500_dt_ids,
		.pm = &imx500_pm_ops,
	},
	.probe = imx500_probe,
	.remove = imx500_remove,
};

static int imx500_spi_probe(struct spi_device *spi)
{
	int result;

	spi->bits_per_word = 8;
	spi->max_speed_hz = 35000000;
	spi->mode = SPI_MODE_3;

	result = spi_setup(spi);
	if (result < 0)
		return dev_err_probe(&spi->dev, result, "spi_setup() failed");

	return 0;
}

static void imx500_spi_remove(struct spi_device *spi)
{
}

static const struct spi_device_id imx500_spi_id[] = {
	{ "imx500", 0 },
	{},
};
MODULE_DEVICE_TABLE(spi, imx500_spi_id);

static struct spi_driver imx500_spi_driver = {
	.driver = {
		.name = "imx500",
		.of_match_table = imx500_dt_ids,
	},
	.probe = imx500_spi_probe,
	.remove = imx500_spi_remove,
	.id_table = imx500_spi_id,
};

static int __init imx500_driver_init(void)
{
	int ret;

	ret = spi_register_driver(&imx500_spi_driver);
	if (ret)
		return ret;

	ret = i2c_add_driver(&imx500_i2c_driver);
	if (ret)
		spi_unregister_driver(&imx500_spi_driver);

	return ret;
}
module_init(imx500_driver_init);

static void __exit imx500_driver_exit(void)
{
	i2c_del_driver(&imx500_i2c_driver);
	spi_unregister_driver(&imx500_spi_driver);
}
module_exit(imx500_driver_exit);

MODULE_AUTHOR("Naushir Patuck <naush@raspberrypi.com>");
MODULE_DESCRIPTION("Sony IMX500 sensor driver");
MODULE_LICENSE("GPL");
