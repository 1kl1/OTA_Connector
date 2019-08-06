# OTA_Connector
Arduino Over The Air Connector.
Enables multiple Over The Air Upload.

It only supports Arduino SAMD Boards (32-bits ARM Cortex-M0+)

------------------------

## Installation
1. [Download](https://github.com/1kl1/OTA_Connector/archive/master.zip) the latest release from [github](https://github.com/1kl1/OTA_Connector)
2. [Download](https://github.com/JChristensen/Timer/archive/master.zip) the latest release from [github](https://github.com/JChristensen/Timer)
2. Unzip and rename the folder to "OTA_Connector", "Timer" (remove the -master)
3. Paste the modified folders on your Library folder (.../Arduino/libraries)
4. Reopen the Arduino IDE

------------------------

## Usage

Let's see the basic usage

```cpp
#include <OTA_Connector.h>
#include <Timer.h>

Timer t;
Connector connector("<WIFI_SSID>","<WIFI_PW>","<UPLOAD_GROUP>","<VERSION>","<DEVICE_ID>","<DEVICE_PASSWORD>",InternalStorage);
int latency = 5000

void setup() {
  Serial.begin(9600);
  Serial.println(connector.connectWifi());
  //connector.create();
  connector.beginOTA();
  int tickEvent = t.every(latency, pollOTA);
  Serial.print("10 second HTTP Request started id=");
  Serial.println(tickEvent);
}

void loop() {
  t.update();
}

void pollOTA(){
  connector.pollOTA();
}
```
Connector Object needs your wifi info.
If you want multiple upload, you have to set same UPLOAD_GROUP to each devices.
I recommend to note your DEVICE_PASSWORD. Because it needs when you upload your codes via OTA.
```cpp
Connector connector("<WIFI_SSID>","<WIFI_PW>","<UPLOAD_GROUP>","<VERSION>","<DEVICE_ID>","<DEVICE_PASSWORD>",InternalStorage);
```

If you are the first time upload code with our library, you have to create device at our [homepage](https://oongyi.com/arduino/OTA) or use underlying function
```cpp
    // this function will register your device to our web page.
  connector.create();
```

If you want to shorten the Over The Air update latency, decrease latency

```cpp
  int latency = 2000
```

------------------------

## Library Reference

### `Connector` class

*  `Connector` - Constructor

*  `String connectWifi()` - Connects Wifi. you have to call this method at the beginning. Returns Wifi Connect Results.

*  `void beginOTA()` - set up OTA polling. You have to call this method before you call pollOTA().

*  `void create()` - registers your device to our webserver.

*  `void pollOTA()` - requests code and send device's status. This function have to located in loop()

------------------------
