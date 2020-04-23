#ifndef OTA_connector_h
#define OTA_connector_h

#include <Arduino.h>
#include <WiFi101.h>
#include <SPI.h>
#include "OTAStorage.h"
#include "InternalStorage.h"

class Connector
{
  public:
    Connector(const char* ssid, const char* pass, const char* group, const char* version, const char* name,const char* password, OTAStorage& storage);
    void beginOTA();
    void pollOTA();
    void connectWifi();

  private:
    String _ssid;
    String _pass;
    String _group;
    String _version;
    String _name;
    String _expectedAuthorization;
    String _ipString;
    char* _server;
    bool _flag;
    OTAStorage* _storage;
    WiFiSSLClient _client;
    unsigned long previousMillis = 0; 
    const long interval = 3000;


  private:
    void pollServer();
    void printWiFiStatus();
    void checkClient();
};


#endif