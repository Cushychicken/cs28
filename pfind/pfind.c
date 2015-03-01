#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include "searchdir.h"

// Ex usage 
//	pfind starting_dir [-name fname/pattern] [-type {f|d|b|c|p|l|s}]
int main(int argc, char *argv[]) {

	regex_t re_target;
		
	parse_args(argc, argv, &re_target);

	if ( argc < 2 ) {
		printf("Error: too few arguments\n");
		return -1;
	}
	
	searchdir(argv[1]);	

	regfree(&re_target);
	
	return 0;
}
