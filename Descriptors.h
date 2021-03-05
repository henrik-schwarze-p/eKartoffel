#ifndef KernelDescriptors_h
#define KernelDescriptors_h

#include "Frequency.h"

#include "stdint.h"

typedef void (*command)(int);
typedef void (*screen)(void);

#include "NumberOfDescriptors.inc.h"

#define APP_CARDINALITY_ALWAYS_ONE 0
#define APP_CARDINALITY_MAX_ONE 1
#define APP_CARDINALITY_UNDEFINED 2

uint16_t getDescriptor(int);
int      getDescriptorIndex(uint16_t);

#endif
