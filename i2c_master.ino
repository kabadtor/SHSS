//#include <Wire.h>

#include <Wire.h>
// #include <ESP8266WiFi.h>
// const char WiFiAPPSK[] = "sparkfun";
// WiFiServer server(80);
int x = 0;
void setup() {
 Wire.begin(2, 14);
 Serial.begin(115200);
 Wire.beginTransmission(0x08);
 Serial.begin(9600); // start serial for output
//  setupWiFi();
//  server.begin();
}
//void setupWiFi()
//{
// WiFi.mode(WIFI_AP);
// // Do a little work to get a unique-ish name. Append the
// // last two bytes of the MAC (HEX'd) to "Thing-":
// uint8_t mac[WL_MAC_ADDR_LENGTH];
// WiFi.softAPmacAddress(mac);
// String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
// String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
// macID.toUpperCase();
// String AP_NameString = "ESP8266 Thing " + macID;
// char AP_NameChar[AP_NameString.length() + 1];
// memset(AP_NameChar, 0, AP_NameString.length() + 1);
// for (int i = 0; i < AP_NameString.length(); i++)
// AP_NameChar[i] = AP_NameString.charAt(i);
// WiFi.softAP(AP_NameChar, WiFiAPPSK);
//}
void loop() {
 delay(100);
 // Check if a client has connected
//  WiFiClient client = server.available();
//  if (!client) {
//  return;
//  }
 // Read the first line of the request
//  String req = client.readStringUntil('\r');
//  Serial.println(req);
//  client.flush();
 Wire.requestFrom(0x08, 1);
 byte x = Wire.read();
 Serial.print("x: ");
 Serial.println(x);
 if (x)
 {
  Serial.println("ALERT");
 }

 // Prepare the response. Start with the common header:
//  String s = "HTTP/1.1 200 OK\r\n";
//  s += "Content-Type: text/html\r\n\r\n";
//  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
//  s += "The message is:";
//  s += String(x);
//  s += "<br>"; // Go to the next line.
//  s += "</html>\n";
//  // Send the response to the client
//  client.print(s);
//  delay(1);
//  Serial.println("Client disonnected");
 // The client will actually be disconnected
 // when the function returns and 'client' object is destroyed
}
