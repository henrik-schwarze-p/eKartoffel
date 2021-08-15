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

extern long loadCumulativeDuration;
extern long loadTotalDuration;

// The slice counter.
unsigned long callCounter = 0;
unsigned long lastMillis = 0;

int durations;

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

    if (clockSecs() % 10 == 0)
        showMem();

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
    durations = ourMillis() - begin;
    consoleln(durations);
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

void showMem(int x, int aHeap) {
    setColor(colorBlack);
    fillRect(margin + x, menuBarHeight - aHeap - 1, 5, aHeap + 1);
    setColor(colorGreen);
    if (aHeap < 3)
        setColor(colorRed);
    if (aHeap > 0)
        fillRect(x + margin + 1, menuBarHeight - aHeap, 3, aHeap);
}

void showMem() {
    /*    setColor(colorWhite);
        fillRect(0, 0, 22, menuBarHeight);
        showMem(0, availablePersistantHeap() * (menuBarHeight - 2) / 4096);
        showMem(4, availableHeap() * (menuBarHeight - 2) / 4096);
        showMem(8, 1300 * (menuBarHeight - 2) / 4096);
     */
}

int currentLoad() {
    return durations/10;
}
