#include "arduino_secrets.h" 
#include <OTA_Connector.h>

Connector connector(SECRET_SSID,SECRET_PASS,"testgroup3","v 1.1.0","testid3","password",InternalStorage);

void setup() {
  Serial.begin(9600);
  Serial.println(connector.connectWifi());  
  connector.create();
  connector.beginOTA();  
}

void loop() {
  connector.pollOTA();
  
  delay(1000);
}
