
#include "UI.h"
#include "Drawing.h"
#include "Text.h"
#include "UINumPad.h"
#include "UIPort.h"

/*
 Gruppe I:
 1 ON: 1381717     1 OFF: 1381716
 2 ON: 1394005     2 OFF: 1394004
 3 ON: 1397077     3 OFF: 1397076
 4 ON: 1397845     4 OFF: 1397844

 Gruppe II:
 1 ON: 4527445     1 OFF: 4527444
 2 ON: 4539733     2 OFF: 4539732
 3 ON: 4542805     3 OFF: 4542804
 4 ON: 4543573     4 OFF: 4543572

 Gruppe III:
 1 ON: 5313877     1 OFF: 5313876
 2 ON: 5326165     2 OFF: 5326164
 3 ON: 5329237     3 OFF: 5329236
 4 ON: 5330005     4 OFF: 5330004

 Gruppe IV:
 1 ON: 5510485     1 OFF: 5510484
 2 ON: 5522773     2 OFF: 5522772
 3 ON: 5525845     3 OFF: 5525844
 4 ON: 5526613     4 OFF: 5526612

 */

// PORT DIALOG

namespace rcDialog {
    unsigned long* codeOn;
    unsigned long* codeOff;
    char*          rcPort;
    void (*screenCallback)();

    void configureRC();

    void configureOffCallback(int ok, long value) {
        if (ok)
            *codeOff = (long)value;
        goToScreen(configureRC);
    }

    void configureOff(int param) {
        showNumPad(PSTR("Off Code"), *codeOff, 0, 100000000, configureOffCallback);
    }

    void configureOnCallback(int ok, long value) {
        if (ok)
            *codeOn = (long)value;
        goToScreen(configureRC);
    }

    void configureOn(int param) {
        showNumPad(PSTR("On Code"), *codeOn, 0, 100000000, configureOnCallback);
    }

    void configureRCPortCallback(int port) {
        if (port)
            *rcPort = port;
        goToScreen(configureRC);
    }

    void configureRCPort(int param) {
        showDigitalPortDialog(*rcPort, configureRCPortCallback);
    }

    void configure(int param) {
        goToScreen(screenCallback);
    }

    void configureRC() {
        println(
            PSTR("You must configure the 'codes' for turning the plug on and off. These are two different long integer"
                 " numbers. Not all plugs work like this, look at the documentation at www.sketches.cloud for "
                 "compatible hardware."));
        println();
        print(PSTR("On Code: "));
        println(*codeOn);
        print(PSTR("Off Code: "));
        println(*codeOff);
        print(PSTR("Port: "));
        println(*rcPort);
        toolbarAdd(PSTR("|Port"), configureRCPort);
        toolbarAdd(PSTR("|On Code"), configureOn);
        toolbarAdd(PSTR("|Off Code"), configureOff);
        toolbarAddBack(configure);
    }
}

void showRCDialog(char*          port,
                  unsigned long* code1,
                  unsigned long* code2,
                  void (*callback)()) {
    rcDialog::codeOn = code1;
    rcDialog::codeOff = code2;
    rcDialog::rcPort = port;
    rcDialog::screenCallback = callback;
    goToScreen(rcDialog::configureRC);
}
