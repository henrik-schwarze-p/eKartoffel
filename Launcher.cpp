//
//  Launcher.c
//  AquaOS
//
//  Created by Enrico on 20.09.20.
//  Copyright © 2020 Synium. All rights reserved.
//

#include "Launcher.h"
#include "BoardLib.h"
#include "Instance.h"
#include "Target.h"
#include "Text.h"
#include "Drawing.h"
#include "MainLoop.h"
#include "Heap.h"
#include "Chunks.h"
#include "Descriptors.h"
#include "Registration.h"
#include "Arduino.h"
#include "Rules.h"



#include "EepromConf.inc.h"

unsigned char readConf(int a) {
    return pgm_read_byte_near(eepromConfiguration + a);
}

void launchAll() {
    boardPrintln("Build b01.");
    boardPrintln("Launching...");

    for (int i = 0; i < MAX_NUMBER_OF_INSTANCES; i++)
        lastState[i] = 65535;

    unsigned char epromRandom0 = readByteFromEEPROM(EPROM_RANDOM);
    unsigned char epromRandom1 = readByteFromEEPROM(EPROM_RANDOM + 1);

    // Case 0
    // Random Bits match
    if (randomBits[0] == epromRandom0 && randomBits[1] == epromRandom1) {
        boardPrintln("Random Bits match");
        for (int i = 0; i < numberOfInstances(); i++)
            launchInstance(i);
        return;
    }

    // Case 1
    // Configuration in appStore, random bits are different
    // => Configuration + random bits written to EPROM
    if (readConf(0) != 0xFE) {
        boardPrintln("There is a configuration.");
        // the random bytes differ, we will overwrite all the eprom
        boardPrintln("Configuration is going to be written to the EPROM.");
        setPrintY(20);
        drawCenteredString(PSTR("Configuring..."));
        for (int i = 0; i < EPROM_SIZE; i++) {
            if (i % 64 == 0)
                fillRect(i / 64 * 5, 100, 5, 2);
            writeByteToEEPROM(i, readConf(i));
        }
        writeByteToEEPROM(EPROM_RANDOM, randomBits[0]);
        writeByteToEEPROM(EPROM_RANDOM + 1, randomBits[1]);
        boardPrintln("Configuration written.");
        for (int i = 0; i < numberOfInstances(); i++)
            launchInstance(i);
        return;
    }

    // Case 2
    // No configuration, random bits dont match

    boardPrintln("Random bits do not match, writing data for the first time.");
    boardPrintln("Date: 2019/2/1 12:30");

    setClockYear(19);
    setClockMonth(8);
    setClockDay(1);
    setClockHours(12);
    setClockMins(30);
    setClockSecs(5);

    writeByteToEEPROM(EPROM_FORMATTED, (uint8_t)45);
    writeByteToEEPROM(EPROM_FORMATTED + 1, (uint8_t)41);
    writeByteToEEPROM(EPROM_NUMBER_OF_INSTANCES, (uint8_t)0);

    writeByteToEEPROM(PERSISTANT_HEAP_ADDRESS + P_INSTANCE, UNUSED_CHUNK);
    writeByteToEEPROM(PERSISTANT_HEAP_ADDRESS + P_HANDLE, 0);
    writeIntToEEPROM(PERSISTANT_HEAP_ADDRESS + P_SIZE,
                     (unsigned int)(EPROM_SIZE - PERSISTANT_HEAP_ADDRESS - P_OVERHEAD));

    writeByteToEEPROM(EPROM_RANDOM, randomBits[0]);
    writeByteToEEPROM(EPROM_RANDOM + 1, randomBits[1]);

    boardPrintln("Adding one instance of each app.");

    for (int i = 0; i < NUMBER_OF_DESCRIPTORS; i++)
        launchDescriptor(i);
}

void launchDescriptor(int descriptor) {
    // Where are we going to write in the table?
    int address = INSTANCE_TABLE_START + INSTANCE_OVERHEAD * numberOfInstances();
    // First thing: the instance id
    writeIntToEEPROM(address + INSTANCE_ID, getDescriptor(descriptor));
    // How many instances are there now?
    int howManyInstances = numberOfInstances() + 1;
    writeByteToEEPROM(EPROM_NUMBER_OF_INSTANCES, howManyInstances);
    int newInstance = howManyInstances - 1;

    // Then the status: OK, not registering
    // From here, it is already safe to change contexts (because of registering)
    writeByteToEEPROM(address + INSTANCE_STATUS, STATUS_OK);

    boardPrint(PSTR("Launching "));
    boardPrintln(nameForInstance(newInstance));
    // First, we call when created, the persistant variable will be initialized
    // But they are still not registered, so context switching is harmless
    callWhenCreated(newInstance);

    // Persistant variables will be registered, and they will begin to switch.
    // Instances status is changed in this fn.
    switchContextToInstance(newInstance);

    registerAllocPersistantMemory();
    serializeRegisteredVariables();
    popContext();

    launchInstance(newInstance);
}

void launchInstance(int instance) {
    // The instance was already created, it should be in the table
    // If it crashed because of no memory, we clear the bit
    removeStatusForInstance(instance, STATUS_NO_MEMORY);
    removeStatusForInstance(instance, STATUS_REGISTERM_ACTIVATED);

    // Memory variables get initialized
    callWhenPowered(instance);

    // We will alloc memory for the registered variables, and context
    // switching will start for them.
    switchContextToInstance(instance);
    registerAllocMemory();
    popContext();
}
