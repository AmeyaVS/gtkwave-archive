/*
 * Copyright (c) Tony Bybell 2005.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_PIPEIO_H
#define WAVE_PIPEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "debug.h"

struct pipe_ctx
{
FILE *sin, *sout;
int fd0, fd1;
pid_t pid;
};


struct pipe_ctx *pipeio_create(char *execappname);
void pipeio_destroy(struct pipe_ctx *p);
        
#endif

/*
 * $Id$
 * $Log$
 */

