#include "Target.h"
#include "Drawing.h"

int _color;
int pushedColor;

int clipMinX = 0;
int clipMaxX = 319;
int clipMinY = 0;
int clipMaxY = 239;

int active = 1;

// USED TO GIVE THE APPS ACCESS TO THE SCREEN

void activateGraphics() {
    active = 1;
}

void deactivateGraphics() {
    active = 0;
}

int isActive() {
    return active;
}

// We use the arduino colorspace internaly, 32 bits externally
// 5 bits for red
// 6 bits for green
// 5 bits for blue
int colorFromRGB(int r, int g, int b) {
    r = (31 * r / 255) << 11;
    g = (63 * g / 255) << 5;
    b = (31 * b / 255);
    return r + g + b;
}

void setColor(int c) {
    _color = c;
}

void drawPixel(int x, int y) {
    if (!isActive())
        return;
    tftPixel(y, HR - 1 - x, _color);
}

void drawHorizontalLine(int x, int y, int w) {
    if (!isActive())
        return;
    if (y < clipMinY || y > clipMaxY)
        return;
    if (x < clipMinX) {
        w -= clipMinX - x;
        x = clipMinX;
    }
    if (x + w - 1 > clipMaxX)
        w = clipMaxX - x + 1;

    tftVline(y, HR - x - w, w, _color);
}

void drawVerticalLine(int x, int y, int w) {
    if (!isActive())
        return;
    if (x < clipMinX || x > clipMaxX)
        return;
    if (y < clipMinY) {
        w -= clipMinY - y;
        y = clipMinY;
    }
    if (y + w - 1 > clipMaxY)
        w = clipMaxY - y + 1;
    tfHline(y, HR - 1 - x, w, _color);
}

void pushClipping(int x0, int y0, int x1, int y1) {
    clipMinX = x0;
    clipMaxX = x1;
    clipMinY = y0;
    clipMaxY = y1;
}

void popClipping() {
    clipMinX = 0;
    clipMinY = 0;
    clipMaxX = horizontalResolution - 1;
    clipMaxY = verticalResolution - 1;
}

void drawLine(int x0, int y0, int x1, int y1) {
    if (!isActive())
        return;
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    for (;;) {
        drawPixel(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void fillRect(int x, int y, int w, int h) {
    if (!isActive())
        return;
    if (x < clipMinX) {
        w -= clipMinX - x;
        x = clipMinX;
    }
    if (y < clipMinY) {
        h -= clipMinY - y;
        y = clipMinY;
    }
    if (x + w - 1 > clipMaxX)
        w = clipMaxX - x + 1;
    if (y + h - 1 > clipMaxY)
        h = clipMaxY - y + 1;

    tftRect(y, HR - x - w, h, w, _color);
}

void pushColor() {
    pushedColor = _color;
}

void popColor() {
    _color = pushedColor;
}

void pushColor(int color) {
    pushColor();
    _color = color;
}

int color() {
    return _color;
}

void drawRect(int x, int y, int w, int h) {
    if (!isActive())
        return;
    drawHorizontalLine(x, y, w);
    drawHorizontalLine(x, y + h - 1, w);
    drawVerticalLine(x, y, h);
    drawVerticalLine(x + w - 1, y, h);
}

void drawBar(int y, int h) {
    if (!isActive())
        return;
    setColor(colorPink);
    fillRect(0, y, horizontalResolution, h);
    setColor(colorWhite);
    fillRect(0, y + 1, horizontalResolution, h - 2);
}

void pushClip() {
    clipMaxX = 639 + 320;
    clipMaxY = 719;
    clipMinX = 0;
    clipMinY = 0;
}

void popClip() {
    clipMaxX = 319;
    clipMaxY = 239;
    clipMinX = 0;
    clipMinY = 0;
}
