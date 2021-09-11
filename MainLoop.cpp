#include "Rules.h"
#include "Drawing.h"
#include "Instance.h"
#include "MainLoop.h"
#include "UI.h"
#include "Heap.h"
#include "Chunks.h"
#include "FatalError.h"
#include "Text.h"
#include "PMClock.h"
#include "Icon.h"

extern long loadCumulativeDuration;
extern long loadTotalDuration;

// The slice counter.
unsigned long callCounter = 0;
unsigned long lastMillis = 0;

long durations;

int _justOpened = 1;

void setJustOpened(int s) {
    _justOpened = s;
}

void initScheduler() {
    durations = 0;
}

void mainLoop() {
    processTouch();
    unsigned long currentMillis = ourMillis();

    while ((currentMillis / 1000) > callCounter) {
        schedule();
        callCounter += 1;
    }
    callCounter = currentMillis / 1000;
    if (lastMillis > currentMillis)
        callCounter = 0;
    lastMillis = currentMillis;
}

void schedule() {
    evalRules();

    int           begin = ourMillis();
    frequencyType type = incrementOneSecond();

    // Poor man's multitasking
    for (int i = 0; i < numberOfInstances(); i++) {
        int fg = getForegroundInstance() == i;
        if (statusForInstance(i, STATUS_OK)) {
            setPrintY(menuBarHeight + margin + 1);
            setPrintX(margin);
            setStandardFont();
            setColor(colorBlack);
            deactivateGraphics();
            if (fg)
                activateGraphics();
            callTimeSlice(i, 1, type, _justOpened);
            activateGraphics();
        } else {
            fatalError(6, i);
        }
    }
    _justOpened = 0;
    durations = (durations * 9 + ourMillis() - begin) / 10;
}

unsigned int ourMillis() {
    return millis();
}

void processTouch() {
    if (!touched())
        return;

    int b = buttonForCoordinates(getTouchX(), getTouchY());
    if (b >= 0) {
        Button v = buttonAtIndex(b / 1000);
        void (*cmd)(int) = v.command;
        void (*screen)(void) = v.screen;
        if (cmd) {
            switchContextToInstance(getForegroundInstance());
            if (v.nx == v.ny && v.nx == 1) {
                cmd(v.param);
            } else
                cmd(b % 1000);
            popContext();
        }
        if (screen) {
            switchContextToInstance(getForegroundInstance());
            goToScreen(screen);
            popContext();
        }
    }
}

int currentLoad() {
    return (int)(durations / 10);
}
