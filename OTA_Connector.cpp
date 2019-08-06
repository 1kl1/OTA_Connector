#include <Arduino.h>
#include "OTA_Connector.h"

#if defined(ARDUINO_SAMD_ZERO)
  #define BOARD "arduino_zero_edbg"
#elif defined(ARDUINO_SAMD_MKR1000)
  #define BOARD "mkr1000"
#elif defined(ARDUINO_SAMD_MKRZERO)
  #define BOARD "mkrzero"
#else
  #error "Unsupported board!"
#endif

static String base64Encode(const String& in)
{
  static const char* CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

  int b;
  String out;
  out.reserve((in.length()) * 4 / 3);
  
  for (unsigned int i = 0; i < in.length(); i += 3) {
    b = (in.charAt(i) & 0xFC) >> 2;
    out += CODES[b];

    b = (in.charAt(i) & 0x03) << 4;
    if (i + 1 < in.length()) {
      b |= (in.charAt(i + 1) & 0xF0) >> 4;
      out += CODES[b];
      b = (in.charAt(i + 1) & 0x0F) << 2;
      if (i + 2 < in.length()) {
         b |= (in.charAt(i + 2) & 0xC0) >> 6;
         out += CODES[b];
         b = in.charAt(i + 2) & 0x3F;
         out += CODES[b];
      } else {
        out += CODES[b];
        out += '=';
      }
    } else {
      out += CODES[b];
      out += "==";
    }
  }

  return out;
}

Connector::Connector(const char* ssid, const char* pass, const char* group, const char* version, const char* name,const char* password, OTAStorage& storage) :
  _storage(NULL)
{
  _ssid = ssid;
  _pass = pass;
  _group = group;
  _version = version;
  _name = name;
  _expectedAuthorization = "Basic " + base64Encode(String(name) + ":" + String(password));
  _storage = &storage; 
  _server = "oongyi.com";
}

String Connector::connectWifi(){
  int status = WL_IDLE_STATUS;
  if (WiFi.status() == WL_NO_SHIELD) {
    return "WiFi shield not present";
  }
  while (status != WL_CONNECTED) {
    status = WiFi.begin(_ssid, _pass);
    delay(1000);
  }

  IPAddress ip = WiFi.localIP();
  
  _ipString = String(ip[0]);
  for (byte octet = 1; octet < 4; ++octet) {
    _ipString += '.' + String(ip[octet]);
  }
  return "WiFi Connected With "+_ipString;
}

void Connector::create(){
  WiFiSSLClient client;
  String PostData = "{\"board\": \""+String(BOARD)+"\", \"deviceID\": \""+_name+"\",\"ip\": \""+_ipString+"\",\"group\": \""+_group+"\",\"version\": \""+_version+"\"}";
  

  if (client.connectSSL(_server, 443)) {
    client.println("POST /arduino/OTA/ HTTP/1.1");
    client.println("Host: www.oongyi.com");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println("Connection: close");
    client.println();
    client.println(PostData);
  }
  while (client.available()) {
    char c = client.read();
  }
  client.stop();
}

void Connector::beginOTA()
{
  _client.connectSSL(_server, 443);
  _flag = true;
}

void Connector::pollOTA()
{
  pollServer();
}

void Connector::pollServer()
{
  if(_flag){
    String PostData = "{\"deviceID\": \""+_name+"\",\"group\": \""+_group+"\"}";
    _client.connectSSL(_server, 443);
    _client.println("POST /arduino/OTA/node HTTP/1.1");
    _client.println("Host: oongyi.com");
    _client.println("Content-Type: application/json");
    _client.println("Connection: close");
    _client.print("Content-Length: ");
    _client.println(PostData.length());
    _client.println();
    _client.println(PostData);
    _flag = false;
    return;
  }
  if (_client.available()) {
    String response = _client.readStringUntil('\n');
    response.trim();
    //response : HTTP/1.1 200 OK
    int last = response.lastIndexOf(" ");
    int responseCode = response.substring(9,12).toInt();
    String header;
    String authorization;
    long contentLength = -1;
    do {
      header = _client.readStringUntil('\n');
      header.trim();

      if (header.startsWith("Content-Length: ")) {
        header.remove(0, 16);

        contentLength = header.toInt();
      }
      else if (header.startsWith("Authorization: ")) {
        header.remove(0, 15);

        authorization = header;
      }
    } while (header != "");

    Serial.println(responseCode);
    Serial.println(contentLength);

    if(responseCode == 404){
      _flag = true;
      _client.stop();
      Serial.println("404 Not Exist");
      return;
    }
    if(responseCode == 400){
      _flag = true;
      _client.stop();
      Serial.println("400 Bad Request");
      return;
    }
    if (contentLength <= 0) {
      _flag = true;
      _client.stop();
      Serial.println("No Contents");
      return;
    }
    //code arrive
    Serial.println("code arrived");
    if (_expectedAuthorization != authorization) {
      _flag = true;
      _client.stop();
      Serial.println("No Auth");
      return;
    }
    if (contentLength > _storage->maxSize()) {
      _storage->close();
      _flag = true;
      _client.stop();
      Serial.println("Too Large");
      return;
    }
    if (_storage == NULL || !_storage->open()) {
      _flag = true;
      _client.stop();
      return;
    }
    
    long read = 0;
    Serial.println("Writing Start");

    while (_client.connected() && read < contentLength) {
      if (_client.available()) {
        read++;

        _storage->write((char)_client.read());
      }
    }

    _storage->close();

    if (read == contentLength) {
      _flag = true;

      delay(250);

      // apply the update
      _storage->apply();
      
      while (true);
    } else {
      _storage->clear();

      _flag = true;
      _client.stop();
    }
  }
}
  
  

  // if (client) {
    
  //   String request = client.readStringUntil('\n');
  //   request.trim();

  //   String header;
  //   long contentLength = -1;
  //   String authorization;

  //   do {
  //     header = client.readStringUntil('\n');
  //     header.trim();

  //     if (header.startsWith("Content-Length: ")) {
  //       header.remove(0, 16);

  //       contentLength = header.toInt();
  //     } else if (header.startsWith("Authorization: ")) {
  //       header.remove(0, 15);

  //       authorization = header;
  //     }
  //   } while (header != "");
  //   Serial.println(authorization);
  //   Serial.println(contentLength);

  //   if (request == "GET / HTTP/1.1"){
  //     flushRequestBody(client, contentLength);
  //     sendHttpResponse(client, 200, "OK");
  //     return;
  //   }

  //   if (request != "POST /sketch HTTP/1.1") {
  //     flushRequestBody(client, contentLength);
  //     sendHttpResponse(client, 404, "Not Found");
  //     return;
  //   }

  //   if (_expectedAuthorization != authorization) {
  //     flushRequestBody(client, contentLength);
  //     sendHttpResponse(client, 401, "Unauthorized");
  //     return;
  //   }

  //   if (contentLength <= 0) {
  //     sendHttpResponse(client, 400, "Bad Request");
  //     return;
  //   }

  //   if (_storage == NULL || !_storage->open()) {
  //     flushRequestBody(client, contentLength);
  //     sendHttpResponse(client, 500, "Internal Server Error");
  //     return;
  //   }

  //   if (contentLength > _storage->maxSize()) {
  //     _storage->close();
  //     flushRequestBody(client, contentLength);
  //     sendHttpResponse(client, 413, "Payload Too Large");
  //     return;
  //   }

  //   long read = 0;
  //   Serial.println("Writing Start");

  //   while (client.connected() && read < contentLength) {
  //     if (client.available()) {
  //       read++;

  //       _storage->write((char)client.read());
  //     }
  //   }

  //   _storage->close();

  //   if (read == contentLength) {
  //     sendHttpResponse(client, 200, "OK");
      
  //     delay(250);

  //     // apply the update
  //     _storage->apply();
      
  //     while (true);
  //   } else {
  //     _storage->clear();

  //     client.stop();
  //   }
  // }
// }

