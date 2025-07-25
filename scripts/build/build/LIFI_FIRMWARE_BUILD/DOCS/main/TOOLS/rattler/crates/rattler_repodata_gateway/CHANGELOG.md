# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.23.4](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.23.3...rattler*repodata*gateway-v0.23.4) - 2025-06-26

### Other

- updated the following local packages: rattler*conda*types, rattler*networking, rattler*cache

## [0.23.3](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.23.2...rattler*repodata*gateway-v0.23.3) - 2025-06-25

### Other

- updated the following local packages: rattler*conda*types, rattler*networking, rattler*cache

## [0.23.2](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.23.1...rattler*repodata*gateway-v0.23.2) - 2025-06-24

### Other

- update Cargo.toml dependencies

## [0.23.1](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.23.0...rattler*repodata*gateway-v0.23.1) - 2025-06-23

### Fixed

- end progress sharded repodata ([#1375](https://github.com/conda/rattler/pull/1375))

### Other

- update npm name ([#1368](https://github.com/conda/rattler/pull/1368))
- update readme ([#1364](https://github.com/conda/rattler/pull/1364))

## [0.23.0](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.7...rattler*repodata*gateway-v0.23.0) - 2025-05-23

### Added

- *(py)* query based on matchspec and package count ([#1347](https://github.com/conda/rattler/pull/1347))
- control over selection of .conda and .tar.bz2 ([#1344](https://github.com/conda/rattler/pull/1344))

### Fixed

- *(py)* package count was incorrect for prefer-conda ([#1350](https://github.com/conda/rattler/pull/1350))
- properly dedup package names ([#1342](https://github.com/conda/rattler/pull/1342))
- consistent usage of rustls-tls / native-tls feature ([#1324](https://github.com/conda/rattler/pull/1324))

## [0.22.7](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.6...rattler*repodata*gateway-v0.22.7) - 2025-05-16

### Other

- make sure that md5 also works as `CacheKey` ([#1293](https://github.com/conda/rattler/pull/1293))
- Bump zip to 3.0.0 ([#1310](https://github.com/conda/rattler/pull/1310))

## [0.22.6](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.5...rattler*repodata*gateway-v0.22.6) - 2025-05-03

### Other

- lock workspace member dependencies ([#1279](https://github.com/conda/rattler/pull/1279))

## [0.22.5](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.4...rattler*repodata*gateway-v0.22.5) - 2025-04-10

### Other

- update Cargo.toml dependencies

## [0.22.4](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.3...rattler*repodata*gateway-v0.22.4) - 2025-04-04

### Added

- more helpful error when failing to persist repodata ([#1220](https://github.com/conda/rattler/pull/1220))

## [0.22.3](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.2...rattler*repodata*gateway-v0.22.3) - 2025-03-18

### Added

- allow to pass a semaphore for concurrency control ([#1169](https://github.com/conda/rattler/pull/1169))

## [0.22.2](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.1...rattler*repodata*gateway-v0.22.2) - 2025-03-14

### Other

- updated the following local packages: rattler*conda*types, rattler_networking

## [0.22.1](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.22.0...rattler*repodata*gateway-v0.22.1) - 2025-03-10

### Other

- update Cargo.toml dependencies

## [0.22.0](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.40...rattler*repodata*gateway-v0.22.0) - 2025-03-04

### Added

- *(js)* compile `rattler*solve` and `rattler*repodata_gateway` ([#1108](https://github.com/conda/rattler/pull/1108))

## [0.21.40](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.39...rattler*repodata*gateway-v0.21.40) - 2025-02-28

### Other

- update Cargo.toml dependencies

## [0.21.39](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.38...rattler*repodata*gateway-v0.21.39) - 2025-02-27

### Other

- updated the following local packages: rattler*redaction, rattler*conda*types, rattler*networking

## [0.21.38](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.37...rattler*repodata*gateway-v0.21.38) - 2025-02-25

### Added

- add run_exports cache (#1060)

### Fixed

- split matchspec on start of constraint (#1094)

## [0.21.37](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.36...rattler*repodata*gateway-v0.21.37) - 2025-02-18

### Other

- update Cargo.toml dependencies

## [0.21.36](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.35...rattler*repodata*gateway-v0.21.36) - 2025-02-06

### Other

- bump rust 1.84.1 (#1053)

## [0.21.35](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.34...rattler*repodata*gateway-v0.21.35) - 2025-02-06

### Other

- updated the following local packages: rattler_networking

## [0.21.34](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.33...rattler*repodata*gateway-v0.21.34) - 2025-02-03

### Added

- add S3 support (#1008)

## [0.21.33](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.32...rattler*repodata*gateway-v0.21.33) - 2025-01-23

### Other

- Improve AuthenticationStorage (#1026)

## [0.21.32](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.31...rattler*repodata*gateway-v0.21.32) - 2025-01-09

### Other

- updated the following local packages: rattler*conda*types

## [0.21.31](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.30...rattler*repodata*gateway-v0.21.31) - 2025-01-09

### Other

- updated the following local packages: rattler*conda*types

## [0.21.30](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.29...rattler*repodata*gateway-v0.21.30) - 2025-01-08

### Fixed

- retry failed repodata streaming on io error (#1017)

## [0.21.29](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.28...rattler*repodata*gateway-v0.21.29) - 2024-12-20

### Other

- updated the following local packages: rattler*cache, rattler*conda_types

## [0.21.28](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.27...rattler*repodata*gateway-v0.21.28) - 2024-12-17

### Other

- update Cargo.toml dependencies

## [0.21.27](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.26...rattler*repodata*gateway-v0.21.27) - 2024-12-13

### Other

- updated the following local packages: rattler*conda*types

## [0.21.26](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.25...rattler*repodata*gateway-v0.21.26) - 2024-12-12

### Other
- updated the following local packages: rattler*cache, rattler*conda_types

## [0.21.25](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.24...rattler*repodata*gateway-v0.21.25) - 2024-12-05

### Other

- updated the following local packages: rattler_networking

## [0.21.24](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.23...rattler*repodata*gateway-v0.21.24) - 2024-11-30

### Added

- use `fs-err` also for tokio ([#958](https://github.com/conda/rattler/pull/958))

## [0.21.23](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.22...rattler*repodata*gateway-v0.21.23) - 2024-11-18

### Other

- updated the following local packages: rattler_networking

## [0.21.22](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.21...rattler*repodata*gateway-v0.21.22) - 2024-11-18

### Other

- updated the following local packages: rattler*conda*types

## [0.21.21](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.20...rattler*repodata*gateway-v0.21.21) - 2024-11-14

### Other

- enable using sharded repodata for custom channels ([#910](https://github.com/conda/rattler/pull/910))

## [0.21.20](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.19...rattler*repodata*gateway-v0.21.20) - 2024-11-05

### Fixed

- gateway recursive records ([#930](https://github.com/conda/rattler/pull/930))

## [0.21.19](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.18...rattler*repodata*gateway-v0.21.19) - 2024-11-04

### Other

- update Cargo.toml dependencies

## [0.21.18](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.17...rattler*repodata*gateway-v0.21.18) - 2024-10-21

### Other

- updated the following local packages: file*url, rattler*cache

## [0.21.17](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.16...rattler*repodata*gateway-v0.21.17) - 2024-10-07

### Other

- stream JLAP repodata writes ([#891](https://github.com/conda/rattler/pull/891))

## [0.21.16](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.15...rattler*repodata*gateway-v0.21.16) - 2024-10-03

### Other

- updated the following local packages: rattler*conda*types

## [0.21.15](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.14...rattler*repodata*gateway-v0.21.15) - 2024-10-01

### Other

- start using fs-err in repodata_gateway ([#877](https://github.com/conda/rattler/pull/877))

## [0.21.14](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.13...rattler*repodata*gateway-v0.21.14) - 2024-09-23

### Other

- updated the following local packages: rattler*conda*types

## [0.21.13](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.12...rattler*repodata*gateway-v0.21.13) - 2024-09-09

### Other

- updated the following local packages: rattler*conda*types

## [0.21.12](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.11...rattler*repodata*gateway-v0.21.12) - 2024-09-05

### Fixed
- *(gateway)* clear subdir cache based on `base_url` ([#852](https://github.com/conda/rattler/pull/852))
- typos ([#849](https://github.com/conda/rattler/pull/849))

## [0.21.11](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.10...rattler*repodata*gateway-v0.21.11) - 2024-09-03

### Fixed
- allow `gcs://` and `oci://` in gateway ([#845](https://github.com/conda/rattler/pull/845))

## [0.21.10](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.9...rattler*repodata*gateway-v0.21.10) - 2024-09-03

### Other
- make PackageCache multi-process safe ([#837](https://github.com/conda/rattler/pull/837))

## [0.21.9](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.8...rattler*repodata*gateway-v0.21.9) - 2024-09-02

### Other
- updated the following local packages: rattler*conda*types

## [0.21.8](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.7...rattler*repodata*gateway-v0.21.8) - 2024-08-16

### Other
- updated the following local packages: rattler_networking

## [0.21.7](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.6...rattler*repodata*gateway-v0.21.7) - 2024-08-16

### Added
- add package names api for gateway ([#819](https://github.com/conda/rattler/pull/819))

## [0.21.6](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.5...rattler*repodata*gateway-v0.21.6) - 2024-08-15

### Fixed
- move more links to the conda org from conda-incubator ([#816](https://github.com/conda/rattler/pull/816))

### Other
- change links from conda-incubator to conda ([#813](https://github.com/conda/rattler/pull/813))
- update banner ([#808](https://github.com/conda/rattler/pull/808))

## [0.21.5](https://github.com/baszalmstra/rattler/compare/rattler*repodata*gateway-v0.21.4...rattler*repodata*gateway-v0.21.5) - 2024-08-06

### Other
- updated the following local packages: rattler*conda*types

## [0.21.4](https://github.com/baszalmstra/rattler/compare/rattler*repodata*gateway-v0.21.3...rattler*repodata*gateway-v0.21.4) - 2024-08-02

### Fixed
- redact secrets in the `canonical_name` functions ([#801](https://github.com/baszalmstra/rattler/pull/801))

### Other
- mark some crates 1.0 ([#789](https://github.com/baszalmstra/rattler/pull/789))

## [0.21.3](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.2...rattler*repodata*gateway-v0.21.3) - 2024-07-23

### Other
- updated the following local packages: rattler*conda*types

## [0.21.2](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.1...rattler*repodata*gateway-v0.21.2) - 2024-07-23

### Other
- updated the following local packages: rattler*conda*types

## [0.21.1](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.21.0...rattler*repodata*gateway-v0.21.1) - 2024-07-15

### Other
- bump dependencies and remove unused ones ([#771](https://github.com/conda/rattler/pull/771))

## [0.21.0](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.20.5...rattler*repodata*gateway-v0.21.0) - 2024-07-08

### Added
- improve error message when parsing file name ([#757](https://github.com/conda/rattler/pull/757))
- add direct url repodata building ([#725](https://github.com/conda/rattler/pull/725))
- add shards*base*url and write shards atomically ([#747](https://github.com/conda/rattler/pull/747))

### Fixed
- direct_url query for windows ([#768](https://github.com/conda/rattler/pull/768))
- Fix GatewayQuery.query to filter records based on provided specs ([#756](https://github.com/conda/rattler/pull/756))
- run clippy on all targets ([#762](https://github.com/conda/rattler/pull/762))
- allow empty json repodata ([#745](https://github.com/conda/rattler/pull/745))

### Other
- document gateway features ([#737](https://github.com/conda/rattler/pull/737))

## [0.20.5](https://github.com/baszalmstra/rattler/compare/rattler*repodata*gateway-v0.20.4...rattler*repodata*gateway-v0.20.5) - 2024-06-04

### Other
- remove lfs ([#512](https://github.com/baszalmstra/rattler/pull/512))

## [0.20.4](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.20.3...rattler*repodata*gateway-v0.20.4) - 2024-06-03

### Other
- updated the following local packages: rattler*conda*types, rattler*conda*types

## [0.20.3](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.20.2...rattler*repodata*gateway-v0.20.3) - 2024-05-28

### Other
- updated the following local packages: rattler*conda*types, rattler*conda*types

## [0.20.2](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.20.1...rattler*repodata*gateway-v0.20.2) - 2024-05-27

### Fixed
- result grouped by subdir instead of channel ([#666](https://github.com/conda/rattler/pull/666))

### Other
- introducing the installer ([#664](https://github.com/conda/rattler/pull/664))

## [0.20.1](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.20.0...rattler*repodata*gateway-v0.20.1) - 2024-05-14

### Added
- exclude repodata records based on timestamp ([#654](https://github.com/conda/rattler/pull/654))

## [0.20.0](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.11...rattler*repodata*gateway-v0.20.0) - 2024-05-13

### Added
- add clear subdir cache function to repodata gateway ([#650](https://github.com/conda/rattler/pull/650))
- high level repodata access ([#560](https://github.com/conda/rattler/pull/560))

### Other
- update README.md

## [0.19.11](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.10...rattler*repodata*gateway-v0.19.11) - 2024-05-06

### Other
- updated the following local packages: rattler*conda*types, rattler_networking

## [0.19.10](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.9...rattler*repodata*gateway-v0.19.10) - 2024-04-30

### Added
- create SparseRepoData from byte slices ([#624](https://github.com/conda/rattler/pull/624))

## [0.19.9](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.8...rattler*repodata*gateway-v0.19.9) - 2024-04-25

### Other
- updated the following local packages: rattler_networking

## [0.19.8](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.7...rattler*repodata*gateway-v0.19.8) - 2024-04-25

### Other
- updated the following local packages: rattler*conda*types

## [0.19.7](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.6...rattler*repodata*gateway-v0.19.7) - 2024-04-19

### Added
- make root dir configurable in channel config ([#602](https://github.com/conda/rattler/pull/602))

### Other
- update dependencies incl. reqwest ([#606](https://github.com/conda/rattler/pull/606))

## [0.19.6](https://github.com/baszalmstra/rattler/compare/rattler*repodata*gateway-v0.19.5...rattler*repodata*gateway-v0.19.6) - 2024-04-05

### Other
- updated the following local packages: rattler*conda*types, rattler_networking

## [0.19.5](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.4...rattler*repodata*gateway-v0.19.5) - 2024-03-30

### Other
- updated the following local packages: rattler*conda*types

## [0.19.4](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.3...rattler*repodata*gateway-v0.19.4) - 2024-03-21

### Other
- updated the following local packages: rattler*conda*types, rattler_networking

## [0.19.3](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.2...rattler*repodata*gateway-v0.19.3) - 2024-03-14

### Other
- add pixi badge ([#563](https://github.com/conda/rattler/pull/563))

## [0.19.2](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.1...rattler*repodata*gateway-v0.19.2) - 2024-03-08

### Fixed
- chrono deprecation warnings ([#558](https://github.com/conda/rattler/pull/558))

## [0.19.1](https://github.com/conda/rattler/compare/rattler*repodata*gateway-v0.19.0...rattler*repodata*gateway-v0.19.1) - 2024-03-06

### Fixed
- correct condition to downweigh track-feature packages ([#545](https://github.com/conda/rattler/pull/545))
- dont use workspace dependencies for local crates ([#546](https://github.com/conda/rattler/pull/546))

### Other
- every crate should have its own version ([#557](https://github.com/conda/rattler/pull/557))

## [0.19.0](https://github.com/baszalmstra/rattler/compare/rattler*repodata*gateway-v0.18.0...rattler*repodata*gateway-v0.19.0) - 2024-02-26
