#ifndef FSLAZYWINDOW_XAML_BRIDGE_IS_INCLUDED
#define FSLAZYWINDOW_XAML_BRIDGE_IS_INCLUDED
/* { */

// Functions are defined in FsSimpleWindow library.
extern void FsUWPReportWindowSize(int wid,int hei);
extern void FsUWPMakeReportWindowSize(void);
extern void FsUWPSwapBuffers(void);
extern void FsUWPReportMouseEvent(int evt,int lb,int mb,int rb,int mx,int my);
extern void FsUWPReportCharIn(int unicode);

/* } */
#endif
