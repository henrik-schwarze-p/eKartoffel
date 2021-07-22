//
//  LogLib.c
//  AquaOS
//
//  Created by Kurt Schwarze on 01.02.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//

#include "Log.h"
#include "Chunks.h"
#include "FatalError.h"
#include "MainLoop.h"
#include "Target.h"
#include "Text.h"
#include "Drawing.h"
#include "UI.h"

/*
 Cost of logging:

 * Every few minutes: 2 bytes
 * Every few hours: 3 bytes
 * Every few days: 4 bytes
 * Every few years: 5 bytes

 Anatomy
     10    Version
     FF BEGIN
     EVENT 0 // Bit 8 not used, no FF will be found
     EVENT 1 // Events can be 2 bytes (less than 256s diff)
     EVENT 2 // or 3 bytes (less than 16000s diff)
     EVENT 3 // or even 4 bytes (less than 23 days), and 5 bytes for 9 years, 6 bytes forever
     SECONDS SINCE 2019-01-01 00:00:00 (4 bytes) (time of last log)
     FE END
     [Bytes...]*

     Can overflow!

    251..255 are reserved!
    Examples for event "18" and different intervals

    Example of EVENT with 120s   : 18 120
    Example of EVENT with 128s   : 18 128 128 (128 [10]000000)
    Example of EVENT with 250s   : 18 250 128 (128 [10]000000)
    Example of EVENT with 251s   : 18 0 129 (128 [10]000001)
    Example of EVENT with 15810s : 18 248 190 (128 [10]111110, 62*251+248)
    Example of EVENT with 30000s : 18 59 119 192 (128 [110]00000, 0*251*251+119*251+59)
    Example of EVENT with 22d    : 18 221

 */

#define LOG_VERSION 0
#define LOG_END 254
#define LOG_START 255

namespace aqualog {
    int           offset = 0;
    int           currentOffset;
    unsigned long currentSecs;
    void (*callback)(int);
    void (*home)();
    void (*formatter)(unsigned char);

    void showLog();

    void createChunk(int memory, int size) {
        if (memory < 30)
            fatalError(38, memory);
        allocChunk(LOG_HANDLE, memory);
        pset(LOG_HANDLE, LOG_VERSION, 0x0A + size - 1);
        pset(LOG_HANDLE, 1, LOG_START);
        pset(LOG_HANDLE, 2, epochSecs() / 251 / 251 / 251);
        pset(LOG_HANDLE, 3, epochSecs() / 251 / 251 % 251);
        pset(LOG_HANDLE, 4, epochSecs() / 251 % 251);
        pset(LOG_HANDLE, 5, epochSecs() % 251);
        pset(LOG_HANDLE, 6, LOG_END);
    }

    int chunkIsEmpty() {
        return chunkSize(LOG_HANDLE) == 7;
    }

    int offsetDeltaed(int offset, int delta) {
        offset += delta;
        if (offset < 1)
            offset += chunkSize(LOG_HANDLE) - 1;
        if (offset > chunkSize(LOG_HANDLE) - 1)
            offset -= chunkSize(LOG_HANDLE) - 1;
        return offset;
    }

    void offsetDeltaed(int delta) {
        currentOffset = offsetDeltaed(currentOffset, delta);
    }

    int endOffset() {
        for (int i = 1; i < chunkSize(LOG_HANDLE); i++)
            if (pget(LOG_HANDLE, i) == LOG_END)
                return i;
        fatalError(39, 0);
        return 0;
    }

    int startOffset() {
        for (int i = 0; i < chunkSize(LOG_HANDLE); i++)
            if (pget(LOG_HANDLE, i) == LOG_START)
                return i;
        fatalError(40, 0);
        return 0;
    }

    unsigned char logValue(int offset, int offsetDelta) {
        return pget(LOG_HANDLE, offsetDeltaed(offset, offsetDelta));
    }

    unsigned char logValue(int offsetDelta) {
        return pget(LOG_HANDLE, offsetDeltaed(currentOffset, offsetDelta));
    }

    void setLogValue(int offset, int offsetDelta, unsigned char value) {
        pset(LOG_HANDLE, offsetDeltaed(offset, offsetDelta), value);
    }

    unsigned long lastLogInSeconds() {
        int offset = endOffset();
        return logValue(offset, -4) * 251 * 251 * 251 + logValue(offset, -3) * 251 * 251 + logValue(offset, -2) * 251 +
               logValue(offset, -1);
    }

    void jumpToTailOfLastLog() {
        currentSecs = lastLogInSeconds();
        currentOffset = endOffset() - 5;
    }

    // returns 0 if no more entries, the info in the params
    int read(unsigned char* event,
             int*           year,
             unsigned char* month,
             unsigned char* day,
             unsigned char* hour,
             unsigned char* min,
             unsigned char* sec) {
        if (logValue(0) == LOG_START)
            return 0;
        unsigned char lastByte = logValue(0);
        long          delta = 0;
        if ((lastByte & 128) == 0) {
            *event = logValue(-1);
            delta = logValue(0);
            currentOffset -= 2;
        } else if ((lastByte & 192) == 128) {
            *event = logValue(-2);
            delta = (logValue(0) - 128) * 251 + logValue(-1);
            currentOffset -= 3;
        } else if ((lastByte & 224) == 192) {
            *event = logValue(-3);
            delta = (logValue(0) - 192) * 251 * 251 + logValue(-1) * 251 + logValue(-2);
            currentOffset -= 4;
        } else if ((lastByte & 240) == 224) {
            *event = logValue(-4);
            delta =
                (logValue(0) - 224) * 251 * 251 * 251 + logValue(-1) * 251 * 251 + logValue(-2) * 251 + logValue(-3);
            currentOffset -= 5;
        } else
            fatalError(41, lastByte);

        *sec = (unsigned char)(currentSecs % 60L);
        *min = (unsigned char)(currentSecs % 3600L / 60L);
        *hour = (unsigned char)(currentSecs % (24L * 3600L) / 3600L);
        int inDays = (int)(currentSecs / 86400L);
        *day = civilDay(inDays);
        *month = civilMonth(inDays);
        *year = civilYear(inDays);

        currentSecs -= delta;
        return 1;
    }

    void advanceStart() {
        jumpToTailOfLastLog();
        int           ignoreInt = 0;
        unsigned char ignoreChar = 0;
        int           n = 0;
        while (read(&ignoreChar, &ignoreInt, &ignoreChar, &ignoreChar, &ignoreChar, &ignoreChar, &ignoreChar))
            n++;
        jumpToTailOfLastLog();
        for (int i = 0; i < n - 1; i++)
            read(&ignoreChar, &ignoreInt, &ignoreChar, &ignoreChar, &ignoreChar, &ignoreChar, &ignoreChar);
        setLogValue(startOffset(), 0, 0);
        setLogValue(currentOffset, 0, LOG_START);
    }

    void ensure10BytesBetweenBeginAndEnd() {
        int rep = 1;
        while (rep) {
            int start = startOffset();
            int end = endOffset();
            rep = 0;
            for (int i = 0; i < 10; i++) {
                if (start == offsetDeltaed(end, i)) {
                    advanceStart();
                    rep = 1;
                    break;
                }
            }
        }
    }

    void log(unsigned char event) {
        if (event > 250)
            return;
        ensure10BytesBetweenBeginAndEnd();
        currentOffset = endOffset();
        unsigned long delta = epochSecs() - lastLogInSeconds();
        int           n = -4;
        setLogValue(currentOffset, n++, event);
        setLogValue(currentOffset, n++, delta % 251);
        if (delta < 128) {
            // life is easy for numbers less than 128... I would be one.
        } else if (delta <= 251 * 63)
            setLogValue(currentOffset, n++, delta / 251 + 128);
        else if (delta <= 251 * 251 * 31) {
            setLogValue(currentOffset, n++, delta / 251 % 251);
            setLogValue(currentOffset, n++, delta / 251 / 251 + 192);
        } else if (delta <= 251 * 251 * 251 * 15) {
            setLogValue(currentOffset, n++, delta / 251 % 251);
            setLogValue(currentOffset, n++, delta / 251 / 251 % 251);
            setLogValue(currentOffset, n++, delta / 251 / 251 / 251 + 224);
        } else
            fatalError(42, (int)delta);

        setLogValue(currentOffset, n++, epochSecs() / 251 / 251 / 251);
        setLogValue(currentOffset, n++, epochSecs() / 251 / 251 % 251);
        setLogValue(currentOffset, n++, epochSecs() / 251 % 251);
        setLogValue(currentOffset, n++, epochSecs() % 251);
        setLogValue(currentOffset, n, 0xFE);
    }

    void dump() {
        jumpToTailOfLastLog();
        unsigned char event;
        int           y;
        unsigned char m;
        unsigned char d;
        unsigned char h;
        unsigned char mi;
        unsigned char s;
        consoleln("LOG");
        consoleln("==========");
        while (read(&event, &y, &m, &d, &h, &mi, &s)) {
            console(y);
            console("-");
            console(m);
            console("-");
            console(d);
            console(" ");
            console(h);
            console("-");
            console(mi);
            console("-");
            console(s);
            console("    ");
            consoleln(event);
        }
        consoleln("==========");
    }

    int numberOfLogs() {
        jumpToTailOfLastLog();
        int           n = 0;
        unsigned char event;
        int           y;
        unsigned char m;
        while (read(&event, &y, &m, &m, &m, &m, &m))
            n++;
        return n;
    }

    void up(int param) {
        if (offset > 0)
            offset--;
        goToScreen(showLog);
    }

    void down(int param) {
        if (offset < numberOfLogs())
            offset++;
        goToScreen(showLog);
    }

    void print2(int i) {
        if (i < 10)
            print(PSTR("0"));
        print(i);
    }

    void showLog() {
        jumpToTailOfLastLog();
        unsigned char event;
        int           y;
        unsigned char m;
        unsigned char d;
        unsigned char h;
        unsigned char mi;
        unsigned char s;
        print(PSTR("Date"));
        setPrintX(136);
        println(PSTR("Event"));
        setColor(colorDarkGray);
        drawLine(0, 38, 319, 38);
        setColor(colorBlack);
        int skip = offset;
        while (skip-- && read(&event, &y, &m, &d, &h, &mi, &s))
            ;

        int i = 0;
        while (read(&event, &y, &m, &d, &h, &mi, &s) && i < 9) {
            i++;
            print(y);
            print(PSTR("-"));
            print2(m);
            print(PSTR("-"));
            print2(d);
            print(PSTR(" "));
            print2(h);
            print(PSTR(":"));
            print2(mi);
            print(PSTR(":"));
            print2(s);
            setPrintX(136);
            formatter(event);
            println();
        }
        toolbarAdd(standard, 1 > 0, MINI_ICON_UP, up);
        toolbarAdd(MINI_ICON_DOWN, down);
        toolbarAddBack(home);
    }

}

void createLogChunk(int i) {
    aqualog::createChunk(i, 1);
}

int logChunkIsEmpty() {
    return aqualog::chunkIsEmpty();
}

void logEvent(unsigned char data) {
    if (!chunkForHandleExists(LOG_HANDLE))
        createLogChunk(200);
    aqualog::log(data);
}

void showLogDialog(void (*f)(), void (*fn)(unsigned char)) {
    if (!chunkForHandleExists(LOG_HANDLE))
        createLogChunk(200);
    aqualog::offset = 0;
    aqualog::home = f;
    aqualog::formatter = fn;
    goToScreen(aqualog::showLog);
}

void dumpLog() {
    aqualog::dump();
}
