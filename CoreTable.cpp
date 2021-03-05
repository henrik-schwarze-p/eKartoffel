//
//  CoreTable.c
//  Henrikontroller
//
//  Created by Kurt Schwarze on 14.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#include "AquaOS.h"

// Table
void table(int* dataTDS, int first, int last, int x0, int maxY) {
    for (int i = first; i <= last; i++) {
        setPrintX(x0);
        print(dataTDS[i]);
        println();
    }
}
