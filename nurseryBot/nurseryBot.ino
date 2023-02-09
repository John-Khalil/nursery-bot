#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "src/basics.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver servoModules[]={Adafruit_PWMServoDriver(0x40),Adafruit_PWMServoDriver(0x41)};
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  650 // this is the 'maximum' pulse length count (out of 4096)


void setServo(uint16_t servoMotor,uint16_t angle){
  static uint8_t firstRun;
  if(!firstRun){
    for(auto &servoModule:servoModules){
      servoModule.begin();
      servoModule.setPWMFreq(60);
    }
    firstRun =1;
  }
  servoModules[servoMotor/16].setPWM((servoMotor%16),0,map(angle,0, 180, SERVOMIN,SERVOMAX));
  return;
}


const char *ssid = "Shagie3_EXT";
const char *password = "Sh0233373721";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);
  while(1){
    setServo(13,0);
    delay(500);
    setServo(13,180);
    delay(500);
  }
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }
  // Serial.println("Connected to WiFi");

  // server.begin();
  // webSocket.begin();
  // webSocket.onEvent(webSocketEvent);
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

// #include <Wire.h>

// #include <Adafruit_PWMServoDriver.h>

// // called this way, it uses the default address 0x40
// Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);
// Adafruit_PWMServoDriver board2 = Adafruit_PWMServoDriver(0x41);

// // Depending on your servo make, the pulse width min and max may vary, you 
// // want these to be as small/large as possible without hitting the hard stop
// // for max range. You'll have to tweak them as necessary to match the servos you
// // have!
// // Watch video V1 to understand the two lines below: http://youtu.be/y8X9X10Tn1k
// #define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
// #define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)


// int servoNumber = 0;

// void setup() {
//   Serial.begin(9600);
//   Serial.println("32 channel Servo test!");

//   board1.begin();
//   board2.begin();  
//   board1.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
//   board2.setPWMFreq(60);
//   //yield();
// }

// // the code inside loop() has been updated by Robojax
// void loop() {


//     for( int angle =0; angle<181; angle +=10){
//       for(int i=0; i<16; i++)
//         {      
//             board2.setPWM(i, 0, angleToPulse(angle) );
//             board1.setPWM(i, 0, angleToPulse(angle) );
//         }
//     }
  
// // robojax PCA9865 16 channel Servo control
//   delay(100);
 
// }

// /*
//  * angleToPulse(int ang)
//  * gets angle in degree and returns the pulse width
//  * also prints the value on seial monitor
//  * written by Ahmad Nejrabi for Robojax, Robojax.com
//  */
// int angleToPulse(int ang){
//    int pulse = map(ang,0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
//    Serial.print("Angle: ");Serial.print(ang);
//    Serial.print(" pulse: ");Serial.println(pulse);
//    return pulse;
// }