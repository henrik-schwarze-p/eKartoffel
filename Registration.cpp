//
//  Kernel+Registration.c
//  AquaOS
//
//  Created by Kurt Schwarze on 26.06.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "Chunks.h"
#include "FatalError.h"
#include "Heap.h"
#include "Instance.h"
#include "Registration.h"
#include "Target.h"

uint16_t _registerPersistentSize;
uint16_t _registerMemorySize;
uint16_t _registerMode;
char     _registrationState = 0;

void startRegisterMode(int mode) {
    _registerPersistentSize = _registerMemorySize = 0;
    _registerMode = mode;
    _registrationState = 1;
}

void endRegisterMode() {
    _registrationState = 0;
}

void registerAllocPersistantMemory() {
    callRegisterDirectly(getActiveInstance(), MODE_COUNT_SIZE);
    if (_registerPersistentSize > 0) {
        allocChunk(MAIN_CHUNK_HANDLE, _registerPersistentSize);
        addStatusForInstance(STATUS_REGISTERP_ACTIVATED);
    }
}

void registerAllocMemory() {
    callRegisterDirectly(getActiveInstance(), MODE_COUNT_SIZE);
    if (_registerMemorySize > 0) {
        allocHeap(MAIN_HEAP_HANDLE, _registerMemorySize);
        addStatusForInstance(STATUS_REGISTERM_ACTIVATED);
    }
}

void unserializeRegisteredVariables() {
    if (statusForInstance(getActiveInstance(), STATUS_REGISTERM_ACTIVATED))
        callRegisterDirectly(getActiveInstance(), MODE_UNSERIALIZE_VARS);
    if (statusForInstance(getActiveInstance(), STATUS_REGISTERP_ACTIVATED))
        callRegisterDirectly(getActiveInstance(), MODE_UNSERIALIZE_PERSISTENT_VARS);
}

void serializeRegisteredVariables() {
    if (statusForInstance(getActiveInstance(), STATUS_REGISTERM_ACTIVATED))
        callRegisterDirectly(getActiveInstance(), MODE_SERIALIZE_VARS);
    if (statusForInstance(getActiveInstance(), STATUS_REGISTERP_ACTIVATED))
        callRegisterDirectly(getActiveInstance(), MODE_SERIALIZE_PERSISTENT_VARS);
}

void pgregister(void* a, int size) {
    if (!_registrationState)
        fatalError(36, 0);
    for (int i = 0; i < size; i++)
        if (_registerMode == MODE_UNSERIALIZE_PERSISTENT_VARS)
            ((unsigned char*)a)[i] = pget(_registerPersistentSize + i);
        else if (_registerMode == MODE_SERIALIZE_PERSISTENT_VARS)
            pset(_registerPersistentSize + i, ((unsigned char*)a)[i]);
    _registerPersistentSize += size;
}

void mgregister(void* a, int size) {
    if (!_registrationState)
        fatalError(37, 0);
    for (int i = 0; i < size; i++)
        if (_registerMode == MODE_UNSERIALIZE_VARS)
            ((unsigned char*)a)[i] = mget(_registerMemorySize + i);
        else if (_registerMode == MODE_SERIALIZE_VARS)
            mset(_registerMemorySize + i, ((unsigned char*)a)[i]);
    _registerMemorySize += size;
}

void pregister(int* a) {
    pgregister(a, 2);
}

void pregister(float* a) {
    pgregister(a, 4);
}

void pregister(long* a) {
    pgregister(a, 4);
}

void pregister(unsigned long* a) {
    pgregister(a, 4);
}

void pregister(char* a) {
    pgregister(a, 1);
}

void mregister(int* a) {
    mgregister(a, 2);
}

void mregister(uint16_t* a) {
    mgregister(a, 2);
}

void mregister(float* a) {
    mgregister(a, 4);
}

void mregister(float* a, int n) {
    mgregister(a, 4 * n);
}

void mregister(long* a) {
    mgregister(a, 4);
}

void mregister(char* a) {
    mgregister(a, 1);
}
