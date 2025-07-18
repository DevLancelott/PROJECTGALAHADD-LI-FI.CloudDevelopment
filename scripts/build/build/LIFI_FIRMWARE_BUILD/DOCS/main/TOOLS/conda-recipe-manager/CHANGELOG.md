# Changelog
NOTES:
- Version releases in the 0.x.y range may introduce breaking changes.
- See the auto-generated release notes for more details.

## 0.6.0
- Contains many significant parsing bug fixes. Most of these fixes relate to quoting strings and JINJA statements.
  Our integration tests indicate that this increases parsing compatibility across the board. We are seeing that
  ~97% of all recipe files in the `conda-recipe-manager-test-data` set can now be parsed without throwing an
  exception.
- Contains a few bug fixes around parsing and rendering comments in V0 recipe files.
- Fixes several edge cases for PyPi URL upgrades with `crm bump-recipe`
- Adds a `pytest-socket` smoke test
- Includes several new quality-of-life improvements for CRM developers
- Huge shout-out to @mrbean-bremen for their help on diagnosing some `pyfakefs` issues on the latest
  point releases of Python.

## 0.5.1
- Fixes a JINJA rendering bug for V0 recipes in `RecipeReader::get*value(..., sub*vars=True)` and
  related functions.

## 0.5.0
- BREAKING CHANGE: `RecipeParser::search*and*patch()` has been replaced by `RecipeParser::search*and*patch_replace()`.
- Adds some support for interpreting `split` and `join` JINJA functions.
- Adds support for negative indexing in JINJA statements.
- `crm bump-recipe` now attempts to correct PyPi URLs when the predicted source artifact URL cannot be found.
  This is to counteract changes introduced in the last year where some PyPi tarballs may have switched from
  using `_`s to `-`s. This comes from a PEP standard change for source artifacts.
- `crm bump-recipe` now replaces all older PyPi URLs with `pypi.org` addresses.
- Minor fixes for the `crm graph` command.
- Fixes risky threading behavior in `crm bump-recipe`.
- Fixes for converting `/build/force*use*keys`, `/build/ignore*run*exports*from`, and `/build/ignore*run_exports` in `crm convert`.

## 0.4.2
- Various bug fixes and improvements related to `crm bump-recipe`
  - Introduces the new `--override-build-num` flag to allow bumped recipes to start counting
    from a non-zero value
  - Adds support for `| replace()` JINJA functions to be evaluated.

## 0.4.1
- `crm convert` now prints stacktraces when the `--debug` flag is used.
- `crm bump-recipe` now includes a `--save-on-failure` flag that can save the
  contents of a bumped recipe if the bump fails to fully complete.
- Significant recipe compatibility improvements, brought on by changes made in
  `rattler-build @0.34.1`.
- Several community-reported bug fixes.

## 0.4.0
- Introduces MVP for the `bump-recipe` command. This command should be able to update the
  version number, build number, and SHA-256 hash for most simple recipe files.
- Starts work for scanning `pyproject.toml` dependencies
- Minor bug fixes and infrastructure improvements.

## 0.3.4
- Makes `DependencyVariable` type hashable.

## 0.3.3
- Fixes a bug discovered by user testing relating to manipulating complex dependencies.
- Renames a few newer functions from `**string` to `**str` for project consistency.

## 0.3.2
- Refactors `RecipeParserDeps` into `RecipeReaderDeps`. Creates a new `RecipeParserDeps` that adds the high-level
  `add*dependency()` and `remove*dependency` functions.
- A few bug fixes and some unit testing improvements

## 0.3.1
Minor bug fixes. Addresses feedback from `conda-forge` users.

## 0.3.0
With this release, Conda Recipe Manager expands past it's original abilities to parse and
upgrade Conda recipe files.

Some highlights:
- Introduces the `scanner`, `fetcher`, and `grapher` modules.
- Adds significant tooling around our ability to parse Conda recipe dependencies.
- Adds some initial V1 recipe file format support.
- Introduces many bug fixes, parser improvements, and quality of life changes.
- Adds `pyfakefs` to the unit testing suite.

Full changelog available at:
https://github.com/conda/conda-recipe-manager/compare/v0.2.1...v0.3.0

## 0.2.1
Minor bug fixes and documentation improvements. Conversion compatibility with Bioconda recipe has improved significantly.

Includes many previously missing recipe transformations.

### Pull Requests
- Fixes integration tests from rattler-build 0.18.0 update (#76)
- Adds common environment settings (#75)
- Adds demo day intro slidedeck to CRM (#73)
- Upgrades basic quoted multiline strings (#72)
- Adds missing transforms for "git source" fields (#71)
- pip check improvements, more missing transforms, and some spelling enhancements (#70)
- Multiline summary fix issue 44 (#68)
- Preprocessor: Replace dot with bar functions (#67)
- Corrects using hash_type as a JINJA variable for the sha256 key (#66)
- Adds script_env support (#65)
- Adds missing build transforms (#62)
- Some minor improvements (#61)

## 0.2.0
Major improvements from `0.1.0`, mostly dealing with compatibility with `rattler-build`.
This marks the first actual release of the project, but still consider this work to be experimental
and continually changing.

## 0.1.0
Migrates parser from [percy](https://github.com/anaconda-distribution/percy/tree/main)
, ,
