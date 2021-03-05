//
//  BoardLib.h
//  AquaOS
//
//  Created by Kurt Schwarze on 11.02.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef BoardLib_h
#define BoardLib_h

void initBoard();
int  boardReadAnalog(int);
int  boardReadDigital(int p);
void boardPrintln(const char* c);
void boardPrintln(int u);
void boardPrintln(int u);
void boardPrint(const char* c);
void boardPrint(int u);
void boardPrintln(float f);
void boardTextRepaint();
void boardDigitalPortClicked(int param);
void boardAnalogPortClicked(int param);
void boardAnalogValueClicked(int param);

void boardWriteDigital(int p, int v);
void boardWriteAnalog(int p, int v);

void boardBegin();
void boardCommit();

#endif /* BoardLib_h */
