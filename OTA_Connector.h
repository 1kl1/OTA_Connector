/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  OTA_Connector.h - Library for flashing Arduino Over The Air.
  Written with Wifi1010OTA library.
*/

#ifndef OTA_connector_h
#define OTA_connector_h

#include <Arduino.h>
#include "WiFi101.h"
#include <SPI.h>
#include "OTAStorage.h"
#include "InternalStorage.h"

class Connector
{
  public:
    Connector(const char* ssid, const char* pass, const char* group, const char* version, const char* name,const char* password, OTAStorage& storage);
    void beginOTA();
    void create();
    void pollOTA();
    String connectWifi();

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

  private:
    void pollServer();
};



// the #include statment and code go here...

#endif