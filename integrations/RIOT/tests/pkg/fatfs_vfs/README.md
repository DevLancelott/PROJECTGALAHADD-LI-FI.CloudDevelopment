Using FatFs (with VFS) on RIOT
=======================================

# native

To use this test on native you can either use a FAT-formatted image file or
directly use the mkfs command from the RIOT shell. Use `make image` to extract
a prepared image file that already contains a simple test.txt file. This is
only a convenience function to allow testing against a "default linux"
formatted fat volume without the need to call mount or other stuff that may
require super user privileges. Optionally `make compressed-image` can be used
to generate the compressed image that is in turn used by `make image`.

To tell RIOT where your image file is located you can use the define
`MTD*NATIVE*FILENAME`.

NOTE: You shouldn't leave the image mounted while you use it in RIOT, the
abstraction layer between FatFs and the image file mimics a dumb block device
(i.e. behaves much like the devices that are actually meant to be used with
FAT) That implies it doesn't show any modifications in RIOT that you perform on
your OS and the other way round. So always remember to mount/unmount correctly
or your FS will probably get damaged.

    make flash test-with-config

# Real Hardware

On real hardware, an SD card is used by default either via an SD/MMC controller
in SD mode or via an SPI interface in SPI mode using the driver module
`sdcard_spi`. Whether the SD card is used with an SD/MMC controller in
SD mode is determined by the board by enabling the `mtd*sdmmc*default` module.
Generally any device that supports the mtd-interface can be used with FatFS.

To use the automated test in pkg*fatfs*vfs you need to copy the generated image
to your storage device (e.g. your SD-card). To copy the image onto the card you
can use something like

    make image && dd if=bin/riot_fatfs_disk.img of=/dev/<your_sdcard>

After that you can connect the card to your RIOT device
and check the test output via terminal.

    make flash test-with-config
