# ESP-IDF 版本与乐鑫芯片版本兼容性

* [English Version](./COMPATIBILITY.md)

为不断提高芯片性能，乐鑫会为其芯片发布新版本。但新芯片版本中的某些性能提升依赖特殊的软件支持，有时候新芯片版本必须在一定的软件版本下才能正常运行。

本文档介绍了具体 ESP-IDF 版本与乐鑫芯片版本之间的兼容性情况。

注意：各分支上的兼容性文档可能不是最新版本，请参考 [master 分支上的兼容性文件](https://github.com/espressif/esp-idf/blob/master/COMPATIBILITY_CN.md) 以获取最新信息。

有关乐鑫芯片版本的编码方式，请参考 [关于芯片版本 (Chip Revision) 编码方式的兼容性公告](https://www.espressif.com/sites/default/files/advisory_downloads/AR2022-005%20%E5%85%B3%E4%BA%8E%E8%8A%AF%E7%89%87%E7%89%88%E6%9C%AC%E7%BC%96%E7%A0%81%E6%96%B9%E5%BC%8F%20%28Chip%20Revision%29%20%E7%9A%84%E5%85%BC%E5%AE%B9%E6%80%A7%E5%85%AC%E5%91%8A.pdf)。

运行 `esptool chip_id` 可查看芯片系列及其版本。有关区分芯片版本及版本改进内容的更多信息，请参考 [芯片勘误表](https://www.espressif.com.cn/zh-hans/support/documents/technical-documents?keys=%E5%8B%98%E8%AF%AF%E8%A1%A8)。运行 `idf.py --version` 可查看当前的 ESP-IDF 版本。

## ESP-IDF 对各芯片版本的支持

下文介绍了 ESP-IDF 对各芯片版本的支持情况，每个芯片版本都有对应的 ESP-IDF `推荐版本` 和 `需求版本`：

- `推荐版本`：表示从该版本的 ESP-IDF 开始，软件可以利用芯片版本提升的性能。如果在该芯片版本上运行低于 `推荐版本` 的 ESP-IDF 来编译二进制文件，软件可能无法利用该芯片版本修复的错误或新增的功能，芯片行为将与其上一版本几乎相同。

- `需求版本`：表示该芯片版本正常运行所需的最低 ESP-IDF 版本。如果在该芯片版本上运行低于 `需求版本` 的 ESP-IDF 来编译二进制文件，可能会出现不可预测的芯片行为。

即便使用的软件版本已高于该芯片版本的对应 `推荐版本`，软件已经能够利用该芯片版本的所有功能，我们仍建议用户升级到该发布分支的最新 bugfix 版本。新的 bugfix 版本修复了一些问题，有助于提升产品稳定性。

例如，对于某一芯片版本，其 `release/v5.1` 分支的 `需求版本` 和 `推荐版本` 分别是 `v5.1.2` 和 `v5.1.4`，而该分支的最新版本是 `v5.1.6`。那么，在使用 ESP-IDF `v5.1` - `v5.1.1` 时，芯片将无法启动，或会出现不可预测的行为，而在使用 ESP-IDF `v5.1.2` 或 `v5.1.3` 时，应用程序可能无法使用芯片的部分性能。此外，虽然 `v5.1.4` 已支持该芯片版本，但仍建议将 ESP-IDF 升级到 `v5.1.6`。

### ESP32

#### v0.0、v1.0 和 v3.0

从最初版本的 ESP-IDF 开始支持。

#### v1.1

待更新。

#### v2.0

待更新。

#### v3.1

待更新。

### ESP32-S2

#### v0.0

从 ESP-IDF v4.2 开始支持。

#### v1.0

| 发布分支                | 推荐版本     | 需求版本 |
|------------------------|-------------|----------|
| release/v4.2           | v4.2.3+     | v4.2     |
| release/v4.3           | v4.3.3+     | v4.3     |
| release/v4.4           | v4.4.6+     | v4.4     |
| release/v5.0           | v5.0.4+     | v5.0     |
| release/v5.1           | v5.1.2+     | v5.1     |
| release/v5.2 及以上     | v5.2+       | v5.2     |

### ESP32-C3

#### v0.2 - v0.4

从 ESP-IDF v4.3 开始支持。

#### v1.1

| 发布分支                | 推荐版本      | 需求版本  |
|------------------------|-------------|----------|
| release/v4.2           | EOL         | EOL      |
| release/v4.3           | v4.3.7+     | v4.3.7   |
| release/v4.4           | v4.4.7+     | v4.4.7   |
| release/v5.0           | v5.0.5+     | v5.0.5   |
| release/v5.1           | v5.1.3+     | v5.1.3   |
| release/v5.2 及以上     | v5.2+       | v5.2     |

### ESP32-S3

#### v0.1, v0.2

从 ESP-IDF v4.4 开始支持。

### ESP32-C2 & ESP8684

#### v1.0, v1.1

从 ESP-IDF v5.0 开始支持。

#### v1.2

| 发布分支                | 推荐版本      | 需求版本  |
|------------------------|-------------|----------|
| release/v5.0           | v5.0.7+     | v5.0     |
| release/v5.1           | v5.1.4+     | v5.1     |
| release/v5.2           | v5.2.2+     | v5.1     |
| release/v5.3 及以上     | v5.3+       | v5.3     |

#### v2.0

| 发布分支                | 推荐版本      | 需求版本  |
|------------------------|-------------|----------|
| release/v5.0           | v5.0.8+     | v5.0.8   |
| release/v5.1           | v5.1.5+     | v5.1.5*  |
| release/v5.2           | v5.2.4+     | v5.2.4   |
| release/v5.3           | v5.3.2+     | v5.3.2*  |
| release/v5.4 及以上     | v5.4+       | v5.4     |

提示: IDF v5.1.5 及 v5.3.2 与 C2 v2.0 兼容，但芯片版本检查尚未在这些发布版本更新。使能 `ESP32C2*REV2*DEVELOPMENT` 选项来跳过这些过时的检查。

### ESP32-C6

#### v0.0, v0.1

从 ESP-IDF v5.1 开始支持。

#### v0.2

| 发布分支                | 推荐版本      | 需求版本  |
|------------------------|-------------|----------|
| release/v5.1           | v5.1.5+     | v5.1     |
| release/v5.2           | v5.2.4+     | v5.2     |
| release/v5.3           | v5.3.2+     | v5.3     |
| release/v5.4 及以上     | v5.4+       | v5.4     |

### ESP32-H2

#### v0.1, v0.2

从 ESP-IDF v5.1 开始支持。

#### v1.2

| 发布分支                | 推荐版本      | 需求版本  |
|------------------------|-------------|----------|
| release/v5.1           | v5.1.6+     | v5.1.6   |
| release/v5.2           | v5.2.5+     | v5.2.5   |
| release/v5.3           | v5.3.3+     | v5.3.3   |
| release/v5.4           | v5.4.1+     | v5.4.1   |
| release/v5.5 及以上     | v5.5+       | v5.5     |


## 如果 ESP-IDF 版本低于 `需求版本` 会出现什么情况？

使用最新的 ESP-IDF 版本时，软件会阻止下载二进制文件到不支持的芯片版本上，甚至可以防止二进制文件在不支持的芯片版本上被执行。v4.4.5+、v5.0.1+、v5.1 及以上版本的 ESP-IDF 都支持针对芯片版本的 esptool 下载检查和引导加载器加载检查，但 ESP-IDF v4.3.5 只支持 esptool 下载检查。

更早的 ESP-IDF 版本没有此类检查，若与芯片版本不兼容，芯片运行软件时可能会出现不可预测的行为。
