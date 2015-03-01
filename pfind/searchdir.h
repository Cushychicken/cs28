#include <regex.h>

char *parse_args(int argc, char *argv[], regex_t *re);
void searchdir(char *dirname);//, char *findme, char type)
int testmatch(regex_t *regex, char *teststr);
