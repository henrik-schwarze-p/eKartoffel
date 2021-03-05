//
//  CoreUI-Select.c
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "UISelect.h"
#include "Drawing.h"
#include "UI.h"
#include "Namespaces.h"
#include "Text.h"

extern char ramSource[];

namespace selectNS {
    int         selected = 0;
    int         page = 0;
    int         cols = 2;
    const char* selectName;
    const char* selectName2;
    int         rows = 4;

    const char* (*fnTextSource)(int);
    void (*actionSelectFnCallback)(int, int);

    void setSelectSetName(const char* c) {
        selectName = c;
    }

    int  selectNumberOfOptions();
    void drawButton(int index);
    void selectItemPressed(int param) {
        if (param + page * cols * rows >= selectNumberOfOptions())
            return;
        if (param == selected)
            return;
        int old = selected;
        selected = -1;
        drawButton(old % (rows * cols));
        selected = param + page * cols * rows;
        drawButton(selected % (rows * cols));
    }

    int selectNumberOfOptions() {
        int i = 0;
        if (fnTextSource)
            while (fnTextSource(i))
                i++;
        return i;
    }

    void screenSelect();
    void goLeftOrRight(int param) {
        page += param;
        selected = page * cols * rows;
        goToScreen(screenSelect, 0);
    }

    void selectCall(int param) {
        actionSelectFnCallback(selected, param);
    }

    int width() {
        return (horizontalResolution - cols - 1) / cols;
    }

    int height() {
        return (verticalResolution - 4 - 1 - menuBarHeight - toolbarHeight) / 4;
    }

    void drawButton(int indexInPage) {
        int globalIndex = indexInPage + page * rows * cols;
        int high = globalIndex == selected;
        int k = indexInPage % (rows * cols);
        int i = k % cols;
        int j = k / cols;
        int x = i * (width() + 1);
        int y = j * (height() + 1) + menuBarHeight + 1;
        int ewidth = (i == cols - 1) ? horizontalResolution - x : width();
        int eheight = (j == rows - 1) ? verticalResolution - toolbarHeight - y - 1 : height();
        setColor(high ? colorWhite : colorDarkGray);
        fillRect(x, y, ewidth, eheight);
        if (globalIndex < selectNumberOfOptions()) {
            char* text = (char*)fnTextSource(globalIndex);
            addLabel(x, y, text, ewidth, eheight, high ? colorDarkGray : colorWhite, STYLE_CENTERED, 0);
        }
    }

    void goBack(int param) {
        actionSelectFnCallback(0, -1);
    }

    void screenSelect() {
        for (int i = 0; i < cols * rows; i++)
            drawButton(i);
        setColor(colorDarkerGray);
        for (int j = 0; j < rows; j++) {
            int y = j * (height() + 1) + menuBarHeight + 1;
            drawLine(0, y - 1, horizontalResolution - 1, y - 1);
        }
        int y = verticalResolution - toolbarHeight - 1;
        drawLine(0, y, horizontalResolution - 1, y);
        for (int i = 1; i < cols; i++) {
            int x = i * (width() + 1) - 1;
            drawLine(x, menuBarHeight, x, verticalResolution - toolbarHeight);
        }

        addButton(0, menuBarHeight, 320 / cols, 44, selectItemPressed, 0, cols, 4, 0, 0, -1);

        int n = rows * cols;

        toolbarAdd(mini, page, MINI_ICON_LEFT, goLeftOrRight, -1);
        toolbarAdd(selectName, selectCall, 1);
        if (selectName2)
            toolbarAdd(selectName2, selectCall, 2);
        toolbarAdd(mini, page * n + n - 1 < selectNumberOfOptions(), MINI_ICON_RIGHT, goLeftOrRight, 1);
        toolbarAdd(mini, isenabled, PSTR("<<"), goBack);
    }

}

void showSelectDialog(const char* setName,
                      int         cols,
                      const char* (*fnTextSource)(int),
                      void (*fnCallback)(int, int),
                      int firstSelected) {
    selectNS::page = 0;
    selectNS::cols = cols;
    selectNS::selected = firstSelected;
    selectNS::fnTextSource = fnTextSource;
    selectNS::actionSelectFnCallback = fnCallback;
    selectNS::selectName = setName;
    selectNS::selectName2 = 0;
    goToScreen(selectNS::screenSelect);
}

void showSelectDialog(const char* setName,
                      int         cols,
                      const char* (*fnTextSource)(int),
                      void (*fnCallback)(int, int),
                      int         firstSelected,
                      const char* m) {
    selectNS::page = 0;
    selectNS::cols = cols;
    selectNS::selected = firstSelected;
    selectNS::fnTextSource = fnTextSource;
    selectNS::actionSelectFnCallback = fnCallback;
    selectNS::selectName = setName;
    selectNS::selectName2 = 0;
    message(m, selectNS::screenSelect);
}

void showSelectDialog(const char* setName,
                      const char* setName2,
                      int         cols,
                      const char* (*fnTextSource)(int),
                      void (*fnCallback)(int, int),
                      int         firstSelected,
                      const char* m) {
    selectNS::page = 0;
    selectNS::cols = cols;
    selectNS::selected = firstSelected;
    selectNS::fnTextSource = fnTextSource;
    selectNS::actionSelectFnCallback = fnCallback;
    selectNS::selectName = setName;
    selectNS::selectName2 = setName2;
    message(m, selectNS::screenSelect);
}
