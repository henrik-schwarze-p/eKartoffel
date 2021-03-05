#ifndef KernelRules_h
#define KernelRules_h

#define AX_ACTIVE 0
#define AX_END 0xED
#define AX_IS_CONDITION 0xC0
#define AX_IS_ACTION 0xA0

#define AX_INSTANCE_OFFSET 1
#define AX_KIND_OFFSET 2
#define AX_PARAMS_OFFSET 3
#define AX_FIRST_AXON_ADDRESS 1

#include "stdint.h"

void updateRulesBecauseOfDeletionOfInstance(int instance);
int  rulesUsingInstance(int instance);

typedef struct StructAxon {
    unsigned char headValue;

    // poor man's functions
    unsigned char isEnd;
    unsigned char isAction;     // undefined if not valid
    unsigned char isCondition;  // undefined if not valid
    unsigned char notFound;     // couldn't find it in memory

    int address;  // may be 0 for in-memory axons
                  // may be set for invalid axons
                  // if 0, then the Axon is "dettached"
    int           instance;
    int           kind;
    int           length;
    int           nextAddress;  // undefined for in-memory access
    uint16_t      params[5];
    unsigned char numberOfParams;
    const char*   label;
} Axon;

typedef struct structInstanceKind {
    int instance;
    int kind;
    int found;
} InstanceKind;

Axon         invalidAxon(int address);
void         writeAxon(Axon a);
void         createRulesChunk();
void         evalRules();
void         insertEmptyAction(int axonIndex);
void         insertEmptyCondition(int axonIndex);
int          numberOfAxons();
void         deleteAxon(int axonIndex);
void         insertCondition(int axonIndex, int globalAxonIndex);
Axon         axonForIndex(int axonIndex);
void         insertAction(int axonIndex, int globalAxonIndex);
int          toolbarIntForParameter(const char* text, int index, const char* toWhat);
int          toolbarIntForParameterX(const char* text, int index, const char* compareTo);
InstanceKind instanceAndKindForGlobalIndex(int considerConditions, int globalAxonIndex);
int          toolbarLabelForParameter1(const char*, int index);
int          toolbarLabelForParameter2(const char*, int index);
int          rulesReference(int instance);
void         instanceWasRemoved(int instance);
int          thereIsAnActiveCC();

#define RULES_HANDLE 253
#endif
