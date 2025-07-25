# How-to guides

## Control which kind of installer gets generated

Constructor is currently limited to generating installers for the platform on
which it is running. In other words, if you run constructor on a Windows
computer, you can only generate Windows installers. This is largely because
OS-native tools are needed to generate the Windows `.exe` files and macOS `.pkg`
files. There is a key in `construct.yaml`, `installer_type`, which dictates
the type of installer that gets generated. This is primarily only useful for
macOS, where you can generate either `.pkg` or `.sh` installers. When not set in
`construct.yaml`, this value defaults to `.sh` on Unix platforms, and `.exe` on
Windows. Using this key is generally done with selectors.  For example, to
build a `.pkg` installer on MacOS, but fall back to default behavior on other
platforms:

```yaml
installer_type: pkg  #[osx]
```

See [`installer*type`](construct-yaml.md#installer*type) for more details.


## Customization and branding

Graphical installers (`.pkg` on macOS and `.exe` on Windows) support some level of branding and customization.

Logos, backgrounds and banners:
- Refer to [`welcome*image`](construct-yaml.md#welcome*image) and [`icon*image`](construct-yaml.md#icon*image). Windows also supports [`header*image`](construct-yaml.md#header*image).
- Alternatively, a text-based image can be autogenerated from text if you set [`welcome*image*text`](construct-yaml.md#welcome*image*text)  and [`header*image*text`](construct-yaml.md#header*image*text), respectively. The color of such text can be provided via [`default*image*color`](construct-yaml.md#default*image*color).

Messages and texts. You can specify these via `**file` (a path is expected) or `*text` (raw string expected).
- [`welcome*file`](construct-yaml.md#welcome*file) and [`welcome*text`](construct-yaml.md#welcome*text): The text that is shown in the first page of the installer.
- [`readme*file`](construct-yaml.md#readme*file) and [`readme*text`](construct-yaml.md#readme*text): Optional text to be displayed on an extra page before the license. macOS only.
- [`conclusion*file`](construct-yaml.md#conclusion*file) and [`conclusion*text`](construct-yaml.md#conclusion*text): The text to be shown at the end of the installer, on success.

On Windows, you can also add extra pages to the installer. This is an advanced option, so your best bet is to check the examples in the source repository at `examples/customized*welcome*conclusion`.

## Signing and notarization

```{seealso}
Example of a CI pipeline implementing:
- [Signing on Windows](https://github.com/napari/packaging/blob/6f5fcfaf7b/.github/workflows/make_bundle_conda.yml#L390)
- [Signing](https://github.com/napari/packaging/blob/6f5fcfaf7b/.github/workflows/make_bundle_conda.yml#L349) and [notarization](https://github.com/napari/packaging/blob/6f5fcfaf7b/.github/workflows/make_bundle_conda.yml#L459) on macOS
```

### Signing EXE installers

Windows can trigger SmartScreen alerts for EXE installers, signed or not. It does help when they are signed, though. [Read this SO answer about SmartScreen reputation for more details](https://stackoverflow.com/questions/48946680/how-to-avoid-the-windows-defender-smartscreen-prevented-an-unrecognized-app-fro/66582477#66582477).

`constructor` supports the following tools to sign installers:

* [SignTool](https://learn.microsoft.com/en-us/windows/win32/seccrypto/signtool)
* [AzureSignTool](https://github.com/vcsjones/AzureSignTool)

The signtool that is used can be set in the `construct.yaml` file via the [`windows*signing*tool`](construct-yaml.md#windows*signing*tool) key.
If the [`signing*certificate`](construct-yaml.md#signing*certificate) key is set, `windows*signing*tool` defaults to `signtool`.

For each tool, there are environment variables that may need to be set to configure signing.

#### Environment variables for SignTool

| Variable                                    | Description                                                    | CLI flag | Default                      |
|---------------------------------------------|----------------------------------------------------------------|----------|------------------------------|
| `CONSTRUCTOR*PFX*CERTIFICATE_PASSWORD`      | Password for the `pfx` certificate file.                       | `/p`     | Empty                        |
| `CONSTRUCTOR*SIGNTOOL*PATH`                 | Path to `signtool.exe`. Needed if `signtool` is not in `PATH`. | N/A      | `signtool`                   |
| `CONSTRUCTOR*SIGNTOOL*FILE_DIGEST`          | Digest algorithm for creating the file signatures.             | `/fd`    | `sha256`                     |
| `CONSTRUCTOR*SIGNTOOL*TIMESTAMP*SERVER*URL` | URL to the RFC 3161 timestamp server.                          | `/tr`    | http://timestamp.sectigo.com |
| `CONSTRUCTOR*SIGNTOOL*TIMESTAMP_DIGEST`     | Digest algorithm for the RFC 3161 time stamp.                  | `/td`    | `sha256`                     |

#### Environment variables for AzureSignTool

| Variable                               | Description                                                                                 | CLI flag | Default                      |
|----------------------------------------|---------------------------------------------------------------------------------------------|----------|------------------------------|
| `AZURE*SIGNTOOL*FILE_DIGEST`           | Digest algorithm for creating the file signatures.                                          | `-fd`    | `sha256`                     |
| `AZURE*SIGNTOOL*KEY*VAULT*ACCESSTOKEN` | An access token used to authenticate to Azure.                                              | `-kva`   | Empty                        |
| `AZURE*SIGNTOOL*KEY*VAULT*CERTIFICATE` | The name of the certificate in the key vault.                                               | `-kvc`   | Empty                        |
| `AZURE*SIGNTOOL*KEY*VAULT*CLIENT_ID`   | The client ID used to authenticate to Azure. Required for authentication with a secret.     | `-kvi`   | Empty                        |
| `AZURE*SIGNTOOL*KEY*VAULT*SECRET`      | The client secret used to authenticate to Azure. Required for authentication with a secret. | `-kvs`   | Empty                        |
| `AZURE*SIGNTOOL*KEY*VAULT*TENANT_ID`   | The tenant ID used to authenticate to Azure. Required for authentication with a secret.     | `-kvt`   | Empty                        |
| `AZURE*SIGNTOOL*KEY*VAULT*URL`         | The URL of the key vault with the certificate.                                              | `-kvu`   | Empty                        |
| `AZURE*SIGNTOOL*PATH`                  | Path to `AzureSignTool.exe`. Needed if `azuresigntool` is not in `PATH`.                    | N/A      | `azuresigntool`              |
| `AZURE*SIGNTOOL*TIMESTAMP*SERVER*URL`  | URL to the RFC 3161 timestamp server.                                                       | `-tr`    | http://timestamp.sectigo.com |
| `AZURE*SIGNTOOL*TIMESTAMP_DIGEST`      | Digest algorithm for the RFC 3161 time stamp.                                               | `-td`    | `sha256`                     |

:::{note}

If neither `AZURE*SIGNTOOL*KEY*VAULT*ACCESSTOKEN` nor `AZURE*SIGNTOOL*KEY*VAULT*SECRET` are set, `constructor` will use a Managed Identity (`-kvm` CLI option).
:::

### Signing and notarizing PKG installers

In the case of macOS, users might get warnings for PKGs if the installers are not signed *and* notarized. However, once these two requirements are fulfilled, the warnings disappear instantly.
`constructor` offers some configuration options to help you in this process:

You will need to provide two identity names:
* the installer certificate identity (via [`signing*identity*name`](construct-yaml.md#signing*identity*name)) to sign the pkg installer,
* the application certificate identity to pass the notarization (via [`notarization*identity*name`](construct-yaml.md#notarization*identity*name));
  this certificate is used to sign binaries and plugins inside the pkg installer.
These can be obtained in the [Apple Developer portal](https://developer.apple.com/account/).
Once signed, you can notarize your PKG with Apple's `notarytool`.

:::{note}

To sign a pkg installer, the keychain containing the identity names must be unlocked and in the keychain search list.

## Create shortcuts

On Windows, `conda` supports `menuinst 1.x` shortcuts. If a package provides a certain JSON file
under `$PREFIX/Menu`, `conda` will process it to create the specified menu items.
This happens by default for *all packages*. If you only want this to happen for certain packages,
use the [`menu*packages`](construct-yaml.md#menu*packages) key.

Starting with `conda` 23.11, `menuinst 2.x` is supported, which means you can create shortcuts in all platforms (Linux, macOS and Windows).
The JSON document format is slightly different, so make sure to check the [menuinst documentation](https://conda.github.io/menuinst/).
Your installer will need to be created with `conda-standalone 23.11` or above.
`micromamba` does not currently support `menuinst 2.x` style shortcuts (only `1.x` on Windows).

To learn more about `menuinst`, visit [`conda/menuinst`](https://github.com/conda/menuinst).

## Find out the used constructor version

Recent constructor versions (>=3.4.2) burn-in their version into created installers in order to be able to trace back bugs in created installers to the constructor code base.

The burned-in version can be retrieved in different ways depending on the installer type:

- For `.sh` intallers (via cli): `head $installer.sh | grep "Created by constructor"`
- For `.exe` installers (via Windows Explorer): `$installer.exe` → Properties → Details → Comments, or (via cli) `exiftool $installer.exe`
- For `.pkg` installers (via cli on macOS): `xar -xf $installer.pkg -n run*installation.pkg/Scripts; zgrep -a "Created by constructor" run*installation.pkg/Scripts` or `pkgutil --expand $installer.pkg extracted; grep "Created by constructor" extracted/run_installation.pkg/Scripts/postinstall`

## Uninstall `constructor`-generated installations

:::{note}

Many `constructor` installers ship `conda` (Miniconda, Miniforge, etc), and offer the user to *initialize* the installation. This adds further changes to the system configuration. These changes won't be reverted by simply deleting the installation directory. If you want to revert these changes, you can execute this command *before* deleting the installation directory.

```bash
$ conda init --reverse --all
```

Use the `--dry-run` flag if you want to check what things will be reverted before actually doing it.
:::

### Windows

On Windows, `constructor`-generated installations include an uninstaller executable, which is also exposed in the Control Panel menu "Add or remove programs". The uninstaller executable is located in the installation directory, and is named `Uninstall-<INSTALLATION_NAME>.exe`.

Once opened, the uninstaller will guide the user through the uninstallation process. It will also remove the installation directory, and the uninstaller executable itself. In certain cases, some dangling files might be left behind, but these will be removed in the next reboot.

If you want to perform the uninstallation steps manually, you can:

1. Remove the installation directory. Usually this is a directory in your user's home directory (user installs), or under `C:\Program Files` for system-wide installations.
2. In some installations, remove the entries added to the registry. System installs will use `HKEY*LOCAL*MACHINE` as the top level key; user installs will use `HKEY*CURRENT*USER`. You might find these items:
    - Uninstaller information: `TOP_LEVEL_KEY\Software\Microsoft\Windows\CurrentVersion\Uninstall\<INSTALLATION_NAME>`.
    - Python information: `TOP_LEVEL_KEY\Software\Python\PythonCore\<PYTHON_VERSION>`. Verify that these entries correspond to the installation directory you removed in step 1.
    - PATH modifications. You'll need to remove the entries corresponding to the installation directory, but leave the other locations intact. This is better handled via the UI available in the Control Panel (follow [these instructions](https://docs.oracle.com/en/database/oracle/machine-learning/oml4r/1.5.1/oread/creating-and-modifying-environment-variables-on-windows.html)). The actual registry keys (in case you are curious) are located in:
        - System install: `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment\PATH`.
        - User install: `HKEY_CURRENT_USER\Environment\PATH`
3. In some installations, remove the associated shortcuts under `%APPDATA%\Microsoft\Windows\Start Menu\Programs\` (user installs) or `%PROGRAMDATA%\Microsoft\Windows\Start Menu\Programs\` (system installs). You can enter these paths directly in the Windows Explorer address bar to open them.

### macOS and Linux

:::{note}
The following sections requires installers to be built using the `uninstall*with*conda_exe` option.
This is currently only implemented for `conda-standalone` 24.11.0 and higher.

For other installers, all files need to be removed manually.
:::

Unlike Windows, macOS and Linux installers do not ship an uninstaller executable.
However, some standalone applications (like `conda-standalone`) provide an uninstaller subcommand.
The following can be used to uninstall an existing installation:

```bash
$ $INSTDIR/_conda constructor uninstall --prefix $INSTDIR
```

where `$INSTDIR` is the installation directory. This command recursively removes all environments
and removes shell initializers that point to `$INSTDIR`.

The command supports additional options to delete files outside the installation directory:

- `--remove-caches`:
  Removes cache directories such as package caches and notices.
  Not recommended with multiple conda installations when softlinks are enabled.
- `--remove-config-files {user,system,all}`:
   Removes all configuration files such as `.condarc` files outside the installation directory.
   `user` removes the files inside the current user's home directory
   and `system` removes all files outside of that directory.
- `--remove-user-data`:
  This removes user data files such as the `~/.conda` directory.

These options are not recommended if multiple conda installations are on the same system because
they delete commonly used files.

:::{note}
If removing these files requires superuser privileges, use `sudo -E` instead of `sudo` since
finding these files may rely on environment variables, especially `$HOME`.
:::

For more detailed implementation notes, see the documentation of the standalone application:

* [conda-standalone](https://github.com/conda/conda-standalone)
