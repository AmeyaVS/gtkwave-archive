/*
 * Copyright (c) Tony Bybell 2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include "pipeio.h"

#if defined _MSC_VER || defined __MINGW32__

struct pipe_ctx *pipeio_create(char *execappname)
{
/* nothing, not supported in win32 */
return(NULL);
}

void pipeio_destroy(struct pipe_ctx *p)
{
/* nothing */
}

#else
#include <sys/wait.h>

struct pipe_ctx *pipeio_create(char *execappname)
{
int rc1, rc2;
pid_t pid, wave_pid;
int filedes_w[2];
int filedes_r[2];
struct pipe_ctx *p;
int mystat;

FILE *fsin=NULL, *fsout = NULL;

rc1 = pipe(filedes_r);
if(rc1) return(NULL);

rc2 = pipe(filedes_w);
if(rc2) { close(filedes_r[0]); close(filedes_r[1]); return(NULL); }

wave_pid = getpid();

if((pid=fork()))
	{
	fsout = fdopen(filedes_w[1], "wb");
	fsin = fdopen(filedes_r[0], "rb");
	close(filedes_w[0]);
	close(filedes_r[1]);
	}
	else
	{
	dup2(filedes_w[0], 0);
	dup2(filedes_r[1], 1);
	
	close(filedes_w[1]);
	close(filedes_r[0]);

#ifdef _AIX
	/* NOTE: doesn't handle ctrl-c or killing, but I don't want to mess with this right now for AIX */
	execl(execappname, execappname, NULL);
	exit(0);
#else
	if((pid=fork()))	/* monitor process */
		{
		do 	{
			sleep(1);
			} while(wave_pid == getppid()); /* inherited by init yet? */

		kill(pid, SIGKILL);
		waitpid(pid, &mystat, 0);

		exit(0);
		}
		else		/* actual helper */
		{
		execl(execappname, execappname, NULL);
		exit(0);
		}
#endif
	}

p = malloc_2(sizeof(struct pipe_ctx));
p->pid = pid;
p->sin = fsin;
p->sout = fsout;
p->fd0 = filedes_r[0]; /* for potential select() ops */
p->fd1 = filedes_w[1]; /* ditto */

return(p);
}


void pipeio_destroy(struct pipe_ctx *p)
{
#ifdef _AIX
int mystat;
kill(p->pid, SIGKILL);
waitpid(p->pid, &mystat, 0);
#endif

fclose(p->sout);
fclose(p->sin);
free_2(p);
}

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/08/26 21:35:43  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.2  2007/08/06 03:50:48  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:21  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:29  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

