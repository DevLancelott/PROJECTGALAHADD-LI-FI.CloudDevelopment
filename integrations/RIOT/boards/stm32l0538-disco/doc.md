@defgroup    boards_stm32l0538-disco STM32L0538-DISCO
@ingroup     boards
@brief       Support for the STM32L0538-DISCO board

### Introduction

The
[STM32L0538-DISCO](https://www.st.com/en/evaluation-tools/32l0538discovery.html)
discovery kit features an ultra low-power stm32l053c8t6 microcontroller with
64KB of FLASH and 8KB of RAM.
The board also provides an on-board 2.04\" E-paper display (not supported yet).

![STM32L0538-DISCO](https://www.st.com/content/ccc/fragment/product*related/rpn*information/board*photo/group0/67/a2/3f/98/6b/24/4a/27/stm32l0538-discovery.jpg/files/stm32l0538-disco.jpg/*jcr_content/translations/en.stm32l0538-disco.jpg)

## Pinout

@image html pinouts/stm32l0538-disco.svg "Pinout for the stm32l0538-disco (from STM board manual)" width=50%

### MCU

| MCU          | STM32L053C8
|:-------------|:--------------------|
| Family       | ARM Cortex-M0+      |
| Vendor       | ST Microelectronics |
| RAM          | 8KiB                |
| Flash        | 64KiB               |
| Frequency    | up to 32 MHz        |
| Timers       | 9 (2x watchdog, 1 SysTick, 5x 16-bit, 1x RTC) |
| ADCs         | 1x 12 bit (up to 16 channels) |
| UARTs        | 3 (two USARTs and one Low-Power UART) |
| I2Cs         | 2                   |
| SPIs         | 4                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32l053c6.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0367-ultralowpower-stm32l0x3-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0223-stm32-cortexm0-mcus-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/um1775-discovery-kit-with-stm32l053c8-mcu-stmicroelectronics.pdf)|

### Supported features

| Peripheral            | Configuration                                                                             |
|:--------------------- |:----------------------------------------------------------------------------------------- |
| TIMs                  | TIM2                                                                                      |
| UARTs                 | USART1 on PA10 (RX), PA9 (TX)                                                             |
| SPIs                  | SPI1 on PB5 (MOSI), PB4 (MISO), PB3 (SCLK); SPI2 on PB15 (MOSI), PB14 (MISO), PB13 (SCLK) |

## Flashing the device

### Flashing the board using OpenOCD

The board can be flashed using OpenOCD via the on-board ST-Link adapter.
Then use the following command:

    make BOARD=stm32l0538-disco -C examples/basic/hello-world flash

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=stm32l0538-disco PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
could be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

### STDIO

STDIO is connected to pins PA9 (TX) and PA10 (RX) so an USB to UART adapter is
required. Use the `term` target to open a terminal:

    make BOARD=stm32l0538-disco -C examples/basic/hello-world term

If an external ST-Link adapter is used, RX and TX pins can be directly connected
to it. In this case, STDIO is available on /dev/ttyACMx (Linux case).
