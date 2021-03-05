#define EPROM_YEAR 123
#define EPROM_MONTH 124
#define EPROM_DAY 125
#define EPROM_HOURS 126
#define EPROM_MINS 127

unsigned long epochSecs();
int           daysFromCivil(int y, int m, int d);
int           civilYear(int days);
int           civilMonth(int days);
int           civilDay(int days);

int           clockYear();
void          setClockYear(int year);
int           clockMonth();
void          setClockMonth(int month);
int           clockDay();
void          setClockDay(int day);
int           clockHours();
void          setClockHours(int hours);
int           clockMins();
void          setClockMins(int mins);
int           clockSecs();
void          setClockSecs(int secs);
long clockEpoch();

frequencyType incrementOneSecond();

void initTimer();
