
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
    for (int j = y; j < h + y; j++) {
        int i = x;
        int count = w;
        while (count > 0) {
            int          reps = (int)icon(iconNumber, index);
            unsigned int c = icon(iconNumber, index + 1);
            index += 2;
            count -= reps;
            if (c != 0) {
                setColor((int)c);
                if (reps == 1)
                    drawPixel(i, j);
                else
                    drawHorizontalLine(i, j, reps);
            }
            i += reps;
        }
    }
}
