//
//  CoreAction-UI-ListActions.c
//  AquaOS
//
//  Created by Kurt Schwarze on 04.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "Namespaces.h"
#include "UI.h"
#include "UIRulesDialog.h"
#include "Rules.h"
#include "Target.h"
#include "UISelect.h"
#include "Text.h"
#include "Drawing.h"
#include "UIPort.h"
#include "UINumPad.h"
#include "Instance.h"
#include "Chunks.h"

const char* activeLabel;
int         activeIndex;
Axon        activeAxon;

#define PAR_GET_FROM_USER 100
#define PAR_PARAM_SIZE 101
#define PAR_TO_PRINT 102

int paramIs(const char* what) {
    return toolbarIntForParameterX(activeLabel, activeIndex, what);
}

namespace rules {

    int firstShownAxon = 0;
    int toolbarSet = 0;
    int selectedAxon;
    int axonForTplFeeder;

    void (*screenCallback)(void);

    void list();
    void down(int param);

    void selectAxon(int param) {
        selectedAxon = param + firstShownAxon;
        if (toolbarSet)
            toolbarSet = 0;
        goToScreen(list);
    }

    void leave(int param) {
        goToScreen(screenCallback);
    }

    void newAction(int param) {
        insertEmptyAction(selectedAxon);
        toolbarSet = 1;
        down(0);
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void newCondition(int param) {
        insertEmptyCondition(selectedAxon);
        toolbarSet = 1;
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void up(int param) {
        if (firstShownAxon > 0)
            firstShownAxon--;
        if (selectedAxon > firstShownAxon + 5)
            selectedAxon = firstShownAxon + 5;
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void down(int param) {
        if (firstShownAxon < numberOfAxons() - 5)
            firstShownAxon++;
        if (selectedAxon < firstShownAxon)
            selectedAxon = firstShownAxon;
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void setToolbarSet(int param) {
        toolbarSet = param;
        goToScreen(list, DRAWSCREEN_CLEAR_BOTTOM);
    }

    void tplFeeder(int t) {
        Axon axon = axonForIndex(axonForTplFeeder);
        activeLabel = axon.label;
        activeIndex = t;
        activeAxon = axon;
        switchContextToInstance(axon.instance);
        if (axon.isAction) {
            // pico
            callGeneratedPrintActionParameter(axon.instance, axon.kind, t, axon.params[t]);
            // callSelector(SEL_PRINTACTIONPARAMETER, axon.instance, 0, 0, axon.kind, t, axon.params[t], 0, fq, 0);
        } else {
            // pico
            callGeneratedPrintConditionParameter(axon.instance, axon.kind, t, axon.params[t]);
            // callSelector(SEL_PRINTCONDITIONPARAMETER, axon.instance, 0, 0, axon.kind, t, axon.params[t], 0, fq, 0);
        }
        popContext();
    }

    void deleteSelectedAxon(int index) {
        deleteAxon(index);
        while (selectedAxon < numberOfAxons() && axonForIndex(selectedAxon).isAction)
            deleteAxon(selectedAxon);
        toolbarSet = 1;
        if (selectedAxon >= numberOfAxons())
            selectedAxon = numberOfAxons() - 1;
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void setAction(int sel, int button) {
        if (sel >= 0) {
            deleteAxon(selectedAxon);
            insertAction(selectedAxon, sel);
        }
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void setCondition(int sel, int button) {
        if (sel >= 0) {
            deleteAxon(selectedAxon);
            insertCondition(selectedAxon, sel);
        }
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    const char* allConditions(int index) {
        InstanceKind ik = instanceAndKindForGlobalIndex(1, index);
        if (ik.found == 0)
            return NULL;
        return callConditionNames(ik.instance, ik.kind);
    }

    /**
    Return the action considering all instances
     */
    const char* allActions(int index) {
        InstanceKind ik = instanceAndKindForGlobalIndex(0, index);
        if (ik.found == 0)
            return NULL;
        return callActionNames(ik.instance, ik.kind);
    }

    void changeAxon(int param) {
        if (axonForIndex(selectedAxon).isAction)
            showSelectDialog(PSTR("Set Action"), 2, allActions, setAction, 0);
        else
            showSelectDialog(PSTR("Set Condition"), 2, allConditions, setCondition, 0);
    }

    void portsCont(int selectedPort) {
        if (selectedPort > 0) {
            Axon axon = axonForIndex(selectedAxon);
            axon.params[activeIndex] = selectedPort;
            writeAxon(axon);
        }
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    // runs in app space
    void analogValueCont(int valid, long value) {
        if (valid) {
            Axon axon = axonForIndex(selectedAxon);
            axon.params[activeIndex] = value;
            writeAxon(axon);
        }
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    // runs in app space
    void continueSetConditionParameter(int success, uint16_t p) {
        if (success) {
            Axon axon = axonForIndex(selectedAxon);
            axon.params[activeIndex] = p;
            writeAxon(axon);
        }
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

    void changeParameter(int paramIndex) {
        Axon axon = axonForIndex(selectedAxon);

        activeLabel = axon.label;
        activeIndex = paramIndex;

        switchContextToInstance(axon.instance);
        int value = axon.params[paramIndex];
        // We assume that "Port", "An. Value" have precedence over the setAxonParameter methods
        if (toolbarIntForParameter(axon.label, paramIndex, PSTR("Port")))
            showDigitalPortDialog(value, portsCont);
        else if (toolbarIntForParameter(axon.label, paramIndex, PSTR("An. Value")))
            showNumPad(PSTR("Enter the value for the Analog Port. It must be inside the range [0,1023]."), value,
                       0, 1023, analogValueCont);
        else if (axon.isAction) {
            callGeneratedSetActionParameter(axon.instance, axon.kind, paramIndex);
            // pico
        } else {
            callGeneratedSetConditionParameter(axon.instance, axon.kind, paramIndex);
        }
        popContext();

        return;
    }

    void list() {
        int n = numberOfAxons();
        if (n == 0) {
            print(PSTR("There are no conditions nor actions. Create a condition by pressing "));
            setBigFont();
            setPrintY(printY() - 2);
            print(MINI_ICON_ADD);
            setStandardFont();
        } else {
            addPrintY(margin * 3 / 2);
            int h = fontHeight() + 2 * margin;
            int y = printY();
            int counter = 0;
            for (int indexAxon = firstShownAxon; indexAxon < n + 1 && counter < 6; indexAxon++) {
                counter++;
                if (indexAxon == selectedAxon)
                    drawBar(y - h / 2 - 2, h + 4);
                setColor(colorBlack);
                setPrintX(margin);
                setPrintY(y - 9);
                setBigFont();
                axonForTplFeeder = indexAxon;
                Axon axon = axonForIndex(indexAxon);
                if (indexAxon == n)
                    addLabel(margin + 3, y - h / 2, PSTR("..."), horizontalResolution - 36, h, colorBlack, 0, 0);
                else if (axon.isAction) {
                    setPrintX(35);
                    print(MINI_ICON_ACTION);
                    setStandardFont();
                    drawTPLString(axon.label, 70 - margin, y - h / 2 + 8, tplFeeder);
                } else {
                    setPrintX(margin);
                    print(MINI_ICON_CONDITION);
                    setStandardFont();
                    drawTPLString(axon.label, 35, y - h / 2 + 8, tplFeeder);
                }
                y += h;
            }
            addButton(0, menuBarHeight + 5, 319, h, selectAxon, 0, 1, counter, 0, 0, 0);
        }
        Axon axon = axonForIndex(selectedAxon);
        if (toolbarSet == 0) {
            toolbarAdd(mini, isenabled, MINI_ICON_UP, up);
            if (selectedAxon == n)
                toolbarAdd(MINI_ICON_ADD, setToolbarSet, 4);
            else
                toolbarAdd(PSTR("Edit"), setToolbarSet, 1);
            toolbarAdd(mini, isenabled, MINI_ICON_DOWN, down);
            toolbarAddBack(leave);
        } else if (toolbarSet == 1) {
            toolbarAdd(mini, numberOfAxons() > 0, MINI_ICON_GARBAGE, setToolbarSet, 3);
            toolbarAdd(mini, isenabled, MINI_ICON_ADD, setToolbarSet, 4);
            toolbarAdd(mini, isenabled, PSTR("| "), changeAxon);
            int hasParameters = axon.numberOfParams;
            toolbarAdd(standard, hasParameters, PSTR("Values"), setToolbarSet, 2);
            toolbarAddBack(setToolbarSet);
        } else if (toolbarSet == 2) {
            for (int i = 0; i < axon.numberOfParams; i++) {
                const char* s = axon.label;
                int         f1 = toolbarLabelForParameter1(s, i);
                int         f2 = toolbarLabelForParameter2(s, i);
                toolbarAdd(s, changeParameter, i, f1, f2);
            }
            toolbarAddBack(setToolbarSet);
        } else if (toolbarSet == 3) {
            toolbarAdd(PSTR("    <<    "), setToolbarSet, 1);
            if (axon.isAction)
                toolbarAdd(PSTR("Delete Action!   "), deleteSelectedAxon, selectedAxon);
            else
                toolbarAdd(PSTR("Delete Condition!"), deleteSelectedAxon, selectedAxon);
        } else if (toolbarSet == 4) {
            toolbarAdd(PSTR("\x06 Condition"), newCondition);
            if (selectedAxon)
                toolbarAdd(PSTR("\x06 Action   "), newAction);
            toolbarAddBack(setToolbarSet);
        }
    }

    void showActions() {
        selectedAxon = 0;
        goToScreen(list, DRAWSCREEN_CLEAR_MIDDLE);
    }

}

void showRulesDialog(void (*fn)(void)) {
    if (!chunkForHandleExists(RULES_HANDLE))
        createRulesChunk();
    rules::screenCallback = fn;
    rules::toolbarSet = 0;
    rules::firstShownAxon = 0;
    rules::showActions();
}

void continueSetConditionParameter(int success, uint16_t p) {
    rules::continueSetConditionParameter(success, p);
}
