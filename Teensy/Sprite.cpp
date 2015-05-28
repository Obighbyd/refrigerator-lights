/*
 * Circles implementation for the GauntletII project.
 */

#include "Effect.h"

#define SPRITE_WIDTH 9
#define SPRITE_HEIGHT 9

static uint8_t heartData[SPRITE_WIDTH * SPRITE_HEIGHT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

static uint8_t ghostData[SPRITE_WIDTH * SPRITE_HEIGHT] = {
    0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF,
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
};

class Sprite : public Effect {

   
private:
    
    int spriteWidth, spriteHeight;
    

public:
    Sprite(CRGB *leds, int width, int height) : Effect(leds, width, height), spriteWidth(SPRITE_WIDTH), spriteHeight(SPRITE_HEIGHT) {
    }
    
    void start() {
        for (int i = 0; i < 400; i++) {
            if (random(100) > 98) {
                if (!blit(ghostData, 178, random(0, width - spriteWidth), random(0, height - spriteHeight)))
                  return;
            } else {
                if (!blit(heartData, 0, random(0, width - spriteWidth), random(0, height - spriteHeight)))
                  return;
            }
            FastLED.show();
            delay(100);
            for (int i = 0; i < width * height; i++) {
                leds[i].fadeToBlackBy(64);
            }
        }
    }
    
    bool blit(uint8_t *sprite, uint8_t hue, int x, int y) {
        uint8_t saturation = random(8, 16) * 16;
        for (int spx = 0; spx < spriteWidth; spx++) {
            for (int spy = 0; spy < spriteHeight; spy++) {
                SerialReceiver::processSerialEvent();
                if (SerialReceiver::isReady) {
                  return false;
                }
                if (inXRange(spx + x) && inYRange(spy + y)) {
                    uint8_t spritePixel = sprite[spy * spriteHeight + spx];
                    if (spritePixel) {
                        pixel(spx + x, spy + y) = CHSV(hue, saturation, spritePixel);
                    }
                }
            }
        }
        return true;
    }

};

