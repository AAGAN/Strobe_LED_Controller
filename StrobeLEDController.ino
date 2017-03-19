/*
strobe using adafruit shield
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <stdio.h> // for function sprintf

#define TFT_CS     10
#define TFT_RST    -1  // you can also connect this to the Arduino reset
#define TFT_DC     8
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define red_pin 5
#define green_pin 6
#define blue_pin 7
#define max_freq 500
#define cycle 1000
#define on_time 1

#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

uint16_t previousMillis = 0;
uint16_t buttonMillis = 0;

int frequency = 0;
uint8_t strobe_color = 0; //0 white, 1 red, 2 green, 3 blue, 4 purple
uint8_t cursor_location = 0; //0 digit, 1 tens, 2 hundreds


int off_time = 1;
int interval = on_time;
int ledState = HIGH;

void setup() {
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  //pinMode(3,INPUT);
  show_frequency();
  show_Hertz();
}

void loop() {
  uint16_t currentMillis = millis();
    
  if (currentMillis - previousMillis >= interval && frequency != 0) 
  {
    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
      interval = on_time;
      strobe_on();
    } else {
      ledState = LOW;
      interval = off_time;
      strobe_off();
    }  
  }
  else if (frequency == 0)
  {
    digitalWrite(red_pin, HIGH);
    digitalWrite(green_pin, HIGH);
    digitalWrite(blue_pin, HIGH);
  }
  
  if (currentMillis - buttonMillis > 150)
  { 
    uint8_t button_state = readButton();
    buttonMillis = currentMillis;
    if (button_state != BUTTON_NONE)
    {
      if (button_state == BUTTON_LEFT && frequency != max_freq)
      {
        frequency++;
        if (frequency > max_freq) {frequency = max_freq;}
        show_frequency();
        calculate_off_time();
      }
      else if (button_state == BUTTON_RIGHT && frequency != 0)
      {
        frequency--;
        if (frequency < 1) {frequency = 0;}
        show_frequency();
        calculate_off_time();
      }
      else if (button_state == BUTTON_UP && frequency != max_freq)
      {
        frequency = frequency + 10;
        if (frequency > max_freq) {frequency = max_freq;}
        show_frequency();
        calculate_off_time();
      }
      else if (button_state == BUTTON_DOWN && frequency != 0)
      {
        frequency = frequency - 10;
        if (frequency < 1) {frequency = 0;}
        show_frequency();
        calculate_off_time();
      }
      else if (button_state == BUTTON_SELECT)
      {
        update_strobe_color();
      }
    }
  }
}

void strobe_on()
{
  switch (strobe_color)
  {
    case 0:
      digitalWrite(red_pin, HIGH);
      digitalWrite(green_pin, HIGH);
      digitalWrite(blue_pin, HIGH);
      break;
    case 1:
      digitalWrite(red_pin, HIGH);
      break;
    case 2:
      digitalWrite(green_pin, HIGH);
      break;
    case 3:
      digitalWrite(blue_pin, HIGH);
      break;
    case 4:
      digitalWrite(red_pin, HIGH);
      digitalWrite(blue_pin, HIGH);
      break;
    default:
      digitalWrite(red_pin, HIGH);
      digitalWrite(green_pin, HIGH);
      digitalWrite(blue_pin, HIGH);
  }
}

void strobe_off()
{
  digitalWrite(red_pin, LOW);
  digitalWrite(blue_pin,LOW);
  digitalWrite(green_pin,LOW);
}

void update_strobe_color()
{
  strobe_color==4 ? strobe_color = 0 : strobe_color++;
}

void show_frequency()
{
  tft.setCursor(10, 10);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  //tft.setTextWrap(true);
  tft.setTextSize(8);
  char tbs[3];
  sprintf(tbs, "%03d",frequency);
  tft.print(tbs);
}

void show_Hertz()
{
  tft.setCursor(60, 80);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setTextWrap(true);
  tft.setTextSize(4);
  tft.print("Hz");
}

uint8_t readButton(void) {
  float a = analogRead(3);
  
  a *= 5.0;
  a /= 1024.0;

  if (a < 0.2) return BUTTON_DOWN;
  if (a < 1.0) return BUTTON_RIGHT;
  if (a < 1.5) return BUTTON_SELECT;
  if (a < 2.0) return BUTTON_UP;
  if (a < 3.2) return BUTTON_LEFT;
  else return BUTTON_NONE;
}

void calculate_off_time()
{
  off_time = (int)((cycle - frequency * on_time) / frequency);
}