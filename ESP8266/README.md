Refrigerator Lights
===================
Refrigerator Lights is a simple blinkenlights update of the refrigerator of the [MunichMakerLab](https://munichmakerlab.de/) hackerspace.

It drives a small WS2812b LED strip based matrix of 15x10 LEDs behind a diffuse cover; which replaces the old lamp. 
Controllable by MQTT over ESP8266 WiFi module.

[![Refrigerator Lights screenshot](https://cloud.githubusercontent.com/assets/6171589/7560420/513b217e-f7c3-11e4-8fe3-a6d3037b3baf.jpg)](https://www.tumblr.com/video/munichmakerlab/116758165625/700/)

Features
--------
* FastLED library for all the fancy animations
* ESP8266 WiFi module for remote access
* MQTT to remote control the animations and state

Hardware
--------
* WS2812b LED strips (15x10 ~ 50x15cm)
* 5V 12A power supply
* Teensy 3.1 microcontroller
* ESP8266 (ESP-12) WiFi board

Wiring
------
![wiring diagram](/wiring/refrigerator-lights-wiring.png "wiring diagram")

Libraries
---------
* [Espduino](https://github.com/tuanpmt/espduino) (ESP8266 Arduino library + ESP-bridge)
* [FastLED](https://github.com/FastLED/FastLED) (LED light controller by FastLED library)
* [RGBLEDS](https://github.com/AaronLiddiment/RGBLEDS) (LEDMatrix & LEDText)
* [LedEffects by ratkins](https://bitbucket.org/ratkins/ledeffects/src/?at=default)

MQTT-topics
-----------
| topic                                | message                                            |
| ------------------------------------ | -------------------------------------------------- |
| mumalab/fridge/connected             | connected                                          |
| mumalab/fridge/state                 | on / off                                           |
| mumalab/fridge/brightness            | 0 to 255                                           |
| mumalab/fridge/mode/animation/state  | on / off                                           |
| mumalab/fridge/mode/animation/effect | twinkle / plasma / snake / life / sprite / rainbow |
| mumalab/fridge/mode/ticker/state     | on / off                                           |
| mumalab/fridge/mode/ticker/color     | #000000 to #ffffff                                 |
| mumalab/fridge/mode/ticker/bounce    | 1 / 0                                              |
| mumalab/fridge/mode/ticker/direction | left / right / up / down                           |
| mumalab/fridge/mode/ticker/text      | scroll message as text...                          |

TODOs
-----
* bugfix and add the text ticker class
* stay alive or reset ESP8266 on freeze or unstable handling...