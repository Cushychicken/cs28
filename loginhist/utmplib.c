/* utmplib.c  - functions to buffer reads from utmp file 
 *
 *      functions are
 *              int utmp_open( filename )   - open file
 *                      returns -1 on error
 *              struct utmp *utmp_next( )   - return pointer to next struct
 *                      returns NULL on eof or read error
 *              int utmp_close()            - close file
 *
 *      reads NRECS per read and then doles them out from the buffer
 *      hist: 2012-02-14 slightly modified error handling (thanks mk)
 */
#include        <stdio.h>
#include        <fcntl.h>
#include        <sys/types.h>
#include        <utmp.h>
#include	<unistd.h>

#define NRECS   64
#define UTSIZE  (sizeof(struct utmp))


static  char    utmpbuf[NRECS * UTSIZE];                /* storage      */
static  int     num_recs;                               /* num stored   */
static  int     cur_rec;                                /* next to go   */
static  int     fd_utmp = -1;                           /* read from    */

static  int  utmp_reload();

int utmp_open( char *filename )
{
        fd_utmp = open( filename, O_RDONLY );           /* open it      */
        cur_rec = num_recs = 0;                         /* no recs yet  */
        return fd_utmp;                                 /* report       */
}

struct utmp *utmp_next()
{
        struct utmp *recp;

        if ( fd_utmp == -1 )                            /* error ?      */
                return NULL;
        if ( cur_rec==num_recs && utmp_reload() <= 0 )  /* any more ?   */
                return NULL;
                                        /* get address of next record   */
        recp = (struct utmp *) &utmpbuf[cur_rec * UTSIZE];
        cur_rec++;
        return recp;
}

struct utmp *utmp_head()
/*
 *	Proivdes pointer to the first utmp struct in buffer
 */
{
	return (struct utmp *) &utmpbuf[0];
}

struct utmp *utmp_tail()
/*
 *	Proivdes pointer to the last utmp struct in buffer
 */
{
	return (struct utmp *) &utmpbuf[(NRECS-1)*UTSIZE];
}

static int utmp_reload()
/*
 *      read next bunch of records into buffer
 *      rets: 0=>EOF, -1=>error, else number of records read
 */
{
        int     amt_read;
	                                                
	amt_read = read(fd_utmp, utmpbuf, NRECS*UTSIZE);   /* read data	*/
	if ( amt_read < 0 )			/* mark errors as EOF   */
		return -1;
                                                
        num_recs = amt_read/UTSIZE;		/* how many did we get?	*/
        cur_rec  = 0;				/* reset pointer	*/
        return num_recs;			/* report results	*/
}

int utmp_close()
{
	int rv = 0;
        if ( fd_utmp != -1 ){                   /* don't close if not   */
                rv = close( fd_utmp );          /* open                 */
		fd_utmp = -1;			/* record as closed	*/
	}
	return rv;
}
