#include "Target.h"
#include "Namespaces.h"
#include "MainLoop.h"
#include "UI.h"
#include "BoardLib.h"
#include "Launcher.h"
#include "Heap.h"
#include "Rules.h"
#include "Graph.h"

void loop() {
    mainLoop();
}

void setup() {
    initScheduler();
    initBoard();
    tftReset();
    initHeap();
    launchAll();
    initTimer();
    showMem();
    message(PSTR("Sketches - Zulke & Schwarze 0.7.1"), desktop::startScreen);
}
