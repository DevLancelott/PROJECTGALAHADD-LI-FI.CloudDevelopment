# NodeMCU Documentation

NodeMCU is an open source [Lua](https://www.lua.org/) based firmware for the [ESP32](https://www.espressif.com/en/products/socs/esp32) and [ESP8266](https://www.espressif.com/en/products/socs/esp8266) WiFi SOCs from Espressif. It uses an on-module flash-based [SPIFFS](https://github.com/pellepl/spiffs) file system. NodeMCU is implemented in C and the ESP8266 version is layered on the [Espressif NON-OS SDK](https://github.com/espressif/ESP8266*NONOS*SDK).

The firmware was initially developed as is a companion project to the popular ESP8266-based [NodeMCU development modules](https://github.com/nodemcu/nodemcu-devkit-v1.0), but the project is now community-supported, and the firmware runs on any ESP module.

→ [Getting Started](getting-started.md)

!!! important

    The NodeMCU [`release`](https://github.com/nodemcu/nodemcu-firmware/tree/release) and [`dev`](https://github.com/nodemcu/nodemcu-firmware/tree/dev) branches target the ESP8266. The [`dev-esp32`](https://github.com/nodemcu/nodemcu-firmware/tree/dev-esp32) branch targets the ESP32.

## Programming Model
The NodeMCU programming model is similar to that of [Node.js](https://en.wikipedia.org/wiki/Node.js), only in Lua. It is asynchronous and event-driven. Many functions, therefore, have parameters for callback functions. To give you an idea what a NodeMCU program looks like study the short snippets below. For more extensive examples have a look at the [`/lua*examples`](https://github.com/nodemcu/nodemcu-firmware/tree/release/lua*examples) folder in the repository on GitHub.

```lua
-- a simple HTTP server
srv = net.createServer(net.TCP)
srv:listen(80, function(conn)
  conn:on("receive", function(sck, payload)
    print(payload)
    sck:send("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1> Hello, NodeMCU. </h1>")
  end)
  conn:on("sent", function(sck) sck:close() end)
end)
```

```lua
-- connect to WiFi access point (DO NOT save config to flash)
wifi.setmode(wifi.STATION)
station_cfg={}
station_cfg.ssid = "SSID"
station_cfg.pwd = "password"
station_cfg.save = false
wifi.sta.config(station_cfg)
```

```lua
-- register event callbacks for WiFi events
wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, function(T)
  print("\n\tSTA - CONNECTED".."\n\tSSID: "..T.SSID.."\n\tBSSID: "..
  T.BSSID.."\n\tChannel: "..T.channel)
end)
```

```lua
-- manipulate hardware like with Arduino
pin = 1
gpio.mode(pin, gpio.OUTPUT)
gpio.write(pin, gpio.HIGH)
print(gpio.read(pin))
```

→ [Getting Started](getting-started.md)

## Lua Flash Store (LFS)
In September 2018 support for a [Lua Flash Store (LFS)](lfs.md) was introduced. LFS allows Lua code and its associated constant data to be executed directly out of flash-memory; just as the firmware itself is executed. This now enables NodeMCU developers to create Lua applications with up to 256Kb Lua code and read-only constants executing out of flash. All of the RAM is available for read-write data!

## Releases

This project uses two main branches, `release` and `dev`. `dev` is actively worked on and it's also where PRs should be created against. `release` thus can be considered "stable" even though there are no automated regression tests. The goal is to merge back to `release` roughly every 2 months. Depending on the current "heat" (issues, PRs) we accept changes to `dev` for 5-6 weeks and then hold back for 2-3 weeks before the next snap is completed.

A new tag is created every time `dev` is merged back to `release` branch. They are listed in the [releases section on GitHub](https://github.com/nodemcu/nodemcu-firmware/releases). Tag names follow the `<SDK-version>-release_yyyymmdd` pattern.

## Up-To-Date Documentation
At the moment the only up-to-date documentation maintained by the current NodeMCU team is in English. It is part of the source code repository (`/docs` subfolder) and kept in sync with the code.
