#define BLYNK_TEMPLATE_ID "TMPL6MBElU0aV"
#define BLYNK_TEMPLATE_NAME "Pet Feeder"
#define BLYNK_AUTH_TOKEN "0zvFHBTmjbe8tAPDQs9p6yRMQXNZAXPy"

#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define WIFI_SSID "RKP2"
#define WIFI_PASSWORD "EmpatLima6"

Servo myservo;
int servoPin = D0; // GPIO2

// Blynk virtual pins
#define SERVO_CONTROL_PIN V1

// Blynk authorization token
char auth[] = BLYNK_AUTH_TOKEN;

int delayTime = 1000; // Default delay time in milliseconds

void setup()
{
  Serial.begin(115200);

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);

  myservo.attach(servoPin);
  myservo.write(0); // Move servo to initial position (0 degrees)
}

// Control servo based on button input
BLYNK_WRITE(SERVO_CONTROL_PIN)
{
  int value = param.asInt();
  if (value == 1)
  {
    myservo.write(180); // Open feeder
    delay(1000);   // Use delay based on grams set
    myservo.write(0);   // Close feeder after delay
    Blynk.virtualWrite(SERVO_CONTROL_PIN, 0); // Turn off the button in the app
  }
  else
  {
    myservo.write(0); // Ensure servo is closed if button is off
  }
}


void loop()
{
  Blynk.run();
}
