//
//  CoreGraphics.h
//  Henrikontroller
//
//  Created by Kurt Schwarze on 13.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#ifndef CoreGraphics_h
#define CoreGraphics_h

#include "Target.h"

#define margin 8

#define menuBarHeight 16
#define toolbarHeight 44

#define LCD_CS A3  // Chip Select goes to Analog 3

#define LCD_CD A2     // Command/Data goes to Analog 2
#define LCD_WR A1     // LCD Write goes to Analog 1
#define LCD_RD A0     // LCD Read goes to Analog 0
#define LCD_RESET A4  // Can alternately just connect to Arduino's reset pin

#define verticalResolution 240
#define horizontalResolution 320

#define colorDisabledToolbar colorFromRGB(200, 200, 200)
#define colorWhite colorFromRGB(255, 255, 255)
#define colorLightGray colorFromRGB(248, 248, 248)
#define colorRed colorFromRGB(255, 0, 0)
#define colorGreen colorFromRGB(0, 255, 0)
#define colorGreen2 colorFromRGB(0, 200, 0)
#define colorYellow colorFromRGB(255, 255, 0)
#define colorOrange colorFromRGB(255, 140, 60)
#define colorBlack colorFromRGB(0, 0, 0)
#define colorBlue colorFromRGB(10, 10, 200)
#define colorDarkGray colorFromRGB(127, 127, 127)
#define colorBitDarkerGray colorFromRGB(100, 100, 100)
#define colorPink colorFromRGB(187, 187, 240)
#define colorDarkerGray colorFromRGB(84, 84, 84)
#define colorBG colorFromRGB(232, 236, 240)
#define colorToolbar colorFromRGB(20, 120, 213)
#define colorMonitor colorFromRGB(20 * 255 / 100, 36 * 255 / 100, 51 * 255 / 100)

void tftReset(void);

// Slow methods, respect clipping
void pushClipping(int x0, int y0, int x1, int y1);
void popClipping(void);
void pushClip();
void popClip();
void drawLine(int x0, int y0, int x1, int y1);
void drawRectDeg(int x, int y, int w, int h);

// Fast methods, don't respect clipping
void drawPixel(int, int);
void drawHorizontalLine(int x, int y, int w);
void drawVerticalLine(int x, int y, int w);
void fillRect(int x, int y, int w, int h);
void drawRect(int x, int y, int w, int h);

int  color(void);
void setColor(int c);
void popColor(void);
void pushColor(void);
void pushColor(int color);

int paletteColor(int index);
int colorFromRGB(int r, int g, int b);

void drawBar(int y, int h);

void activateGraphics();
void deactivateGraphics();
int  isActive();

#endif
