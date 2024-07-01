#define BLYNK_TEMPLATE_ID "TMPL6MBElU0aV"
#define BLYNK_TEMPLATE_NAME "Pet Feeder"
#define BLYNK_AUTH_TOKEN "0zvFHBTmjbe8tAPDQs9p6yRMQXNZAXPy"

#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <time.h>

#define WIFI_SSID "Kayaki"
#define WIFI_PASSWORD "aaskiiii"

// NTP server to be used for time synchronization
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // GMT+7 for Western Indonesia Time (WIB)
const int daylightOffset_sec = 0;    // No daylight saving time in Indonesia

Servo myservo;
int servoPin = D0;

// Blynk virtual pins
#define SERVO_CONTROL_PIN V1
#define GRAMS_SLIDER_PIN V5
#define TIME_INPUT_PIN V2
#define TIME_INPUT2_PIN V3
#define TIME_INPUT3_PIN V4

// Blynk authorization token
char auth[] = BLYNK_AUTH_TOKEN;

int delayTime = 1000;
const int GRAMS_PER_SECOND = 10;
bool servoActivated = false;
bool timeProcessed = false;
char currentTime[9];
char blynkTime[3][9];
unsigned long lastCheckTime = 0;
const unsigned long checkInterval = 1000; // Check every second

void getCurrentTimeString();

void setup()
{
  Serial.begin(9600);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);

  myservo.attach(servoPin);
  myservo.write(0);

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Waiting for time");
  delay(2000);

  // Get the current time and store it in currentTime
  getCurrentTimeString();

  // Print the stored current time to the serial monitor
  Serial.print("Current time: ");
  Serial.println(currentTime);
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
  delayTime = (grams * 1000) / GRAMS_PER_SECOND;
  Serial.print("Grams set to: ");
  Serial.print(grams);
  Serial.print(", Delay time set to: ");
  Serial.println(delayTime);
}

void loop()
{
  Blynk.run();

  unsigned long currentMillis = millis();
  if (currentMillis - lastCheckTime >= checkInterval)
  {
    lastCheckTime = currentMillis;
    getCurrentTimeString();
    bool timeMatch = false;
    for (int i = 0; i < 3; i++)
    {
      if (strcmp(currentTime, blynkTime[i]) == 0)
      {
        timeMatch = true;
        break;
      }
    }
    if (timeMatch && !timeProcessed)
    {
      Serial.println("Time matches!");
      if (!servoActivated)
      {
        Blynk.virtualWrite(SERVO_CONTROL_PIN, 1);
        myservo.write(180);
        delay(delayTime);
        myservo.write(0);
        Blynk.virtualWrite(SERVO_CONTROL_PIN, 0);
        servoActivated = true;
        timeProcessed = true; // Set the timeProcessed flag
      }
    }
    else if (!timeMatch)
    {
      Serial.println("Time does not match.");
      servoActivated = false;
      timeProcessed = false; // Reset the timeProcessed flag if no match
    }
  }
}

void handleTimeInput(BlynkParam param, int index)
{
  int rawTime = param.asInt();

  // Convert raw time (seconds since midnight) to hours, minutes, and seconds
  int hours = rawTime / 3600;
  int minutes = (rawTime % 3600) / 60;
  int seconds = rawTime % 60;

  // Format the time string with leading zeros for hours, minutes, and seconds (HH:MM:SS)
  snprintf(blynkTime[index], sizeof(blynkTime[index]), "%02d:%02d:%02d", hours, minutes, seconds);

  // Print the formatted time for verification (optional)
  Serial.print("Time");
  Serial.print(index + 1);
  Serial.print(" set to: ");
  Serial.println(blynkTime[index]);
}

BLYNK_WRITE(TIME_INPUT_PIN)
{
  handleTimeInput(param, 0);
}

BLYNK_WRITE(TIME_INPUT2_PIN)
{
  handleTimeInput(param, 1);
}

BLYNK_WRITE(TIME_INPUT3_PIN)
{
  handleTimeInput(param, 2);
}

void getCurrentTimeString()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(currentTime, sizeof(currentTime), "%H:%M:%S", &timeinfo);
}
