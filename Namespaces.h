//
//  AquaOS.h
//  AquaOS
//
//  Created by Kurt Schwarze on 13.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef ns_h
#define ns_h

#define isenabled 1
#define isdisabled 0

#define CLOCK_ID 15
#define PORT_MANAGER_ID 16
#define DESKTOP_ID 17

// Some functionality is delegated to nanoapps

namespace admin {
    char alarmPort();
}
namespace desktop {
    void startScreen(void);
    void sca(int);
}

namespace pm {
    int  _digitalRead(int);
    void _digitalWrite(int, int);
    int  _analogRead(int);
    void _analogWrite(int, int);
}

namespace rules {
    void list();
}

int  pmDigitalRead(int port);
void pmDigitalWrite(int port, int value);
void pmAnalogWrite(int port, int value);

#endif
