//
//  CoreUI-Select.h
//  AquaOS
//
//  Created by Kurt Schwarze on 01.01.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#ifndef CoreUI_Select_h
#define CoreUI_Select_h

void showSelectDialog(const char* setName,
                      int         cols,
                      const char* (*fnTextSource)(int),
                      void (*fnCallback)(int, int),
                      int firstSelected);

void showSelectDialog(const char* setName,
                      int         cols,
                      const char* (*fnTextSource)(int),
                      void (*fnCallback)(int, int),
                      int         firstSelected,
                      const char* m);
void showSelectDialog(const char* setName,
                      const char* setName2,
                      int         cols,
                      const char* (*fnTextSource)(int),
                      void (*fnCallback)(int, int),
                      int         firstSelected,
                      const char* m);

#endif /* CoreUI_Select_h */
