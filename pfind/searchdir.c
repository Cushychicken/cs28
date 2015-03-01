#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <regex.h>
#include <dirent.h>

char parse_args(int argc, char *argv[], regex_t *re) 
/*
 *	Parses system argument variables for the starting directory, the 
 *		search term for the underlying directories, and the file 
 *		type to search for. 
 *
 *	Inp	
 */
{
	int i;
	char ftype;

	for (i = 2; i < argc; i++) {
		if ( strcmp(argv[i], (char *)"-name") == 0 ) { 
			if (regcomp(re, argv[i+1], 0)) {
				fprintf( stderr, "Error %d\n", errno );
				perror( argv[i+1] );
				return NULL;
			}		
		} else if ( strcmp(argv[i], (char *)"-type") == 0 ) {
			if ( strlen(argv[i+1]) == 1 ) {
				ftype = (char)argv[i+1][0];
			} else {
				printf("Error: invalid file type\n");
				exit(1);
			}
		}
	}
	
	return ftype;	
}

void searchdir(char *dirname, regex_t *findme, char type)
/*
	Searches through a directory tree for a file. 
 */ 
{
	DIR 		*p_dir = opendir(dirname);
	struct dirent 	*p_dirent;
	struct stat	*p_fstat = malloc(sizeof((struct stat *)p_fstat));

	if ( p_dir == NULL ) {
		fprintf(stderr, "Cannot open %s \n", dirname);
		free(p_fstat);
		closedir(p_dir);
		exit(1);
	}

	while (( p_dirent = readdir(p_dir) )) {
		if ( lstat(p_dirent->d_name, p_fstat) ) {
			fprintf( stderr, "Error %d: ", errno );	
			perror( p_dirent->d_name );
			free(p_fstat);
			closedir(p_dir);
			return;
		} 
		
		// Skip this entry if "." or ".."
		if ( testcwd(p_dirent->d_name) ) {
			continue;
		}
		
		printf("%s ", p_dirent->d_name);	
		printf("%x\n", (S_IFMT & p_fstat->st_mode));
		if ( S_ISDIR( p_fstat->st_mode ) ) {
			printf("%s is a directory.\n", p_dirent->d_name);
		}
	}

	free(p_fstat);
	closedir(p_dir);
}

int testcwd(char *curdir) 
/*
	Tests current dirent entry name to not be "." or ".."

	Input - name of current working directory

	Returns 1 if cwd == "." or cwd == "..", returns 0 if false
*/	
{
	int thisdir = strcmp(curdir, (char *)".");
	int topdir  = strcmp(curdir, (char *)"..");

	if ( (thisdir == 0) || (topdir == 0) ) {
		return 1;
	} else {
		return 0;
	}
}

int testmatch(regex_t *regex, char *teststr)
/*
	Tests filename against regex and file type stored at runtime
	
	Inputs
		regex   - pointer to the compiled regex
		teststr - target string to run against *regex 

	Outputs
		Returns 1 if match found
		Returns 0 otherwise
 */ 
{
	if (!regexec(regex, teststr, (size_t) 0, NULL, 0)) {
		return 1;
	} else {
		return 0;
	}
}
