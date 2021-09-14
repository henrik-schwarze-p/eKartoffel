//
//  CoreUI-Port.c
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "Namespaces.h"
#include "Descriptors.h"
#include "Instance.h"
#include "UI.h"
#include "Drawing.h"
#include "Text.h"

// PORT DIALOG

namespace portDialog {
    int portX[] = {-1, 64, 64 * 2, 64 * 3, 64 * 4, 320};
    int portY[] = {17, 17 + 58 + 1, 17 + 58 + 1 + 59 + 1, 17 + 58 + 1 + 59 + 1 + 59 + 1};

    int  digital = 0;
    int  selected = -1;
    int  firstPortShown = 1;
    int  firstTime = 0;
    char currentPort;

    void screenDialogPorts(void);
    void (*portCallback)(int);

    int numberOfPorts() {
        return digital ? 55 : 15;
    }

    const char* getLabelForPort(int p, int digital) {
        const char* result = 0;
        for (int i = 0; i < numberOfInstances(); i++)
            if (callGetLabelForPort(i, p, digital)) {
                if (result) {
                    result = PSTR("Many!");
                } else {
                    result = callGetLabelForPort(i, p, digital);
                }
            }
        return result;
    }

    void portSelected(int param) {
        portCallback(selected);
    }

    void setSelectedPort(int param) {
        param = param + firstPortShown;
        // if (!getLabelForPort(param, digital)) {
        selected = param;
        goToScreen(screenDialogPorts, 0);
        //}
    }

    void decMinPort(int param) {
        if (firstPortShown < 2)
            return;
        firstPortShown -= 15;
        goToScreen(screenDialogPorts, 0);
    }

    void incMinPort(int param) {
        if (firstPortShown + 14 > numberOfPorts())
            return;
        firstPortShown += 15;
        goToScreen(screenDialogPorts, 0);
    }

    void portCancelled(int param) {
        portCallback(0);
    }

    void screenDialogPorts() {
        // 16 + 1 + 58 + 1 + 59  + 1 + 59 + 1 + 44
        // 63 + 1 + 63 + 1 + 64 + 1 + 63 + 1 +63
        if (firstTime) {
            setColor(colorDarkerGray);
            for (int i = 0; i < 4; i++)
                drawLine(0, portDialog::portY[i] - 1, horizontalResolution - 1, portDialog::portY[i] - 1);
            for (int i = 1; i < 5; i++)
                drawLine(portDialog::portX[i], menuBarHeight, portDialog::portX[i], verticalResolution - toolbarHeight);
            firstTime--;
        }

        for (int i = 0; i < 15; i++) {
            int thePort = i + firstPortShown;

            const char* used = getLabelForPort(thePort, digital);

            int fontColor = colorWhite;
            setColor(colorDarkGray);
            if (used) {
                setColor(colorOrange);
            }
            if (thePort == selected && used && selected != currentPort) {
                setColor(colorRed);
                fontColor = colorWhite;
            } else if (thePort == selected) {
                setColor(colorWhite);
                fontColor = colorBlack;
            }

            fillRect(portX[i % 5] + 1, portY[i / 5], portX[i % 5 + 1] + 1 - portX[i % 5] - 2,
                     portY[i / 5 + 1] - portY[i / 5] - 1);

            if (thePort > numberOfPorts())
                continue;

            setColor(fontColor);
            if (used) {
                setStandardFont();
                if (stringWidth(used) > 60)
                    drawStringWithIntSubscript(used, portX[i % 5], portY[i / 5] + 58 / 2 - fontHeight() / 2 + 2, 64, 0);
                else
                    drawString(used, portX[i % 5] + 63 / 2 - stringWidth(used) / 2,
                               portY[i / 5] + 58 / 2 - fontHeight() / 2 + 2);
            }
            setBigFont();
            drawInt(thePort, portX[i % 5] + 63 / 2 - intWidth(i + 1) / 2,
                    portY[i / 5] + 58 / 2 - fontHeight() / 2 - 14);
        }
        addButton(0, menuBarHeight, 64, 60, setSelectedPort, 0, 5, 3, 0, 0, -1);

        toolbarAdd(mini, firstPortShown > 1, MINI_ICON_LEFT, decMinPort);

        if (selected && getLabelForPort(selected, digital) && selected != currentPort)
            toolbarAdd(PSTR("Force!"), portSelected);
        else
            toolbarAdd(PSTR("  Set  "), portSelected);
        toolbarAdd(PSTR("Cancel"), portCancelled);
        toolbarAdd(mini, firstPortShown + 14 < numberOfPorts(), MINI_ICON_RIGHT, incMinPort);
    }
}

void showDigitalPortDialog(int currentPort, void (*callback)(int)) {
    portDialog::firstPortShown = 1;
    portDialog::digital = 1;
    portDialog::selected = currentPort;
    portDialog::portCallback = callback;
    portDialog::firstTime = 2;
    portDialog::currentPort = currentPort;
    message(PSTR("Please select a digital port and then press 'Set'"), portDialog::screenDialogPorts, 0);
}

void showAnalogPortDialog(int currentPort, void (*callback)(int)) {
    portDialog::firstPortShown = 1;
    portDialog::digital = 0;
    portDialog::selected = currentPort;
    portDialog::portCallback = callback;
    portDialog::firstTime = 2;
    portDialog::currentPort = currentPort;
    message(PSTR("Please select an analog port and then press 'Set'"), portDialog::screenDialogPorts, 0);
}
