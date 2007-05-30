/*
 * Copyright (c) Tony Bybell 2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

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
int stat;

FILE *sin=NULL, *sout = NULL;

rc1 = pipe(filedes_r);
if(rc1) return(NULL);

rc2 = pipe(filedes_w);
if(rc2) { close(filedes_r[0]); close(filedes_r[1]); return(NULL); }

wave_pid = getpid();

if((pid=fork()))
	{
	sout = fdopen(filedes_w[1], "wb");
	sin = fdopen(filedes_r[0], "rb");
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
		waitpid(pid, &stat, 0);

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
p->sin = sin;
p->sout = sout;
p->fd0 = filedes_r[0]; /* for potential select() ops */
p->fd1 = filedes_w[1]; /* ditto */

return(p);
}


void pipeio_destroy(struct pipe_ctx *p)
{
#ifdef _AIX
int stat;
kill(p->pid, SIGKILL);
waitpid(p->pid, &stat, 0);
#endif

fclose(p->sout);
fclose(p->sin);
free_2(p);
}

#endif

/*
 * $Id$
 * $Log$
 * Revision 1.2  2007/04/20 02:08:13  gtkwave
 * initial release
 *
 */

