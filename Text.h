//
//  TextLib.h
//  Copyright © 2018 Kurt Schwarze. All rights reserved.
//

#ifndef TextLib_h
#define TextLib_h

#define MINI_ICON_GARBAGE PSTR("\x01")
#define MINI_ICON_CONDITION PSTR("\x02")
#define MINI_ICON_ACTION PSTR("\x03")
#define MINI_ICON_ALL PSTR("\x04")
#define MINI_ICON_ANY PSTR("\x05")
#define MINI_ICON_ADD PSTR("\x06")
#define MINI_ICON_UP PSTR("\x07")
#define MINI_ICON_DOWN PSTR("\x08")
#define MINI_ICON_HOME PSTR("`")
#define MINI_ICON_LEFT PSTR("^")
#define MINI_ICON_RIGHT PSTR("\\")

#define BYTE_READING_MODE_PSTR 0
#define BYTE_READING_MODE_EEPROM 1
#define BYTE_READING_MODE_RAM 2

#define standardFont 1
#define bigFont 2

#define characterSeparation 2

#define NAME_HANDLE 248

void setStandardFont(void);
void setBigFont(void);

void setNormalStyle(void);
void setBoldStyle(void);

void pushStyle();
void popStyle();

int drawChar(int a, int x, int y);

void setPrintX(int x);
void setPrintY(int y);
void addPrintY(int y);
int  printY(void);
int  printX(void);
void println(int n);
void println(float n);
void print(float a);
void print(unsigned int i);

void         println();
unsigned int stringLen(const char* s);

void setDefaultMaxPrintX();
void setMaxPrintX(int x);

int fontHeight(void);
int lineHeight();

unsigned int stringWidth(const char* s);
unsigned int stringWidth(const char* s, int from, int toLabel);

int intWidth(int a);
int intWidth(int a, int padding);

void drawTPLString(const char* a, int x, int y, void (*fnTpl)(int));
void drawString(const char* a, int x, int y);
void drawString(int x, int y);

void drawStringWithIntSubscript(const char* s, int x, int y, int width, int suffix);
void drawInt(int n, int x, int y);
void drawInt(int n, int x, int y, int padding);
void drawFloat(double f, int decimals, int x, int y);
int  floatWidth(double f, int decimals);
void drawString(const char* s, int x, int y, int from, int end);
void drawString(int x, int y, int from, int end);

void drawCenteredString(const char* s);
void print(const char* s);
void println(const char* s);
void print(int);
void print(unsigned long a);

void print(int a, int padding);
void println(unsigned int n);
void println(unsigned long n);

void cprint(const char* c);
void cprint(float);
void cprintln(const char* c);

void printTitle(const char* title);
void printAlignedRight(const char* s, int x);
void cleanIntPrint(int a, int extra);

void measureText(const char* a, int* widths, int* count);
void drawCenteredStringPart(const char* m, int start, int end, int y);

unsigned int stringWidth();
void         printStringRespectingMode();

// ???
void printMiniIcon(const char* c);

void setMargins(int x1, int x2);
void cleanRestOfLine();

void printDefinition(const char* title);
void endDefinitions();

int  ramSourceLength();
char getRamSource(int i);
void setRamSource(int index, char val);
void clearRamSource();

void setPrintModeRamSource();
void setPrintModePSTR();
void setPrintModeEEPROM(int offset);
void drawStringWithIntSubscript(int ox, int y, int width, int suffix);
void setScale(int s);
int  hasSpaces(const char* l);

#endif /* TextLib_h */
