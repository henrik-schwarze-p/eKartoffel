//
//  CoreUI-Port.h
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef CoreUI_Port_h
#define CoreUI_Port_h

void showDigitalPortDialog(int currentPort, void (*fn)(int));
void showAnalogPortDialog(int currentPort, void (*fn)(int));

#endif /* CoreUI_Port_h */
