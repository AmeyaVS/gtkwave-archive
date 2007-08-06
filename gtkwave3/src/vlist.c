/* 
 * Copyright (c) Tony Bybell 2006.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

/* this code implements generic vlists.  (see the original paper
   from Phil Bagwell in 2002.)  the original idea was to
   clean up histents by using vlist_alloc() to create a growable
   array that doesn't require next pointers per-element, however
   that doesn't seem necessary given the space savings that
   gzipped dormant vlist entries buys you.  
 */

#include <config.h>
#include "globals.h"
#include "vlist.h"
#include <zlib.h>


/* create / destroy
 */
struct vlist_t *vlist_create(unsigned int elem_siz, unsigned int elem_start_cnt)
{
struct vlist_t *v;

if(elem_start_cnt < 2)
	{
	v = calloc_2(1, sizeof(struct vlist_t) + elem_siz);
	v->siz = 1;
	v->elem_siz = elem_siz;
	}
	else /* non-growable, non-indexable list.  elem_start_cnt is constant size */
	{
	v = calloc_2(1, sizeof(struct vlist_t) + (elem_siz * elem_start_cnt));
	v->siz = elem_start_cnt;
	v->elem_siz = elem_siz;
	}

return(v);
}


void vlist_destroy(struct vlist_t *v)
{
struct vlist_t *vt;

while(v)
	{
	vt = v->next;
	free_2(v);
	v = vt;
	}
}


/* realtime compression/decompression of bytewise vlists
 * this can obviously be extended if elem_siz > 1, but
 * the viewer doesn't need that feature
 */
struct vlist_t *vlist_compress_block(struct vlist_t *v)
{
if(v->siz > 32)
	{
	struct vlist_t *vz;
	unsigned int *ipnt;
	char *dmem = malloc_2(v->siz);
	unsigned long destlen = v->siz;
	int rc;
	
	rc = compress2((unsigned char *)dmem, &destlen, (unsigned char *)(v+1), v->siz, GLOBALS.vlist_compression_depth);
	if((rc == Z_OK)&&(destlen < (v->siz - sizeof(long))))
		{
		/* printf("siz: %d, dest: %d rc: %d\n", v->siz, (int)destlen, rc); */

		vz = malloc_2(sizeof(struct vlist_t) + sizeof(int) + destlen);
		memcpy(vz, v, sizeof(struct vlist_t));
	
		ipnt = (unsigned int *)(vz + 1);
		ipnt[0] = destlen;
		memcpy(&ipnt[1], dmem, destlen);
		vz->offs = (unsigned int)(-(int)v->offs); /* neg value signified compression */
		free_2(v);
		v = vz;
		}

	free_2(dmem);
	}

return(v);
}


void vlist_uncompress(struct vlist_t **v)
{
struct vlist_t *vl = *v;
struct vlist_t *vprev = NULL;

while(vl)
	{
	if((int)vl->offs < 0)
		{
		struct vlist_t *vz = malloc_2(sizeof(struct vlist_t) + vl->siz);
		unsigned int *ipnt;
		unsigned long sourcelen, destlen;
		int rc;

		memcpy(vz, vl, sizeof(struct vlist_t));
		vz->offs = (unsigned int)(-(int)vl->offs);

		ipnt = (unsigned int *)(vl + 1);
		sourcelen = (unsigned long)ipnt[0];
		destlen = (unsigned long)vl->siz;

		rc = uncompress((unsigned char *)(vz+1), &destlen, (unsigned char *)&ipnt[1], sourcelen);	
		if(rc != Z_OK)
			{
			fprintf(stderr, "Error in vlist uncompress(), rc=%d/destlen=%d exiting!\n", rc, (int)destlen);
			exit(255);
			}
	
		free_2(vl);
		vl = vz;

		if(vprev) 
			{
			vprev->next = vz;
			}
			else
			{
			*v = vz; 
			}
		}	

	vprev = vl;
	vl = vl->next;
	}

}


/* get pointer to one unit of space
 */
void *vlist_alloc(struct vlist_t **v, int compressable)
{
struct vlist_t *vl = *v;
char *px;

if(vl->offs == vl->siz)
	{
	struct vlist_t *v2;
	unsigned int siz;

	/* 2 times versions are the growable, indexable vlists */
	if(vl->next)
		{
		if(vl->siz == vl->next->siz)
			{
			siz = vl->siz;
			}
			else
			{
			siz = 2 * vl->siz;
			}
		}
		else
		{
		if(vl->siz != 1)
			{
			siz = vl->siz;
			}
			else
			{
			siz = 2 * vl->siz;
			}
		}

	if((compressable)&&(vl->elem_siz == 1))
		{
		if(GLOBALS.vlist_compression_depth>=0)
			{
			vl = vlist_compress_block(vl);
			}
		}

	v2 = calloc_2(1, sizeof(struct vlist_t) + (siz * vl->elem_siz));
	v2->siz = siz;
	v2->elem_siz = vl->elem_siz;
	v2->next = vl;
	*v = v2;
	vl = *v;
	}

px =(((char *)(vl)) + sizeof(struct vlist_t) + ((vl->offs++) * vl->elem_siz));
return((void *)px);
}


/* vlist_size() and vlist_locate() do not work properly on 
   compressed lists...you'll have to call vlist_uncompress() first!
 */
unsigned int vlist_size(struct vlist_t *v)
{
return(v->siz - 1 + v->offs);
}


void *vlist_locate(struct vlist_t *v, unsigned int idx)
{
unsigned int here = v->siz - 1;
unsigned int siz = here + v->offs; /* siz is the same as vlist_size() */

if((!siz)||(idx>=siz)) return(NULL);

while (idx < here)
	{
	v = v->next;
	here = v->siz - 1;
	}

idx -= here;

return((void *)(((char *)(v)) + sizeof(struct vlist_t) + (idx * v->elem_siz)));
}


/* calling this if you don't plan on adding any more elements will free
   up unused space as well as compress final blocks (if enabled)
 */
void vlist_freeze(struct vlist_t **v)
{
struct vlist_t *vl = *v;
int siz = vl->offs;

if((vl->elem_siz == 1)&&(siz))
	{
	struct vlist_t *w = vlist_compress_block(vl);
	*v = w;
	}
else
if(siz != vl->siz)
	{
	struct vlist_t *w = malloc_2(sizeof(struct vlist_t) + (siz * vl->elem_siz));
	memcpy(w, vl, sizeof(struct vlist_t) + (siz * vl->elem_siz));
	free_2(vl);
	*v = w;
	}
}

/*
 * $Id$
 * $Log$
 * Revision 1.1.1.1.2.1  2007/08/05 02:27:28  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1  2007/05/30 04:27:52  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:18  gtkwave
 * initial release
 *
 */

