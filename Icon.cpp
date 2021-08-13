
//
//  CoreIcon.c
//  AquaOS
//
//  Created by Kurt Schwarze on 22.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#include "Arduino.h"
#include "Target.h"
#include "Icon.h"
#include "Icons.h"
#include "Drawing.h"

#include "NumberOfDescriptors.inc.h"
#include "Icons4.inc.h"

const int icon(int number, int i) {
    return pgm_read_word_near(iconData[number] + i);
}

int iconWidth(int iconNumber) {
    return (int)icon(iconNumber, 0);
}

int iconHeight(int iconNumber) {
    return (int)icon(iconNumber, 1);
}

void drawIcon(int iconNumber, int x, int y) {
    int w = (int)icon(iconNumber, 0);
    int h = (int)icon(iconNumber, 1);
    int index = 2;
    pushColor();
    for (int j = y; j < h + y; j++) {
        // int count = w;
        for (int i = x; i < x + w; i++) {
            int col = (int)icon(iconNumber, index);
            if (col != 0) {
                setColor(col);
                drawPixel(i, j);
            }
            index += 1;
        }
    }
    popColor();
}
