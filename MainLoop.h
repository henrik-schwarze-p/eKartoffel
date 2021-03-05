#ifndef KernelProcess_h
#define KernelProcess_h

#define SCHEDULE_FIRST firstCall
#define SCHEDULE_SEC second
#define SCHEDULE_MIN minute
#define SCHEDULE_10MIN minute10
#define SCHEDULE_15MIN minute15
#define SCHEDULE_30MIN minute30
#define SCHEDULE_HOUR hour
#define SCHEDULE_DAY day

unsigned long epochSecs();
int           daysFromCivil(int y, int m, int d);
int           civilYear(int days);
int           civilMonth(int days);
int           civilDay(int days);

// Gets called aproximately every second. Never misses a second! But huge delays
// may be there! Every 50 days one or 2 seconds may be lost.
void schedule(void);
void initTimer();
void initScheduler();

void         processTouch();
unsigned int ourMillis();
void         showMem();
void         mainLoop();
void         setJustOpened(int s);

int  clockYear();
int  clockMonth();
int  clockDay();
int  clockHours();
int  clockMins();
int  clockSecs();
long clockEpoch();
void setClockHours(int);
void setClockSecs(int);
void setClockMins(int);
void setClockDay(int);
void setClockMonth(int);
void setClockYear(int);
#endif
