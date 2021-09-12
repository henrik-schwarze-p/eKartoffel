//
//  BoardLib.c
//  AquaOS
//
//  Created by Kurt Schwarze on 11.02.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

int readCount = 135 * 137;
int boardInTransaction = 0;

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "BoardLib.h"
#include "Drawing.h"
#include "Chunks.h"
#include "Text.h"
#include "Heap.h"
#include "Namespaces.h"

typedef struct StructBoardButton {
    int value;  // 0,1 ; 0->1023
    int justRead;
} BoardButton;

#define colorTitle colorFromRGB(250, 250, 250)
#define colorTitle2 colorFromRGB(60, 60, 60)
#define colorPortWhite colorFromRGB(200, 200, 220)
#define maxNumberOfBoardLines 15000
#define maxShownLines 40

StructBoardButton boardAnalogButtons[100];
StructBoardButton boardDigitalButtons[100];

int   boardInitialized = 0;
int   boardSelectedAPort = -1;
int   boardAnalogValue = 0;
int   numberOfBoardLines = 0;
int   firstBoardLine = 0;
char* boardLinesText[maxNumberOfBoardLines];
char* boardCurrentLine = 0;

void boardDigitalPortClicked(int param) {
    boardDigitalButtons[param].value = !boardDigitalButtons[param].value;
    pmDigitalWrite(param, boardDigitalButtons[param].value);
    boardTextRepaint();
}

void boardAnalogPortClicked(int param) {
    boardSelectedAPort = param;
    boardAnalogValue = boardAnalogButtons[param].value;
    boardTextRepaint();
}

void boardInitialize() {
    boardInitialized = 1;
    for (int i = 0; i < 100; i++) {
        boardAnalogButtons[i].justRead = 0;
        boardAnalogButtons[i].value = 0;
        boardDigitalButtons[i].justRead = 0;
        boardDigitalButtons[i].value = 0;
    }
}

void dump(int param) {
    dumpChunks();
    dumpHeaps();
    dumpChunksTxt();
    dumpHeapsTxt();
}

void boardDrawDigitalPort(int p, int x, int y) {
    int d = 9;
    int add = 30;
    if (boardDigitalButtons[p].value)
        add = 165;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++) {
            int c = add + i * 5 + j * 5;
            setColor(colorFromRGB(c, c, c));
            drawPixel(x + i, y + j);
        }
    setColor(colorPortWhite);
    drawLine(x, y, x + d, y);
    drawLine(x, y + d, x + d, y + d);
    drawLine(x + d, y, x + d, y + d);
    drawLine(x, y, x, y + d);
    setColor(colorWhite);
    setStandardFont();
    drawInt(p, x + d + 6, y - 1, 2);
}

void boardDrawAnalogPort(int v, int p, int x, int y) {
    int otherCol = 0;
    if (boardAnalogButtons[p].justRead > 0 && (readCount % 2) == 0) {
        boardAnalogButtons[p].justRead--;
        otherCol = 1;
    }
    int d = 9;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++) {
            int c = 165 + i * 5 + j * 5;
            setColor(colorFromRGB(c, c, c));
            if (otherCol)
                setColor(colorFromRGB(c, 0, 0));
            drawPixel(x + i, y + j);
        }

    setColor(colorPortWhite);
    drawLine(x, y, x + d, y);
    drawLine(x, y + d, x + d, y + d);
    drawLine(x + d, y, x + d, y + d);
    drawLine(x, y, x, y + d);
    setColor(colorWhite);
    setStandardFont();
    drawInt(p, x + d + 6, y - 1, 2);
    setColor(colorPortWhite);

    if (boardSelectedAPort == p) {
        setColor(colorYellow);
        drawRect(x - 1, y - 1, 12, 12);
    }

    drawInt(v, x + 1, y + 12, 4);
}

int boardReadAnalog(int p) {
    boardAnalogButtons[p].justRead = 1;
    return boardAnalogButtons[p].value;
}

int boardReadDigital(int p) {
    boardDigitalButtons[p].justRead = 1;
    return boardDigitalButtons[p].value;
}

void boardWriteDigital(int p, int v) {
    boardDigitalButtons[p].justRead = 1;
    boardDigitalButtons[p].value = v;
    boardTextRepaint();
}

void boardWriteAnalog(int p, int v) {
    boardAnalogButtons[p].justRead = 1;
    boardAnalogButtons[p].value = v;
    boardTextRepaint();
}

void boardDrawAnalogScroller(int x, int y) {
    setColor(colorDarkerGray);
    drawLine(20, 190 + 480, 300, 190 + 480);
    int d = 9;
    x = boardAnalogValue * 280 / 1024 + 20;
    int w = intWidth(boardAnalogValue) + 10;

    x -= w / 2;

    setColor(colorDarkerGray);
    drawRect(x, y, w, 2 * d + 2);
    setColor(colorPortWhite);
    fillRect(x + 1, y + 1, w - 2, 18);
    setColor(colorDarkerGray);
    drawInt(boardAnalogValue, x + 6, y + 4);
}

void initBoard() {
    numberOfBoardLines = 0;
}

void boardPrintln() {
    if (numberOfBoardLines > maxShownLines)
        firstBoardLine++;
    if (!boardCurrentLine) {
        boardCurrentLine = (char*)malloc(2);
        strcpy(boardCurrentLine, "");
    }
    boardLinesText[numberOfBoardLines++] = boardCurrentLine;
    boardCurrentLine = (char*)malloc(5);
    strcpy(boardCurrentLine, "");
    boardTextRepaint();
}

void boardPrint(const char* c) {
    if (!boardCurrentLine) {
        boardCurrentLine = (char*)malloc(5);
        strcpy(boardCurrentLine, "");
    }
    char* m2 = (char*)malloc(strlen(c) + 4 + strlen(boardCurrentLine));
    strcpy(m2, boardCurrentLine);
    strcat(m2, c);
    free(boardCurrentLine);
    boardCurrentLine = m2;
    boardTextRepaint();
}

void boardPrintln(const char* c) {
    boardPrint(c);
    boardPrintln();
}

void boardPrintln(int u) {
    boardPrint(u);
    boardPrintln();
}

void boardPrintln(float f) {
    boardPrint((int)f);
    boardPrint(".");
    boardPrint(f * 1000 - (int)f * 1000);
    boardPrintln();
}

void boardPrint(int u) {
    char* m2 = (char*)malloc(10);
    sprintf(m2, "%d", u);
    boardPrint(m2);
}

void boardDrawTitle(const char* t, int x, int y) {
    setBigFont();
    setColor(colorTitle2);
    for (int i = -1; i < 2; i++)
        for (int j = -1; j < 2; j++)
            drawString(t, x + i, y + j);
    setColor(colorTitle);
    drawString(t, x, y);
}

void boardAnalogValueClicked(int param) {
    boardAnalogValue = (getTouchX() - 20) * 1024 / 280;
    if (boardAnalogValue < 0)
        boardAnalogValue = 0;
    if (boardAnalogValue > 1023)
        boardAnalogValue = 1023;
    if (boardSelectedAPort >= 0) {
        boardAnalogButtons[boardSelectedAPort].value = boardAnalogValue;
        pmAnalogWrite(boardSelectedAPort, boardAnalogValue);
    }
    boardTextRepaint();
}

void boardBegin() {
    boardInTransaction = 1;
}

void boardCommit() {
    boardInTransaction = 0;
    boardTextRepaint();
}

void boardTextRepaint() {
    int exPrintx = printX();
    int exPrinty = printY();
    if (boardInTransaction)
        return;
    readCount++;

    pushClip();
    setMaxPrintX(320 * 3 - margin);
    setStandardFont();
    setNormalStyle();

    pushColor();
    setColor(colorBitDarkerGray);
    fillRect(330, 0, 630, 720);

    int dx = 10 + margin;
    int dy = 254;
    boardDrawTitle("Digital Ports", dx, dy);
    for (int i = 0; i < 54; i++)
        boardDrawDigitalPort(i, dx + (i % 6) * 50, margin + (i / 6) * 20 + dy + 24);

    setColor(colorBlack);
    fillRect(10, 480, 319, 206);

    int ax = 10 + margin;
    int ay = 480;
    boardDrawTitle("Analog Ports", ax, ay);
    for (int i = 0; i < 16; i++)
        boardDrawAnalogPort(boardAnalogButtons[i].value, i, ax + (i % 6) * 50, ay + margin + (i / 6) * 40 + 24);

    boardDrawAnalogScroller(100, 180 + 480);

    boardDrawTitle("Log", 330 + margin, 10);

    setPrintY(40);
    setStandardFont();

    int y = 40;
    for (int i = firstBoardLine; i < numberOfBoardLines; i++) {
        setPrintX(340);
        setPrintY(y);
        println(boardLinesText[i]);
        y += 16;
    }
    setPrintX(340);
    setPrintY(y);
    println(boardCurrentLine);

    setPrintX(15);
    setPrintY(695);
    println("Dump RAM+EEPROM");

    popClip();
    setMaxPrintX(320 - margin);

    setPrintX(exPrintx);
    setPrintY(exPrinty);
    popColor();
}
