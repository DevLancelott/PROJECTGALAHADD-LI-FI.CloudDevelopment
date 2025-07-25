---
sidebar_label: 产品简介
title: TDengine 产品简介
toc*max*heading_level: 4
---

TDengine 是一个高性能、分布式的时序数据库。通过集成的缓存、数据订阅、流计算和数据清洗与转换等功能，TDengine 已经发展成为一个专为物联网、工业互联网、金融和 IT 运维等关键行业量身定制的时序大数据平台。该平台能够高效地汇聚、存储、分析、计算和分发来自海量数据采集点的大规模数据流，每日处理能力可达 TB 乃至 PB 级别。借助 TDengine，企业可以实现实时的业务监控和预警，进而发掘出有价值的商业洞察。

自 2019 年 7 月 以来，涛思数据陆续将 TDengine 的不同版本开源，包括单机版（2019 年 7 月）、集群版（2020 年 8 月）以及云原生版（2022 年 8 月）。开源之后，TDengine 迅速获得了全球开发者的关注，多次在 GitHub 网站全球趋势排行榜上位居榜首，最新的关注热度见 [涛思数据首页](https://www.taosdata.com/)。

## TDengine 产品

为满足不同用户的需求和场景，涛思数据推出 TDengine 系列产品，包括开源版 TDengine OSS、企业版 TDengine Enterprise 以及云服务 TDengine Cloud。

TDengine OSS 是一个开源的高性能时序数据库，与其他时序数据库相比，它的核心优势在于其集群开源、高性能和云原生架构。而且除了基础的写入、查询和存储功能外，TDengine OSS 还集成了缓存、流式计算和数据订阅等高级功能，这些功能显著简化了系统设计，降低了企业的研发和运营成本。

在 TDengine OSS 的基础上，TDengine Enterprise 提供了增强的辅助功能，包括数据的备份恢复、异地容灾、多级存储、视图、权限控制、安全加密、IP 白名单、支持 MQTT、OPC-UA、OPC-DA、PI、Wonderware、Kafka 等各种数据源。这些功能为企业提供了更为全面、安全、可靠和高效的时序数据管理解决方案。更多的细节请看 [TDengine Enterprise](https://www.taosdata.com/tdengine-pro)。

此外，TDengine Cloud 作为一种全托管的云服务，存储与计算分离，分开计费，为企业提供了企业级的工具和服务，彻底解决了运维难题，尤其适合中小规模的用户使用。更多的细节请看 [TDengine 云服务](https://cloud.taosdata.com/?utm*source=menu&utm*medium=webcn)。

## TDengine 主要功能与特性

TDengine 经过特别优化，以适应时间序列数据的独特需求，引入了“一个数据采集点一张表”和“超级表”的创新数据组织策略。这些策略背后的支撑是一个革命性的存储引擎，它极大地提升了数据处理的速度和效率，无论是在数据的写入、查询还是存储方面。接下来，逐一探索 TDengine 的众多功能，帮助您全面了解这个为高效处理时间序列数据而生的大数据平台。

1. 写入数据：TDengine 支持多种数据写入方式。首先，它完全兼容 SQL，允许用户使用标准的 SQL 语法进行数据写入。而且 TDengine 还支持无模式（Schemaless）写入，包括流行的 InfluxDB Line 协议、OpenTSDB 的 Telnet 和 JSON 协议，这些协议的加入使得数据的导入变得更加灵活和高效。更进一步，TDengine 与众多第三方工具实现了无缝集成，例如 Telegraf、Prometheus、EMQX、StatsD、collectd 和 HiveMQ 等。在 TDengine Enterprise 中，还提供了 MQTT、OPC-UA、OPC-DA、PI、Wonderware、Kafka、InfluxDB、OpenTSDB、MySQL、Oracle 和 SQL Server 等连接器。这些工具通过简单的配置，无需一行代码，就可以将来自各种数据源的数据源源不断的写入数据库，极大地简化了数据收集和存储的过程。

2. 查询数据：TDengine 提供标准的 SQL 查询语法，并针对时序数据和业务的特点优化和新增了许多语法和功能，例如降采样、插值、累计求和、时间加权平均、状态窗口、时间窗口、会话窗口、滑动窗口等。TDengine 还支持用户自定义函数（UDF）。

3. 缓存：TDengine 使用时间驱动缓存管理策略（First-In-First-Out，FIFO），将最近到达的（当前状态）数据保存在缓存中，这样便于获取任何监测对象的实时状态，而无需使用 Redis 等其他缓存工具，简化系统架构和运营成本。

4. 流式计算：TDengine 流式计算引擎提供了实时处理写入的数据流的能力，不仅支持连续查询，还支持事件驱动的流式计算。它提供了替代复杂流处理系统的轻量级解决方案，并能够在高吞吐的数据写入的情况下，提供毫秒级的计算结果延迟。

5. 数据订阅：TDengine 提供了类似 Kafka 的数据订阅功能。但用户可以通过 SQL 来灵活控制订阅的数据内容，并使用和 Kafka 相同的 API 来订阅一张表、一组表、全部列或部分列、甚至整个数据库的数据。TDengine 可以替代需要集成消息队列产品的场景，从而简化系统设计的复杂度，降低运营维护成本。

6. 可视化/BI：TDengine 本身不提供可视化或 BI 的功能。但通过其 RESTful API，标准的 JDBC、ODBC 接口，TDengine 能够和 Grafana、Google Looker Studio、Power BI、Tableau 以及国产 BI 工具无缝集成。

7. 集群功能：TDengine 支持集群部署，能够随着业务数据量的增长，通过增加节点线性提升系统处理能力，实现水平扩展。同时，通过多副本技术提供高可用性，支持 Kubernetes 部署，提供了多种运维工具，方便系统管理员更好地管理和维护集群的健壮运行。

8. 数据迁移：TDengine 提供了多种便捷的数据导入导出功能，包括脚本文件导入导出、数据文件导入导出、taosdump 工具导入导出等。

9. 编程连接器：TDengine 提供多种语言的连接器，包括 C/C++、Java、Go、Node.js、Rust、Python、C#、R、PHP 等。这些连接器大多都支持原生连接和 WebSocket 两种连接方式。TDengine 也提供 RESTful 接口，任何语言的应用程序可以直接通过 HTTP 请求访问数据库。

10. 数据安全：TDengine 提供了丰富的用户管理和权限管理功能以控制不同用户对数据库和表的访问权限，提供了 IP 白名单功能以控制不同帐号只能从特定的服务器接入集群。TDengine 支持系统管理员对不同数据库按需加密，数据加密后对读写完全透明且对性能的影响很小。还提供了审计日志功能以记录系统中的敏感操作。

11. 常用工具：TDengine 提供了交互式命令行程序（CLI），便于管理集群、检查系统状态、做即时查询。压力测试工具 taosBenchmark，用于测试 TDengine 的性能。TDengine 还提供了图形化管理界面，简化了操作和管理过程。

12. 零代码数据接入：TDengine 企业版提供了丰富的数据接入功能，依托强大的数据接入平台，无需一行代码，只需要做简单的配置即可实现多种数据源的数据接入，目前已经支持的数据源包括：OPC-UA、OPC-DA、PI、MQTT、Kafka、InfluxDB、OpenTSDB、MySQL、SQL Server、Oracle、Wonderware Historian、MongoDB。

## TDengine 与典型时序数据库的区别

由于充分利用了时序数据特点，并采用独特创新的“一个数据采集点一张表” “超级表”的属于模型，与其他时序数据库相比，TDengine 拥有以下特点：

1. 高性能：TDengine 通过创新的存储引擎设计，实现了数据写入和查询性能的超群，速度比通用数据库快 10 倍以上，也远超过其他时序数据库。同时，其存储空间需求仅为通用数据库的 1/10，极大地提高了资源利用效率。

2. 云原生：TDengine 采用原生分布式设计，充分利用云平台的优势，提供了水平扩展能力。它具备弹性、韧性和可观测性，支持 Kubernetes 部署，并可在公有云、私有云和混合云上灵活运行。

3. 极简时序数据平台：TDengine 内置了消息队列、缓存、流式计算等功能，避免了应用集成 Kafka、Redis、HBase、Spark 等软件的复杂性，从而大幅降低系统的复杂度和应用开发及运营成本。

4. 强大的分析能力：TDengine 不仅支持标准 SQL 查询，还为时序数据特有的分析提供了 SQL 扩展。通过超级表、存储计算分离、分区分片、预计算、UDF 等先进技术，TDengine 展现出强大的数据分析能力。

5. 简单易用：TDengine 安装无依赖，集群部署仅需几秒即可完成。它提供了 RESTful 接口和多种编程语言的连接器，与众多第三方工具无缝集成。此外，命令行程序和丰富的运维工具也极大地方便了用户的管理和即时查询需求。

6. 核心开源：TDengine 的核心代码，包括集群功能，均在开源协议下公开发布。它在 GitHub 网站全球趋势排行榜上多次位居榜首，显示出其受欢迎程度。同时，TDengine 拥有一个活跃的开发者社区，为技术的持续发展和创新提供了有力支持。

采用 TDengine，企业可以在物联网、车联网、工业互联网等典型场景中显著降低大数据平台的总拥有成本，主要体现在以下几个方面：

1. 高性能带来的成本节约：TDengine 卓越的写入、查询和存储性能意味着系统所需的计算资源和存储资源可以大幅度减少。这不仅降低了硬件成本，还减少了能源消耗和维护费用。

2. 标准化与兼容性带来的成本效益：由于 TDengine 支持标准 SQL，并与众多第三方软件实现了无缝集成，用户可以轻松地将现有系统迁移到 TDengine 上，无须重写大量代码。这种标准化和兼容性大大降低了学习和迁移成本，缩短了项目周期。

3. 简化系统架构带来的成本降低：作为一个极简的时序数据平台，TDengine 集成了消息队列、缓存、流计算等必要功能，避免了额外集成众多其他组件的需要。这种简化的系统架构显著降低了系统的复杂度，从而减少了研发和运营成本，提高了整体运营效率。

## 技术生态

在整个时序大数据平台中，TDengine 扮演的角色如下：

<figure>

![TDengine Database 技术生态图](eco_system.webp)

<center><figcaption>图 1. TDengine 技术生态图</figcaption></center>
</figure>

上图中，左侧是各种数据采集或消息队列，包括 OPC-UA、MQTT、Telegraf、也包括 Kafka，它们的数据将被源源不断的写入到 TDengine。右侧是可视化、BI 工具、组态软件、应用程序。下侧是 TDengine 自身提供的命令行程序（CLI）以及可视化管理工具。

## 典型适用场景

作为一个高性能、分布式、支持 SQL 的时序数据库（Time-series Database），TDengine 的典型适用场景包括但不限于 IoT、工业互联网、车联网、IT 运维、能源、金融证券等领域。需要指出的是，TDengine 是针对时序数据场景设计的专用数据库和专用大数据处理工具，因其充分利用了时序大数据的特点，它无法用来处理网络爬虫、微博、微信、电商、ERP、CRM 等通用型数据。下面本文将对适用场景做更多详细的分析。

### 数据源特点和需求

从数据源角度，设计人员可以从下面几个角度分析 TDengine 在目标应用系统里面的适用性。

| 数据源特点和需求             | 不适用 | 可能适用 | 非常适用 | 简单说明                                                                                                                        |
| ---------------------------- | ------ | -------- | -------- | ------------------------------------------------------------------------------------------------------------------------------- |
| 总体数据量巨大               |        |          | √        | TDengine 在容量方面提供出色的水平扩展功能，并且具备匹配高压缩的存储结构，达到业界最优的存储效率。                               |
| 数据输入速度偶尔或者持续巨大 |        |          | √        | TDengine 的性能大大超过同类产品，可以在同样的硬件环境下持续处理大量的输入数据，并且提供很容易在用户环境里面运行的性能评估工具。 |
| 数据源数目巨大               |        |          | √        | TDengine 设计中包含专门针对大量数据源的优化，包括数据的写入和查询，尤其适合高效处理海量（千万或者更多量级）的数据源。           |

### 系统架构要求

| 系统架构要求           | 不适用 | 可能适用 | 非常适用 | 简单说明                                                                                              |
| ---------------------- | ------ | -------- | -------- | ----------------------------------------------------------------------------------------------------- |
| 要求简单可靠的系统架构 |        |          | √        | TDengine 的系统架构非常简单可靠，自带消息队列，缓存，流式计算，监控等功能，无需集成额外的第三方产品。 |
| 要求容错和高可靠       |        |          | √        | TDengine 的集群功能，自动提供容错灾备等高可靠功能。                                                   |
| 标准化规范             |        |          | √        | TDengine 使用标准的 SQL 语言提供主要功能，遵守标准化规范。                                            |

### 系统功能需求

| 系统功能需求               | 不适用 | 可能适用 | 非常适用 | 简单说明                                                                                                                  |
| -------------------------- | ------ | -------- | -------- | ------------------------------------------------------------------------------------------------------------------------- |
| 要求完整的内置数据处理算法 |        | √        |          | TDengine 实现了通用的数据处理算法，但是还没有做到妥善处理各行各业的所有需求，因此特殊类型的处理需求还需要在应用层面解决。 |
| 需要大量的交叉查询处理     |        | √        |          | 这种类型的处理更多应该用关系型数据库处理，或者应该考虑 TDengine 和关系型数据库配合实现系统功能。                          |

### 系统性能需求

| 系统性能需求           | 不适用 | 可能适用 | 非常适用 | 简单说明                                                                                           |
| ---------------------- | ------ | -------- | -------- | -------------------------------------------------------------------------------------------------- |
| 要求较大的总体处理能力 |        |          | √        | TDengine 的集群功能可以轻松地让多服务器配合达成处理能力的提升。                                    |
| 要求高速处理数据       |        |          | √        | TDengine 专门为 IoT 优化的存储和数据处理设计，一般可以让系统得到超出同类产品多倍数的处理速度提升。 |
| 要求快速处理小粒度数据 |        |          | √        | 这方面 TDengine 性能可以完全对标关系型和 NoSQL 型数据处理系统。                                    |

### 系统维护需求

| 系统维护需求           | 不适用 | 可能适用 | 非常适用 | 简单说明                                                                                                              |
| ---------------------- | ------ | -------- | -------- | --------------------------------------------------------------------------------------------------------------------- |
| 要求系统可靠运行       |        |          | √        | TDengine 的系统架构非常稳定可靠，日常维护也简单便捷，对维护人员的要求简洁明了，最大程度上杜绝人为错误和事故。         |
| 要求运维学习成本可控   |        |          | √        | 同上。                                                                                                                |
| 要求市场有大量人才储备 | √      |          |          | TDengine 作为新一代产品，目前人才市场里面有经验的人员还有限。但是学习成本低，我们作为厂家也提供运维的培训和辅助服务。 |

## 与其他数据库的对比测试

- [用 InfluxDB 开源的性能测试工具对比 InfluxDB 和 TDengine](https://www.taosdata.com/blog/2020/01/13/1105.html)
- [TDengine 与 OpenTSDB 对比测试](https://www.taosdata.com/blog/2019/08/21/621.html)
- [TDengine 与 Cassandra 对比测试](https://www.taosdata.com/blog/2019/08/14/573.html)
- [TDengine VS InfluxDB，写入性能大 PK！](https://www.taosdata.com/2021/11/05/3248.html)
- [TDengine 和 InfluxDB 查询性能对比测试报告](https://www.taosdata.com/2022/02/22/5969.html)
- [TDengine 与 InfluxDB、OpenTSDB、Cassandra、MySQL、ClickHouse 等数据库的对比测试报告](https://www.taosdata.com/downloads/TDengine*Testing*Report_cn.pdf)