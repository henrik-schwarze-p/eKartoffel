#ifndef CoreAction_UI_ListActions_h
#define CoreAction_UI_ListActions_h
void        showRulesDialog(void (*fn)(void));
void        showActions();
const char* aeAllActions(int index);
const char* aeAllConditions(int index);
int         paramIs(const char*);
void        continueSetConditionParameter(int, uint16_t p);

#endif
