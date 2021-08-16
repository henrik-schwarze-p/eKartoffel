
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
        int i = 0;
        while (i < w) {
            int count = icon(iconNumber, index++);
            if (count < 100) {
                int col = (int)icon(iconNumber, index++);
                setColor(col);
                if (col != 0)
                    drawHorizontalLine(x + i, j, count);
                i += count;
            } else {
                int col = count;
                setColor(col);
                drawPixel(x + i, j);
                i += 1;
            }
        }
    }
    popColor();
}
