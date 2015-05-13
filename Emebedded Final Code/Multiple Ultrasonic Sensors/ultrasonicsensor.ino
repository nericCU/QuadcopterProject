// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>

#define TRIGGER_PIN_1  10  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_1     11  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define TRIGGER_PIN_2  A0  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_2     A1  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define TRIGGER_PIN_3  A2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_3     A3  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar_1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar_2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar_3(TRIGGER_PIN_3, ECHO_PIN_3, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int distance;

void setup() {
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.
}

void loop() {
  
  delay(30); // Wait 10ms between pings. 29ms should be the shortest delay between pings.
  distance = 2 * sonar_1.ping() / US_ROUNDTRIP_CM; // Send ping, get ping time in microseconds (uS).
  if (distance > 0 && distance < 50) {
    Serial.print("L");// Serial.println(distance, DEC); 
  }

  delay(30); // Wait 10ms between pings. 29ms should be the shortest delay between pings.
  distance = sonar_2.ping() / US_ROUNDTRIP_CM; // Send ping, get ping time in microseconds (uS).
  if (distance > 0 && distance < 50) {
    Serial.print("F");// Serial.println(distance, DEC); 
  }

  delay(30); // Wait 10ms between pings. 29ms should be the shortest delay between pings.
  distance = sonar_3.ping() / US_ROUNDTRIP_CM; // Send ping, get ping time in microseconds (uS).
  if (distance > 0 && distance < 50) {
    Serial.print("R");// Serial.println(distance, DEC); 
  }
}


