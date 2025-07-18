---
sidebar_label: 容错与灾备
title: 容错与灾备
toc*max*heading_level: 4
---

为了防止数据丢失、误删操作，TDengine 提供全面的数据备份、恢复、容错、异地数据实时同步等功能，以保证数据存储的安全。本节简要说明 TDengine 中的容错与灾备。

## 容错

TDengine 支持 WAL 机制，实现数据的容错能力，保证数据的高可靠。TDengine 接收到应用程序的请求数据包时，会先将请求的原始数据包写入数据库日志文件，等数据成功写入数据库数据文件后，再删除相应的 WAL。这样保证了 TDengine 能够在断电等因素导致的服务重启时，从数据库日志文件中恢复数据，避免数据丢失。涉及的配置参数有如下两个：

- wal_level：WAL 级别，1 表示写 WAL，但不执行 fsync；2 表示写 WAL，而且执行 fsync。默认值为 1。
- wal*fsync*period：当 wal*level 设置为 2 时，执行 fsync 的周期；当 wal*fsync_period 设置为 0 时，表示每次写入，立即执行 fsync。

如果要 100% 保证数据不丢失，则需要将 wal*level 设置为 2，wal*fsync*period 设置为 0。这时写入速度将会下降。但如果应用程序侧启动的写数据的线程数达到一定的数量（超过 50），那么写入数据的性能也会很不错，只会比 wal*fsync_period 设置为 3000ms 下降 30% 左右。

## 灾备

在异地的两个数据中心中部署两个 TDengine Enterprise 集群，利用其数据复制能力即可实现数据灾备。假定两个集群为集群 A 和集群 B，其中集群 A 为源集群，承担写入请求并提供查询服务。集群 B 可以实时消费集群 A 中新写入的数据，并同步到集群 B。如果发生了灾难，导致集群 A 所在数据中心不可用，可以启用集群 B 作为数据写入和查询的主节点。

以下步骤描述了如何轻松在两个 TDengine Enterprise 集群之间搭建数据灾备体系：

- 第 1 步，在集群 A 中创建一个数据库 db1，并向该数据库持续写入数据。

- 第 2 步，通过 Web 浏览器访问集群 A 的 taosExplorer 服务，访问地址通常 为 TDengine 集群所在 IP 地址的端口 6060，如 `http://localhost:6060`。

- 第 3 步，访问 TDengine 集群 B，创建一个与集群 A 中数据库 db1 参数配置相同的数据库 db2。

- 第 4 步，通过 Web 浏览器访问集群 B 的 taosExplorer 服务，在“数据浏览器”页面找到 db2，在“查看数据库配置”选项中可以获取该数据库的 DSN，例如 `taos+ws://root:taosdata@clusterB:6041/db2`

- 第 5 步，在 taosExplorer 服务的“系统管理 - 数据同步”页面新增一个数据同步任务，在任务配置信息中填写需要同步的数据库 db1 和目标数据库 db2 的 DSN，完成创建任务后即可启动数据同步。

- 第 6 步，访问集群 B，可以看到集群 B 中的数据库 db2 源源不断写入来自集群 A 数据库 db1 的数据，直至两个集群的数据库数据量基本保持一致。至此，一个简单的基于 TDengine Enterprise 的数据灾备体系搭建完成。