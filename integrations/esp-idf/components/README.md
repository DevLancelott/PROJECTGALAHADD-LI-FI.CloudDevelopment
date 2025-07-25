# Core Components

## Overview

This document contains details about what the core components are, what they contain, and how they are organized.

## Organization

The core components are organized into two groups.

The first group (referred to as `G0`) includes `hal`, `arch` (where `arch` is either `riscv` or `xtensa` depending on the chip), `esp*rom`, `esp*common`, and `soc`. This group contains information about and provides low-level access to the underlying hardware. In the case of `esp_common`, it contains hardware-agnostic code and utilities. These components may have dependencies on each other within the group, but outside dependencies should be minimized. The reason for this approach is that these components are fundamental, and many other components may require them. Ideally, the dependency relationship only goes one way, making it easier for this group to be usable in other projects.

The second group (referred to as `G1`) operates at a higher level than the first group. `G1` includes the components `esp*hw*support`, `esp*system`, `newlib`, `spi*flash`, `freertos`, `log`, and `heap`. Like the first group, circular dependencies within this group are allowed, and these components can have dependencies on the first group. G1 components represent essential software mechanisms for building other components.

## Descriptions

The following is a short description of the components mentioned above.

### `G0` Components

#### `hal`

Contains the hardware abstraction layer and low-level operation implementations for the various peripherals. The low-level functions assign meaningful names to register-level manipulations; the hardware abstraction provide operations one level above this, grouping these low-level functions
into routines that achieve a meaningful action or state of the peripheral.

Example:

-   `spi*flash*ll*set*address` is a low-level function part of the hardware abstraction `spi*flash*hal*read*block`

#### `arch`

Contains low-level architecture operations and definitions, including those for customizations (can be thought of on the same level as the low-level functions of `hal`).
This can also contain files provided by the architecture vendor.

Example:

-   `xt*set*exception_handler`
-   `rv*utils*intr_enable`
-   `ERI*PERFMON*MAX`

#### `esp_common`

Contains hardware-agnostic definitions, constants, macros, utilities, 'pure' and/or algorithmic functions that is useable by all other components (that is, barring there being a more appropriate component to put them in).

Example:

-   `BIT(nr)` and other bit manipulation utilities in the future
-   `IDF_DEPRECATED(REASON)`
-   `ESP*IDF*VERSION_MAJOR`

#### `soc`

Contains description of the underlying hardware: register structure, addresses, pins, capabilities, etc.

Example:

-   `DR*REG*DPORT_BASE`
-   `SOC*MCPWM*SUPPORTED`
-   `uart*dev*s`

#### `esp_rom`

Contains headers, linker scripts, abstraction layer, patches, and other related files to ROM functions.

Example:

-   `esp32.rom.eco3.ld`
-   `rom/aes.h`

### `G1` Components

#### `spi_flash`

SPI flash device access implementation.

#### `freertos`

FreeRTOS port to targets supported by ESP-IDF.

#### `log`

Logging library.

#### `heap`

Heap implementation.

#### `newlib`

Some functions n the standard library are implemented here, especially those needing other `G1` components.

Example:

-   `malloc` is implemented in terms of the component `heap`'s functions
-   `gettimeofday` is implemented in terms of system time in `esp_system`

#### `esp_mm`

Memory management. Currently, this encompasses:

-   Memory mapping for MMU supported memories
-   Memory synchronisation via Cache
-   Utils such as APIs to convert between virtual address and physical address

#### `esp_psram`

Contains implementation of PSRAM services

#### `esp_system`

Contains implementation of system services and controls system behavior. The implementations
here may take hardware resources and/or decide on a hardware state needed for support of a system service/feature/mechanism.
Currently, this encompasses the following, but not limited to:

-   Startup and initialization
-   Panic and debug
-   Reset and reset reason
-   Task and interrupt watchdogs

#### `esp*hw*support`

Contains implementations that provide hardware operations, arbitration, or resource sharing, especially those that
is used in the system. Unlike `esp_system`, implementations here do not decide on a hardware state or takes hardware resource, acting
merely as facilitator to hardware access. Currently, this encompasses the following, but not limited to:

-   Interrupt allocation
-   Sleep functions
-   Memory functions (external SPIRAM, async memory, etc.)
-   Clock and clock control
-   Random generation
-   CPU utilities
-   MAC settings

### `esp*hw*support` vs `esp_system`

This section details list some implementations and the reason for placing it in either `esp*hw*support` or `esp_system`.

#### `task*wdt.c` (`esp*system`) vs `intr*alloc.c` (`esp*hw_support`)

The task watchdog fits the definition of taking and configuring hardware resources (wdt, interrupt) for implementation of a system service/mechanism.

This is in contrast with interrupt allocation that merely facilitates access to the underlying hardware for other implementations -
drivers, user code, and even the task watchdog mentioned previously!

#### `crosscore*int.c` (`esp*system`)

The current implementation of crosscore interrupts is tightly coupled with a number of interrupt reasons
associated with system services/mechanisms: REASON*YIELD (scheduler), REASON*FREQ_SWITCH (power management)
REASON*PRINT*BACKTRACE (panic and debug).

However, if an implementation exists that makes it possible to register an arbitrary interrupt reason - a
lower level inter-processor call if you will, then this implementation is a good candidate for `esp*hw*support`.
The current implementation in `esp_system` can then just register the interrupt reasons mentioned above.

#### `esp*mac.h`, `esp*chip*info.h`, `esp*random.h` (`esp*hw*support`)

The functions in these headers used to be in `esp_system.h`, but have been split-off.

The remaining functions in `esp_system.h` are those that deal with system behavior, such
as `esp*register*shutdown_handler`, or are proxy for other system components's APIs such as
`esp*get*free*heap*size`.

The functions split-off from `esp_system.h` are much more hardware manipulation oriented such as:
`esp*read*mac`, `esp*random` and `esp*chip_info`.
