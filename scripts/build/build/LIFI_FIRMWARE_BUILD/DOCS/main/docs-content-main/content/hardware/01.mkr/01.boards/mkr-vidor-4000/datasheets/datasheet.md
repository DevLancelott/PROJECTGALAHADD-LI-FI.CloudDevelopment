---
identifier: ABX00022
title: Arduino® MKR Vidor 4000
type: maker
author: Julián Caro Linares
---

![](assets/image.svg)

# Description

<p style="text-align: justify;">The Arduino MKR Vidor 4000 (from now on referred to as MKR Vidor 4000) is without a doubt the most advanced and featured-packed board in the MKR family and the only one with a FPGA chip on board. With a camera & HDMI connector, a Wi-Fi® / Bluetooth® module and up to 25 configurable pins, the board offers a huge range of possibilities to implement solutions in different environments and applications.</p>


# Target Areas

FPGA, IoT, automation, industry, smart cities, signal processing

# CONTENTS

## Features

The MKR Vidor 4000 is nothing less than a powerhouse of a board, packing a huge set of features into a small form factor. It features the Intel® Cyclone® 10CL016 for FPGA (Field Programming Gate Array), allowing you to configure a large set of pins to accommodate any of your preferences. But why stop there? The board also has a camera connector, a Micro HDMI connector, Wi-Fi® / Bluetooth® connectivity through the NINA-W102 module, and cyber-security through the ECC508 crypto chip. Just like the other members of the MKR family, it uses the popular Arm® Cortex®-M0 32-bit SAMD21 microprocessor.


### Microcontroller Block

The board's microcontroller is a low power Arm® Cortex®-M0 32-bit SAMD21, like in the other boards within the Arduino MKR family. The Wi-Fi® and Bluetooth® connectivity is performed with a module from u-blox, the NINA-W10, a low power chipset operating in the 2.4GHz range. On top of that, secure communication is ensured through the Microchip® ECC508 crypto chip. Also, you can find a battery charger, and a directionable RGB LED on-board.


|          Component           |                     Details                     |
| :--------------------------: | :---------------------------------------------: |
|     **Microcontroller**      | SAMD21 Arm® Cortex®-M0+ 32bit low power ARM MCU |
|      **USB connector**       |                Micro USB (USB-B)                |
|     **Built-in LED Pin**     |                      Pin 6                      |
|     **Digital I/O Pins**     |                       x8                        |
|    **Analog Input Pins**     |              x7 (ADC 8/10/12 bit)               |
|    **Analog Output Pins**    |                 x1 (DAC 10 bit)                 |
|         **PMW Pins**         |           x13 (0 - 8, 10, 12, A3, A4)           |
|   **External interrupts**    |     x10 (Pin 0, 1, 4, 5, 6, 7, 8,9, A1, A2)     |
|        **Bluetooth®**        |            Nina W102 u-blox® module             |
|          **Wi-Fi®**          |            Nina W102 u-blox® module             |
|      **Secure element**      |                    ATECC508A                    |
|           **UART**           |                       Yes                       |
|           **I2C**            |                       Yes                       |
|           **SPI**            |                       Yes                       |
|       **I/O Voltage**        |                      3.3 V                      |
| **Input Voltage (nominal)**  |                      5-7 V                      |
|  **DC Current per I/O pin**  |                      7 mA                       |
|    **Supported battery**     |   Li-Po Single Cell, 3.7 V, 1024 mAh Minimum    |
|    **Battery connector**     |                     JST PH                      |
|        **Processor**         |                     48 MHz                      |
|           **RTC**            |                   32.768 kHz                    |
|        **SAMD21G18A**        |            256 kB Flash, 32 kB SRAM             |
| **Nina W102 u-blox® module** |       448 kB ROM, 520 kB SRAM, 2 MB Flash       |

### FPGA Block

The FPGA is the Intel® Cyclone® 10CL016. It contains 16K logic elements, 504 kB of embedded RAM, and x56 18x18 bits HW multipliers for high-speed DSP operations. Each pin can toggle at over 150 MHz and can be configured for functions such as UARTs, (Q)SPI, high-resolution/high-frequency PWM, quadrature encoder, I2C, I2S, Sigma Delta DAC, etc.

|           Component           |                   Details                    |
| :---------------------------: | :------------------------------------------: |
|           **FPGA**            |           Intel® Cyclone® 10CL016            |
|            **PCI**            | Mini PCI Express port with programmable pins |
|     **Camera Connector**      |            MIPI camera connector             |
|       **Video Output**        |                  Micro HDMI                  |
| **Circuit Operating Voltage** |                    3.3 V                     |
|     **Digital I/O Pins**      |       22 headers + 25 Mini PCI Express       |
|         **PWM Pins**          |                   All Pins                   |
|           **UART**            |   Up to 7 (depends on FPGA configuration)    |
|            **SPI**            |   Up to 7 (depends on FPGA configuration)    |
|            **I2C**            |   Up to 7 (depends on FPGA configuration)    |
|  **DC Current per I/O Pin**   |                  4 or 8 mA                   |
|       **Flash Memory**        |                     2 MB                     |
|           **SDRAM**           |                     8 MB                     |
|        **Clock Speed**        |            48 MHz - up to 200 MHz            |


The board comes with 8 MB of SRAM to support the FPGA operations on video and audio. The FPGA code is stored in a 2 MB QSPI Flash chip, of which 1 MB is allocated for user applications. It is possible to perform high-speed DSP operations for audio and video processing. Therefore, the Vidor includes a Micro HDMI connector for audio and video output and a MIPI camera connector for video input. All of the board's pins are driven both by SAMD21 and FPGA while respecting the MKR family format. Finally, there is a Mini PCI Express connector with up to x25 user-programmable pins that can be used for connecting your FPGA as a peripheral to a computer or to create your own PCI interfaces.


### Wireless Communication

|          Component           |                Details                |
| :--------------------------: | :-----------------------------------: |
| **Nina W102 u-blox® module** | 2.4 GHz Wi-Fi® (802.11 b/g/n) support |
|                              |  Bluetooth® 4.2 Low Energy dual-mode  |


### Security

|   Component   |                                               Details                                                |
| :-----------: | :--------------------------------------------------------------------------------------------------: |
| **ATECC508A** | Secure boot process that verifies the authenticity and integrity of the firmware before it is loaded |
|               |                           Performs High-Speed Public Key (PKI) Algorithms                            |
|               |                              NIST Standard P256 Elliptic Curve Support                               |
|               |                               SHA-256 Hash Algorithm with HMAC Option                                |
|               |                                      Host and Client Operations                                      |
|               |                                          256-bit Key Length                                          |
|               |                                      Storage for up to 16 Keys                                       |

### Related Products

- Arduino MKR Family boards
- Arduino MKR Family shields
- Arduino MKR Family carriers

**Note**: Check the Arduino official documentation to know more about the compatibility and specifications of each of these products.


## Ratings

### Recommended Operating Conditions

<p style="text-align: justify;">
The following table is a comprehensive guideline for the optimal use of the MKR Vidor 4000, outlining typical operating conditions and design limits. The operating conditions of the MKR Vidor 4000 are largely a function based on its component's specifications.

</p>

<div style="text-align:center;">

|         **Parameter**                            | **Min** | **Typ** | **Max** | **Unit** |
|:------------------------------------------------:|:-------:|:-------:|:-------:|:--------:|
|     USB Supply Input Voltage                     |    -    |   5.0   |    -    |     V    |
|     Battery Supply Input Voltage                 |    -    |   3.7   |    -    |     V    |
|     Supply Input Voltage                         |    -    |   5.0   |   6.0   |     V    |
|     Microprocessor Circuit Operating Voltage     |    -    |   3.3   |    -    |     V    |
|     FPGA Circuit Operating Voltage               |    -    |   3.3   |    -    |     V    |

</div>

## Functional Overview

The cores of the MKR Vidor 4000 are the SAMD21 Arm® Cortex®-M0+ microcontroller and the Intel® Cyclone® 10CL016 FPGA. The board also contains several peripherals connected to the microcontroller and the FPGA blocks.

**![Board Overview](assets/Vidor_Mappa.jpg)**

### Pinout

The basic pinout is shown in Figure 1.

**![Figure 1. MKR Vidor basic pinout](assets/MKR*Vidor*Pinout.png)**

<div style="page-break-after: always;"></div>

The pinout of the main FPGA connections is shown in Figure 2.

**![Figure 2. MKR Vidor FPGA pinout](assets/MKR*Vidor*Pinout_fpga.png)**

Check the official Arduino documentation to see the full pinout document and the schematics of the product.

<div style="page-break-after: always;"></div>

### Block Diagram

An overview of the MKR Vidor 4000 high-level architecture is showed in the next figure:

**![High-level architecture of the MKR Vidor 4000](assets/MKR*Vidor*4000*Block*Diagram.svg)**

### Power Supply

The MKR Vidor can be powered through one of these interfaces:

- **USB:** Micro USB-B port. Used to power the board at 5 V.
- **Vin:** This pin can be used to power the board with a regulated 5 V source. If the power is fed through this pin, the USB power source is disconnected. This is the only way you can supply 5 V (range is 5 V to maximum 6 V) to the board not using USB. The pin is only an INPUT.
- **5V:** This pin outputs 5 V from the board when powered from the USB connector or from the VIN pin of the board. It is unregulated and the voltage is taken directly from the inputs.
- **VCC:** This pin outputs 3.3 V through the on-board voltage regulator. This voltage is 3.3 V if USB or VIN is used.
- **Battery:** 3.7 V single-cell lithium-ion/lithium-polymer battery, connected through the onboard battery connector JST S2B-PH-SM4-TB(LF)(SN). The mating connector is JST PHR-2.

## Device Operation

### Getting Started - IDE

If you want to program your MKR Vidor 4000 while offline you need to install the Arduino Desktop IDE **[1]**. To connect the MKR Vidor 4000 to your computer, you will need a micro USB-B cable.


### Getting Started - Intel Cyclone HDL & Synthesis

If you want to use HDL Languages to design, synthesize and upload new circuits inside the Intel® Cyclone FPGA you need to install the official Intel® Quartus Prime software. Check the following documentation to know more **[2]**.


### Getting Started - Arduino Web Editor

All Arduino devices work out-of-the-box on the Arduino Web Editor **[3]** by just installing a simple plugin.

The Arduino Web Editor is hosted online, therefore it will always be up-to-date with the latest features and support for all boards and devices. Follow **[4]** to start coding on the browser and upload your sketches onto your device.


### Getting Started - Arduino IoT Cloud

All Arduino IoT-enabled products are supported on Arduino IoT Cloud which allows you to log, graph and analyze sensor data, trigger events, and automate your home or business.


### Sample Sketches

Sample sketches for MKR Vidor 4000 can be found either in the “Examples” menu in the Arduino IDE or the “MKR Vidor Documentation” section of Arduino **[5]**.


### Online Resources

Now that you have gone through the basics of what you can do with the device, you can explore the endless possibilities it provides by checking exciting projects on Arduino Project Hub **[6]**, the Arduino Library Reference **[7]** and the online store **[8]** where you will be able to complement your MKR Vidor 4000 product with additional extensions, sensors and actuators.

## Mechanical Information

### Board Dimensions

MKR Vidor 4000 board dimensions and weight are the following:

| Dimensions & Weight |            |        |
| :-----------------: | :--------: | :----: |
|                     | **Width**  | 25 mm  |
|                     | **Length** | 83 mm  |
|                     | **Weight** | 43.5 g |

The MKR Vidor 4000 has two 2.22 mm drilled mounting holes to provide for mechanical fixing.


<div style="page-break-after: always;"></div>

## Certifications

### Declaration of Conformity CE DoC (EU)

We declare under our sole responsibility that the products above are in conformity with the essential requirements of the following EU Directives and therefore qualify for free movement within markets comprising the European Union (EU) and European Economic Area (EEA).

### Declaration of Conformity to EU RoHS & REACH 211 01/19/2021

Arduino boards are in compliance with RoHS 2 Directive 2011/65/EU of the European Parliament and RoHS 3 Directive 2015/863/EU of the Council of 4 June 2015 on the restriction of the use of certain hazardous substances in electrical and electronic equipment.

| **Substance**                          | **Maximum Limit (ppm)** |
|----------------------------------------|-------------------------|
| Lead (Pb)                              | 1000                    |
| Cadmium (Cd)                           | 100                     |
| Mercury (Hg)                           | 1000                    |
| Hexavalent Chromium (Cr6+)             | 1000                    |
| Poly Brominated Biphenyls (PBB)        | 1000                    |
| Poly Brominated Diphenyl ethers (PBDE) | 1000                    |
| Bis(2-Ethylhexyl) phthalate (DEHP)     | 1000                    |
| Benzyl butyl phthalate (BBP)           | 1000                    |
| Dibutyl phthalate (DBP)                | 1000                    |
| Diisobutyl phthalate (DIBP)            | 1000                    |

Exemptions: No exemptions are claimed.

Arduino Boards are fully compliant with the related requirements of European Union Regulation (EC) 1907 /2006 concerning the Registration, Evaluation, Authorization and Restriction of Chemicals (REACH). We declare none of the SVHCs (https://echa.europa.eu/web/guest/candidate-list-table), the Candidate List of Substances of Very High Concern for authorization currently released by ECHA, is present in all products (and also package) in quantities totaling in a concentration equal or above 0.1%. To the best of our knowledge, we also declare that our products do not contain any of the substances listed on the "Authorization List" (Annex XIV of the REACH regulations) and Substances of Very High Concern (SVHC) in any significant amounts as specified by the Annex XVII of Candidate list published by ECHA (European Chemical Agency) 1907 /2006/EC.

### Conflict Minerals Declaration

As a global supplier of electronic and electrical components, Arduino is aware of our obligations concerning laws and regulations regarding Conflict Minerals, specifically the Dodd-Frank Wall Street Reform and Consumer Protection Act, Section 1502. Arduino does not directly source or process conflict minerals such as Tin, Tantalum, Tungsten, or Gold. Conflict minerals are contained in our products in the form of solder, or as a component in metal alloys. As part of our reasonable due diligence, Arduino has contacted component suppliers within our supply chain to verify their continued compliance with the regulations. Based on the information received thus far we declare that our products contain Conflict Minerals sourced from conflict-free areas.

## FCC Caution

Any Changes or modifications not expressly approved by the party responsible for compliance could void the user’s authority to operate the equipment.

This device complies with part 15 of the FCC Rules. Operation is subject to the following two conditions:

1. This device may not cause harmful interference

2. This device must accept any interference received, including interference that may cause undesired operation.

**FCC RF Radiation Exposure Statement:**

1. This Transmitter must not be co-located or operating in conjunction with any other antenna or transmitter

2. This equipment complies with RF radiation exposure limits set forth for an uncontrolled environment

3. This equipment should be installed and operated with a minimum distance of 20 cm between the radiator and your body.

<div style="page-break-after: always;"></div>

**Note:** This equipment has been tested and found to comply with the limits for a Class B digital
device, pursuant to part 15 of the FCC Rules. These limits are designed to provide
reasonable protection against harmful interference in a residential installation. This equipment
generates, uses and can radiate radio frequency energy and, if not installed and used in
accordance with the instructions, may cause harmful interference to radio communications.
However, there is no guarantee that interference will not occur in a particular installation. If
this equipment does cause harmful interference to radio or television reception, which can be
determined by turning the equipment off and on, the user is encouraged to try to correct the
interference by one or more of the following measures:
- Reorient or relocate the receiving antenna.
- Increase the separation between the equipment and receiver.
- Connect the equipment into an outlet on a circuit different from that to which the
receiver is connected.
- Consult the dealer or an experienced radio/TV technician for help.

English:
User manuals for license-exempt radio apparatus shall contain the following or equivalent notice in a conspicuous location in the user manual or alternatively on the device or both. This device complies with Industry Canada license-exempt RSS standard(s). Operation is subject to the following two conditions:

1. This device may not cause interference

2. This device must accept any interference, including interference that may cause undesired operation of the device.

French:
Le présent appareil est conforme aux CNR d’Industrie Canada applicables aux appareils radio exempts de licence. L’exploitation est autorisée aux deux conditions suivantes:

1. L’ appareil nedoit pas produire de brouillage

2. L’utilisateur de l’appareil doit accepter tout brouillage radioélectrique subi, même si le brouillage est susceptible d’en compromettre le fonctionnement.

**IC SAR Warning:**

English:
This equipment should be installed and operated with a minimum distance of 20 cm between the radiator and your body.

French:
Lors de l’ installation et de l’ exploitation de ce dispositif, la distance entre le radiateur et le corps est d ’au moins 20 cm.

**Important:** The operating temperature of the EUT can’t exceed 85 °C and shouldn’t be lower than -40 °C.

Hereby, Arduino S.r.l. declares that this product is in compliance with essential requirements and other relevant provisions of Directive 2014/53/EU. This product is allowed to be used in all EU member states.

## Company Information

| **Company name** |              **Arduino S.r.l.**              |
| :--------------: | :------------------------------------------: |
| Company address  | Via Andrea Appiani, 25 - 20900 MONZA (Italy) |

## Reference Documentation

|        **Ref**                                      | **Link**                                                                                    |
|:---------------------------------------------------:|---------------------------------------------------------------------------------------------|
| Arduino IDE (Desktop)                               | https://www.arduino.cc/en/Main/Software                                                     |
| Getting Started With FPGAs Using the MKR Vidor 4000 | https://www.arduino.cc/en/Main/Software                                                     |
| Arduino IDE (Cloud)                                 | https://create.arduino.cc/editor                                                            |
| Arduino Cloud - Getting started                     | https://docs.arduino.cc/arduino-cloud/getting-started/iot-cloud-getting-started             |
| MKR Vidor Documentation                             | https://docs.arduino.cc/hardware/mkr-vidor-4000                                             |
| Arduino Project Hub                                 | https://create.arduino.cc/projecthub?by=part&part_id=11332&sort=trending                    |
| Library Reference                                   | https://www.arduino.cc/reference/en/                                                        |
| Online Store                                        | https://store.arduino.cc/                                                                   |


## Document Revision History

|  **Date**  | **Revision** |  **Changes**  |
| :--------: | :----------: | :-----------: |
| 14/11/2023 |      2       |  FCC Update   |
| 07/09/2023 |      1       | First release |
