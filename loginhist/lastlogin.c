#include	<stdio.h>
#include	<sys/types.h>
#include	<utmp.h>
#include	<fcntl.h>
#include	<time.h>
#include	<stdlib.h>
#include 	<string.h>
#include	"utmplib.h"	/* include interface def for utmplib.c	*/

/*
 *	lastlogin v1.0		- read /etc/wtmp or supplied wtmp file
 *				- searches file for "username" supplied 
 *				 	at runtime 
 *				- formats time nicely
 *				- buffers input (using utmplib)
 */

#define TEXTDATE
#ifndef	DATE_FMT
#ifdef	TEXTDATE
#define	DATE_FMT	"%b %e %H:%M"		/* text format	*/
#else
#define	DATE_FMT	"%Y-%m-%d %H:%M"	/* the default	*/
#endif
#endif

int main(int ac, char *av[])
{
	
	struct utmp	*utbufp,	/* holds pointer to next rec	*/
			*utmp_next();	/* returns pointer to next	*/
	struct utmp 	last_login;     /* storage of most recent login */
	void		show_info( struct utmp * );
	char		*parse_args(int argc, char **argv);
	char		*username;


	last_login.ut_time = 0;		/* Init last_login time to 0 */
	username = parse_args(ac, av);

	while ( ( utbufp = utmp_next() ) != ((struct utmp *) NULL) ) {
		if (strcmp(username, utbufp->ut_user) == 0) {
			if (utbufp->ut_time > last_login.ut_time) { 
				last_login = *utbufp;
			} 
		}
	}
	utmp_close( );
	
	show_info( &last_login );
	return 0;
}

char *parse_args(int argc, char *argv[]) 
/*
 *	parse_args()
 *			Extracts runtime variables from argv
 *			
 *			* extracts username to search for in wtmp
 * 				* opens wtmp file after -f flag if spec'd
 *				* opens sys file if none specified using
 *			* returns pointer to username to search for
 *			
 */
{
	char *targetlog;
	char *username;
	const char flag[] = "-f";
	int i = 1;

	/* If */
	if ( argc > 4 ) { // Exit if too many argc
		printf("Error: too many arguments.\n");
		exit(1);
	} else if ( argc > 2 ) { // Find target file if argc =< 4
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], flag) == 0) {
				targetlog = argv[i+1];
				i++; // Extra inc to skip targetlog in argv
			} else {
				username = argv[i];		
			}
		}
	} else { // Assumes system wtmp file is target
		targetlog = WTMP_FILE;
		username = argv[1];
	}

	if ( utmp_open( targetlog ) == -1 ) {
		fprintf(stderr,"%s: cannot open %s\n", *argv, targetlog);
		exit(1);
	}

	return username;
}

/*
 *	show info()
 *			displays the contents of the utmp struct
 *			in human readable form
 *			* displays nothing if record has no user name
 */
void
show_info( struct utmp *utbufp )
{
	void	showtime( time_t , char *);

	if ( utbufp->ut_type != USER_PROCESS )
		return;

	printf("%-8s", utbufp->ut_name);		/* the logname	*/
	printf(" ");					/* a space	*/
	printf("%-12.12s", utbufp->ut_line);		/* the tty	*/
	printf(" ");					/* a space	*/
	showtime( utbufp->ut_time, DATE_FMT );		/* display time	*/
	if ( utbufp->ut_host[0] != '\0' )
		printf(" (%s)", utbufp->ut_host);	/* the host	*/
	printf("\n");					/* newline	*/
}

#define	MAXDATELEN	100

void
showtime( time_t timeval , char *fmt )
/*
 * displays time in a format fit for human consumption.
 * Uses localtime to convert the timeval into a struct of elements
 * (see localtime(3)) and uses strftime to format the data
 */
{
	char	result[MAXDATELEN];

	struct tm *tp = localtime(&timeval);		/* convert time	*/
	strftime(result, MAXDATELEN, fmt, tp);		/* format it	*/
	fputs(result, stdout);
}


