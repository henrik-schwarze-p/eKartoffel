//
//  CoreUI-NumPad.c
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "Namespaces.h"
#include "UIKeyboard.h"
#include "Drawing.h"
#include "Text.h"
#include "UI.h"
#include "Namespaces.h"
#include "Arduino.h"

namespace keyboard {
    const char c[] PROGMEM = "1234567890QWERTYUIOPASDFGHJKL^ZXCVBNM   ";
    const char d[] PROGMEM = "!'$%&?+-,.qwertyuiopasdfghjkl^zxcvbnm   ";

    int         theKeyboard = 0;
    int         numPadW = 32;
    int         numPadH = 40;
    int         xVisor = 160;
    int         yVisor = 165;
    int         widthVisor = 160;
    float       min;
    float       max;
    char        value[10];
    int         tooSmall = 0;
    int         tooBig = 0;
    const char* message;
    void (*callback)(int);
    int maxLen = 20;

    void drawKeyboard();

    void drawVisor() {
        setColor(colorBlack);
        fillRect(xVisor, yVisor, widthVisor, 24);
        setColor(colorDarkGray);
        drawRect(xVisor, yVisor, widthVisor, 24);
        setPrintX(xVisor + 6);
        setPrintY(yVisor + 3);
        setBigFont();
        setColor(colorGreen);
        setPrintModeRamSource();
        printStringRespectingMode();
        setPrintModePSTR();
    }

    void pressed(int param) {
        int len = ramSourceLength();
        if (param == 29) {
            if (len > 0) {
                setRamSource(len - 1, 0);
            }
        } else if (len < maxLen - 1) {
            if (theKeyboard == 0)
                setRamSource(len, pgm_read_byte(c + param));
            else
                setRamSource(len, pgm_read_byte(d + param));
            setRamSource(len + 1, 0);
        }
        drawVisor();
    }

    void showKeyboard();

    void enter(int param) {
        callback(1);
    }

    void cancel(int param) {
        callback(0);
    }

    void otherKeyboard(int param) {
        theKeyboard = !theKeyboard;
        drawKeyboard();
    }

    void drawKeyboard() {
        int x = 0;
        int y = menuBarHeight + margin;
        setBigFont();

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 4; j++) {
                setColor(colorDarkGray);
                if (i == 7 && j == 3)
                    fillRect(x + i * numPadW, y + j * numPadH + j + 1, 32 * 3, numPadH);
                else if ((i != 8 && i != 9) || j != 3)
                    fillRect(x + i * numPadW, y + j * numPadH + j + 1, 31 + (i == 9), numPadH);
                setColor(colorWhite);
                if (theKeyboard)
                    drawChar(pgm_read_byte(d + j * 10 + i), 11 + x + i * numPadW, 8 + y + j * numPadH + j + 1);
                else
                    drawChar(pgm_read_byte(c + j * 10 + i), 11 + x + i * numPadW, 8 + y + j * numPadH + j + 1);
            }
        }
    }

    void showKeyboard() {
        clearRamSource();
        setMargins(margin, 172);
        setPrintY(70);
        numPadH = 32;
        int x = 0;
        int y = menuBarHeight + margin;
        setColor(colorDarkerGray);
        fillRect(x, y, horizontalResolution, numPadH * 4 + 5);
        drawKeyboard();
        addButton(0, 24, 32, 33, pressed, 0, 10, 4, 0, 0, 0);
        setColor(colorBlack);
        setMargins(margin, margin);
        setPrintX(margin);
        setPrintY(171);
        setStandardFont();
        println(message);
        setNormalStyle();
        xVisor = stringWidth(message) + 2 * margin;
        widthVisor = horizontalResolution - xVisor - margin;
        drawVisor();
        toolbarAdd(mini, isenabled, PSTR("|Aa"), otherKeyboard);
        toolbarAdd(PSTR("   Set   "), enter);
        toolbarAdd(PSTR("Cancel"), cancel);
    }

}

void showKeyboardDialog(const char* message, void (*callback)(int)) {
    keyboard::message = message;
    keyboard::callback = callback;
    goToScreen(keyboard::showKeyboard);
}
