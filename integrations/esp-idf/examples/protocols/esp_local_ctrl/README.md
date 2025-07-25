| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# ESP Local Control using HTTPS server

This example creates a `esp*local*ctrl` service over HTTPS transport, for securely controlling the device over local network. In this case the device name is resolved through `mDNS`, which in this example is `my*esp*ctrl_device.local`.

See the `esp*local*ctrl` component documentation for details.

Before using the example, run `idf.py menuconfig` (or `idf.py menuconfig` if using CMake build system) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../README.md) for more details.

Note, that this example in not supported for IPv6-only configuration.

## Client Side Implementation

A python test script `scripts/esp*local*ctrl.py` has been provided for as a client side application for controlling the device over the same Wi-Fi network. The script relies on a pre-generated `main/certs/rootCA.pem` to verify the server certificate. The server side private key and certificate can also be found under `main/certs`, namely `prvtkey.pem` and `servercert.pem`.

After configuring the Wi-Fi, flashing and booting the device, run the following command to test the device name
resolution through mDNS:

```
ping my_esp_ctrl_device.local
```

Sample output:

```
64 bytes from 192.168.32.156 (192.168.32.156): icmp_seq=1 ttl=255 time=58.1 ms
64 bytes from 192.168.32.156 (192.168.32.156): icmp_seq=2 ttl=255 time=89.9 ms
64 bytes from 192.168.32.156 (192.168.32.156): icmp_seq=3 ttl=255 time=123 ms
```

After you've tested the name resolution, run:

```
python scripts/esp_local_ctrl.py --sec_ver 2 --sec2_username wifiprov --sec2_pwd abcd1234
```
Sample output:

```
python scripts/esp_local_ctrl.py --sec_ver 2 --sec2_username wifiprov --sec2_pwd abcd1234

++++ Connecting to my_esp_ctrl_device.local++++

==== Starting Session ====
==== Session Established ====

==== Available Properties ====
S.N. Name             Type       Flags            Value
[ 1] timestamp (us)   TIME(us)   Read-Only        168561481
[ 2] property1        INT32                       123456
[ 3] property2        BOOLEAN    Read-Only        True
[ 4] property3        STRING

Select properties to set (0 to re-read, 'q' to quit) : 0

==== Available Properties ====
S.N. Name             Type       Flags            Value
[ 1] timestamp (us)   TIME(us)   Read-Only        22380117
[ 2] property1        INT32                       123456
[ 3] property2        BOOLEAN    Read-Only        False
[ 4] property3        STRING

Select properties to set (0 to re-read, 'q' to quit) : 2,4
Enter value to set for property (property1) : -5555
Enter value to set for property (property3) : hello world!

==== Available Properties ====
S.N. Name             Type       Flags            Value
[ 1] timestamp (us)   TIME(us)   Read-Only        55110859
[ 2] property1        INT32                       -5555
[ 3] property2        BOOLEAN    Read-Only        False
[ 4] property3        STRING                      hello world!

Select properties to set (0 to re-read, 'q' to quit) : q
Quitting...
```

The script also allows to connect over plain HTTP or BLE, and provide a custom service name. To display the list of supported parameters, run:

```
python scripts/esp_local_ctrl.py --help
```

**Note:** To use plain HTTP transport, disable the config option `CONFIG*ESP*HTTPS*SERVER*ENABLE`.

## Certificates

You can generate a new server certificate using the OpenSSL command line tool.

For the purpose of this example, lets generate a rootCA, which we will use to sign the server certificates and which the client will use to verify the server certificate during SSL handshake. You will need to set a password for encrypting the generated `rootkey.pem`.

```
openssl req -new -x509 -subj "/CN=root" -days 3650 -sha256 -out rootCA.pem -keyout rootkey.pem -addext "keyUsage=critical,digitalSignature,keyCertSign"
```

Now generate a certificate signing request for the server, along with its private key `prvtkey.pem`.

```
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -days 3650 -out server.csr -subj "/CN=my_esp_ctrl_device.local"
```

Now use the previously generated rootCA to process the server's certificate signing request, and generate a signed certificate `servercert.pem`. The password set for encrypting `rootkey.pem` earlier, has to be entered during this step.

```
openssl x509 -req -in server.csr -CA rootCA.pem -CAkey rootkey.pem -CAcreateserial -out servercert.pem -days 500 -sha256
```

Now that we have `rootCA.pem`, `servercert.pem` and `prvtkey.pem`, copy these into main/certs. Note that only the server related files (`servercert.pem` and `prvtkey.pem`) are embedded into the firmware.

Expiry time and metadata fields can be adjusted in the invocation.

Please see the `openssl` man pages (man `openssl-req`) for more details.

It is **strongly recommended** to not reuse the example certificate in your application;
it is included only for demonstration.
