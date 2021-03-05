//
//  CoreFastFont.c
//  AquaOS
//
//  Created by Kurt Schwarze on 27.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#include "Drawing.h"

#include "FastFont.inc.h"

typedef int (*fx)(int, int);
fx fns[94] = {draw33,  draw34,  draw35,  draw36,  draw37,  draw38,  draw39,  draw40,  draw41,  draw42,  draw43,
              draw44,  draw45,  draw46,  draw47,  draw48,  draw49,  draw50,  draw51,  draw52,  draw53,  draw54,
              draw55,  draw56,  draw57,  draw58,  draw59,  draw60,  draw61,  draw62,  draw63,  draw64,  draw65,
              draw66,  draw67,  draw68,  draw69,  draw70,  draw71,  draw72,  draw73,  draw74,  draw75,  draw76,
              draw77,  draw78,  draw79,  draw80,  draw81,  draw82,  draw83,  draw84,  draw85,  draw86,  draw87,
              draw88,  draw89,  draw90,  draw91,  draw92,  draw93,  draw94,  draw95,  draw96,  draw97,  draw98,
              draw99,  draw100, draw101, draw102, draw103, draw104, draw105, draw106, draw107, draw108, draw109,
              draw110, draw111, draw112, draw113, draw114, draw115, draw116, draw117, draw118, draw119, draw120,
              draw121, draw122, draw123, draw124, draw125, draw126};

int drawFx(int f, int x, int y) {
    return fns[f](x, y);
}
