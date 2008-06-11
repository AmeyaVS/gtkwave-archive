/* 
 * Copyright (c) Tony Bybell 1999-2008.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "globals.h"
#include <config.h>
#include <math.h>
#include <string.h>
#include "gtk12compat.h"
#include "currenttime.h"
#include "pixmaps.h"
#include "symbol.h"
#include "bsearch.h"
#include "color.h"
#include "strace.h"
#include "debug.h"
#include "translate.h"
#include "ptranslate.h"
#include "pipeio.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif


/*
 * convert trptr+vptr into an ascii string
 */
static char *convert_ascii_2(Trptr t, vptr v)
{
Ulong flags;
int nbits;
unsigned char *bits;
char *os, *pnt, *newbuff;
int i, j, len;
char xtab[AN_COUNT];

const char xfwd[AN_COUNT]= AN_NORMAL  ;
const char xrev[AN_COUNT]= AN_INVERSE ;

flags=t->flags;
nbits=t->n.vec->nbits;
bits=v->v;

if(flags&TR_INVERT)
	{
	memcpy(xtab,xrev,AN_COUNT);
	}
	else
	{
	memcpy(xtab,xfwd,AN_COUNT);
	}

if(flags&(TR_ZEROFILL|TR_ONEFILL))
	{
	char whichfill = (flags&TR_ZEROFILL) ? AN_0 : AN_1;
	int msi = 0, lsi = 0, ok = 0;
	if((t->name)&&(nbits > 1))
		{
		char *lbrack = strrchr(t->name, '[');
		if(lbrack)
			{
			int rc = sscanf(lbrack+1, "%d:%d", &msi, &lsi);
			if(rc == 2)
				{
				if(((msi - lsi + 1) == nbits) || ((lsi - msi + 1) == nbits))
					{
					ok = 1;	/* to ensure sanity... */
					}
				}
			}
		}

	if(ok)
		{
		if(msi > lsi)
			{
			if(lsi > 0)
				{
				pnt=wave_alloca(msi + 1);
	
				memcpy(pnt, bits, nbits);
	
	        		for(i=nbits;i<msi+1;i++)
	                		{
	                		pnt[i]=whichfill;
	                		}
	
				bits = (unsigned char *)pnt;
				nbits = msi + 1;
				}
			}
			else
			{
			if(msi > 0)
				{
				pnt=wave_alloca(lsi + 1);
				
	        		for(i=0;i<msi;i++)
	                		{
	                		pnt[i]=whichfill;
	                		}
	
				memcpy(pnt+i, bits, nbits);
	
				bits = (unsigned char *)pnt;
				nbits = lsi + 1;
				}
			}
		}
	}


newbuff=(char *)malloc_2(nbits+6); /* for justify */
if(flags&TR_REVERSE)
	{
	char *fwdpnt, *revpnt;

	fwdpnt=(char *)bits;
	revpnt=newbuff+nbits+6;
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(--revpnt)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	}
	else
	{
	char *fwdpnt, *fwdpnt2;

	fwdpnt=(char *)bits;
	fwdpnt2=newbuff;
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(fwdpnt2++)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	}


if(flags&TR_ASCII) 
	{
	char *parse;	
	int found=0;

	len=(nbits/8)+2+2;		/* $xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='"'; }

	parse=(flags&TR_RJUSTIFY)?(newbuff+((nbits+3)&3)):(newbuff+3);

	for(i=0;i<nbits;i+=8)
		{
		unsigned long val;

		val=0;
		for(j=0;j<8;j++)
			{
			val<<=1;

			if((parse[j]==AN_X)||(parse[j]==AN_Z)||(parse[j]==AN_W)||(parse[j]==AN_U)||(parse[j]==AN_DASH)) { val=1000; /* arbitrarily large */}
			if((parse[j]==AN_1)||(parse[j]==AN_H)) { val|=1; }
			}
		

		if (val) {
			if (val > 0x7f || !isprint(val)) *pnt++ = '.'; else *pnt++ = val;
			found=1;
		}
		
		parse+=8;
		}
	if (!found && !GLOBALS->show_base) {
		*(pnt++)='"';
		*(pnt++)='"';
	}
		
	if(GLOBALS->show_base) { *(pnt++)='"'; }
	*(pnt++)=0x00;
	}
else if((flags&TR_HEX)||((flags&(TR_DEC|TR_SIGNED))&&(nbits>64)))
	{
	char *parse;

	len=(nbits/4)+2+1;		/* $xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='$'; }

	parse=(flags&TR_RJUSTIFY)?(newbuff+((nbits+3)&3)):(newbuff+3);

	for(i=0;i<nbits;i+=4)
		{
		unsigned char val;

		val=0;
		for(j=0;j<4;j++)
			{
			val<<=1;

			if((parse[j]==AN_1)||(parse[j]==AN_H)) 
				{ 
				val|=1; 
				}
			else
			if((parse[j]==AN_0)||(parse[j]==AN_L)) 
				{
				}
			else
			if(parse[j]==AN_X)
				{ 
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
				for(k=j+1;k<4;k++)
					{
					if(parse[k]!=AN_X) 
						{
						char *thisbyt = parse + i + k;
						char *lastbyt = newbuff + 3 + nbits - 1;
						if((lastbyt - thisbyt) >= 0) match = 0; 
						break;
						}
					}
				val = (match) ? 16 : 21; break; 
				}
			else
			if(parse[j]==AN_Z)    
				{ 
				int xover = 0;
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
				for(k=j+1;k<4;k++)
					{
					if(parse[k]!=AN_Z) 
						{
						if(parse[k]==AN_X) 
							{
							xover = 1;
							}
							else
							{
							char *thisbyt = parse + i + k;
							char *lastbyt = newbuff + 3 + nbits - 1;
							if((lastbyt - thisbyt) >= 0) match = 0; 
							}
						break;
						}
					}

				if(xover) val = 21;
				else val = (match) ? 17 : 22; 
				break;
				}
			else
			if(parse[j]==AN_W)    
				{ 
				int xover = 0;
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
				for(k=j+1;k<4;k++)
					{
					if(parse[k]!=AN_W) 
						{
						if(parse[k]==AN_X) 
							{
							xover = 1;
							}
							else
							{
							char *thisbyt = parse + i + k;
							char *lastbyt = newbuff + 3 + nbits - 1;
							if((lastbyt - thisbyt) >= 0) match = 0; 
							}
						break;
						}
					}

				if(xover) val = 21;
				else val = (match) ? 18 : 23; 
				break;
				}
			else
			if(parse[j]==AN_U)
				{ 
				int xover = 0;
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
				for(k=j+1;k<4;k++)
					{
					if(parse[k]!=AN_U) 
						{
						if(parse[k]==AN_X) 
							{
							xover = 1;
							}
							else
							{
							char *thisbyt = parse + i + k;
							char *lastbyt = newbuff + 3 + nbits - 1;
							if((lastbyt - thisbyt) >= 0) match = 0; 
							}
						break;
						}
					}

				if(xover) val = 21;
				else val = (match) ? 19 : 24; 
				break;
				}
			else
			if(parse[j]==AN_DASH)
				{ 
				int xover = 0;
				int k;
				for(k=j+1;k<4;k++)
					{
					if(parse[k]!=AN_DASH) 
						{
						if(parse[k]==AN_X) 
							{
							xover = 1;
							}
						break;
						}
					}

				if(xover) val = 21;
				else val = 20;
				break;
				}
			}

		*(pnt++)="0123456789ABCDEFXZWU-xzwu"[val];
		
		parse+=4;
		}

	*(pnt++)=0x00;
	}
else if(flags&TR_OCT)
	{
	char *parse;

	len=(nbits/3)+2+1;		/* #xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='#'; }

	parse=(flags&TR_RJUSTIFY)
		?(newbuff+((nbits%3)?(nbits%3):3))
		:(newbuff+3);

	for(i=0;i<nbits;i+=3)
		{
		unsigned char val;

		val=0;
		for(j=0;j<3;j++)
			{
			val<<=1;

			if(parse[j]==AN_X) { val=8; break; }
			if(parse[j]==AN_Z) { val=9; break; }
			if(parse[j]==AN_W) { val=10; break; }
			if(parse[j]==AN_U) { val=11; break; }
			if(parse[j]==AN_DASH) { val=12; break; }

			if((parse[j]==AN_1)||(parse[j]==AN_H)) { val|=1; }
			}

		*(pnt++)="01234567XZWU-"[val];
		
		parse+=3;
		}

	*(pnt++)=0x00;
	}
else if(flags&TR_BIN)
	{
	char *parse;

	len=(nbits/1)+2+1;		/* %xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='%'; }

	parse=newbuff+3;

	for(i=0;i<nbits;i++)
		{
		*(pnt++)=AN_STR[(int)(*(parse++))];
		}

	*(pnt++)=0x00;
	}
else if(flags&TR_SIGNED)
	{
	char *parse;
	TimeType val = 0;
	unsigned char fail=0;

	len=21;	/* len+1 of 0x8000000000000000 expressed in decimal */
	os=(char *)calloc_2(1,len);

	parse=newbuff+3;

	if((parse[0]==AN_1)||(parse[0]==AN_H))
		{ val = LLDescriptor(-1); }
	else
	if((parse[0]==AN_0)||(parse[0]==AN_L))
		{ val = LLDescriptor(0); }
	else
		{ fail = 1; }
		
	if(!fail)
	for(i=1;i<nbits;i++)
		{
		val<<=1;
		
		if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
		else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}

	if(!fail)
		{
		sprintf(os, TTFormat, val);
		}
		else
		{
		strcpy(os, "XXX");
		}
	}
else if(flags&TR_REAL)
	{
	char *parse;

	if(nbits==64)
		{
		UTimeType utt = LLDescriptor(0);
		double d;

		parse=newbuff+3;

		for(i=0;i<nbits;i++)
			{
			char ch = AN_STR[(int)(*(parse++))];
			if ((ch=='0')||(ch=='1'))
				{
				utt <<= 1;
				if(ch=='1')
					{
					utt |= LLDescriptor(1);
					}
				}
				else
				{
				goto rl_go_binary;
				}
			}

		memcpy(&d, &utt, sizeof(double));
		os=pnt=(char *)calloc_2(1,32);		
		sprintf(os, "%.16g", d);
		}
		else
		{
rl_go_binary:	len=(nbits/1)+2+1;		/* %xxxxx */
		os=pnt=(char *)calloc_2(1,len);
		if(GLOBALS->show_base) { *(pnt++)='%'; }
	
		parse=newbuff+3;
	
		for(i=0;i<nbits;i++)
			{
			*(pnt++)=AN_STR[(int)(*(parse++))];
			}
	
		*(pnt++)=0x00;
		}
	}
else	/* decimal when all else fails */
	{
	char *parse;
	UTimeType val=0;
	unsigned char fail=0;

	len=21;	/* len+1 of 0xffffffffffffffff expressed in decimal */
	os=(char *)calloc_2(1,len);

	parse=newbuff+3;

	for(i=0;i<nbits;i++)
		{
		val<<=1;

		if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
		else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}

	if(!fail)
		{
		sprintf(os, UTTFormat, val);
		}
		else
		{
		strcpy(os, "XXX");
		}
	}

free_2(newbuff);
return(os);
}


/*
 * convert trptr+hptr vectorstring into an ascii string
 */
char *convert_ascii_real(double *d)
{
char *rv;

rv=malloc_2(24);	/* enough for .16e format */

if(d)
	{
	sprintf(rv,"%.16g",*d);	
	}
else
	{
	strcpy(rv,"UNDEF");
	}

return(rv);
}

char *convert_ascii_string(char *s)
{
char *rv;

if(s)
	{
	rv=(char *)malloc_2(strlen(s)+1);
	strcpy(rv, s);
	}
	else
	{
	rv=(char *)malloc_2(6);
	strcpy(rv, "UNDEF");
	}
return(rv);
}

int vtype(Trptr t, char *vec)
{
	int i, nbits, res;

	if (vec == NULL)
		return(AN_X);
	nbits=t->n.nd->ext->msi-t->n.nd->ext->lsi;
	if(nbits<0)nbits=-nbits;
	nbits++;
	res = AN_1;
	for (i = 0; i < nbits; i++)
	switch (*vec) {
	case AN_X:		
	case 'x':
	case 'X':
			return(AN_X);
	case AN_Z:		
	case 'z':
	case 'Z':
			if (res == AN_0) return(AN_X); vec++; res = AN_Z; break;
	default:	if (res == AN_Z) return(AN_X); vec++; res = AN_0; break;
	}

	return(res);
}

int vtype2(Trptr t, vptr v)
{
	int i, nbits, res;
	char *vec=(char *)v->v;

	if (vec == NULL)
		return(1);
	nbits=t->n.vec->nbits;
	res = 3;
	for (i = 0; i < nbits; i++)
		{
		switch (*vec) 
			{
			case AN_X:		
			case 'x':
			case 'X':
					return(AN_X);
			case AN_Z:		
			case 'z':
			case 'Z':
					if (res == AN_0) return(AN_X); vec++; res = AN_Z; break;
			default:	if (res == AN_Z) return(AN_X); vec++; res = AN_0; break;
			}
		}

	return(res);
}

/*
 * convert trptr+hptr vectorstring into an ascii string
 */
static char *convert_ascii_vec_2(Trptr t, char *vec)
{
Ulong flags;
int nbits;
char *bits;
char *os, *pnt, *newbuff;
int i, j, len;
char xtab[AN_COUNT];

const char xfwd[AN_COUNT]= AN_NORMAL  ;
const char xrev[AN_COUNT]= AN_INVERSE ;

flags=t->flags;

nbits=t->n.nd->ext->msi-t->n.nd->ext->lsi;
if(nbits<0)nbits=-nbits;
nbits++;

if(vec)
        {  
        bits=vec;
        if(*vec>AN_MSK)              /* convert as needed */
        for(i=0;i<nbits;i++)
                {
                switch(*(vec))
                        {
                        case '0': *vec++=AN_0; break;
                        case '1': *vec++=AN_1; break;
                        case 'X':
                        case 'x': *vec++=AN_X; break;
                        case 'Z':
                        case 'z': *vec++=AN_Z; break;
                        case 'H':
                        case 'h': *vec++=AN_H; break;
                        case 'U':
                        case 'u': *vec++=AN_U; break;
                        case 'W':
                        case 'w': *vec++=AN_W; break;
                        case 'L':
                        case 'l': *vec++=AN_L; break;
                        default:  *vec++=AN_DASH; break;
                        }
                }
        }
        else
        {
        pnt=bits=wave_alloca(nbits);
        for(i=0;i<nbits;i++)
                {
                *pnt++=AN_X;
                }
        }

if((flags&(TR_ZEROFILL|TR_ONEFILL))&&(nbits>1)&&(t->n.nd->ext->msi)&&(t->n.nd->ext->lsi))
	{
	char whichfill = (flags&TR_ZEROFILL) ? AN_0 : AN_1;

	if(t->n.nd->ext->msi > t->n.nd->ext->lsi)
		{
		if(t->n.nd->ext->lsi > 0)
			{
			pnt=wave_alloca(t->n.nd->ext->msi + 1);

			memcpy(pnt, bits, nbits);

        		for(i=nbits;i<t->n.nd->ext->msi+1;i++)
                		{
                		pnt[i]=whichfill;
                		}

			bits = pnt;
			nbits = t->n.nd->ext->msi + 1;
			}
		}
		else
		{
		if(t->n.nd->ext->msi > 0)
			{
			pnt=wave_alloca(t->n.nd->ext->lsi + 1);
			
        		for(i=0;i<t->n.nd->ext->msi;i++)
                		{
                		pnt[i]=whichfill;
                		}

			memcpy(pnt+i, bits, nbits);

			bits = pnt;
			nbits = t->n.nd->ext->lsi + 1;
			}
		}
	}

if(flags&TR_INVERT)
	{
	memcpy(xtab,xrev,AN_COUNT);
	}
	else
	{
	memcpy(xtab,xfwd,AN_COUNT);
	}

newbuff=(char *)malloc_2(nbits+6); /* for justify */
if(flags&TR_REVERSE)
	{
	char *fwdpnt, *revpnt;

	fwdpnt=bits;
	revpnt=newbuff+nbits+6;
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(--revpnt)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	}
	else
	{
	char *fwdpnt, *fwdpnt2;

	fwdpnt=bits;
	fwdpnt2=newbuff;
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(fwdpnt2++)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	}

if(flags&TR_ASCII) 
	{
	char *parse;	
	int found=0;

	len=(nbits/8)+2+2;		/* $xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='"'; }

	parse=(flags&TR_RJUSTIFY)?(newbuff+((nbits+3)&3)):(newbuff+3);

	for(i=0;i<nbits;i+=8)
		{
		unsigned long val;

		val=0;
		for(j=0;j<8;j++)
			{
			val<<=1;

			if((parse[j]==AN_X)||(parse[j]==AN_Z)||(parse[j]==AN_W)||(parse[j]==AN_U)||(parse[j]==AN_DASH)) { val=1000; /* arbitrarily large */}
			if((parse[j]==AN_1)||(parse[j]==AN_H)) { val|=1; }
			}

		if (val) {
			if (val > 0x7f || !isprint(val)) *pnt++ = '.'; else *pnt++ = val;
			found=1;
		}
		
		parse+=8;
		}
	if (!found && !GLOBALS->show_base) {
		*(pnt++)='"';
		*(pnt++)='"';
	}
		
	if(GLOBALS->show_base) { *(pnt++)='"'; }
	*(pnt++)=0x00;
	}
else if((flags&TR_HEX)||((flags&(TR_DEC|TR_SIGNED))&&(nbits>64)))
	{
	char *parse;

	len=(nbits/4)+2+1;		/* $xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='$'; }

	parse=(flags&TR_RJUSTIFY)?(newbuff+((nbits+3)&3)):(newbuff+3);

	for(i=0;i<nbits;i+=4)
		{
		unsigned char val;

		val=0;
		for(j=0;j<4;j++)
			{
			val<<=1;

			if((parse[j]==AN_1)||(parse[j]==AN_H)) 
				{ 
				val|=1; 
				}
			else
			if((parse[j]==AN_0)||(parse[j]==AN_L)) 
				{
				}
			else
			if(parse[j]==AN_X)
				{ 
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
				for(k=j+1;k<4;k++)
					{
                                        if(parse[k]!=AN_X)
                                                {
                                                char *thisbyt = parse + i + k;
                                                char *lastbyt = newbuff + 3 + nbits - 1;
                                                if((lastbyt - thisbyt) >= 0) match = 0;
                                                break;
                                                }
					}
				val = (match) ? 16 : 21; break; 
				}
			else
			if(parse[j]==AN_Z)    
				{ 
				int xover = 0;
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
                                for(k=j+1;k<4;k++)
                                        {
                                        if(parse[k]!=AN_Z)
                                                {
                                                if(parse[k]==AN_X)
                                                        {
                                                        xover = 1;
                                                        }
                                                        else
                                                        {
                                                        char *thisbyt = parse + i + k;
                                                        char *lastbyt = newbuff + 3 + nbits - 1;
                                                        if((lastbyt - thisbyt) >= 0) match = 0;
                                                        }
                                                break;
                                                }
                                        }

				if(xover) val = 21;
				else val = (match) ? 17 : 22; 
				break;
				}
			else
			if(parse[j]==AN_W)    
				{ 
				int xover = 0;
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
                                for(k=j+1;k<4;k++)
                                        {
                                        if(parse[k]!=AN_W)
                                                {
                                                if(parse[k]==AN_X)
                                                        {
                                                        xover = 1;
                                                        }
                                                        else
                                                        {
                                                        char *thisbyt = parse + i + k;   
                                                        char *lastbyt = newbuff + 3 + nbits - 1;
                                                        if((lastbyt - thisbyt) >= 0) match = 0;
                                                        }
                                                break;
                                                }
                                        }

				if(xover) val = 21;
				else val = (match) ? 18 : 23; 
				break;
				}
			else
			if(parse[j]==AN_U)
				{ 
				int xover = 0;
				int match = (j==0) || ((parse + i + j) == (newbuff + 3));
				int k;
                                for(k=j+1;k<4;k++)
                                        {
                                        if(parse[k]!=AN_U)
                                                {
                                                if(parse[k]==AN_X)
                                                        {
                                                        xover = 1;
                                                        }
                                                        else
                                                        {
                                                        char *thisbyt = parse + i + k;   
                                                        char *lastbyt = newbuff + 3 + nbits - 1;
                                                        if((lastbyt - thisbyt) >= 0) match = 0;
                                                        }
                                                break;
                                                }
                                        }

				if(xover) val = 21;
				else val = (match) ? 19 : 24; 
				break;
				}
			else
			if(parse[j]==AN_DASH)
				{ 
				int xover = 0;
				int k;
                                for(k=j+1;k<4;k++)
                                        {
                                        if(parse[k]!=AN_DASH)
                                                {  
                                                if(parse[k]==AN_X)
                                                        {
                                                        xover = 1;
                                                        }
                                                break;
                                                }
                                        }

				if(xover) val = 21;
				else val = 20;
				break;
				}
			}

		*(pnt++)="0123456789ABCDEFXZWU-xzwu"[val];
		
		parse+=4;
		}

	*(pnt++)=0x00;
	}
else if(flags&TR_OCT)
	{
	char *parse;

	len=(nbits/3)+2+1;		/* #xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='#'; }

	parse=(flags&TR_RJUSTIFY)
		?(newbuff+((nbits%3)?(nbits%3):3))
		:(newbuff+3);

	for(i=0;i<nbits;i+=3)
		{
		unsigned char val;

		val=0;
		for(j=0;j<3;j++)
			{
			val<<=1;

			if(parse[j]==AN_X) { val=8; break; }
			if(parse[j]==AN_Z) { val=9; break; }
			if(parse[j]==AN_W) { val=10; break; }
			if(parse[j]==AN_U) { val=11; break; }
			if(parse[j]==AN_DASH) { val=12; break; }

			if((parse[j]==AN_1)||(parse[j]==AN_H)) { val|=1; }
			}

		*(pnt++)="01234567XZWU-"[val];
		parse+=3;
		}

	*(pnt++)=0x00;
	}
else if(flags&TR_BIN)
	{
	char *parse;

	len=(nbits/1)+2+1;		/* %xxxxx */
	os=pnt=(char *)calloc_2(1,len);
	if(GLOBALS->show_base) { *(pnt++)='%'; }

	parse=newbuff+3;

	for(i=0;i<nbits;i++)
		{
		*(pnt++)=AN_STR[(int)(*(parse++))];
		}

	*(pnt++)=0x00;
	}
else if(flags&TR_SIGNED)
	{
	char *parse;
	TimeType val = 0;
	unsigned char fail=0;

	len=21;	/* len+1 of 0x8000000000000000 expressed in decimal */
	os=(char *)calloc_2(1,len);

	parse=newbuff+3;

        if((parse[0]==AN_1)||(parse[0]==AN_H))
                { val = LLDescriptor(-1); }
        else     
        if((parse[0]==AN_0)||(parse[0]==AN_L))
                { val = LLDescriptor(0); }
        else     
                { fail = 1; }
                 
        if(!fail)
	for(i=1;i<nbits;i++)
		{
		val<<=1;

                if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
                else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}

	if(!fail)
		{
		sprintf(os, TTFormat, val);
		}
		else
		{
		strcpy(os, "XXX");
		}
	}
else if(flags&TR_REAL)
	{
	char *parse;

	if(nbits==64)
		{
		UTimeType utt = LLDescriptor(0);
		double d;

		parse=newbuff+3;

		for(i=0;i<nbits;i++)
			{
			char ch = AN_STR[(int)(*(parse++))];
			if ((ch=='0')||(ch=='1'))
				{
				utt <<= 1;
				if(ch=='1')
					{
					utt |= LLDescriptor(1);
					}
				}
				else
				{
				goto rl_go_binary;
				}
			}

		memcpy(&d, &utt, sizeof(double));
		os=pnt=(char *)calloc_2(1,32);		
		sprintf(os, "%.16g", d);
		}
		else
		{
rl_go_binary:	len=(nbits/1)+2+1;		/* %xxxxx */
		os=pnt=(char *)calloc_2(1,len);
		if(GLOBALS->show_base) { *(pnt++)='%'; }
	
		parse=newbuff+3;
	
		for(i=0;i<nbits;i++)
			{
			*(pnt++)=AN_STR[(int)(*(parse++))];
			}
	
		*(pnt++)=0x00;
		}
	}
else	/* decimal when all else fails */
	{
	char *parse;
	UTimeType val=0;
	unsigned char fail=0;

	len=21; /* len+1 of 0xffffffffffffffff expressed in decimal */
	os=(char *)calloc_2(1,len);

	parse=newbuff+3;

	for(i=0;i<nbits;i++)
		{
		val<<=1;

                if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
                else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}

	if(!fail)
		{
		sprintf(os, UTTFormat, val);
		}
		else
		{
		strcpy(os, "XXX");
		}
	}

free_2(newbuff);
return(os);
}


static char *dofilter(Trptr t, char *s)
{
GLOBALS->xl_file_filter[t->f_filter] = xl_splay(s, GLOBALS->xl_file_filter[t->f_filter]);
	
if(!strcasecmp(s, GLOBALS->xl_file_filter[t->f_filter]->item))
	{
	free_2(s);
	s = malloc_2(strlen(GLOBALS->xl_file_filter[t->f_filter]->trans) + 1);
	strcpy(s, GLOBALS->xl_file_filter[t->f_filter]->trans);
	}

if((*s == '?') && (!GLOBALS->color_active_in_filter))
	{
	char *s2a;
	char *s2 = strchr(s+1, '?');
	if(s2)
		{
		s2++;
		s2a = malloc_2(strlen(s2)+1);
		strcpy(s2a, s2);
		free_2(s);
		s = s2a;
		}
	}

return(s);
}

#if !defined _MSC_VER && !defined __MINGW32__
static char *pdofilter(Trptr t, char *s)
{
struct pipe_ctx *p = GLOBALS->proc_filter[t->p_filter];
char buf[1025];
int n;

if(p)
	{
	fputs(s, p->sout);
	fputc('\n', p->sout);
	fflush(p->sout);

	buf[0] = 0;

	n = fgets(buf, 1024, p->sin) ? strlen(buf) : 0;
	buf[n] = 0;

	if(n)
		{
		if(buf[n-1] == '\n') { buf[n-1] = 0; n--; }
		}

	if(buf[0])
		{
		free_2(s);
		s = malloc_2(n + 1);
		strcpy(s, buf);
		}
	}	

if((*s == '?') && (!GLOBALS->color_active_in_filter))
	{
	char *s2a;
	char *s2 = strchr(s+1, '?');
	if(s2)
		{
		s2++;
		s2a = malloc_2(strlen(s2)+1);
		strcpy(s2a, s2);
		free_2(s);
		s = s2a;
		}
	}
return(s);
}
#endif


char *convert_ascii_vec(Trptr t, char *vec)
{
char *s = convert_ascii_vec_2(t, vec);

#if defined _MSC_VER || defined __MINGW32__

if(!t->f_filter)
	{
	}
	else
	{
	s = dofilter(t, s);
	}

#else

if(!(t->f_filter|t->p_filter))
	{
	}
	else
	{
	if(t->f_filter)
		{
		s = dofilter(t, s);
		}
		else
		{
		s = pdofilter(t, s);
		}
	}
#endif

return(s);
}

char *convert_ascii(Trptr t, vptr v)
{
char *s = convert_ascii_2(t, v);

#if defined _MSC_VER || defined __MINGW32__

if(!t->f_filter)
	{
	}
	else
	{
	s = dofilter(t, s);
	}

#else

if(!(t->f_filter|t->p_filter))
	{
	}
	else
	{
	if(t->f_filter)
		{
		s = dofilter(t, s);
		}
		else
		{
		s = pdofilter(t, s);
		}
	}
#endif

return(s);
}


/*
 * convert trptr+hptr vectorstring into a real
 */
double convert_real_vec(Trptr t, char *vec)
{
Ulong flags;
int nbits;
char *bits;
char *pnt, *newbuff;
int i;
char xtab[AN_COUNT];
double mynan = strtod("NaN", NULL);
double retval = mynan;

const char xfwd[AN_COUNT]= AN_NORMAL  ;
const char xrev[AN_COUNT]= AN_INVERSE ;

flags=t->flags;

nbits=t->n.nd->ext->msi-t->n.nd->ext->lsi;
if(nbits<0)nbits=-nbits;
nbits++;

if(vec)
        {  
        bits=vec;
        if(*vec>AN_MSK)              /* convert as needed */
        for(i=0;i<nbits;i++)
                {
                switch(*(vec))
                        {
                        case '0': *vec++=AN_0; break;
                        case '1': *vec++=AN_1; break;
                        case 'X':
                        case 'x': *vec++=AN_X; break;
                        case 'Z':
                        case 'z': *vec++=AN_Z; break;
                        case 'H':
                        case 'h': *vec++=AN_H; break;
                        case 'U':
                        case 'u': *vec++=AN_U; break;
                        case 'W':
                        case 'w': *vec++=AN_W; break;
                        case 'L':
                        case 'l': *vec++=AN_L; break;
                        default:  *vec++=AN_DASH; break;
                        }
                }
        }
        else
        {
        pnt=bits=wave_alloca(nbits);
        for(i=0;i<nbits;i++)
                {
                *pnt++=AN_X;
                }
        }


if(flags&TR_INVERT)
	{
	memcpy(xtab,xrev,AN_COUNT);
	}
	else
	{
	memcpy(xtab,xfwd,AN_COUNT);
	}

newbuff=(char *)malloc_2(nbits+6); /* for justify */
if(flags&TR_REVERSE)
	{
	char *fwdpnt, *revpnt;

	fwdpnt=bits;
	revpnt=newbuff+nbits+6;
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(--revpnt)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	}
	else
	{
	char *fwdpnt, *fwdpnt2;

	fwdpnt=bits;
	fwdpnt2=newbuff;
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(fwdpnt2++)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	}

if(flags&TR_SIGNED)
	{
	char *parse;
	TimeType val = 0;
	unsigned char fail=0;

	parse=newbuff+3;

        if((parse[0]==AN_1)||(parse[0]==AN_H))
                { val = LLDescriptor(-1); }
        else     
        if((parse[0]==AN_0)||(parse[0]==AN_L))
                { val = LLDescriptor(0); }
        else     
                { fail = 1; }
                 
        if(!fail)
	for(i=1;i<nbits;i++)
		{
		val<<=1;

                if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
                else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}
	if(!fail)
		{
		retval = val;
		}
	}
else	/* decimal when all else fails */
	{
	char *parse;
	UTimeType val=0;
	unsigned char fail=0;

	parse=newbuff+3;

	for(i=0;i<nbits;i++)
		{
		val<<=1;

                if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
                else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}
	if(!fail)
		{
		retval = val;
		}
	}

free_2(newbuff);
return(retval);
}



/*
 * convert trptr+vptr into a real
 */
double convert_real(Trptr t, vptr v)
{
Ulong flags;
int nbits;
unsigned char *bits;
char *newbuff;
int i;
char xtab[AN_COUNT];
double mynan = strtod("NaN", NULL);
double retval = mynan;

const char xfwd[AN_COUNT]= AN_NORMAL  ;
const char xrev[AN_COUNT]= AN_INVERSE ;

flags=t->flags;
nbits=t->n.vec->nbits;
bits=v->v;

if(flags&TR_INVERT)
	{
	memcpy(xtab,xrev,AN_COUNT);
	}
	else
	{
	memcpy(xtab,xfwd,AN_COUNT);
	}

newbuff=(char *)malloc_2(nbits+6); /* for justify */
if(flags&TR_REVERSE)
	{
	char *fwdpnt, *revpnt;

	fwdpnt=(char *)bits;
	revpnt=newbuff+nbits+6;
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(--revpnt)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(--revpnt)=xtab[0];
	}
	else
	{
	char *fwdpnt, *fwdpnt2;

	fwdpnt=(char *)bits;
	fwdpnt2=newbuff;
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	for(i=0;i<nbits;i++)
		{
		*(fwdpnt2++)=xtab[(int)(*(fwdpnt++))];
		}
	for(i=0;i<3;i++) *(fwdpnt2++)=xtab[0];
	}


if(flags&TR_SIGNED)
	{
	char *parse;
	TimeType val = 0;
	unsigned char fail=0;

	parse=newbuff+3;

	if((parse[0]==AN_1)||(parse[0]==AN_H))
		{ val = LLDescriptor(-1); }
	else
	if((parse[0]==AN_0)||(parse[0]==AN_L))
		{ val = LLDescriptor(0); }
	else
		{ fail = 1; }
		
	if(!fail)
	for(i=1;i<nbits;i++)
		{
		val<<=1;
		
		if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
		else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}

	if(!fail)
		{
		retval = val;
		}
	}
else	/* decimal when all else fails */
	{
	char *parse;
	UTimeType val=0;
	unsigned char fail=0;

	parse=newbuff+3;

	for(i=0;i<nbits;i++)
		{
		val<<=1;

		if((parse[i]==AN_1)||(parse[i]==AN_H)) {  val|=LLDescriptor(1); }
		else if((parse[i]!=AN_0)&&(parse[i]!=AN_L)) { fail=1; break; }
		}

	if(!fail)
		{
		retval = val;
		}
	}

free_2(newbuff);
return(retval);
}

/*
 * $Id$
 * $Log$
 * Revision 1.7  2008/03/25 03:22:06  gtkwave
 * expanded zero fill to include also a one fill (for pre-inverted nets)
 *
 * Revision 1.6  2008/03/24 19:33:59  gtkwave
 * added zero range fill feature
 *
 * Revision 1.5  2008/02/24 00:26:26  gtkwave
 * added improved x vs X handling for hptrs (was only vptrs in prev patch)
 *
 * Revision 1.4  2008/02/21 03:53:39  gtkwave
 * improved x vs X (z vs Z, etc) handling
 *
 * Revision 1.3  2007/08/26 21:35:39  gtkwave
 * integrated global context management from SystemOfCode2007 branch
 *
 * Revision 1.1.1.1.2.5  2007/08/25 19:43:45  gtkwave
 * header cleanups
 *
 * Revision 1.1.1.1.2.4  2007/08/07 03:18:54  kermin
 * Changed to pointer based GLOBAL structure and added initialization function
 *
 * Revision 1.1.1.1.2.3  2007/08/06 03:50:45  gtkwave
 * globals support for ae2, gtk1, cygwin, mingw.  also cleaned up some machine
 * generated structs, etc.
 *
 * Revision 1.1.1.1.2.2  2007/08/05 02:27:18  kermin
 * Semi working global struct
 *
 * Revision 1.1.1.1.2.1  2007/07/28 19:50:39  kermin
 * Merged in the main line
 *
 * Revision 1.2  2007/07/23 23:13:08  gtkwave
 * adds for color tags in filtered trace data
 *
 * Revision 1.1.1.1  2007/05/30 04:27:50  gtkwave
 * Imported sources
 *
 * Revision 1.2  2007/04/20 02:08:11  gtkwave
 * initial release
 *
 */

