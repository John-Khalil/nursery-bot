#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "src/basics.h"

const char *ssid = "Shagie3_EXT";
const char *password = "Sh0233373721";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void webSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket client disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket client connected");
      break;
    case WStype_TEXT:
      Serial.println("Received data from client:");
      Serial.println((char*)payload);
      break;
  }
}