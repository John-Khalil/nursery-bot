#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "src/basics.h"
#include "src/webService.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <ArduinoJson.h>

Adafruit_PWMServoDriver servoModules[]={Adafruit_PWMServoDriver(0x40),Adafruit_PWMServoDriver(0x41)};
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  650 // this is the 'maximum' pulse length count (out of 4096)

DynamicJsonDocument doc(1024);


#define TYPE "type"
#define SERVO "servo"
#define MOTOR "motor"
#define ID "ID"
#define PIR "pir"
#define ULTRASONIC "ultrasonic"
#define VALUE "value"



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


const char *ssid = "RISC-V";
const char *password = "threadripper";



void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  web::service server(80,"/");

  server.onData([&](uint8_t *data){
    deserializeJson(doc, (char*)data);
    if(doc[TYPE]==std::string(SERVO)){
      Serial.print("servo data >> ");
      Serial.println((uint16_t)doc[ID]);
      Serial.println((uint16_t)doc[VALUE]);
      setServo(doc[ID],doc[VALUE]);
    }

  });

  server.onData([&](uint8_t *data){
    deserializeJson(doc, (char*)data);
    if(doc[TYPE]==std::string(MOTOR)){

    }

  });
  
  server.onData([&](uint8_t *data){
    deserializeJson(doc, (char*)data);
    if(doc[TYPE]==std::string(PIR)){

    }

  });
  
  server.onData([&](uint8_t *data){
    deserializeJson(doc, (char*)data);
    if(doc[TYPE]==std::string(ULTRASONIC)){

    }

  });


  // server.onData([&](uint8_t *data){
  //   Serial.println((char*)data);
  // });

  _delay_ms(-1UL);
}

void loop() {
  
}


