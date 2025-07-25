# Changelog

All notable changes to this project will be documented in this file.

> The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
> and this project adheres to [calendar versioning](https://calver.org/) in the `YY.M.MICRO`format.

<!--
Populate these categories as PRs are merged to `main`. When a release is cut,
copy to its corresponding section, deleting empty sections if any.
Remember to update the hyperlinks at the bottom.
--->

[//]: # (current developments)

## 25.1.1 (2025-03-24)

### Enhancements

* Bump to conda 25.1.1, python 3.12.9, libmambapy 2.0.5, conda-libmamba-solver 25.3.0, constructor 3.11.3. (#127)

### Bug fixes

* Remove macOS apps before and after menuinst tests. (#117)
* Skip `test*uninstallation*remove*caches` if not on CI or if `CONDA*PKGS_DIRS` is set. (#118)
* Skip uninstallation tests that require `sudo` when `sudo` is unavailable. (#126)

### Other

* Run CI builds in a separate conda environment. (#125)

### Contributors

* @marcoesters



## 24.11.0 (2024-12-02)

### Enhancements

* Add `uninstall` subcommand, which uninstalls all environments inside a prefix and reverses
  `conda init` commands. It's also useful for deleting cache directories and configuration files. (#112)
* Bump to conda 24.11.0, libmambapy 1.5.11, constructor 3.10.0, menuinst 2.2.0. (#114)

### Bug fixes

* Prevent unbound error in `test*conda*standalone_config`. (#110)

### Contributors

* @marcoesters



## 24.9.2 (2024-10-30)

### Enhancements

* Bump to conda 24.9.2, python 3.12.7, conda-libmamba-version 24.9.0, constructor 3.9.3, libmambapy 1.5.10. (#107)
* Configure conda-standalone binaries with `.condarc` files. (#97 via #99)
* Add environment variable `CONDA*RESTRICT*RC*SEARCH*PATH` and CLI option `--no-rc` to only load `.condarc` file delivered by `conda-standalone` bundle or `CONDARC` environment variable. (#99)

### Contributors

* @jaimergp
* @marcoesters



## 24.7.1 (2024-08-13)

### Enhancements

* Bump to conda 24.7.1, conda-libmamba-version 24.7.0, constructor 3.9.2, menuinst 2.1.2. (#90)

### Bug fixes

* Add workaround to prevent crash on Windows when the `SSLKEYLOGFILE` environment variable is set. (#86 via #89).

### Contributors

* @jaimergp
* @marcoesters



## 24.5.0 (2024-06-13)

### Enhancements

* Upgrade to `conda 24.5.0`, `constructor 3.8.0` and `menuinst 2.1.1`. (#75)

### Bug fixes

* Respect `umask` when extracting tarballs for `constructor`. (#74)

### Contributors

* @jaimergp



## 24.4.0 (2024-05-15)

### Enhancements

* Update to conda 24.4.0, Python 3.11.9. (#71).

### Bug fixes

* Remove outdated `hiddenimport` configuration in PyInstaller. (#71)

### Contributors

* @jaimergp



## 24.3.0 (2024-05-07)

### Enhancements

* Update to conda 24.3.0, libmamba 1.5.8, constructor 3.7.0, Python 3.10.14. (#69).

### Contributors

* @jaimergp



## 24.1.2 (2024-03-05)

### Enhancements

* Update to conda 24.1.2, conda-libmamba-solver 24.1.0, libmambapy 1.5.6. (#61)
* Add compatibility for archspec 0.2.3. (#60)
* Do not render extraction progress bars in non-interactive runs or when `CONDA_QUIET` is set to a truthy value. (#58)

### Bug fixes

* Handle exceptions raised during `pkgs/` multiprocessor extraction. (#55 via #59)
* Remove lingering `codesign` symlink in `$PREFIX/bin` (macOS only). (#54)

### Contributors

* @jaimergp



## 23.11.0 (2024-01-15)

### Enhancements

* Bump to `python` 3.10.13, `conda` 23.11.0, `conda-libmamba-solver` 23.12.0 and `libmambapy` 1.5.3. (#36, #51)
* Import `menuinst` directly without relying on `constructor`'s `_nsis.py` module. (#36)
* Bundle `menuinst` 2.0.2 and `constructor` 3.6.0. (#51, #52)

### Bug fixes

* Do not crash if the metadata in one or more packages does not specify a `license` field. (#49)

### Contributors

* @jaimergp
* @SC246



## 23.10.0 (2023-11-14)

### Enhancements

* Update to `conda` 23.10.0, `conda-libmamba-solver` 23.11.0, `libmambapy` 1.5.3 and `constructor` 3.5.0 (#48)

### Contributors

* @jaimergp



## 23.9.0 (2023-10-04)

### Enhancements

* Update to `conda` 23.9.0, `conda-libmamba-solver` 23.9.1 and `libmambapy` 1.5.1. (#46)

### Contributors

* @jaimergp



## 23.7.4 (2023-09-25)

### Enhancements

* Update the bundled `conda` to 23.7.4. (#44)

### Contributors

* @jaimergp



## 23.7.3 (2023-09-05)

### Enhancements

* Update the bundled `conda` to 23.7.3. (#40)

### Bug fixes

* Update the SHA256 value of the `conda` tarball in the recipe. (#39)

### Contributors

* @jaimergp
* @conda-bot



## 23.7.2 (2023-08-28)

### Enhancements

* Add tests. (#4)
* Update recipe and patches to use `conda` 23.7.2 and `constructor` 3.4.5. (#25)
* Sign macOS builds with entitlements so that they can be notarized. (#25)
* Add `conda-libmamba-solver` to the list of bundled packages. (#27)
* Add a `python` subcommand so users can leverage the bundled Python interpreter. (#29)
* Prevent `--extract-conda-pkgs` from greedily and inefficiently using all CPUs. (#32)

### Bug fixes

* Adjust `imports.py` to include some previously missing modules. (#25)

### Other

* Add README. (#8)
* Add pre-commit. (#9)
* Start a `pytest` test suite. (#25)
* Refactor `entry_point.py` for better readability and maintainability. (#28)
* Scan and collect all licenses from the bundled conda dependencies. (#30)
* Upload conda packages built from `main` to anaconda.org/conda-canary. (#31)

### Contributors

* @dbast
* @jaimergp
* @conda-bot
