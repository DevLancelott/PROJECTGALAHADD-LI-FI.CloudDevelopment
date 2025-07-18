---
sidebar_label: 数据库
title: 数据库
description: "创建、删除数据库，查看、修改数据库参数"
---

## 创建数据库

```sql
CREATE DATABASE [IF NOT EXISTS] db_name [database_options];

database_options:
    database_option ...

database_option: {
    VGROUPS value
  | PRECISION {'ms' | 'us' | 'ns'}
  | REPLICA value
  | BUFFER value
  | PAGES value
  | PAGESIZE  value
  | CACHEMODEL {'none' | 'last_row' | 'last_value' | 'both'}
  | CACHESIZE value
  | COMP {0 | 1 | 2}
  | DURATION value
  | MAXROWS value
  | MINROWS value
  | KEEP value
  | KEEP_TIME_OFFSET value
  | STT_TRIGGER value
  | SINGLE_STABLE {0 | 1}
  | TABLE_PREFIX value
  | TABLE_SUFFIX value
  | DNODES value
  | TSDB_PAGESIZE value
  | WAL_LEVEL {1 | 2}
  | WAL_FSYNC_PERIOD value
  | WAL_RETENTION_PERIOD value
  | WAL_RETENTION_SIZE value
  | COMPACT_INTERVAL value
  | COMPACT_TIME_RANGE value
  | COMPACT_TIME_OFFSET value
}
```

### 参数说明

#### VGROUPS
数据库中初始 vgroup 的数目。
#### PRECISION
数据库的时间戳精度。
  - ms 表示毫秒（默认值）。
  - us 表示微秒。
  - ns 表示纳秒。
#### REPLICA
表示数据库副本数，取值为 1、2 或 3，默认为 1; 2 仅在企业版 3.3.0.0 及以后版本中可用。在集群中使用时，副本数必须小于或等于 DNODE 的数目。且使用时存在以下限制：
  - 暂不支持对双副本数据库相关 Vgroup 进行 SPLIT VGROUP 或 REDISTRIBUTE VGROUP 操作
  - 单副本数据库可变更为双副本数据库，但不支持从双副本变更为其它副本数，也不支持从三副本变更为双副本。
#### BUFFER
一个 vnode 写入内存池大小，单位为 MB，默认为 256，最小为 3，最大为 16384。
#### PAGES
一个 vnode 中元数据存储引擎的缓存页个数，默认为 256，最小 64。一个 vnode 元数据存储占用 PAGESIZE \* PAGES，默认情况下为 1MB 内存。
#### PAGESIZE
一个 vnode 中元数据存储引擎的页大小，单位为 KB，默认为 4 KB。范围为 1 到 16384，即 1 KB 到 16 MB。
#### CACHEMODEL
表示是否在内存中缓存子表的最近数据。
  - none：表示不缓存（默认值）。
  - last*row：表示缓存子表最近一行数据。这将显著改善 LAST*ROW 函数的性能表现。
  - last_value：表示缓存子表每一列的最近的非 NULL 值。这将显著改善无特殊影响（WHERE、ORDER BY、GROUP BY、INTERVAL）下的 LAST 函数的性能表现。
  - both：表示同时打开缓存最近行和列功能。
    Note：CacheModel 值来回切换有可能导致 last/last_row 的查询结果不准确，请谨慎操作（推荐保持打开）。
#### CACHESIZE
表示每个 vnode 中用于缓存子表最近数据的内存大小。默认为 1，范围是[1, 65536]，单位是 MB。
#### COMP
表示数据库文件压缩标志位，缺省值为 2，取值范围为 [0, 2]。
  - 0：表示不压缩。
  - 1：表示一阶段压缩。
  - 2：表示两阶段压缩。
#### DURATION
数据文件存储数据的时间跨度。缺省值为 10d，取值范围 [60m, 3650d]
  - 可以使用加单位的表示形式，如 DURATION 100h、DURATION 10d 等，支持 m（分钟）、h（小时）和 d（天）三个单位。
  - 不加时间单位时默认单位为天，如 DURATION 50 表示 50 天。
#### MAXROWS
文件块中记录的最大条数，默认为 4096 条。
#### MINROWS
文件块中记录的最小条数，默认为 100 条。
#### KEEP
表示数据文件保存的天数，缺省值为 3650，取值范围 [1, 365000]，且必须大于或等于 3 倍的 DURATION 参数值。
  - 数据库会自动删除保存时间超过 KEEP 值的数据从而释放存储空间；
  - KEEP 可以使用加单位的表示形式，如 KEEP 100h、KEEP 10d 等，支持 m（分钟）、h（小时）和 d（天）三个单位；
  - 也可以不写单位，如 KEEP 50，此时默认单位为天；
  - 仅企业版支持[多级存储](https://docs.taosdata.com/operation/planning/#%E5%A4%9A%E7%BA%A7%E5%AD%98%E5%82%A8)功能，因此，可以设置多个保存时间（多个以英文逗号分隔，最多 3 个，满足 keep 0 \<= keep 1 \<= keep 2，如 KEEP 100h,100d,3650d）；
  - 社区版不支持多级存储功能（即使配置了多个保存时间，也不会生效，KEEP 会取最大的保存时间）；
  - 了解更多，请点击 [关于主键时间戳](https://docs.taosdata.com/reference/taos-sql/insert/)。
#### KEEP*TIME*OFFSET
删除或迁移保存时间超过 KEEP 值的数据的延迟执行时间（自 3.2.0.0 版本生效），默认值为 0 (小时)。
  - 在数据文件保存时间超过 KEEP 后，删除或迁移操作不会立即执行，而会额外等待本参数指定的时间间隔，以实现与业务高峰期错开的目的。
#### STT_TRIGGER
表示落盘文件触发文件合并的个数。
  - 对于少表高频写入场景，此参数建议使用默认配置；
  - 而对于多表低频写入场景，此参数建议配置较大的值。
#### SINGLE_STABLE
表示此数据库中是否只可以创建一个超级表，用于超级表列非常多的情况。
  - 0：表示可以创建多张超级表。
  - 1：表示只可以创建一张超级表。
#### TABLE_PREFIX
分配数据表到某个 vgroup 时，用于忽略或仅使用表名前缀的长度值。
  - 当其为正值时，在决定把一个表分配到哪个 vgroup 时要忽略表名中指定长度的前缀；
  - 当其为负值时，在决定把一个表分配到哪个 vgroup 时只使用表名中指定长度的前缀；
  - 例如：假定表名为 "v30001"，当 TSDB*PREFIX = 2 时，使用 "0001" 来决定分配到哪个 vgroup，当 TSDB*PREFIX = -2 时使用 "v3" 来决定分配到哪个 vgroup。
#### TABLE_SUFFIX
分配数据表到某个 vgroup 时，用于忽略或仅使用表名后缀的长度值。
  - 当其为正值时，在决定把一个表分配到哪个 vgroup 时要忽略表名中指定长度的后缀；
  - 当其为负值时，在决定把一个表分配到哪个 vgroup 时只使用表名中指定长度的后缀；
  - 例如：假定表名为 "v30001"，当 TSDB*SUFFIX = 2 时，使用 "v300" 来决定分配到哪个 vgroup，当 TSDB*SUFFIX = -2 时使用 "01" 来决定分配到哪个 vgroup。
#### TSDB_PAGESIZE
一个 vnode 中时序数据存储引擎的页大小，单位为 KB，默认为 4 KB。范围为 1 到 16384，即 1 KB 到 16 MB。
#### DNODES
指定 vnode 所在的 DNODE 列表，如 '1,2,3'，以逗号区分且字符间不能有空格（**仅企业版支持**）
#### WAL_LEVEL
WAL 级别，默认为 1。
  - 1：写 WAL，但不执行 fsync。
  - 2：写 WAL，而且执行 fsync。
#### WAL*FSYNC*PERIOD
当 WAL_LEVEL 参数设置为 2 时，用于设置落盘的周期。默认为 3000，单位毫秒。最小为 0，表示每次写入立即落盘；最大为 180000，即三分钟。
#### WAL*RETENTION*PERIOD
为了数据订阅消费，需要 WAL 日志文件额外保留的最大时长策略。WAL 日志清理，不受订阅客户端消费状态影响。单位为 s。默认为 3600，表示在 WAL 保留最近 3600 秒的数据，请根据数据订阅的需要修改这个参数为适当值。
#### WAL*RETENTION*SIZE
为了数据订阅消费，需要 WAL 日志文件额外保留的最大累计大小策略。单位为 KB。默认为 0，表示累计大小无上限。
#### COMPACT_INTERVAL
自动 compact 触发周期（从 1970-01-01T00:00:00Z 开始切分的时间周期）（**企业版 v3.3.5.0 开始支持**）。
  - 取值范围：0 或 [10m, keep2]，单位：m（分钟），h（小时），d（天）；
  - 不加时间单位默认单位为天，默认值为 0，即不触发自动 compact 功能；
  - 如果 db 中有未完成的 compact 任务，不重复下发 compact 任务。
#### COMPACT*TIME*RANGE
自动 compact 任务触发的 compact 时间范围（**企业版 v3.3.5.0 开始支持**）。
  - 取值范围：[-keep2, -duration]，单位：m（分钟），h（小时），d（天）；
  - 不加时间单位时默认单位为天，默认值为 [0, 0]；
  - 取默认值 [0, 0] 时，如果 COMPACT_INTERVAL 大于 0，会按照 [-keep2, -duration] 下发自动 compact；
  - 因此，要关闭自动 compact 功能，需要将 COMPACT_INTERVAL 设置为 0。
#### COMPACT*TIME*OFFSET
自动 compact 任务触发的 compact 时间相对本地时间的偏移量（**企业版 v3.3.5.0 开始支持**）。取值范围：[0, 23]，单位：h（小时），默认值为 0。以 UTC 0 时区为例：
  - 如果 COMPACT*INTERVAL 为 1d，当 COMPACT*TIME_OFFSET 为 0 时，在每天 0 点下发自动 compact；
  - 如果 COMPACT*TIME*OFFSET 为 2，在每天 2 点下发自动 compact。

### 创建数据库示例

```sql
create database if not exists db vgroups 10 buffer 10;
```

以上示例创建了一个有 10 个 vgroup 名为 db 的数据库，其中每个 vnode 分配 10MB 的写入缓存

### 使用数据库

```sql
USE db_name;
```

使用/切换数据库（在 REST 连接方式下无效）。

## 删除数据库

```sql
DROP DATABASE [IF EXISTS] db_name;
```

删除数据库。指定 Database 所包含的全部数据表将被删除，该数据库的所有 vgroups 也会被全部销毁，请谨慎使用！

## 修改数据库参数

```sql
ALTER DATABASE db_name [alter_database_options]

alter_database_options:
    alter_database_option ...

alter_database_option: {
    CACHEMODEL {'none' | 'last_row' | 'last_value' | 'both'}
  | CACHESIZE value
  | BUFFER value
  | PAGES value
  | REPLICA value
  | STT_TRIGGER value
  | WAL_LEVEL value
  | WAL_FSYNC_PERIOD value
  | KEEP value
  | WAL_RETENTION_PERIOD value
  | WAL_RETENTION_SIZE value
  | MINROWS value
  | COMPACT_INTERVAL value
  | COMPACT_TIME_RANGE value
  | COMPACT_TIME_OFFSET value  
}
```

### 修改 CACHESIZE

修改数据库参数的命令使用简单，难的是如何确定是否需要修改以及如何修改。本小节描述如何判断数据库的 cachesize 是否够用。

1. 如何查看 cachesize?

通过 select * from information*schema.ins*databases; 可以查看这些 cachesize 的具体值（单位：MB，兆）。

2. 如何查看 cacheload?

通过 show \<db_name>.vgroups; 可以查看 cacheload（单位：Byte，字节）。

3. 判断 cachesize 是否够用

- 如果 cacheload 非常接近 cachesize，则 cachesize 可能过小。
- 如果 cacheload 明显小于 cachesize 则 cachesize 是够用的。
- 可以根据这个原则判断是否需要修改 cachesize。
- 具体修改值可以根据系统可用内存情况来决定是加倍或者是提高几倍。

:::note
其它参数在 3.0.0.0 中暂不支持修改

:::

## 查看数据库

### 查看系统中的所有数据库

```sql
SHOW DATABASES;
```

### 显示一个数据库的创建语句

```sql
SHOW CREATE DATABASE db_name \G;
```

常用于数据库迁移。对一个已经存在的数据库，返回其创建语句；在另一个集群中执行该语句，就能得到一个设置完全相同的 Database。

### 查看数据库参数

```sql
SELECT * FROM INFORMATION_SCHEMA.INS_DATABASES WHERE NAME='db_name' \G;
```

会列出指定数据库的配置参数，并且每行只显示一个参数。

## 删除过期数据

```sql
TRIM DATABASE db_name;
```

删除过期数据，并根据多级存储的配置归整数据。

## 落盘内存数据

```sql
FLUSH DATABASE db_name;
```

落盘内存中的数据。在关闭节点之前，执行这条命令可以避免重启后的预写数据日志回放，加速启动过程。

## 调整 VGROUP 中 VNODE 的分布

```sql
REDISTRIBUTE VGROUP vgroup_no DNODE dnode_id1 [DNODE dnode_id2] [DNODE dnode_id3];
```

按照给定的 dnode 列表，调整 vgroup 中的 vnode 分布。因为副本数目最大为 3，所以最多输入 3 个 dnode。

## 自动调整 VGROUP 中 LEADER 的分布

```sql
BALANCE VGROUP LEADER;
```

触发集群所有 vgroup 中的 leader 重新选主，对集群各节点进行负载均衡操作。（**企业版功能**）

## 查看数据库工作状态

```sql
SHOW db_name.ALIVE;
```

查询数据库 db_name 的可用状态（返回值）：
- 0：不可用；
- 1：完全可用；
- 2：部分可用（即数据库包含的 VNODE 部分节点可用，部分节点不可用）。

## 查看 DB 的磁盘空间占用

```sql 
select * from  INFORMATION_SCHEMA.INS_DISK_USAGE where db_name = 'db_name';
```  
查看 DB 各个模块所占用磁盘的大小。

```sql
SHOW db_name.disk_info;
```
查看数据库 db_name 的数据压缩压缩率和数据在磁盘上所占用的大小。

该命令本质上等同于： `select sum(data1 + data2 + data3)/sum(raw*data), sum(data1 + data2 + data3) from information*schema.ins*disk*usage where db_name="dbname";`。

