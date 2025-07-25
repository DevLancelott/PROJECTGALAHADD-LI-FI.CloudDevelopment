# SDSPI Host Driver

SD Host side related components are:
- `sdmmc`
- `esp*driver*sdmmc`
- `esp*driver*sdspi` (current component)

For relationship and dependency among these components, see [SD Host Side Related Component Architecture](../sdmmc/README.md).

`esp*driver*sdspi` components is a driver based on ESP GPSPI master driver to help you:
- do SD transactions (under SDSPI mode) via ESP GPSPI peripheral.
- tune ESP GPSPI hardware configurations, such as clock frequency, bus width, etc.
- ...

You can
- use this driver to implement `sdmmc` protocol interfaces
- directly use `esp*driver*sdspi` APIs

to communicate with SD slave devices under SDSPI mode.
