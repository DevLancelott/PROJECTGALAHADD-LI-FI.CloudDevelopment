## 'wpa_supplicant' ##

This component contains the upstream wpa_supplicant ported for ESP family of platforms.
The code is tightly coupled with esp_wifi component which has ESP WiFi libraries and header files that are used in ported supplicant.

ESP uses MbedTLS as crypto library therefore MbedTLS component is also required for some features to work(see ESP*WIFI*MBEDTLS_CRYPTO).

To port it for different OS, esp*wifi and wpa*supplicant should be picked up a whole system(preferably with MbedTLS if we want all features to work.) 
