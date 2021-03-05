//
//  CoreGraph.h
//  Henrikontroller
//
//  Created by Kurt Schwarze on 13.11.18.
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#ifndef CoreGraph_h
#define CoreGraph_h
typedef struct StructGraph {
    const char* title;
    // data
    int n;
    double (*sourceY)(int);
    int (*sourceN)(int);
    // size on screen
    int x0;
    int y0;
    int x1;
    int y1;
    // shown range
    double miny;
    double maxy;
    int    showHlines;
    // labels
    double yLabels;
    int    xLabels;
    int (*label)(int);
    // appearence
    int decimals;
    int verticalX;
    int type;  // 0 = line, 1 = bar

} Graph;

void graph(Graph g);

#endif /* CoreGraph_h */
