# OTA_Connector
Arduino Over The Air Connector.
Enables multiple Over The Air Upload.

It only supports Arduino SAMD Boards (32-bits ARM Cortex-M0+)

------------------------

## Installation
1. [Download](https://github.com/1kl1/OTA_Connector/archive/master.zip) OTA_Connector's the latest release from [github](https://github.com/1kl1/OTA_Connector)
2. Unzip and rename the folder to "OTA_Connector" (remove the -master)
3. Paste the modified folders on your Library folder (.../Arduino/libraries)
4. Reopen the Arduino IDE

------------------------

## Usage

Let's see the basic usage

```cpp
#include "OTA_Connector.h"

Connector connector("<WIFI_SSID>","<WIFI_PW>","<UPLOAD_GROUP>","<VERSION>","<DEVICE_ID>","<DEVICE_PASSWORD>",InternalStorage);

void setup() {
  Serial.begin(9600);
  connector.connectWifi();
  connector.beginOTA();
}

void loop() {
  connector.pollOTA();
}

```
Connector Object needs your wifi info.
If you want multiple upload, you have to set same UPLOAD_GROUP to each devices.
I recommend to note your DEVICE_PASSWORD. Because it needs when you upload your codes via OTA.
```cpp
Connector connector("<WIFI_SSID>","<WIFI_PW>","<UPLOAD_GROUP>","<VERSION>","<DEVICE_ID>","<DEVICE_PASSWORD>",InternalStorage);
```

If you want to shorten the Over The Air update interval, decrease interval

```cpp
  int interval = 2000
```

------------------------

## Library Reference

### `Connector` class

*  `Connector` - Constructor

*  `String connectWifi()` - Connects Wifi. you have to call this method at the beginning. Returns Wifi Connect Results.

*  `void beginOTA()` - set up OTA polling. You have to call this method before you call pollOTA().

*  `void pollOTA()` - requests code and send device's status. This function have to located in loop()

------------------------
