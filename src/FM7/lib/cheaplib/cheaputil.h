#ifndef UTIL_IS_INCLUDED
#define UTIL_IS_INCLUDED
/* { */

#include <vector>
#include <string>

std::vector <std::string> CheapParser(const char str[]);
const char *CheapFGets(std::string &str,FILE *fp);

/* } */
#endif
