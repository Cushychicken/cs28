#include	<stdio.h>
#include	<sys/types.h>
#include	<utmp.h>
#include	<fcntl.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	"utmplib.h"	/* include interface def for utmplib.c	*/

/*
 *	whodate v1.0		- read /etc/wtmp or supplied wtmp file
 *				- searches file for date supplied 
 *				 	at runtime 
 *				- prints nicely formatted logins of users
 *					on specified date
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
			*utmp_next(),	/* returns pointer to next	*/
			*utmp_tail();	/* returns pointer to last utmp */
	void		show_info( struct utmp * );
	time_t		parse_args(int argc, char **argv);
	time_t		et_start,	/* epoch time of start date */
			et_end;		/* epoch time of end of day */

	et_start = parse_args(ac, av);
	et_end = et_start + 86400;


	while ( ( utbufp = utmp_next() ) != ((struct utmp *) NULL) ) {
		if ( utmp_tail()->ut_time < et_start ) {
			utbufp = utmp_tail();		
		} else if ( (utbufp->ut_time > et_start) &&
		    	    (utbufp->ut_time < et_end) )  { 
			show_info( utbufp );
		}
	}
	utmp_close( );
	
	return 0;
}

time_t parse_args(int argc, char *argv[]) 
/*
 *	parse_args()
 *			Extracts runtime variables from argv
 *			
 *			* opens wtmp file for record examination
 * 				* opens wtmp file after -f flag if spec'd
 *				* opens sys file if none specified using
 *			* returns UL int of epoch time in seconds marking 
 *			  start date to search for
 *			
 */
{
	char *targetlog;
	struct tm t;
	time_t t_etime;	
	const char flag[] = "-f";
	int i = 1;
	void fill_time( struct tm *t_etime, int start, char *av[] );
	void showtime( time_t timeval , char *fmt );

	if ( argc > 6 ) { // Exit if too many argc
		printf("Error: too many arguments.\n");
		exit(1);
	} else if ( argc > 4 ) { // Find target file if argc =< 4
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], flag) == 0) {
				targetlog = argv[i+1];
				i++; // Extra inc to skip targetlog in argv
			} else {
				fill_time( &t, i, argv );
				i += 2;
			}
		}
	} else { // Assumes system wtmp file is target
		targetlog = WTMP_FILE;
		fill_time( &t, 1, argv );
	}

	if ( utmp_open( targetlog ) == -1 ) {
		fprintf(stderr,"%s: cannot open %s\n", *argv, targetlog);
		exit(1);
	}

	t_etime = mktime(&t);
	return t_etime;
}

void fill_time( struct tm *t_etime, int start, char *av[] )
/*
	Fills up a time structure so epoch seconds will compute easily
	
	* t_etime - structure of epoch time
	* start - array position of first value in av[] list
	* *av[] - pointer to array of argument variables
 */
{
	int i = start;
	int month_ind = start + 1;
	int day_ind   = start + 2;
	int end = (start + 3);

        t_etime->tm_sec = 0;
        t_etime->tm_min = 0;
        t_etime->tm_hour = 0;
        t_etime->tm_isdst = -1;

	while ( i < end ) {
		if ( i == start ) {
			// requires yrs since 1900
			t_etime->tm_year = (atoi(av[i])-1900);  
		} else if ( i == month_ind ) {
			// 0 == January
			t_etime->tm_mon = (atoi(av[i]) - 1);
		} else if ( i == day_ind ) {
			t_etime->tm_mday = atoi(av[i]);
		}
		i++;
	}
	return;
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


