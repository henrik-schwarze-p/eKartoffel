//
//  AquaOS.h
//  AquaOS
//
//  Created by Kurt Schwarze on 13.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef nsf_h
#define nsf_h

typedef enum enumFrequency { firstCall, second, minute, minute10, minute15, minute30, hour, day, month } frequencyType;
typedef struct structFrequency {
    frequencyType type;
    unsigned char justOpened;
} frequency;

#endif
