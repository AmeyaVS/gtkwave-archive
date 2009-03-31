/*
 * Copyright (c) 2009 Tony Bybell.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "LzmaLib.h"
#include <Alloc.h>
#include <stdio.h>
#include <stdlib.h>

static void *SzAlloc(void *p, size_t size) { p = p; return MyAlloc(size); }
static void SzFree(void *p, void *address) { p = p; MyFree(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

#define LZMA_BLOCK_LEN (4*1024*1024)

enum lzma_state_t { LZMA_STATE_WRITE, LZMA_STATE_READ_ERROR, 
			LZMA_STATE_READ_INIT, LZMA_STATE_READ_GETBLOCK, LZMA_STATE_READ_GETBYTES };

struct lzma_handle_t
{
int fd;
unsigned int offs, blklen;
unsigned int depth;
enum lzma_state_t state;

char mem[LZMA_BLOCK_LEN];
char dmem[LZMA_PROPS_SIZE + LZMA_BLOCK_LEN];
};


static void LZMA_write_varint(struct lzma_handle_t *h, size_t v)
{
size_t nxt;
unsigned char buf[16];
unsigned char *pnt = buf;

while((nxt = v>>7))
        {  
        *(pnt++) = (v&0x7f);
        v = nxt;
        }
*(pnt++) = (v&0x7f) | 0x80;

write(h->fd, buf, pnt-buf);
}


static size_t LZMA_read_varint(struct lzma_handle_t *h)
{
unsigned char buf[16];
int idx = 0;
size_t rc = 0;

for(;;)
	{
	read(h->fd, buf+idx, 1);
	if(buf[idx++] & 0x80) break;
	}

do
	{
	idx--;
	rc <<= 7;
	rc |= (buf[idx] & 0x7f);
	}
	while(idx);

return(rc);
}


static size_t LZMA_write_compress(struct lzma_handle_t *h, unsigned char *mem, size_t len)
{
size_t srclen = len;
size_t destlen = LZMA_BLOCK_LEN;
size_t outPropsSize = LZMA_PROPS_SIZE;
int rc;

CLzmaEncProps props;
LzmaEncProps_Init(&props);
props.level = h->depth;
props.dictSize = LZMA_BLOCK_LEN;

rc = LzmaEncode(h->dmem + LZMA_PROPS_SIZE, &destlen,
	mem, len,
        &props, h->dmem, &outPropsSize, 0,
        NULL, &g_Alloc, &g_Alloc);

if(rc == SZ_OK)
	{
	LZMA_write_varint(h, srclen);
	LZMA_write_varint(h, destlen);

	return(write(h->fd, h->dmem, destlen + LZMA_PROPS_SIZE));
	}
	else
	{
	LZMA_write_varint(h, srclen);
	LZMA_write_varint(h, 0);

	return(write(h->fd, mem, len));
	}
}


void *LZMA_fdopen(int fd, const char *mode)
{
static const char z7[] = "z7";
struct lzma_handle_t *p = MyAlloc(sizeof(struct lzma_handle_t));

p->fd = fd;
p->offs = 0;
p->depth = 4;

if(mode[0] == 'w')
	{
	if(mode[1])
		{
		if(isdigit(mode[1]))
			{
			p->depth = mode[1] - '0';
			}
		else if(mode[2])
			{
			if(isdigit(mode[2]))
				{
				p->depth = mode[2] - '0';
				}
			}
		}

	p->state = LZMA_STATE_WRITE;
	write(p->fd, z7, 2);
	return(p);
	}
else
if(mode[0] == 'r')
	{
	p->state = LZMA_STATE_READ_INIT;
	return(p);
	}
else
	{
	close(p->fd);
	MyFree(p);
	return(NULL);
	}
}


void LZMA_close(void *handle)
{
struct lzma_handle_t *h = (struct lzma_handle_t *)handle;
if(h)
	{
	if(h->state == LZMA_STATE_WRITE)
		{
		if((h) && (h->offs))
			{
			LZMA_write_compress(h, h->mem, h->offs);
			h->offs = 0;
			}
		
		LZMA_write_varint(h, 0);
		}
	close(h->fd);
	MyFree(h);
	}
}


size_t LZMA_flush(void *handle)
{
return(0);
}


size_t LZMA_write(void *handle, void *mem, size_t len)
{
struct lzma_handle_t *h = (struct lzma_handle_t *)handle;
size_t rc = 0;

if(h->state == LZMA_STATE_WRITE)
	{
	while((h)&&(len))
		{
		if((h->offs + len) <= LZMA_BLOCK_LEN)
			{
			memcpy(h->mem + h->offs, mem, len);
			h->offs += len;
			break;
			}
			else
			{
			size_t new_len = LZMA_BLOCK_LEN - h->offs;
			if(new_len)
				{
				memcpy(h->mem + h->offs, mem, new_len);
				}
			rc = LZMA_write_compress(h, h->mem, LZMA_BLOCK_LEN);
			h->offs = 0;
			len -= new_len;
			mem = ((char *)mem) + new_len;
			}
		}
	}

return(len);
}


size_t LZMA_read(void *handle, void *mem, size_t len)
{
struct lzma_handle_t *h = (struct lzma_handle_t *)handle;
size_t rc = 0;
char hdr[2] = {0, 0};
size_t srclen, dstlen;

if(h)
	{
	top:
	switch(h->state)
		{
		case LZMA_STATE_READ_INIT:
			read(h->fd, hdr, 2);
			if((hdr[0] == 'z') && (hdr[1] == '7'))
				{
				h->state = LZMA_STATE_READ_GETBLOCK;
				}
				else
				{
				h->state = LZMA_STATE_READ_ERROR;
				}
			goto top;
			break;

		case LZMA_STATE_READ_GETBLOCK:
			dstlen = LZMA_read_varint(h);
			if(!dstlen)
				{
				return(0);
				}

			srclen = LZMA_read_varint(h);

			if(!srclen)
				{
				rc = read(h->fd, h->mem, dstlen);
				h->blklen = rc;
				h->offs = 0;
				}
				else
				{
				SizeT inSizePure = srclen; 
				ELzmaStatus status;
				char *src = h->dmem;
				char *dest = h->mem;
				SRes res;

				rc = read(h->fd, h->dmem, srclen + LZMA_PROPS_SIZE);

				res = LzmaDecode(dest, &dstlen, src + LZMA_PROPS_SIZE, &inSizePure,
			      		src, LZMA_PROPS_SIZE, LZMA_FINISH_ANY, &status, &g_Alloc);
	
				if(res == SZ_OK)
					{
					h->blklen = dstlen;
					h->offs = 0;
					}
					else
					{
					h->state = LZMA_STATE_READ_ERROR;
					goto top;
					}
				}

			if(len <= dstlen)
				{
				memcpy(mem, h->mem, len);
				h->offs = len;
				rc = len;
				h->state = LZMA_STATE_READ_GETBYTES;
				}
				else
				{
				memcpy(mem, h->mem, dstlen);
				rc = dstlen + LZMA_read(h, ((char *)mem) + dstlen, len - dstlen);
				}
			break;

		case LZMA_STATE_READ_GETBYTES:
			if((len + h->offs) < h->blklen)
				{
				memcpy(mem, h->mem + h->offs, len);
				h->offs += len;
				rc = len;
				}
			else
			if((len + h->offs) == h->blklen)
				{
				memcpy(mem, h->mem + h->offs, len);
				h->offs = 0;
				rc = len;
				h->state = LZMA_STATE_READ_GETBLOCK;
				}
			else
				{
				size_t cpylen = h->blklen - h->offs;
				memcpy(mem, h->mem + h->offs, cpylen);
				h->state = LZMA_STATE_READ_GETBLOCK;
				rc = cpylen + LZMA_read(h, ((char *)mem) + cpylen, len - cpylen);
				}
			break;

		case LZMA_STATE_READ_ERROR:
			break;
		}
	}

return(rc);
}
