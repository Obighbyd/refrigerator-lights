/******************************************************************************
  The MIT License (MIT)

  Copyright (c) 2015 Juergen Skrotzky alias Jorgen (JorgenVikingGod@gmail.com)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
******************************************************************************/

// define, to tell FastLED 3.1 to work as stable version
#define FASTLED_INTERNAL

#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
#include "Animation.h"
#include "Ticker.h"
#include "Twinkle.cpp"
#include "Plasma.cpp"
#include "Snake.cpp"
#include "Life.cpp"
#include "Sprite.cpp"
#include "RainbowColor.cpp"

// Create an IntervalTimer object 
IntervalTimer intervalTimer;

// Teensy 3.1 pinout:
// ====================
//  0 = RX >> EXP8266 TX
//  1 = TX >> ESP8266 RX
//  2 = LED data pin 1
// 14 = LED data pin 2
//  7 = LED data pin 3

// define LED digital pin, color order, chipset and brightness
#define LED_PIN1    2
#define LED_PIN2    14
#define LED_PIN3    7
#define COLOR_ORDER   GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS  170

// helper variables for brightness and mode
int state = 1;
Animation animation;
Ticker ticker;
int brightness = BRIGHTNESS;

// initial matrix layout (to get led strip index by x/y)
#define MATRIX_WIDTH   15
#define MATRIX_HEIGHT  10
#define MATRIX_TYPE  HORIZONTAL_ZIGZAG_MATRIX
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// initial scroll message
cLEDText ScrollingMsg;
const unsigned char TxtDemo[] = { EFFECT_SCROLL_LEFT "      LEFT SCROLL "
                  EFFECT_SCROLL_RIGHT "      LLORCS THGIR"
                  EFFECT_SCROLL_DOWN "      SCROLL DOWN       SCROLL DOWN      " EFFECT_FRAME_RATE "\x04" " SCROLL DOWN      " EFFECT_FRAME_RATE "\x00" " "
                  EFFECT_SCROLL_UP "       SCROLL UP         SCROLL UP       " EFFECT_FRAME_RATE "\x04" "  SCROLL UP       " EFFECT_FRAME_RATE "\x00" " "
                  EFFECT_CHAR_UP EFFECT_SCROLL_LEFT "      UP"
                  EFFECT_CHAR_RIGHT "  RIGHT"
                  EFFECT_CHAR_DOWN "  DOWN"
                  EFFECT_CHAR_LEFT "  LEFT"
                  EFFECT_HSV_CV "\x00\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "       HSV_CV 00-40"
                  EFFECT_HSV_CH "\x00\xff\xff\x40\xff\xff" "  HSV_CH 00-40"
                  EFFECT_HSV_AV "\x00\xff\xff\x40\xff\xff" "  HSV_AV 00-40"
                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "  HSV_AH 00-FF"
                  "       " EFFECT_HSV "\x00\xff\xff" "R" EFFECT_HSV "\x20\xff\xff" "A" EFFECT_HSV "\x40\xff\xff" "I" EFFECT_HSV "\x60\xff\xff" "N" EFFECT_HSV "\xe0\xff\xff" "B" EFFECT_HSV "\xc0\xff\xff" "O"
                  EFFECT_HSV "\xa0\xff\xff" "W" EFFECT_HSV "\x80\xff\xff" "S " EFFECT_DELAY_FRAMES "\x00\x96" EFFECT_RGB "\xff\xff\xff" };

// publish mqtt message to ESP8266 over Serial1
void mqttPublish(String strTopic, String strMessage) {
  // send resetChar + topic + separator + message + \n
  // f.e. "#m/s:1\n"
  Serial1.print(SerialReceiver::resetChar);
  Serial1.print(strTopic);
  Serial1.print(":");
  Serial1.println(strMessage);
}

// handle received serial data (myqtt messages from ESP8266)
void processSerialData(String strTopic, String strMessage)
{ 
  // print out debug messages
  Serial.print("Received: ");
  Serial.print(strTopic);
  Serial.print(":");
  Serial.println(strMessage);
  
  // handle all mqtt messages
  if (strTopic.startsWith("m/")) {
    if (strTopic == "m/s") {
      state = strMessage.toInt();
    } else if (strTopic == "m/b") {
      brightness = strMessage.toInt();
      brightness = min(255, max(brightness, 0));
      FastLED.setBrightness(brightness);
    } else if (strTopic == "m/e") {
      animation.setEffect(strMessage.toInt());
      animation.setState(1);
      ticker.setState(0);
    } else if (strTopic == "m/tc") {
      ticker.setColor(strMessage);
      ticker.setState(1);
      animation.setState(0);
    } else if (strTopic == "m/tb") {
      ticker.setBounce(strMessage.toInt() == 1 ? true : false);
      ticker.setState(1);
      animation.setState(0);
    } else if (strTopic == "m/td") {
      ticker.setDirection(strMessage.toInt());
      ticker.setState(1);
      animation.setState(0);
    } else if (strTopic == "m/tt") {
      ticker.setText(strMessage);
      ticker.setState(1);
      animation.setState(0);
    }
    // print out debug messages
    Serial.print("state: ");
    Serial.println(state);
    Serial.print("brightness: ");
    Serial.println(brightness);
    Serial.print("effect: ");
    Serial.println(animation.effect());
    Serial.println("");
  }
}

// one frame of animations
void DrawOneFrame()
{
  if (state == 0 || (animation.state() == 0 && ticker.state() == 0)) {
    allOff();
  } else {
    FastLED.setBrightness(brightness);
    if (animation.state() == 1) {
      switch (animation.effect()) {
        default:
        case 1:  doTwinkle();       break;
        case 2:  doPlasma();        break;
        case 3:  doSnake();         break;
        case 4:  doLife();          break;
        case 5:  doSprite();        break;
        case 6:  doRainbowColor();  break;
      }
    } else if (ticker.state() == 1) {
      doScrollText();
    }
  }
}

unsigned long initial_time;

// all LEDs black
void allOff() {
  initial_time = millis();
  
  fill_solid(leds[0], leds.Size(), CRGB::Black);
  FastLED.setBrightness(0);
  FastLED.show();
  
  unsigned long current_time = millis();
  Serial.print("allOff - time: ");
  Serial.println(current_time - initial_time);
}

void doTwinkle() {
  initial_time = millis();
    
  Twinkle twinkle(leds[0], leds.Width(), leds.Height(), true, true);
  twinkle.start();
  
  unsigned long current_time = millis();
  Serial.print("doTwinkle - time: ");
  Serial.println(current_time - initial_time);
}

void doPlasma() {
  initial_time = millis();
  
  Plasma plasma(leds[0], leds.Width(), leds.Height());
  plasma.start();
  
  unsigned long current_time = millis();
  Serial.print("doPlasma - time: ");
  Serial.println(current_time - initial_time);
}

void doSnake() {
  initial_time = millis();
  
  Snake snake(leds[0], leds.Width(), leds.Height());
  snake.start();
  
  unsigned long current_time = millis();
  Serial.print("doSnake - time: ");
  Serial.println(current_time - initial_time);
}

void doLife() {
  initial_time = millis();
  
  Life life(leds[0], leds.Width(), leds.Height(), 56);
  life.start();
  
  unsigned long current_time = millis();
  Serial.print("doLife - time: ");
  Serial.println(current_time - initial_time);
}

void doSprite() {
  initial_time = millis();
  
  Sprite sprite(leds[0], leds.Width(), leds.Height());
  sprite.start();
  
  unsigned long current_time = millis();
  Serial.print("doSprite - time: ");
  Serial.println(current_time - initial_time);
}

void doRainbowColor() {
  initial_time = millis();
  
  RainbowColor rainbowColor(leds[0], leds.Width(), leds.Height());
  rainbowColor.start();
  
  unsigned long current_time = millis();
  Serial.print("doRainbowColor - time: ");
  Serial.println(current_time - initial_time);
}

void doScrollText() {
  initial_time = millis();
  
  if (ScrollingMsg.UpdateText() == -1) {
    ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
  } else {
    FastLED.show();
  }
  delay(10);
  
  unsigned long current_time = millis();
  Serial.print("doScrollText - time: ");
  Serial.println(current_time - initial_time);
}

// setup demo code
void setup() {
  // set default speed for serial (for debug with serial monitor)
  Serial.begin(9600);

  // set speed of UART to communicate with ESP8266 WiFi module
  Serial.println("initial UART");
  Serial1.begin(115200);
  
  // initial the serial receiver with 'Serial1' serialport, ':' separator and '#' reset char
  // SerialReceiver will reset collection data on '#'
  SerialReceiver::initial(Serial1, ':', '#');

  // initial
  state = 1;
  animation.setState(1);
  animation.setEffect(6);
  ticker.setState(0);

  brightness = 170;

  // initial LEDs
  FastLED.addLeds<CHIPSET, LED_PIN1, COLOR_ORDER>(leds[0], 0, 60).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN2, COLOR_ORDER>(leds[0], 60, 60).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN3, COLOR_ORDER>(leds[0], 120, 30).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);

  ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
  ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);

  Serial.println("Matrix: initialed");
}

// execute demo fade code
void loop()
{
  // draw animation on each loop frame
  DrawOneFrame();

  // handle received serial commands
  if (!SerialReceiver::isReady) {
    //delay(10);
    return;
  }
  // handles the received data (command and message)
  processSerialData(SerialReceiver::command, SerialReceiver::payload);
  SerialReceiver::reset();
}

// handle serial receiver events
void serialEvent1() {
  SerialReceiver::processSerialEvent();
}
