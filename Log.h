//
//  LogLib.h
//  AquaOS
//
//  Created by Kurt Schwarze on 01.02.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef LogLib_h
#define LogLib_h
#define LOG_HANDLE 252
void createLogChunk(int);

// Logs a value between 0 and 249
void logEvent(unsigned char data);
void dumpLog();
void showLogDialog(void (*f)(), void (*fn)(unsigned char));
int  logChunkIsEmpty();
#endif /* LogLib_h */
