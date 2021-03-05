#include "Graph.h"
#include "Drawing.h"
#include "Text.h"

// Graphs

int maxSourceN(Graph g) {
    return g.sourceN(g.n - 1);
}

int minSourceN(Graph g) {
    return g.sourceN(0);
}

int yPxForValue(double y, Graph g) {
    return g.y1 - (y - g.miny) / (g.maxy - g.miny) * (g.y1 - g.y0);
}

// maps (0->xLabels) => (minSourceN->maxSourceN)
int xLabelsToSourceN(int i, Graph g) {
    return minSourceN(g) + i * (maxSourceN(g) - minSourceN(g)) / (g.xLabels - 1);
}

// maps (0->xLabels) => (x0->x1)
int xLabelsToPx(int i, Graph g) {
    return g.x0 + i * (g.x1 - g.x0 - (g.x1 - g.x0) / g.n) / (g.xLabels - 1) + (g.x1 - g.x0) / g.n / 2;
}

// maps (minSourceN->maxSourceN) => (x0->x1)
int sourceNToPx(int i, Graph g) {
    return g.x0 + (i - minSourceN(g)) * (g.x1 - g.x0 - (g.x1 - g.x0) / g.n) / (maxSourceN(g) - minSourceN(g)) +
           (g.x1 - g.x0) / g.n / 2;
}

void graph(Graph g) {
    int x0 = g.x0;
    int y0 = g.y0;
    int y1 = g.y1;
    int x1 = g.x1;
    int widthPx = x1 - x0 + 1;
    int heightPx = y1 - y0 + 1;

    // the box
    setColor(colorWhite);
    fillRect(x0 + 1, y0 + 1, widthPx - 2, heightPx - 2);
    setColor(colorBlack);
    drawRect(x0, y0, widthPx, heightPx);

    // THE TITLE

    drawString(g.title, x1 - margin - stringWidth(g.title), y0 + 2);

    // yLabels
    double maxy = g.maxy;
    double miny = g.miny;
    int    decimals = g.decimals;
    int    yLabels = g.yLabels;
    double yVal = miny;
    for (int i = 0; i < yLabels; i++) {
        drawFloat(yVal, decimals, x0 - floatWidth(yVal, decimals), yPxForValue(yVal, g) - fontHeight() / 2);
        if (g.showHlines && i && i < yLabels - 1) {
            pushColor();
            setColor(colorLightGray);
            drawHorizontalLine(x0 + 1, yPxForValue(yVal, g), x1 - x0 - 1);
            popColor();
        }
        yVal += (maxy - miny) / (yLabels - 1);
    }

    // xLabels
    int xLabels = g.xLabels;
    for (int i = 0; i < xLabels; i++) {
        int sourceToShow = xLabelsToSourceN(i, g);
        drawInt(sourceToShow, sourceNToPx(sourceToShow, g) - intWidth(sourceToShow) / 2, y1 + 1);
    }

    // lines
    pushClipping(x0 + 1, y0 + 1, x1 - 1, y1 - 1);
    if (g.type == 1) {
        setColor(colorBlue);
        for (int i = 0; i < g.n - 1; i++) {
            drawLine(sourceNToPx(g.sourceN(i), g), yPxForValue(g.sourceY(i), g), sourceNToPx(g.sourceN(i + 1), g),
                     yPxForValue(g.sourceY(i + 1), g));
        }
    } else {
        for (int i = 0; i < g.n; i++) {
            int d = (x1 - x0) / (maxSourceN(g) - minSourceN(g)) * 4 / 11;
            setColor(colorDarkGray);
            drawRect(sourceNToPx(g.sourceN(i), g) - d, yPxForValue(g.sourceY(i), g), 2 * d,
                     y1 - yPxForValue(g.sourceY(i), g) + 1);
            setColor(colorRed);
            fillRect(1 + sourceNToPx(g.sourceN(i), g) - d, 1 + yPxForValue(g.sourceY(i), g), 2 * d - 2,
                     y1 - yPxForValue(g.sourceY(i), g) + 1 - 2);
        }
    }
    popClipping();
}
