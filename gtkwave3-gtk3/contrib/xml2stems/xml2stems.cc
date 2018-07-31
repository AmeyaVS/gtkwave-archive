/*
 * Copyright (c) 2018 Tony Bybell.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include <string>
#include <map>
#include <stack>
using namespace std;

#define BUF_SIZ 65536


void xml2stems(FILE *fi, FILE *fo)
{
std::map <string, string> fId;
std::stack<string> mId;

while(!feof(fi))
	{
	char buf[BUF_SIZ + 1];
	int endtag;
	char *ln = fgets(buf, BUF_SIZ, fi);	
	char *pnt = ln;
	if(!pnt) goto bot;

	while(*pnt)
		{
		if(*pnt == ' ') { pnt++; continue; } else { break; }
		}
	if(*pnt != '<') goto bot;

	pnt++;
	endtag = (*pnt == '/');
	pnt += endtag;

	switch(*pnt)
		{
		case 'm':
			if(!strncmp(pnt, "module", 6))
				{
				if(!endtag)
					{
					char *qts[4];
					char *s = pnt + 6;
					int numqt = 0;

					while(*s)
						{
						if(*s == '"')
							{
							qts[numqt++] = s;
							if(numqt == 4) break;
							}
						s++;
						}
	
					if(numqt == 4)
						{
						char *fl = qts[0] + 1;
						char *nam = qts[2] + 1;
						qts[1][0] = qts[3][0] = 0;
	
						mId.push(nam);

						char fl_dup[strlen(fl)+1];
						char *s = fl; char *d = fl_dup;
						while(isalpha(*s)) { *(d++) = *(s++); }
						*d = 0;

						unsigned int lineno = atoi(s);
						fprintf(fo, "++ module %s file %s lines %d - %d\n", nam, fId[fl_dup].c_str(), lineno, lineno); /* don't need line number it truly ends at */
						}
					}
					else
					{
					if(!mId.empty())
						{
						mId.pop();
						}
					}
				}

		case 'f':
			if((!endtag) && (!strncmp(pnt, "file id", 7)))
				{
				char *qts[4];
				char *s = pnt + 7;
				int numqt = 0;
			
				while(*s)
					{
					if(*s == '"')
						{
						qts[numqt++] = s;
						if(numqt == 4) break;
						}
					s++;
					}

				if(numqt == 4)
					{
					char *cod = qts[0] + 1;
					char *fil = qts[2] + 1;
					qts[1][0] = qts[3][0] = 0;

					fId.insert(pair <string, string> (cod, fil));
					}
				}
			break;

		case 'i':
			if((!endtag) && (!strncmp(pnt, "instance", 8)))
				{
				char *qts[6];
				char *s = pnt + 8;
				int numqt = 0;
			
				while(*s)
					{
					if(*s == '"')
						{
						qts[numqt++] = s;
						if(numqt == 6) break;
						}
					s++;
					}

				if(numqt == 6)
					{
					char *cod = qts[0] + 1;
					char *nam = qts[2] + 1;
					char *defnam = qts[4] + 1;
					qts[1][0] = qts[3][0] = qts[5][0] = 0;

					if(!mId.empty())
						{
						fprintf(fo, "++ comp %s type %s parent %s\n", nam, defnam, mId.top().c_str()); 
						}
					}
				}
			break;

		default:
			break;
		}

	bot: 1;
	}
}


int main(int argc, char **argv)
{
FILE *fi, *fo;
int rc = 0;

switch(argc)
	{
	case 2:
		fi = (!strcmp("-", argv[1])) ? stdin : fopen(argv[1], "rb");
		if(fi)
			{
			fo = stdout;
			xml2stems(fi, fo);
			if(fi != stdin) fclose(fi);
			}
			else
			{
			fprintf(stderr, "Could not open '%s', exiting.\n", argv[1]);
			perror("Why");
			rc = 255;
			}
		break;

	case 3:
		fi = (!strcmp("-", argv[1])) ? stdin : fopen(argv[1], "rb");
		if(fi)
			{
			fo = fopen(argv[2], "wb");
			if(fo)
				{
				xml2stems(fi, fo);
				if(fi != stdin) fclose(fi);
				fclose(fo);
				}
				else
				{
				if(fi != stdin) fclose(fi);
				fprintf(stderr, "Could not open '%s', exiting.\n", argv[2]);
				perror("Why");
				rc = 255;
				}
			}
			else
			{
			fprintf(stderr, "Could not open '%s', exiting.\n", argv[1]);
			perror("Why");
			rc = 255;
			}
		break;

	default:
		printf("Usage:\n------\n%s infile.xml [outfile.stems]\n\n"
			"Converts Verilator xml file to rtlbrowse stems.\n"
			"For example: verilator -Wno-fatal des.v -xml-only --bbox-sys\n\n"
			"Use - as filename for stdin.\n"
			"Omitting optional stems name emits to stdout.\n", 
			argv[0]);
		break;
	}

return(rc);
}
