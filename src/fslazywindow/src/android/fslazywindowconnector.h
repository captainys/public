#ifndef FSLAZYWINDOWCONNECTOR_IS_INCLUDED
#define FSLAZYWINDOWCONNECTOR_IS_INCLUDED
/* { */

#include "../fslazywindow.h"

void FsLazyWindowConnector_BeforeEverything(int argc,char *argv[]);
void FsLazyWindowConnector_GetOpenWindowOption(FsOpenWindowOption &OPT);
void FsLazyWindowConnector_Initialize(int argc,char *argv[]);
void FsLazyWindowConnector_Interval(void);
void FsLazyWindowConnector_BeforeTerminate(void);
void FsLazyWindowConnector_Draw(void);
bool FsLazyWindowConnector_UserWantToCloseProgram(void);
bool FsLazyWindowConnector_MustTerminate(void);
long long int FsLazyWindowConnector_GetMinimumSleepPerInterval(void);
bool FsLazyWindowConnector_NeedRedraw(void);
bool FsLazyWindowConnector_StayIdleUntilEvent(void);
void FsLazyWindowConnector_ContextLostAndRecreated(void);

/* } */
#endif
