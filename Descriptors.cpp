#include "Namespaces.h"
#include "FatalError.h"
#include "Descriptors.h"
#include "Target.h"

// Descriptors are lightweight static classes

#include "Descriptors.inc.h"

uint16_t getDescriptor(int descriptor) {
    if (descriptor < 0 || descriptor >= NUMBER_OF_DESCRIPTORS)
        fatalError(7, descriptor);
    return descriptors[descriptor];
}

int getDescriptorIndex(uint16_t d) {
    for (int i = 0; i < NUMBER_OF_DESCRIPTORS; i++)
        if (getDescriptor(i) == d)
            return i;
    fatalError(9, d);
    terminate();
    return 0;
}
