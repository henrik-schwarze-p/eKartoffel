#include "Target.h"
#include "Chunks.h"
#include "FatalError.h"
#include "Rules.h"
#include "Descriptors.h"
#include "Heap.h"
#include "Instance.h"
#include "Namespaces.h"

/*
 RULES STRUCTURE
 Offset   Element
 0        Active Combined Condition Index (index, not address!, of the first
                        condition of the combined condition. ff=none)
 1        Conditions+
 ?        Actions*
 ?        Conditions+
 ?        Actions*
 ?        0xED End

 Or simplified

 Active
 Axon of index 0
 Axon of index 1
 ...
 EndAxon (uses only 1 byte)

 AXON STRUCTURE
 C0 (HEAD)  Condition (C0) | Action (A0) | End (0)
 03         Defined in (same or external) instance 3
 05         Kind #5, defined in (same or external) instance
 03 07         Parameter #0 (Number of parameters defined in (same or external) instance
 02 01         Parameter #1

 The length of the Axon is NOT stored in the structure.
 */

const char* _axonLabel(Axon a) {
    if (a.isEnd)
        fatalError(1, 0);
    if (a.isCondition)
        return callConditionNames(a.instance, a.kind);
    return callActionNames(a.instance, a.kind);
}

int _numberOfParams(Axon a) {
    const char* c = _axonLabel(a);
    int         expectedNumberOfParameters = 0;
    int         i = 0;
    while (pgm_read_byte(c + i)) {
        if (pgm_read_byte(c + i) == '[')
            expectedNumberOfParameters++;
        i++;
    }
    if (expectedNumberOfParameters > 5)
        fatalError(44, a.instance);
    return expectedNumberOfParameters;
}

// CONSTRUCTORS

Axon notFoundAxon() {
    Axon a;
    a.notFound = 1;
    return a;
}

// May return an invalid Axon
Axon axonForAddress(int axonAddress) {
    int  head = pget(RULES_HANDLE, axonAddress);
    Axon a;
    a.isEnd = head == AX_END;
    a.isAction = head == AX_IS_ACTION;
    a.isCondition = head == AX_IS_CONDITION;
    a.address = axonAddress;
    a.notFound = 0;
    // the end axon has just an address and the "isEnd" set
    if (a.isEnd)
        return a;

    a.headValue = head;

    a.address = axonAddress;
    a.instance = pget(RULES_HANDLE, axonAddress + AX_INSTANCE_OFFSET);
    a.kind = pget(RULES_HANDLE, axonAddress + AX_KIND_OFFSET);

    a.numberOfParams = _numberOfParams(a);
    a.label = _axonLabel(a);

    for (int i = 0; i < a.numberOfParams; i++) {
        int index = axonAddress + AX_PARAMS_OFFSET + i * sizeof(uint16_t);
        a.params[i] = pget(RULES_HANDLE, index) * 256 + pget(RULES_HANDLE, index + 1);
    }
    a.length = AX_PARAMS_OFFSET + a.numberOfParams * sizeof(uint16_t);

    a.nextAddress = axonAddress + a.length;

    return a;
}

Axon insertAxon(int index, int isCondition, int instance, int kind) {
    Axon a;
    a.headValue = AX_IS_ACTION;
    if (isCondition)
        a.headValue = AX_IS_CONDITION;
    a.instance = instance;
    a.kind = kind;
    a.isEnd = 0;
    a.notFound = 0;
    a.isAction = !isCondition;
    a.isCondition = isCondition;
    a.numberOfParams = _numberOfParams(a);
    a.label = _axonLabel(a);
    for (int i = 0; i < a.numberOfParams; i++)
        a.params[i] = 0;
    a.length = AX_PARAMS_OFFSET + a.numberOfParams * sizeof(uint16_t);

    Axon existingAxon = axonForIndex(index);
    a.address = existingAxon.address;
    insertChunkFragment(RULES_HANDLE, existingAxon.address, a.length);
    writeAxon(a);
    return a;
}

InstanceKind instanceAndKindForGlobalIndex(int considerConditions, int globalAxonIndex) {
    InstanceKind ik;
    const char* (*fn)(int, int) = callActionNames;
    if (considerConditions)
        fn = callConditionNames;
    int acc = 0;
    for (int instance = 0; instance < numberOfInstances(); instance++) {
        int         index = 0;
        const char* r = fn(instance, index);
        while (r) {
            if (acc == globalAxonIndex) {
                ik.instance = instance;
                ik.kind = index;
                ik.found = 1;
                return ik;
            }
            acc++;
            index++;
            r = fn(instance, index);
        }
    }
    ik.found = 0;
    return ik;
}

Axon nextAxon(Axon a) {
    if (a.isEnd)
        fatalError(2, 2);
    return axonForAddress(a.nextAddress);
}

Axon axonForIndex(int axonIndex) {
    Axon axon = axonForAddress(AX_FIRST_AXON_ADDRESS);
    while (axonIndex > 0) {
        axon = nextAxon(axon);
        axonIndex--;
    }
    return axon;
}

// WRITE AXON

void writeAxon(Axon a) {
    int writeAddress = a.address;
    pset(RULES_HANDLE, writeAddress++, a.headValue);
    if (a.isEnd)
        return;
    pset(RULES_HANDLE, writeAddress++, a.instance);
    pset(RULES_HANDLE, writeAddress++, a.kind);
    for (int i = 0; i < a.numberOfParams; i++) {
        pset(RULES_HANDLE, writeAddress++, a.params[i] / 256);
        pset(RULES_HANDLE, writeAddress++, a.params[i] % 256);
    }
}

int indexOfAxon(Axon a) {
    Axon axon = axonForIndex(0);
    int  i = 0;
    while (!axon.isEnd && a.address != axon.address) {
        axon = nextAxon(axon);
        i++;
    }
    if (!axon.isEnd)
        return i;
    return -1;
}

int numberOfAxons() {
    Axon a = axonForIndex(0);
    int  n = 0;
    while (!a.isEnd) {
        a = nextAxon(a);
        n++;
    }
    return n;
}

// Insert and delete

void insertEmptyAction(int axonIndex) {
    insertAxon(axonIndex, 0, instanceForId(PORT_MANAGER_ID), 0);
}

void insertEmptyCondition(int axonIndex) {
    insertAxon(axonIndex, 1, instanceForId(PORT_MANAGER_ID), 0);
}

void insertAction(int axonIndex, int globalAxonIndex) {
    InstanceKind ik = instanceAndKindForGlobalIndex(0, globalAxonIndex);
    insertAxon(axonIndex, 0, ik.instance, ik.kind);
}

void insertCondition(int axonIndex, int globalAxonIndex) {
    InstanceKind ik = instanceAndKindForGlobalIndex(1, globalAxonIndex);
    insertAxon(axonIndex, 1, ik.instance, ik.kind);
}

void deleteAxon(int axonIndex) {
    Axon axon = axonForIndex(axonIndex);
    deleteChunkFragment(RULES_HANDLE, axon.address, axon.length);
}

// LABELS

int equals(const char* text, const char* pstr) {
    int i = 0;
    while (pgm_read_byte(pstr + i)) {
        if (pgm_read_byte(pstr + i) != pgm_read_byte(text + i))
            return 0;
        i++;
    }
    return pgm_read_byte(text + i) == ']';
}

// "hi [A] and [B]", index == 0, compareTo=="A" returns true
// "hi [A] and [B]", index == 1, compareTo=="B" returns true
int toolbarIntForParameterX(const char* text, int index, const char* compareTo) {
    int i = 0;
    while (pgm_read_byte(text + i)) {
        if (pgm_read_byte(text + i) == '[') {
            i++;
            if (index == 0)
                return equals(text + i, compareTo);
            index--;
        }
        i++;
    }
    return 0;
}

int toolbarIntForParameter(const char* text, int index, const char* toWhat) {
    int i = 0;
    while (pgm_read_byte(text + i)) {
        if (pgm_read_byte(text + i) == '[') {
            i++;
            if (index == 0)
                return equals(text + i, toWhat);
            index--;
        }
        i++;
    }
    fatalError(4, index);
    return 1;
}

int toolbarLabelForParameter1(const char* text, int index) {
    int i = 0;
    while (pgm_read_byte(text + i)) {
        if (pgm_read_byte(text + i) == '[') {
            i++;
            if (index == 0) {
                return i;
            }
            index--;
        }
        i++;
    }
    fatalError(74, index);
    return 0;
}

int toolbarLabelForParameter2(const char* text, int index) {
    int i = 0;
    while (pgm_read_byte(text + i)) {
        if (pgm_read_byte(text + i) == ']') {
            i++;
            if (index == 0) {
                return i - 2;
            }
            index--;
        }
        i++;
    }
    return 0;
}

int thereIsAnActiveCC() {
    return pget(RULES_HANDLE, AX_ACTIVE) != 255;
}

Axon activeCC() {
    int active = pget(RULES_HANDLE, AX_ACTIVE);
    if (active == 255)
        return notFoundAxon();
    return axonForIndex(active);
}

void setActiveCC(Axon cc) {
    pset(RULES_HANDLE, AX_ACTIVE, indexOfAxon(cc));
}

int evalCC(Axon cc) {
    // The only way a conditions stops, is when another one is true
    if (activeCC().address == cc.address)
        return 1;
    Axon a = cc;
    while (a.isCondition) {
        if (!callEvalCondition(a.instance, a.kind, a.params))
            return 0;
        a = nextAxon(a);
    }
    // The CC is true
    setActiveCC(cc);
    while (a.isAction) {
        callPerformAction(a.instance, a.kind, a.params);
        a = nextAxon(a);
    }
    return 0;
}

Axon firstCC() {
    Axon a = axonForIndex(0);
    while (a.isAction)
        a = nextAxon(a);
    return a;
}

Axon nextCC(Axon cc) {
    while (cc.isCondition)
        cc = nextAxon(cc);
    if (cc.isEnd)
        return cc;
    while (cc.isAction)
        cc = nextAxon(cc);
    return cc;
}

void evalRules() {
    for (int inst = 0; inst < numberOfInstances(); inst++) {
        if (chunkForInstanceAndHandleExists(inst, RULES_HANDLE)) {
            switchContextToInstance(inst);
            for (Axon cc = firstCC(); !cc.isEnd; cc = nextCC(cc))
                evalCC(cc);
            popContext();
        }
    }
}

void instanceWasRemoved(int instance) {
    for (int i = 0; i < numberOfAxons(); i++) {
        Axon axon = axonForIndex(i);
        if (axon.instance >= instance) {
            axon.instance--;
            writeAxon(axon);
        }
    }
}

int rulesReference(int instance) {
    for (int i = 0; i < numberOfAxons(); i++)
        if (axonForIndex(i).instance == instance)
            return 1;
    return 0;
}

/**
 Is there any instance that have rules that reference this instance?
 */
int rulesUsingInstance(int instance) {
    int result = 0;
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkHandle(_chunk) == RULES_HANDLE && chunkInstance(_chunk) != UNUSED_CHUNK &&
            chunkInstance(_chunk) != instance) {
            switchContextToInstance(chunkInstance(_chunk));
            result += rulesReference(instance);
            popContext();
        }
        _chunk = nextChunk(_chunk);
    }
    return result;
}

/**
 The instance was removed. Indexes refering to other instances must be updated.
 */
void updateRulesBecauseOfDeletionOfInstance(int instance) {
    int _chunk = goToFirstChunk();
    while (!endOfChunks(_chunk)) {
        if (chunkHandle(_chunk) == RULES_HANDLE && chunkInstance(_chunk) != UNUSED_CHUNK) {
            switchContextToInstance(chunkInstance(_chunk));
            instanceWasRemoved(instance);
            popContext();
        }
        _chunk = nextChunk(_chunk);
    }
}

void createRulesChunk() {
    allocChunk(RULES_HANDLE, 2);
    pset(RULES_HANDLE, 0, 255);
    pset(RULES_HANDLE, AX_FIRST_AXON_ADDRESS, AX_END);
}
