# Gen Compressed OTA

When using the compressed OTA, we need to generate the compressed app firmware. This document mainly describes how to generate the compressed app firmware.

For more information about compressed OTA, refer to [bootloader*support*plus](https://github.com/espressif/esp-iot-solution/tree/master/components/bootloader*support*plus).

## Use
In order to use this feature, you need to include the needed CMake file in your project's CMakeLists.txt after `project(XXXX)`.

```cmake
project(XXXX)

include(gen_compressed_ota)
```

Generate the compressed app firmware in an ESP-IDF "project" directory by running:

```plaintext
idf.py gen_compressed_ota
```

This command will compile your project first, then it will generate the compressed app firmware. For example, run the command under the project `simple*ota*examples` folder. If there are no errors, the `custom*ota*binaries` folder will be created and contains the following files:

```plaintext
simple_ota.bin.xz  
simple_ota.bin.xz.packed
```

The file named `simple_ota.bin.xz.packed` is the actual compressed app binary file to be transferred.

In addition, if [secure boot](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/security/secure-boot-v2.html) is enabled, the command will generate the signed compressed app binary file:

```plaintext
simple_ota.bin.xz.packed.signed
```

you can also use the script [gen*custom*ota.py](https://github.com/espressif/esp-iot-solution/tree/master/tools/cmake*utilities/scripts/gen*custom_ota.py) to compress the specified app:

```plaintext
python3 gen_custom_ota.py -hv v3 -i simple_ota.bin --add_app_header
```

## Notes

1. Command tool [gen*custom*ota.py](https://github.com/espressif/esp-iot-solution/tree/master/tools/cmake*utilities/scripts/gen*custom_ota.py) can also be used to generate the compressed firmware required by  [esp bootloader plus](https://github.com/espressif/esp-bootloader-plus):

```plaintext
python3 gen_custom_ota.py -i simple_ota.bin
```

Use `python3 gen*custom*ota.py -h` to see a summary of all available command line options.