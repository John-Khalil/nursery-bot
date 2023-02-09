// #include <WiFi.h>
// #include <IOXhop_FirebaseESP32.h>

// const char* ssid = "Shagie3_EXT";
// const char* password = "Sh0233373721";

// #define FIREBASE_HOST "mo7awla-6f40f.firebaseio.com"
// #define FIREBASE_AUTH "4zpJ4GTdYq2hVAWBC7S4AsZnZhUMRrr0dmFDWSij"

// void setup() {
//   Serial.begin(115200);

//   // Connect to Wi-Fi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }

//   Serial.println("Connected to WiFi");

//   // Connect to Firebase
//   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
// }

// void loop() {
//   int sensorValue = 200;

//   // Write sensor data to Firebase
//   Firebase.setInt("sensor/value", sensorValue);

//   // Wait a few seconds before sending the next data
//   delay(3000);
// }

#include <WiFi.h>
#include <FirebaseESP32.h>

const char* ssid = "Shagie3_EXT";
const char* password = "Sh0233373721";

#define FIREBASE_HOST "mo7awla-6f40f.firebaseio.com"
#define FIREBASE_AUTH "4zpJ4GTdYq2hVAWBC7S4AsZnZhUMRrr0dmFDWSij"
FirebaseData fbdo;


void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  int sensorValue = 200;

  // Write sensor data to Firebase
  Firebase.setInt(fbdo,"sensor", sensorValue);

  // Wait a few seconds before sending the next data
  delay(3000);
}