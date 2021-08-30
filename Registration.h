//
//  Kernel+Registration.h
//  AquaOS
//
//  Created by Kurt Schwarze on 26.06.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef Kernel_Registration_h
#define Kernel_Registration_h

#define MODE_COUNT_SIZE 0
#define MODE_UNSERIALIZE_VARS 2
#define MODE_SERIALIZE_VARS 3
#define MODE_UNSERIALIZE_PERSISTENT_VARS 4
#define MODE_SERIALIZE_PERSISTENT_VARS 5

void startRegisterMode(int mode);
void endRegisterMode();

void pregister(int* a);
void pregister(float* a);
void pregister(long* a);
void pregister(char* a);
void pregister(unsigned long* a);

void mregister(int* a);
void mregister(uint16_t* a);
void mregister(float* a);
void mregister(long* a);
void mregister(char* a);
void mregister(float* a, int n);

void registerAllocPersistantMemory();
void registerAllocMemory();
void unserializeRegisteredVariables();
void serializeRegisteredVariables();

#endif /* Kernel_Registration_h */
