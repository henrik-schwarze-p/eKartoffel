//
//  CoreUI-NumPad.h
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef CoreUI_NumPad_h
#define CoreUI_NumPad_h

void showFloatNumPad(const char* message,
                float       initialValue,
                float       min,
                float       max,
                void (*callback)(int, float));

void showNumPad(const char* message,
                long       initialValue,
                 long       min,
                 long       max,
                void (*callback)(int, long));


#endif /* CoreUI_NumPad_h */
