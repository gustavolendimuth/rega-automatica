/*

  It's an irrigation automation system 
  to control a pump and a solenoid valve.
  
  When the soil gets dry, the pump is 
  turned on automatically.

  ------------------------------------------------------

  Este é um sistema de irrigação automatizada
  para controlar uma bomba e uma válvula solenóide.

  Quando o solo fica seco, a bomba
  é acionada automaticamente

*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT_U.h>
#include <DHT.h>
#include "ssd1306.h"
#include <CooperativeMultitasking.h>

// Sensors

// Soil moisture sensor
#define soil_sensor A1
int sensorValue0 = 0;
int moisture;

// Rain sensor
#define rain_sensor A2
int sensorValue1 = 0;
int rain;

// Temperature and humidity sensor
#define DHTPIN 7
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
int h;
int t;

// Relays and buttons
const int pump = 8;
const int relay = 9;
const int btn_pump_toggle = 2;
#define buttons A0

// Leds
const int led_green = 10;
const int led_yellow = 12;
const int led_red = 11;

// Functions
const long refresh = 500;
bool pump_toggle = true;
bool pump_on = false;
int led_state = LOW;
int led;
int buttonValue; //Stores analog value when button is pressed
int col;
//char teste[]{"teste"};

//Delays
unsigned long previousMillis_led = 0; // will store last time LED was updated
const long interval_led = 500;

unsigned long previousMillis_temp = 0; // will store last time temp sensor was updated
const long interval_temp = 60000;

//unsigned long previousMillis_lcd = 0;	// will store last time LCD was updated
//const long interval_lcd = 5000;

// Pump on/off settings
int pump_turn_on = 20;  // Percentage to turn on the pump
int pump_turn_off = 70; // Percentage to turn off the pump
int rain_off = 50;      // Percentege of rain to turn off the pump

// Functions
// Funções

void soil_moisture() {   // Turn pump on if the soil is dry
    if (moisture <= pump_turn_on && pump_toggle)
    {
        digitalWrite(pump, HIGH);
        pump_on = true;
    } 
    if (moisture >= pump_turn_off || rain >= rain_off)  // Turn pump off if the soil is wet or if starts to rain
    {
        digitalWrite(pump, LOW);
        pump_on = false;
    }
}
// Button on/off pump
// Botão para ligar e desligar a bomba manualmente
void pump_toggle_func()
{
    // It will turn o only if the soil is dry
    // Só liga a bomba se o solo estiver seco
    if (pump_on == false && moisture < pump_turn_off)
    {
        digitalWrite(pump, HIGH);
        pump_on = true;
        pump_toggle = true;
    }
    else
    {
        digitalWrite(pump, LOW);
        pump_on = false;
        pump_toggle = false;
    }
}

// Read temperature and humidity sensor
void temp_humi()
{
    // read the analog in value - temperature sensor:
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);
}

// Print sensor values to serial monitor
void sm_print()
{
    // Print Soil moisture sensor value
    Serial.print("Soil moisture: ");
    Serial.print(moisture);
    Serial.println("%  ");

    // Print Humidity and Temperature sensor values
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");

    // Print Rain sensor value
    if (rain > 10 && rain < 50)
    {
        Serial.println("drizzling");
    }
    else if (rain > 50)
    {
        Serial.println("raining");
    }
}

void screen_print()
{
    // Print soil moisture sensor values to screen
    // Convert sensor value string to char
    char b[4];
    String str;
    str = String(moisture);
    str.toCharArray(b, 4);
    ssd1306_printFixedN(0, 0, "    ", STYLE_NORMAL, FONT_SIZE_4X);
    ssd1306_printFixedN(0, 0, b, STYLE_NORMAL, FONT_SIZE_4X);
    if (moisture < 10){
        col = 25;
    }
    else if (moisture < 100){
        col = 50;
    }
    else if (moisture == 100){
        col = 75;
    }
    ssd1306_printFixedN(col, 0, "%", STYLE_NORMAL, FONT_SIZE_4X);
}

void setup()
{

    pinMode(soil_sensor, INPUT);
    pinMode(rain_sensor, INPUT);
    pinMode(buttons, INPUT);
    pinMode(btn_pump_toggle, INPUT_PULLUP);
    pinMode(relay, OUTPUT);
    pinMode(pump, OUTPUT);
    pinMode(led_red, OUTPUT);
    pinMode(led_yellow, OUTPUT);
    pinMode(led_green, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(btn_pump_toggle), pump_toggle_func, FALLING);
    dht.begin();
    Serial.begin(9600);
    ssd1306_128x64_i2c_init();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_fillScreen(0x00);
}

void loop()
{
    //Sensor readings

    // read the analog in value - soil moisture sensor:
    sensorValue0 = analogRead(soil_sensor);
    // map it to the range of the analog out:
    moisture = map(sensorValue0, 1023, 0, 0, 100);

    // read the analog in value - rain sensor:
    sensorValue1 = analogRead(rain_sensor);
    // map it to the range of the analog out:
    rain = map(sensorValue1, 1023, 0, 0, 100);
    // print the results to the serial monitor:

    // Leds
    unsigned long currentMillis_led = millis();

    if (currentMillis_led - previousMillis_led >= interval_led)
    {

        previousMillis_led = currentMillis_led; // save the last time you blinked the LED

        // if the LED is off turn it on and vice-versa:
        if (led_state == LOW)
        {

            led_state = HIGH;
        }
        else
        {

            led_state = LOW;
        }
    }

    if (moisture <= 30)
    {
        digitalWrite(led_green, LOW);
        digitalWrite(led_yellow, LOW);
        led = led_red;
    }
    else if (moisture <= 60)
    {

        pump_toggle = true;
        digitalWrite(led_green, LOW);
        digitalWrite(led_red, LOW);
        led = led_yellow;
    }
    else
    {
        pump_toggle = true;
        digitalWrite(led_yellow, LOW);
        digitalWrite(led_red, LOW);
        led = led_green;
    }

    if (pump_on)
    {
        digitalWrite(led, led_state); // set the LED with the led_state of the variable:
    }
    else
    {
        digitalWrite(led, HIGH);
    }
    soil_moisture();
    temp_humi();
    sm_print();
    screen_print();
    delay(refresh);
}