#ifndef YSCOMMANDLINE_IS_INCLUDED
#define YSCOMMANDLINE_IS_INCLUDED
/* { */

#include "ysarraymask.h"
#include "ysstring.h"

/*! Takes an array of command arguments and make it a command-line strings.
    Arguments must not include double-quotes.
*/
YsString YsAssembleCommandLine(YsConstArrayMask <YsString> argv);
YsWString YsAssembleCommandLine(YsConstArrayMask <YsWString> argv);

/* } */
#endif
