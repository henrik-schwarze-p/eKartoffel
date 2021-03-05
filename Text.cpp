//
//  TextLib.c
//  Henrikontroller
//
//  Created by Kurt Schwarze on 13.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#include "Arduino.h"
#include "FatalError.h"
#include "Text.h"
#include "Fonts.h"
#include "Drawing.h"
#include "FastFont.h"

char byteReadingMode = BYTE_READING_MODE_PSTR;

const char* pstrSource;
int         eepromSource;
char        ramSource[20];

int drawMargin = 0;
int currentFont = standardFont;
int textIsBold = 0;
int textLineHeight = 16;  // just for the small font
int eating;

char firstOfManyDefinition = 0;

int textPrintX = 0;
int textPrintY = 0;
int minPrintX = margin;
int maxPrintX = 320 - margin;

// THE MAIN GET CHARACTER FUNCTION

char charAtIndex(int i) {
    if (byteReadingMode == BYTE_READING_MODE_PSTR)
        return pgm_read_byte(pstrSource + i);
    if (byteReadingMode == BYTE_READING_MODE_EEPROM)
        return readByteFromEEPROM(eepromSource + i);
    if (byteReadingMode == BYTE_READING_MODE_RAM)
        return ramSource[i];
    fatalError(43, byteReadingMode);
    return 'A';
}

//
// FONT DATA FOR BITMAP FONTS
//

// Call with -1 to get the height
const int bigFontData(int i) {
    // first byte is the height, we skip it
    return pgm_read_word_near(_bigChars + i + 1);
}

//
// CHAR METRICS
//

int fontHeight() {
    if (currentFont == standardFont)
        return standardFontHeight;
    return bigFontData(-1);
}

int charWidth(unsigned char a) {
    if (a < 1 || a > 126)
        fatalError(25, a);
    if (currentFont == standardFont)
        return pgm_read_byte_near(_standardCharsWidths + a - 1);
    int fh = fontHeight();
    return bigFontData(a * (fh + 1));
}

int lineHeight() {
    return 12;
}

int spaceSeparation() {
    return 5;
}

//
// STRING FNS
//

unsigned int stringLen() {
    int i = 0;
    while (charAtIndex(i++))
        ;
    return i - 1;
}

//
// PRINT POSITION
//

int printY() {
    return textPrintY;
}

int printX() {
    return textPrintX;
}

void setPrintX(int x) {
    textPrintX = x;
}

void setPrintY(int y) {
    textPrintY = y;
}

void addPrintY(int y) {
    textPrintY += y;
}

void setMaxPrintX(int x) {
    maxPrintX = x;
}

void setDefaultMaxPrintX() {
    maxPrintX = horizontalResolution - margin;
}

void setStandardFont() {
    currentFont = standardFont;
}

void setBigFont() {
    currentFont = bigFont;
}

void setBoldStyle() {
    textIsBold = 1;
}

void setNormalStyle() {
    textIsBold = 0;
}

void setMargins(int x1, int x2) {
    maxPrintX = horizontalResolution - x2;
    minPrintX = x1;
    textPrintX = x1;
}

//
// DRAWING 1 CHARACTER
//

int basicDrawChar(int a, int x, int y) {
    if (currentFont != bigFont && a > 32)
        return drawFx(a - 33, x, y);
    if (a < 1 || a > 127)
        fatalError(26, a);
    int fh = fontHeight() + 1;
    int width = charWidth(a);
    if (a != 32)
        for (int i = 1; i < fh; i++) {
            int b = bigFontData(a * fh + i);
            for (int j = 0; j < width; j++) {
                if (b % 2 == 1)
                    drawPixel(j + x, y + i);
                b = b / 2;
            }
        }
    return width;
}

int bgc = colorBlue;
int drawChar(int a, int x, int y) {
    if (a == '[') {
        bgc = colorDarkerGray;
        drawLine(x, y - 2, x, y + 13);
        return 1;
    }
    if (a == ']') {
        drawLine(x, y - 2, x, y + 13);
        return 1;
    }
    int r;
    if (drawMargin) {
        r = charWidth(a);
        pushColor();
        setColor(bgc);
        fillRect(x - 2, y - 1, r + 4, 14);
        popColor();
        drawLine(x - 2, y - 2, x + r + 2, y - 2);
        drawLine(x - 2, y + 13, x + r + 2, y + 13);
    }
    r = basicDrawChar(a, x, y);
    if (textIsBold)
        return basicDrawChar(a, x + 1, y) + 1;
    return r;
}

//
// MEASURING INTS AND STRINGS
//

int intWidth(int a) {
    int result = 0;
    while (1) {
        int remainder = a % 10;
        result += charWidth(remainder + 48) + characterSeparation;
        a /= 10;
        if (a == 0)
            break;
    }
    return result;
}

int intWidth(int a, int padding) {
    int result = 0;
    int z = 0;
    while (z < padding) {
        int remainder = a % 10;
        result += charWidth(remainder + 48) + characterSeparation;
        a /= 10;
        z++;
    }
    return result;
}

unsigned int stringWidth() {
    int w = 0;
    int i = 0;
    while (charAtIndex(i)) {
        int width = charWidth(charAtIndex(i++));
        w += width + characterSeparation;
    }

    return w;
}

unsigned int stringWidth(int from, int toLabel) {
    int w = 0;
    int i = from;
    while (i <= toLabel) {
        int width = charWidth(charAtIndex(i++));
        w += width + characterSeparation;
    }
    return w;
}

void measureText(int* widths, int* count) {
    int icount = 0;
    int i = 0;
    int x = 0;
    while (charAtIndex(i)) {
        if (charAtIndex(i) == ' ')
            widths[icount] = i;
        x += charWidth(charAtIndex(i)) + characterSeparation;
        if (x > horizontalResolution - 10 * margin) {
            icount++;
            x = 0;
            if (icount == 10)
                return;
        }
        i++;
    }
    widths[icount] = i;
    *count = icount + 1;
}

int stringWidthTo(int limit) {
    int w = 0;
    int i = 0;
    while (charAtIndex(i) && i < limit) {
        int width = charWidth(charAtIndex(i));
        i += 1;
        w += width + characterSeparation;
        if (textIsBold)
            w += 1;
    }
    return w;
}

int floatWidth(double f, int decimals) {
    int w = intWidth((int)f);
    w += stringWidth(PSTR("."));
    f -= int(f);
    while (decimals) {
        decimals--;
        f *= 10;
    }
    w += intWidth(f);
    return w;
}

int wordWidth(int i) {
    int           w = 0;
    unsigned char c = charAtIndex(i);
    while (c && c != ' ' && c != '\n') {
        int width = charWidth(c);
        w += width + characterSeparation;
        i++;
        c = charAtIndex(i);
    }
    return w;
}

int stringWidthFrom(int from) {
    int w = 0;
    int i = from;
    while (charAtIndex(i)) {
        int width = charWidth(charAtIndex(i));
        i += 1;
        w += width + characterSeparation;
    }
    return w;
}

//
// DRAWING STRINGS
//

void drawString(int x, int y) {
    int i = 0;
    while (charAtIndex(i)) {
        if (charAtIndex(i) == '\n') {
            x = 4;
            y += 10;
        } else {
            if (charAtIndex(i) == ']')
                drawMargin = 0;
            int charWidth = drawChar(charAtIndex(i), x, y);
            if (charAtIndex(i) == '[')
                drawMargin = 1;
            x += charWidth + characterSeparation;
        }
        i += 1;
    }
}

void drawString(int x, int y, int from, int end) {
    int i = from;
    while (i <= end) {
        if (charAtIndex(i) == ']')
            drawMargin = 0;
        int charWidth = drawChar(charAtIndex(i), x, y);
        if (charAtIndex(i) == '[')
            drawMargin = 1;
        x += charWidth + characterSeparation;
        i += 1;
    }
}

void drawStringWithIntSubscript(int ox, int y, int width, int subscript) {
    int x = ox;
    int i = 0;
    int l = 0;
    int space = 0;
    int type = 0;
    while (charAtIndex(l))
        l++;
    while (charAtIndex(i)) {
        if (charAtIndex(i) == ' ') {
            if (type == 0) {
                type = 1;
                space = i;
            } else if (type == 1 && i > l / 2) {
                type = 2;
                space = i;
            }
        }
        i++;
    }
    i = 0;
    x = ox + width / 2 - stringWidthTo(space) / 2;
    while (charAtIndex(i)) {
        if (charAtIndex(i) == ' ' && i == space) {
            x = ox + width / 2 - stringWidthFrom(space + 1) / 2;
            y += 16;
        } else {
            if (charAtIndex(i) == ']')
                drawMargin = 0;

            int charWidth = drawChar(charAtIndex(i), x, y);
            if (charAtIndex(i) == '[')
                drawMargin = 1;
            x += charWidth + characterSeparation;
        }
        i += 1;
    }
    if (subscript)
        drawInt(subscript, x, y + 5);
}

// Draws a String with templates inside, like "Hello [Name], how are [You]"
// In this case Name and You are consumed by fnTpl.
void drawTPLString(int x, int y, void (*fnTpl)(int)) {
    int i = 0;
    int j = 0;
    eating = 0;
    unsigned char c = charAtIndex(j);
    while (c) {
        if (c == '[') {
            setBoldStyle();
            textPrintX = x + 5;
            const char* push = pstrSource;
            textPrintY += 3;
            fnTpl(i++);
            pstrSource = push;
            textPrintY -= 3;
            x = textPrintX + 5;
            setNormalStyle();
            eating = 1;
        } else if (c == ']')
            eating = 0;
        else if (!eating)
            x += drawChar(c, x, y) + characterSeparation;
        j++;
        c = charAtIndex(j);
    }
}

void drawFloat(double f, int decimals, int x, int y) {
    drawInt((int)f, x, y);
    x += intWidth((int)f);
    drawString(PSTR("."), x, y);
    x += stringWidth(PSTR("."));
    f -= int(f);
    while (decimals) {
        decimals--;
        f *= 10;
    }
    drawInt((int)f, x, y);
}

void drawInt(int n, int x, int y) {
    long reversedNumber = 0, remainder;
    int  c = 0;
    while (n != 0) {
        remainder = n % 10;
        reversedNumber = reversedNumber * 10 + remainder;
        n /= 10;
        c += 1;
    }
    if (c == 0)
        c = 1;
    while (c > 0) {
        int delta = drawChar((reversedNumber % 10) + 48, x, y) + characterSeparation;
        x += delta;
        reversedNumber /= 10;
        c--;
    }
}

void drawInt(int n, int x, int y, int padding) {
    int reversedNumber = 0, remainder;
    int c = 0;
    while (n != 0) {
        remainder = n % 10;
        reversedNumber = reversedNumber * 10 + remainder;
        n /= 10;
        c += 1;
    }
    if (c == 0)
        c = 1;
    for (int i = 0; i < padding - c; i++)
        x += drawChar('0', x, y) + characterSeparation;
    while (c > 0) {
        int delta = drawChar((reversedNumber % 10) + '0', x, y) + characterSeparation;
        x += delta;
        reversedNumber /= 10;
        c--;
    }
}

int drawWord(int i) {
    unsigned char c = charAtIndex(i);
    while (c && c != ' ' && c != '\n') {
        if (charWidth(c) + textPrintX < maxPrintX)
            textPrintX += drawChar(c, textPrintX, textPrintY) + characterSeparation;
        ;
        i++;
        c = charAtIndex(i);
    }
    return i;
}

void drawCenteredString() {
    int x = horizontalResolution / 2 - stringWidth(pstrSource) / 2;
    drawString(pstrSource, x, printY());
}

void drawCenteredStringPart(int start, int end, int y) {
    int w = 0;
    for (int i = start; i < end; i++)
        w += charWidth(charAtIndex(i)) + characterSeparation;
    int x = horizontalResolution / 2 - w / 2;
    for (int i = start; i < end; i++)
        x += drawChar(charAtIndex(i), x, y) + characterSeparation;
    ;
}

//
// PRINTING
// Like drawing, but moves the cursor and respects margins
//

// the main print string method. The MODE has to be but before calling this method!
void printStringRespectingMode() {
    int i = 0;
    int c = charAtIndex(i);
    while (c) {
        if (c == ' ') {
            textPrintX += spaceSeparation();
            i++;
        } else if (c == '\n') {
            println();
            i++;
        } else if (textPrintX + wordWidth(i) < maxPrintX || (textPrintX == minPrintX)) {
            i = drawWord(i);
        } else {
            textPrintY += textLineHeight;
            textPrintX = minPrintX;
        }
        c = charAtIndex(i);
    }
}

void cleanIntPrint(int a, int extra) {
    int w = intWidth(a) + extra;
    pushColor();
    setColor(colorBG);
    fillRect(textPrintX, textPrintY, w, fontHeight());
    popColor();
    drawInt(a, textPrintX, textPrintY);
}

void print(unsigned int i) {
    print((int)i);
}

void println(int n) {
    print(n);
    println();
}

void println(float n) {
    print(n);
    println();
}

void println() {
    textPrintY += textLineHeight;
    textPrintX = minPrintX;
}

void printMiniIcon() {
    setBigFont();
    printStringRespectingMode();
}

void printDefinition() {
    if (!firstOfManyDefinition) {
        println();
    }
    firstOfManyDefinition = 0;
    setMargins(margin, margin);

    setBoldStyle();
    printStringRespectingMode();
    setNormalStyle();
    print(PSTR("  "));
    int x = printX();
    setMargins(2 * margin, margin);
    setPrintX(x);
}

void endDefinitions() {
    setMargins(margin, margin);
}

// Always two decimals, rounded
void print(float a) {
    print((int)a);
    print(PSTR("."));
    a += 0.005;
    int n = (a - (int)a) * 100;
    print(n / 10);
    print(n % 10);
}

void print(int a) {
    int z = intWidth(a);
    if (textPrintX + z > maxPrintX) {
        textPrintX = minPrintX;
        textPrintY += textLineHeight;
    }
    drawInt(a, textPrintX, textPrintY);
    textPrintX += intWidth(a);
}

void print(int a, int padding) {
    int z = intWidth(a);
    if (textPrintX + z > maxPrintX) {
        textPrintX = minPrintX;
        textPrintY += textLineHeight;
    }
    drawInt(a, textPrintX, textPrintY, padding);
    textPrintX += intWidth(a, padding);
}

void printTitle() {
    setBigFont();
    setColor(colorWhite);
    fillRect(22, 0, 250 - 22, 16);
    setColor(colorDarkGray);
    drawHorizontalLine(0, 16, 320);
    setColor(colorBlack);
    setPrintY(0);
    drawCenteredString();
    setStandardFont();
    setColor(colorBlack);
    setPrintY(menuBarHeight + 1 + margin);
}

void cleanRestOfLine() {
    pushColor();
    setColor(colorBG);
    fillRect(printX(), printY(), horizontalResolution, fontHeight());
    popColor();
}

//
// DEALING WITH THE BUFFER (RAM SOURCE)
//

char getRamSource(int i) {
    return ramSource[i];
}

void setRamSource(int index, char val) {
    ramSource[index] = val;
}

void clearRamSource() {
    for (int i = 0; i < 20; i++)
        ramSource[i] = 0;
}

int ramSourceLength() {
    int i = 0;
    while (ramSource[i])
        i++;
    return i;
}

//
// SETTING MODE
//

void setPrintModeRamSource() {
    byteReadingMode = BYTE_READING_MODE_RAM;
}

void setPrintModePSTR() {
    byteReadingMode = BYTE_READING_MODE_PSTR;
}

void setPrintModeEEPROM(int offset) {
    byteReadingMode = BYTE_READING_MODE_EEPROM;
    eepromSource = offset;
}

//
// SHORTCUT FOR PSTR
//

void drawCenteredString(const char* s) {
    setPrintModePSTR();
    pstrSource = s;
    drawCenteredString();
}

void drawCenteredStringPart(const char* m, int start, int end, int y) {
    setPrintModePSTR();
    pstrSource = m;
    drawCenteredStringPart(start, end, y);
}

void measureText(const char* a, int* widths, int* count) {
    setPrintModePSTR();
    pstrSource = a;
    measureText(widths, count);
}

void printTitle(const char* title) {
    setPrintModePSTR();
    pstrSource = title;
    printTitle();
}

void printDefinition(const char* title) {
    setPrintModePSTR();
    pstrSource = title;
    printDefinition();
}

void drawTPLString(const char* s, int x, int y, void (*fnTpl)(int)) {
    setPrintModePSTR();
    pstrSource = s;
    drawTPLString(x, y, fnTpl);
}

void drawString(const char* s, int x, int y) {
    setPrintModePSTR();
    pstrSource = s;
    drawString(x, y);
}

void drawString(const char* s, int x, int y, int from, int end) {
    setPrintModePSTR();
    pstrSource = s;
    drawString(x, y, from, end);
}

void println(const char* s) {
    print(s);
    println();
}

unsigned int stringWidth(const char* s) {
    setPrintModePSTR();
    pstrSource = s;
    return stringWidth();
}

unsigned int stringWidth(const char* s, int from, int toLabel) {
    setPrintModePSTR();
    pstrSource = s;
    return stringWidth(from, toLabel);
}

void print(const char* a) {
    setPrintModePSTR();
    pstrSource = a;
    printStringRespectingMode();
}

void printAlignedRight(const char* s, int x) {
    setPrintModePSTR();
    pstrSource = s;
    setPrintX(x - stringWidth());
    printStringRespectingMode();
}

void drawStringWithIntSubscript(const char* s, int ox, int y, int width, int suffix) {
    setPrintModePSTR();
    pstrSource = s;
    drawStringWithIntSubscript(ox, y, width, suffix);
}

void printMiniIcon(const char* c) {
    setPrintModePSTR();
    pstrSource = c;
    printMiniIcon();
}

unsigned int stringLen(const char* c) {
    setPrintModePSTR();
    pstrSource = c;
    return stringLen();
}
