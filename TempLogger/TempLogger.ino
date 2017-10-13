#include <SD.h>

/*Led Pin*/
const int led = 13;
static boolean led_state = false;
static unsigned long led_timestamp = 0;

void setup() 
{
  Serial.begin(9600);
  pinMode( led, OUTPUT);
  Serial.println("Hello World");
  led_timestamp = millis();
}

void loop() 
{
  /*Led Task*/
  if ( millis()-led_timestamp >= 1000u )
  {
    led_timestamp = millis();
    if ( led_state )
    {
      led_state = false;
      digitalWrite( led, HIGH);
    }
    else
    {
      led_state = true;
      digitalWrite( led, LOW);
    }
  }
  /*Led Task End*/
}
