---
title: Anode Management
sidebar_label: Anode Management
---

import PkgListV3 from "/components/PkgListV3";

### Starting the TDgpt Service

The `taosanoded` service is created when you install an anode. You can use systemd to manage this service:

```bash
systemctl start  taosanoded
systemctl stop   taosanoded
systemctl status taosanoded
```

### Starting a Time-Series Foundation Model

Time-series foundation models require significant hardware resources. For this reason, they are not started automatically. To start a time-series foundation model manually, use the following procedure:

```bash
# Start TDtsdm
start-tdtsfm

# Start Time-MoE
start-timer-moe
```

```bash
# Stop TDtsfm
stop-tdtsfm

# Stop Time-MoE
stop-timer-moe
```

### Directory and Configuration Information

The directory structure of an anode is described in the following table:

|Directory or File|Description|
|---------------|------|
|/usr/local/taos/taosanode/bin|Directory containing executable files|
|/usr/local/taos/taosanode/resource|Directory containing resource files, linked to `/var/lib/taos/taosanode/resource/`|
|/usr/local/taos/taosanode/lib|Directory containing libraries|
|/usr/local/taos/taosanode/model|Directory containing models, linked to `/var/lib/taos/taosanode/model`|
|/var/log/taos/taosanode/|Log directory|
|/etc/taos/taosanode.ini|Configuration file|

#### Configuration

The anode provides services through an uWSGI driver. The configuration for the anode and for uWSGI are both found in the `taosanode.ini` file, located by default in the `/etc/taos/` directory.
The configuration options are described as follows:

```ini
[uwsgi]

# Anode RESTful service ip:port
http = 127.0.0.1:6090

# base directory for Anode python files, do NOT modified this
chdir = /usr/local/taos/taosanode/lib

# initialize Anode python file
wsgi-file = /usr/local/taos/taosanode/lib/taos/app.py

# pid file
pidfile = /usr/local/taos/taosanode/taosanode.pid

# conflict with systemctl, so do NOT uncomment this
# daemonize = /var/log/taos/taosanode/taosanode.log

# uWSGI log files
logto = /var/log/taos/taosanode/taosanode.log

# uWSGI monitor port
stats = 127.0.0.1:8387

# python virtual environment directory, used by Anode
virtualenv = /usr/local/taos/taosanode/venv/

[taosanode]
# default taosanode log file
app-log = /var/log/taos/taosanode/taosanode.app.log

# model storage directory
model-dir = /usr/local/taos/taosanode/model/

# default log level
log-level = INFO

```

Note
Do not specify a value for the `daemonize` parameter. This parameter causes a conflict between uWSGI and systemctl. If you enable the `daemonize` parameter, your anode will fail to start.
The configuration file above includes only the basic configuration needed for an anode to provide services. For more information about configuring uWSGI, see the [official documentation](https://uwsgi-docs.readthedocs.io/en/latest/).

The main configuration options for an anode are described as follows:

- app-log: Specify the directory in which anode log files are stored.
- model-dir: Specify the directory in which models are stored. Models are generated by algorithms based on existing datasets.
- log-level: Specify the log level for anode logs.

### Managing Anodes

You manage anodes through the TDengine CLI. The following actions must be performed within the CLI on a client that is connected to your TDengine cluster.

#### Create an Anode

```sql
CREATE ANODE {node_url}
```

The `node_url` parameter determines the IP address and port of the anode. This information will be registered to your TDengine cluster. Do not register a single anode to multiple TDengine clusters.

#### View Anodes

You can run the following command to display the FQDN and status of the anodes in your cluster:

```sql
SHOW ANODES;

taos> show anodes;
     id      |              url               |    status    |       create_time       |       update_time       |
==================================================================================================================
           1 | 192.168.0.1:6090               | ready        | 2024-11-28 18:44:27.089 | 2024-11-28 18:44:27.089 |
Query OK, 1 row(s) in set (0.037205s)

```

#### View Advanced Analytics Services

```SQL
SHOW ANODES FULL;

taos> show anodes full;                                                      
     id      |            type            |              algo              | 
============================================================================ 
           1 | anomaly-detection          | grubbs                         | 
           1 | anomaly-detection          | lof                            | 
           1 | anomaly-detection          | shesd                          | 
           1 | anomaly-detection          | ksigma                         | 
           1 | anomaly-detection          | iqr                            | 
           1 | anomaly-detection          | sample_ad_model                | 
           1 | forecast                   | arima                          | 
           1 | forecast                   | holtwinters                    | 
           1 | forecast                   | tdtsfm_1                       | 
           1 | forecast                   | timemoe-fc                     | 
Query OK, 10 row(s) in set (0.028750s)                                       
```

TDgpt includes six anomaly detection algorithms or models and four forecasting algorithms or models. These are describes as follows:

| Type | Name | Description |
|--------|--------|--------------------|
| Anomaly detection | `grubbs` | Statistical model |
| Anomaly detection | `lof` | Density-based model |
| Anomaly detection | `sheds` | Seasonal ESD model |
| Anomaly detection | `ksigma` | Statistical model |
| Anomaly detection | `iqr` | Statistical model |
| Forecasting | `sample*ad*model` | Autoencoder-based sample model |
| Forecasting | `arima` | Autoregressive moving average algorithm |
| Forecasting | `holtwinters` | Exponential smoothing algorithm |
| Forecasting | tdtsfm_1 | TDtsfm v1.0 |
| Forecasting | tdtsfm_1 | Time-MoE |

These algorithms and models are described in detail in the relevant documentation.

#### Refresh the Algorithm Cache

```SQL
UPDATE ANODE {anode_id}
UPDATE ALL ANODES
```

#### Delete an Anode

```sql
DROP ANODE {anode_id}
```

Deleting an anode only removes it from your TDengine cluster. To stop an anode, use systemctl on the machine where the anode is located. To remove an anode, run the `rmtaosanode` command on the machine where the anode is located.
