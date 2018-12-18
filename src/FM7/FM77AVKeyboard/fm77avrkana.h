#ifndef FM77AVRKANA_IS_INCLUDED
#define FM77AVRKANA_IS_INCLUDED
/* { */

#include "fm77avkey.h"

typedef struct {
	const char *inputStr;
	bool carryOver;
	FM77AVKeyCombination comb[3];
} FM77AVRKanaMap;

FM77AVRKanaMap FM77AVFindKeyCombinationForRomaji(const char romaji[]);

/* } */
#endif
