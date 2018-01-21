
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

/*Led Pin*/
const int led = 13;
static boolean led_state = false;
static uint32_t led_timestamp = 0;
static uint32_t log_timestamp = 0;

/*SD Card Pins*/
const int chipSelect = 4;
/*
 * SD card attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK - pin 13
 * CS - pin 4
 */

/*DS1307 Object*/
RTC_DS1307 rtc;

#define FILE_NAME_LEN       13u
char filename[FILE_NAME_LEN];
uint8_t day;
uint8_t month;
uint16_t year;
uint8_t hour;
uint8_t minute;
uint8_t second;
uint16_t adc_count;
#define BUFFER_SIZE         20u
char buffer[BUFFER_SIZE];
File myFile;
    
void setup() 
{
  Serial.begin(9600);
  pinMode( led, OUTPUT);
  /* see if the card is present and can be initialized*/
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    /*Exit Program*/
    return;
  }
  Serial.println("Card Initialized.");
  /*Check for RTC*/
  if ( !rtc.begin() )
  {
    Serial.println("Couldn't find the RTC");
    return;
  }
  Serial.println("RTC DS1307 Initialized");
  led_timestamp = millis();
  log_timestamp = millis();
}

void loop() 
{
  DateTime now;
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
  /*Logging Task Started*/
  if( millis()-log_timestamp >= 1000u )
  {
    log_timestamp = millis();
    now = rtc.now();
    day = now.day();
    month = now.month();
    year = now.year();
    year = year-2000u;
    hour = now.hour();
    minute = now.minute();
    second = now.second();
    snprintf( filename, FILE_NAME_LEN,"%.2d-%.2d-%d.txt", month, day, year);
    // Serial.println( filename );
    /*Get ADC Data and Convert it to Temperature*/
    adc_count = analogRead(A0);
    /*
     * LM35 Temperature Sensor gives 10mV output per degree Celcius
     * Step Size = 5000/1023
     * temperature = (adc_counts * 5000 / 1023)/10;
    */
    uint32_t temp;
    uint8_t fract;  // Fractional Part
    temp = (uint32_t)adc_count * (uint32_t)5000u;
    temp = temp / 1023u;
    fract = temp % 10u;
    temp = temp/10u;
    int n = snprintf(buffer, BUFFER_SIZE, "%.2d:%.2d:%.2d,%d,%lu.%u", hour, minute, second, adc_count, temp, fract);
    Serial.println(buffer);
    myFile = SD.open(filename, FILE_WRITE);
    myFile.println(buffer);
    myFile.close();
  }
}
