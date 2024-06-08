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
int servoPin = D0;

// Blynk virtual pins
#define SERVO_CONTROL_PIN V1
#define GRAMS_SLIDER_PIN V4

// Blynk authorization token
char auth[] = BLYNK_AUTH_TOKEN;

int delayTime = 1000;

void setup()
{
  Serial.begin(115200);

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);

  myservo.attach(servoPin);
  myservo.write(0);
}

BLYNK_WRITE(SERVO_CONTROL_PIN)
{
  int value = param.asInt();
  if (value == 1)
  {
    myservo.write(180); 
    delay(delayTime);   
    myservo.write(0);   
    Blynk.virtualWrite(SERVO_CONTROL_PIN, 0);
  }
  else
  {
    myservo.write(0);
  }
}

BLYNK_WRITE(GRAMS_SLIDER_PIN)
{
  int grams = param.asInt();
  delayTime = grams * 1000; 
  Serial.print("Grams set to: ");
  Serial.print(grams);
  Serial.print(", Delay time set to: ");
  Serial.println(delayTime);
}

void loop()
{
  Blynk.run();
}
