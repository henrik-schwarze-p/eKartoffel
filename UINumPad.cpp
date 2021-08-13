//
//  CoreUI-NumPad.c
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "Namespaces.h"
#include "UINumPad.h"
#include "Drawing.h"
#include "Text.h"
#include "UI.h"

namespace numpad {
    int         floatAllowed;
    int         numPadW = 50;
    int         numPadH = 40;
    int         numPadXVisor = margin;
    int         numPadYVisor = menuBarHeight + margin;
    int         numPadHVisor = 155 - 2 * margin;
    int         clear = 0;
    float       min;
    float       max;
    char        value[10];
    int         counter = 0;
    int         tooSmall = 0;
    int         tooBig = 0;
    const char* numPadMsg;
    void (*numPadCallback)(int, float);

    void drawVisor() {
        setColor(colorBlack);
        fillRect(numPadXVisor, numPadYVisor, numPadHVisor, 24);
        setColor(colorDarkGray);
        drawRect(numPadXVisor, numPadYVisor, numPadHVisor, 24);
        setPrintX(numPadXVisor + 6);
        setPrintY(numPadYVisor + 3);
        setBigFont();
        setColor(colorGreen);
        for (int i = 0; i < counter; i++) {
            if (value[i] == '.')
                print(PSTR("."));
            else
                print(value[i]);
        }
    }

    void pressed(int param) {
        if (clear) {
            value[0] = 0;
            counter = 0;
        }
        clear = 0;
        if (param == 10) {
            if (counter == 1 && value[0] == 0)
                return;
            value[counter++] = 0;
        } else if (param == 9) {
            int already = 0;
            for (int i = 0; i < counter; i++)
                if (value[i] == '.' && floatAllowed)
                    already = 1;
            if (!already)
                value[counter++] = '.';
        } else if (param == 11) {
            if (counter > 0)
                counter--;
            if (counter == 0) {
                value[counter++] = 0;
            }
        } else {
            if (counter == 1 && value[0] == 0)
                counter--;
            value[counter++] = param + 1;
            if (param == -1)
                clear = 1;
        }
        drawVisor();
    }

    float toFloat() {
        float r = 0;
        int   i = 0;
        int   sum = 0;
        while (i < counter) {
            if (sum > 0)
                sum++;
            if (value[i] != '.')
                r = r * 10 + value[i];
            else
                sum = 1;
            i++;
        }
        while (sum-- > 1)
            r = r / 10.0;
        return r;
    }

    void showNumPad();
    void numPadSet(int param) {
        if (toFloat() < min) {
            tooBig = 0;
            tooSmall = 1;
            goToScreen(showNumPad);
            return;
        }
        if (toFloat() > max) {
            tooSmall = 0;
            tooBig = 1;
            goToScreen(showNumPad);
            return;
        }

        numPadCallback(1, toFloat());
    }

    void numPadCancel(int param) {
        numPadCallback(0, 0);
    }

    int reverse(int n) {
        int result = 0;
        while (n > 0) {
            result = 10 * result + (n % 10);
            n /= 10;
        }
        return result;
    }

    int digitsOf(int p) {
        if (!p)
            return 1;
        int r = 0;
        while (p > 0) {
            p /= 10;
            r++;
        }
        return r;
    }

    int digit(int n, int offset) {
        offset = digitsOf(n) - offset - 1;
        for (int i = 0; i < offset; i++)
            n /= 10;
        return n % 10;
    }

    void setInitialValue(float ivalue) {
        int intPart = (int)(ivalue);
        for (int i = 0; i < digitsOf(intPart); i++) {
            value[counter++] = digit(intPart, i);
        }
        int decimal = (int)((ivalue * 100)) % 100;
        if (!decimal)
            return;
        value[counter++] = '.';
        value[counter++] = decimal / 10;
        if (decimal % 10)
            value[counter++] = decimal % 10;
    }

    void showNumPad() {
        const char* n0 = PSTR("1");
        const char* n1 = PSTR("2");
        const char* n2 = PSTR("3");
        const char* n3 = PSTR("4");
        const char* n4 = PSTR("5");
        const char* n5 = PSTR("6");
        const char* n6 = PSTR("7");
        const char* n7 = PSTR("8");
        const char* n8 = PSTR("9");
        const char* n9 = PSTR(" ");
        if (floatAllowed)
            n9 = PSTR(".");
        const char* na = PSTR("0");
        const char* nb = PSTR("<");
        setMargins(margin, 172);
        setPrintY(70);
        print(numPadMsg);
        int x = 320 - 153 - margin;
        int y = menuBarHeight + 4 + margin / 2;
        setColor(colorDarkerGray);
        fillRect(x, y, numPadW * 3 + 4, numPadH * 4 + 5);
        const char* c[] = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, na, nb};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                setColor(colorDarkGray);
                fillRect(x + i * numPadW + i + 1, y + j * numPadH + j + 1, numPadW, numPadH);
                setColor(colorWhite);
                const char* a = c[j * 3 + i];
                addLabel(x + i * numPadW + i + 1, y + j * numPadH + j + 1, a, /* pressed, j * 3 + i, 0,*/ numPadW,
                         numPadH, colorWhite, STYLE_CENTERED | STYLE_BIGFONT, 0);
            }
        }
        addButton(160, 25, 50, 40, pressed, 0, 3, 4, 1, 1, 0);
        setColor(colorRed);
        setStandardFont();
        setPrintX(margin);
        setPrintY(160);
        if (tooBig) {
            print(PSTR("Should be smaller than "));
            println(max);
        } else if (tooSmall) {
            print(PSTR("Should be larger than "));
            println(min);
        }
        numpad::drawVisor();

        toolbarAdd(PSTR("    Set    "), numPadSet);
        toolbarAdd(PSTR("Cancel"), numPadCancel);
    }
}

void showNumPad(const char* message,
                float       initialValue,
                float       min,
                float       max,
                void (*callback)(int, float),
                int floatAllowed) {
    numpad::tooSmall = 0;
    numpad::tooBig = 0;
    numpad::floatAllowed = floatAllowed;
    numpad::min = min;
    numpad::max = max;
    numpad::pressed(-1);
    numpad::counter = 0;
    numpad::numPadMsg = message;
    numpad::numPadCallback = callback;
    numpad::setInitialValue(initialValue);
    goToScreen(numpad::showNumPad);
}
