#ifndef CoreButton_h
#define CoreButton_h

#include "Descriptors.h"

typedef enum enumToolbarButtonSize { standard, mini } toolbarButtonSize;
typedef enum enumButtonType { iconButton, labelButton, invisibleButton } buttonType;

typedef struct StructButton {
    int           y, x, width, deltaX;
    unsigned char height, nx, ny, deltaY;
    void (*command)(int);
    void (*screen)(void);
    int param;
} Button;

void toolbarAdd(toolbarButtonSize, int, const char*, command, int);
void toolbarAdd(toolbarButtonSize, int, const char*, command);
void toolbarAdd(const char*, command, int);
void toolbarAdd(toolbarButtonSize, int, const char*, screen);
void toolbarAdd(const char* label, command fn);
void toolbarAdd(const char* label, screen screen);
void toolbarAdd(toolbarButtonSize mini, int isEnabled, const char* label, command fn, int param, int first, int last);
void toolbarAdd(const char* label, command fn, int param, int f1, int f2);
void toolbarAddBack(screen back);
void toolbarAddBack(command cmd);
void toolbarAdd(int isEnabled, const char* label, screen screen);
void toolbarAdd(int isEnabled, const char* label, command fn);

void addLabel(int x, int y, const char* _label, int width, int height, int color, int style, int suffix);
void addLabel(int x, int y, int instance, int handle, int width, int height, int color, int style, int suffix);

void    resetButtons(void);
int     buttonForCoordinates(int px, int py);
Button* addButton(int     x,
                  int     y,
                  int     width,
                  int     height,
                  command command,
                  screen  screen,
                  int     nx,
                  int     ny,
                  int     deltaX,
                  int     deltaY,
                  int     param);
Button* unsecureAddButton(int     x,
                          int     y,
                          int     width,
                          int     height,
                          command command,
                          screen  screen,
                          int     nx,
                          int     ny,
                          int     deltaX,
                          int     deltaY,
                          int     param);
// void addIcon(int icon, const char* label, int suffix, int x, int y);
void addIcon(int icon, const char* label, int suffix, int x, int y, void (*f)(int));
void stampAnalog(int p, const char* c);

void initCoreUI(void);
void message(const char* m, void (*a)(void));
void message(const char* m, void (*a)(void), int clear);

Button buttonAtIndex(int index);

#define DRAWSCREEN_CLEAR_ALL 7
#define DRAWSCREEN_CLEAR_MIDDLE 1
#define DRAWSCREEN_CLEAR_BOTTOM 2
#define DRAWSCREEN_CLEAR_MENU 4

#define ignoreParam 0

#define STYLE_BIGFONT 1
#define STYLE_CENTERED 2
#define STYLE_BOLD 4
#define STYLE_INDENT 8

void   goToScreen(screen);
void   goToScreen(screen, int);
int    currentScreenIs(screen);
int    goToScreenLock(void);
screen currentScreen();
void   drawDebugButtonRect(Button b);
void   toolbarAddHome();
void   cleanMiddle();
void   appIconWithText(const char* text);
void   appIcon();
void   onIcon(int);

typedef struct StructToolbarButton {
    command       action;
    screen        screen;
    const char*   label;
    char          mini;
    char          enabled;
    int           param;
    int           width;
    unsigned char fromLabel;
    unsigned char toLabel;
} ToolbarButton;

#define MAX_TOOLBAR_BUTTONS 7

#endif
