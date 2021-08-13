//
//  CoreIcon.h
//  AquaOS
//
//  Created by Kurt Schwarze on 22.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#ifndef CoreIcon_h
#define CoreIcon_h

#define ICON_NOT_CONFIGURED NUMBER_OF_DESCRIPTORS
#define ICON_ON NUMBER_OF_DESCRIPTORS + 1
#define ICON_OFF NUMBER_OF_DESCRIPTORS + 2

void drawIcon(int iconNumber, int x, int y);
int  iconWidth(int iconNumber);
int  iconHeight(int iconNumber);
void drawIconNotConfigured(int x, int y);
void drawIconOn(int x, int y);
void drawIconOff(int x, int y);

#include "Icons3.inc.h"

#endif /* CoreIcon_h */
