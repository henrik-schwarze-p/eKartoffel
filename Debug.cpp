#include "Debug.h"
#include "BoardLib.h"
#include "Heap.h"
#include "Instance.h"
#include "Target.h"
#include "Chunks.h"
#include "Log.h"
#include "Rules.h"
#include "Text.h"

extern unsigned char heapData[HEAP_SIZE];

void dumpChunks() {
    boardBegin();
    int _chunk = goToFirstChunk();
    boardPrintln("CHUNKS\n");
    while (!endOfChunks(_chunk)) {
        boardPrint("   ");
        boardPrint(_chunk);
        boardPrint("   ");
        boardPrint(instanceName(chunkInstance(_chunk)));
        if (chunkHandle(_chunk) == LOG_HANDLE)
            boardPrint("(Log)");
        else if (chunkHandle(_chunk) == RULES_HANDLE)
            boardPrint("(Rules)");
        else if (chunkHandle(_chunk) == NAME_HANDLE)
            boardPrint("(Name)");
        else if (chunkHandle(_chunk) == MAIN_CHUNK_HANDLE)
            boardPrint("(Main)");
        else {
            boardPrint("(");
            boardPrint(chunkHandle(_chunk));
            boardPrint(")");
        }
        boardPrint(" Size: ");
        boardPrint(_chunkSize(_chunk));
        boardPrint(" [");
        for (int i = 0; i < 30 && i < _chunkSize(_chunk); i++) {
            boardPrint(_chunkData(_chunk, i));
            boardPrint(" ");
        }
        boardPrintln("]");
        _chunk = nextChunk(_chunk);
    }
    boardCommit();
}

void dumpChunksTxt() {
    int _chunk = goToFirstChunk();
    console("\nCHUNKS");
    while (!endOfChunks(_chunk)) {
        console("   ");
        console(_chunk);
        console("   Instance: ");
        console(chunkInstance(_chunk));
        console(" Handle: ");
        console(chunkHandle(_chunk));
        console(" Size: ");
        console(_chunkSize(_chunk));
        console("[");
        for (int i = 0; i < 50 && i < _chunkSize(_chunk); i++) {
            console(_chunkData(_chunk, i));
            console(" ");
        }
        console("]");
        consoleln();
        _chunk = nextChunk(_chunk);
    }
}

void dumpHeaps() {
    boardBegin();
    int head = 0;
    boardPrintln("HEAPS");
    while (heapData[head + H_INSTANCE] != UNUSED_HEAP) {
        boardPrint("   ");
        boardPrint(head);
        boardPrint("   ");
        boardPrint(instanceName(heapData[head + H_INSTANCE]));
        boardPrint(" Handle:");
        if (heapData[head + H_HANDLE] == 249)
            boardPrint("TIMERS");
        else if (heapData[head + H_HANDLE] == 1)
            boardPrint("MAIN");
        else
            boardPrint(heapData[head + H_HANDLE]);
        boardPrint(" Len:");
        int len = heapData[head + H_LEN];
        boardPrint(len);
        boardPrint(" [");
        if (len > 20)
            len = 20;
        for (int i = 0; i < len; i++) {
            boardPrint(heapData[head + H_LEN + 1 + i]);
            boardPrint(" ");
        }
        boardPrintln("]");
        head += H_OVERHEAD + heapData[head + H_LEN];
    }
    boardCommit();
}

void dumpHeapsTxt() {
    int head = 0;
    console("HEAP");
    consoleln();
    while (heapData[head + H_INSTANCE] != UNUSED_HEAP) {
        console(head);
        console(" Instance:");
        console(instanceName(heapData[head + H_INSTANCE]));
        console(" Handle:");
        console(heapData[head + H_HANDLE]);
        console(" Len:");
        console(heapData[head + H_LEN]);
        console(" [");

        for (int i = 0; i < heapData[head + H_LEN]; i++) {
            console(heapData[head + H_DATA + i]);
            console(" ");
        }
        console("]");
        consoleln();
        head += H_OVERHEAD + heapData[head + H_LEN];
    }
}
