/* 
 Translate TLA (Tektronix Logic Analyser) files to 
 VCD (Verilog Code Dump) or to text listings
 
 Author: Emil
 Date: 06 September 2005
 Todo: MagniVu format not yet understood
*/

#define TLA2VCD_VERSION "1.0"

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#if HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include "../gnu_regex.h"

#ifndef __MINGW32__
#include <zlib.h>
#endif

/* function pointers */
FILE         *( *oopen ) ( const char *path, const char *mode );
int           ( *oclose ) ( FILE * fp );
int           ( *oprintf ) ( FILE * stream, const char *format, ... );

struct switch_t
{
  unsigned      text:1;
  unsigned      selectlist:1;
  unsigned      compress:1;
  unsigned      magnivu:1;
  unsigned      allchn:1;
  unsigned      onlylist:1;
  unsigned      verbose:1;
};

struct chan_t
{
  int           matched;
  int           inversion;
  char         *chan;
  char         *name;
  char          code[3];
};

#define MAXCHAN 136
#define MAXBUF 1024
#define DSETS 3

struct chan_t ch[MAXCHAN];
char         *group[MAXCHAN];
char         *groupdef[MAXCHAN];
int           chnr, grpnr, chtot, edgetot;
char         *match_list;
FILE         *outfile;		/* VCD file handle */
struct switch_t sw;

void
channels( unsigned char *buffer, int lgt, int set )
{
#ifdef __MINGW32__
  __int64       timestamp;
#else
  unsigned long long timestamp;
#endif
  int           i;
  unsigned int  val;
  unsigned char *p;

  if ( !set )
  {
    p = ( unsigned char * ) &timestamp;
    for ( i = 0; i < 8; i++ )	/* big endian 8 byte time stamps */
      *p++ = *( buffer + --lgt );
    timestamp++;		/* mask lower 3 bits */
    timestamp >>= 1;		/* multiply by 4ns */
#ifdef __MINGW32__		/* broken windows "long long" */
    if ( sw.text )
      oprintf( outfile, "%12I64d ns", timestamp );
    else
      oprintf( outfile, "#%I64d\n", timestamp );
#else
    if ( sw.text )
      oprintf( outfile, "%12lld ns", timestamp );
    else
      oprintf( outfile, "#%lld\n", timestamp );
#endif
    if ( !sw.text && timestamp == 0 )
      oprintf( outfile, "$dumpvars\n" );
    for ( i = 0; i <= chnr; i++ )
    {
      if ( ch[i].matched )
      {
	val = ( unsigned int ) buffer[i >> 3];
	val >>= 7 - ( i & 7 );
	val ^= ch[i].inversion;
	val &= 1;
	if ( sw.text )
	  oprintf( outfile, " %d", val );
	else
	  oprintf( outfile, "%d%s\n", val, ch[i].code );
      }
    }
    if ( sw.text )
      oprintf( outfile, "\n" );
    if ( !sw.text && timestamp == 0 )
      oprintf( outfile, "$end\n" );
    edgetot++;
  }
  if ( set == 1 && sw.magnivu )
  {
    if ( sw.text )
#ifdef __MINGW32__		/* broken windows "long long" */
      oprintf( outfile, "0x%16I64x\n", *( __int64 * ) buffer );
#else
      oprintf( outfile, "0x%16llx\n", *( unsigned long long * ) buffer );
#endif
  }
}

void
vcd_header( char *module )
{
  time_t        t;

  time( &t );
  oprintf( outfile, "$date\n" );
  oprintf( outfile, "  %s", ctime( &t ) );
  oprintf( outfile, "$end\n\n" );
  oprintf( outfile, "$version\n" );
  oprintf( outfile, "  TLA2VCD translator v%s\n", TLA2VCD_VERSION );
  oprintf( outfile, "$end\n\n" );
  oprintf( outfile, "$timescale\n" );
  oprintf( outfile, "  1ns\n" );
  oprintf( outfile, "$end\n\n\n" );
  oprintf( outfile, "$scope module %s $end\n", module );
}

int
process( FILE * tlafile )
{
  unsigned char buffer[MAXBUF];
  unsigned char ichar;
  int           i, last, len, chanmatched, dataset = 2;
  int           smp[DSETS], lgt[DSETS];
  long          here, total;
  char         *p, *name = NULL;
  regex_t       rxdatamark, rxdataset, rxsmp, rxlgt, rxchn, rxsgn;
  regex_t       rxgroup, rxgrpdef, rxspace;
  regex_t       rxselect[100];
  int           rxsel = 0;
  regmatch_t    pmatch[3];

  regcomp( &rxdatamark,
      "\"DaSetNormal\"[[:space:]]*\"\\$\\$\"[[:space:]]*=[[:space:]]*(#)",
      REG_EXTENDED );
  regcomp( &rxdataset,
      "CjmTimebaseData[[:space:]]+\"Tb([[:alpha:]]+)TimebaseData\"[[:space:]]+\"\\$\\$\"",
      REG_EXTENDED );
  regcomp( &rxsmp,
      "\"DaNumSamples\"[[:space:]]+\"\\$\\$\"[[:space:]]*=[[:space:]]*\\{[[:space:]]*([[:digit:]]+)[[:space:]]*\\}",
      REG_EXTENDED );
  regcomp( &rxlgt,
      "\"DaBytesPerSample\"[[:space:]]+\"\\$\\$\"[[:space:]]*=[[:space:]]*\\{[[:space:]]*([[:digit:]]+)[[:space:]]*\\}",
      REG_EXTENDED );
  regcomp( &rxchn,
      "CjmChannel[[:space:]]*\"([[:alnum:][:punct:]]+)\"[[:space:]]*\"\\$([^[:space:]]+)\\$\"",
      REG_EXTENDED );
  regcomp( &rxsgn,
      "\"ClaChannelInversion\"[[:space:]]+\"\\$\\$\"[[:space:]]*=[[:space:]]*\\{[[:space:]]*(TRUE|FALSE)[[:space:]]*\\}",
      REG_EXTENDED );
  regcomp( &rxgroup,
      "CjmChannelGroup[[:space:]]+\"[[:alnum:]]+\"[[:space:]]+\"\\$([[:alnum:][:punct:]]+)\\$\"[[:space:]]*\\{",
      REG_EXTENDED );
  regcomp( &rxgrpdef,
      "CafcStringCell[[:space:]]+\"ClaGroupDefinition\"[[:space:]]+\"\\$\\$\"[[:space:]]*=[[:space:]]*\\{[[:space:]]*\"([[:alnum:][:punct:]]+)\"[[:space:]]*\\}",
      REG_EXTENDED );

  if ( sw.selectlist )
  {
    last = 0;
    regcomp( &rxspace, "([^[:space:]]+)", REG_EXTENDED );
    while ( !last && !regexec( &rxspace, match_list, 2, pmatch, 0 )
	&& rxsel < 100 )
    {
      if ( *( match_list + pmatch[0].rm_eo ) )
	*( match_list + pmatch[0].rm_eo ) = 0;
      else
	last = 1;
      regcomp( &rxselect[rxsel++], match_list, REG_EXTENDED );
      if ( last )
	break;
      else
	match_list += pmatch[0].rm_eo + 1;
    }
    regfree( &rxspace );
  }
  fseek( tlafile, 0l, SEEK_SET );
  chnr = -1;
  grpnr = 0;
  chtot = 0;
  edgetot = 0;
  chanmatched = 0;

  while ( ( here = ftell( tlafile ) ) != -1
      && fgets( (char *)buffer, MAXBUF, tlafile ) != NULL )
  {
    if ( !regexec( &rxdatamark, (char *)buffer, 2, pmatch, 0 ) )
    {
      fseek( tlafile, here + pmatch[1].rm_so + 1, SEEK_SET );
      if ( sw.verbose )
	printf( "Datamark at 0x%lx\n", here + pmatch[1].rm_so + 1 );

      if(!fread(&ichar, 1, 1, tlafile )) ichar = 0;
      i = (unsigned int)ichar;
      i -= 0x30;
      if(!fread( buffer, i, 1, tlafile )) buffer[0] = 0;
      buffer[i] = 0;
      total = atol( (char *)buffer );
      if ( sw.verbose )
	printf( "Size in bytes %ld\n", total );
      if ( total != smp[dataset] * lgt[dataset] )
	printf( "Warning %d * %d != %ld\n", smp[dataset], lgt[dataset],
	    total );
      if ( dataset == 0 )
      {
	if ( sw.text || sw.onlylist )
	  oprintf( outfile, "Timestamp " );
	for ( i = 0; i <= chnr; i++ )
	{
	  if ( !ch[i].matched )
	    continue;
	  chtot++;
	  if ( ch[i].name )
	    name = ch[i].name;
	  else
	    name = ch[i].chan;
	  if ( sw.text || sw.onlylist )
	    oprintf( outfile, " %s", name );
	  else
	    oprintf( outfile, "$var wire 1 %s %s $end\n", ch[i].code, name );
	  if ( sw.onlylist && ch[i].name )
	    printf( "(%s)", ch[i].chan );
	}
	if ( sw.text || sw.onlylist )
	  oprintf( outfile, "\n" );
	else
	{
	  oprintf( outfile, "$upscope $end\n\n" );
	  oprintf( outfile, "$enddefinitions $end\n\n" );
	}
	if ( sw.onlylist )
	{
	  for ( i = 0; i < grpnr; i++ )
	    oprintf( outfile, "Group \"%s\": %s\n", group[i], groupdef[i] );
	}
      }
      for ( i = 0; i < smp[dataset]; i++ )
      {
	if(!fread( buffer, lgt[dataset], 1, tlafile )) buffer[0] = 0;
	if ( !sw.onlylist )
	{
	  if ( !i && !dataset )
	    memset( buffer + lgt[dataset] - 8, 0, 8 );	/* zero the timestamp 
							 */
	  channels( buffer, lgt[dataset], dataset );
	}
      }
      continue;
    }
    if ( !regexec( &rxgroup, (char *)buffer, 2, pmatch, 0 ) )
    {
      buffer[pmatch[1].rm_eo] = 0;
      p = (char *)buffer + pmatch[1].rm_so;
      group[grpnr] = strdup( p );
      if ( sw.verbose )
	printf( "Group: %s\n", p );
      continue;
    }
    if ( !regexec( &rxgrpdef, (char *)buffer, 2, pmatch, 0 ) )
    {
      buffer[pmatch[1].rm_eo] = 0;
      p = (char *)buffer + pmatch[1].rm_so;
      groupdef[grpnr++] = strdup( p );
      if ( sw.verbose )
	printf( "Group definition: %s\n", p );
      continue;
    }
    if ( !regexec( &rxdataset, (char *)buffer, 2, pmatch, 0 ) )
    {
      buffer[pmatch[1].rm_eo] = 0;
      dataset = 2;		/* unknown data set type */
      if ( !strcmp( (char *)buffer + pmatch[1].rm_so, "Main" ) )
	dataset = 0;
      if ( !strcmp( (char *)buffer + pmatch[1].rm_so, "HiRes" ) )
	dataset = 1;
      if ( sw.verbose )
	printf( "Dataset %d\n", dataset );
      continue;
    }
    if ( !regexec( &rxsmp, (char *)buffer, 2, pmatch, 0 ) )
    {
      buffer[pmatch[1].rm_eo] = 0;
      smp[dataset] = atoi( (char *)buffer + pmatch[1].rm_so );
      if ( sw.verbose )
	printf( "Samples %d\n", smp[dataset] );
      continue;
    }
    if ( !regexec( &rxlgt, (char *)buffer, 2, pmatch, 0 ) )
    {
      buffer[pmatch[1].rm_eo] = 0;
      lgt[dataset] = atoi( (char *)buffer + pmatch[1].rm_so );
      if ( sw.verbose )
	printf( "Length %d\n", lgt[dataset] );
      continue;
    }
    if ( !regexec( &rxchn, (char *)buffer, 3, pmatch, 0 ) )
    {
      chnr++;
      /* emulate strndup */
      len = pmatch[1].rm_eo - pmatch[1].rm_so;
      ch[chnr].chan = malloc( len + 1 );
      strncpy( ch[chnr].chan, (char *)buffer + pmatch[1].rm_so, len );
      *( ch[chnr].chan + len ) = 0;
      if ( *( buffer + pmatch[2].rm_so ) == '\\' &&
	  *( buffer + pmatch[2].rm_so + 1 ) == 't' )
	ch[chnr].name = NULL;
      else
      {
	/* emulate strndup */
	len = pmatch[2].rm_eo - pmatch[2].rm_so;
	ch[chnr].name = malloc( len + 1 );
	strncpy( ch[chnr].name, (char *)buffer + pmatch[2].rm_so, len );
	*( ch[chnr].name + len ) = 0;
      }
      ch[chnr].inversion = 0;
      if ( sw.selectlist )
      {
	ch[chnr].matched = 0;
	p = ch[chnr].chan;
	if ( ch[chnr].name )
	  p = ch[chnr].name;
	for ( i = 0; i < rxsel; i++ )
	{
	  if ( !regexec( &rxselect[i], p, 2, pmatch, 0 ) )
	    ch[chnr].matched = 1;
	}
      }
      else
      {
	if ( sw.allchn || ch[chnr].name )
	  ch[chnr].matched = 1;
	else
	  ch[chnr].matched = 0;
      }
      if ( ch[chnr].matched )
      {
	i = chanmatched;
	ch[chnr].code[0] = '!' + i % 94;
	i /= 94;
	if ( i )
	{
	  ch[chnr].code[1] = '!' + i;
	  ch[chnr].code[2] = 0;
	}
	else
	  ch[chnr].code[1] = 0;
	chanmatched++;
      }
      if ( sw.verbose )
      {
	printf( "Channel %s", ch[chnr].chan );
	if ( ch[chnr].name )
	  printf( " (%s)", ch[chnr].name );
	printf( "\n" );
      }
      continue;
    }
    if ( !regexec( &rxsgn, (char *)buffer, 2, pmatch, 0 ) )
    {
      buffer[pmatch[1].rm_eo] = 0;
      if ( strcasecmp( (char *)buffer + pmatch[1].rm_so, "false" ) )
	ch[chnr].inversion = 1;
      if ( sw.verbose )
	printf( "\tinversion %d\n", ch[chnr].inversion );
      continue;
    }
  }
  if ( sw.selectlist )
  {
    for ( i = 0; i < rxsel; i++ )
      regfree( &rxselect[i] );
  }
  regfree( &rxdatamark );
  regfree( &rxdataset );
  regfree( &rxgroup );
  regfree( &rxgrpdef );
  regfree( &rxsmp );
  regfree( &rxlgt );
  regfree( &rxchn );
  regfree( &rxsgn );
  return 0;
}

void
usage( char *progname )
{
  fprintf( stderr, "\n%s [options] tla_file(s)\n", progname);
  fprintf( stderr, "\t-t\t\tgenerate text LST file instead of VCD file\n" );
  fprintf( stderr, "\t-l\t\tlist the available channels/groups and exit\n" );
  fprintf( stderr,
      "\t-s signal_list\tselect signals from regular expression list\n" );
  fprintf( stderr, "\t-a\t\tall channels\n" );
  fprintf( stderr, "\t-m\t\tappend MagniVu info (only in listing mode)\n" );
#ifndef __MINGW32__
  fprintf( stderr, "\t-z\t\tcompress output\n" );
#endif
  fprintf( stderr, "\t-v\t\tverbose\n" );
  fprintf( stderr, "\t-V\t\tprint version banner than exit\n" );
  exit( 0 );
}

void
get_options( int argc, char *argv[] )
{
  int           option;

  sw.selectlist = sw.text = sw.magnivu = sw.allchn = sw.onlylist = 0;
  sw.compress = sw.verbose = 0;
  do
  {
    option = getopt( argc, argv, "h?vtlamzs:V" );
    switch ( option )
    {
      case 't':
	sw.text = 1;
	break;
      case 'l':
	sw.onlylist = 1;
      case 'a':
	sw.allchn = 1;
	break;
      case 'm':
	sw.magnivu = 1;
	break;
      case 'z':
	sw.compress = 1;
	break;
      case 's':
	sw.selectlist = 1;
	match_list = strdup( optarg );
	break;
      case 'v':
	sw.verbose = 1;
	break;
      case 'V':
	fprintf(stderr, "\n%s version %s\n", argv[0], TLA2VCD_VERSION);
	exit(0);
	break;
      case 'h':
      case '?':
	usage( argv[0] );
	break;
      case EOF:		/* no more options */
	break;
      default:
	fprintf( stderr, "getopt returned impossible value: %d ('%c')",
	    option, option );
    }
  }
  while ( option != EOF );

  if ( optind == argc )
    usage( argv[0] );
}

int
main( int argc, char *argv[] )
{
  FILE         *tlafile;	/* Tektronix file handle */
  char         *dumpfname, *p;

  WAVE_LOCALE_FIX

  get_options( argc, argv );

  while ( optind < argc )
  {
    tlafile = fopen( argv[optind], "rb" );
    if ( tlafile == NULL )
    {
      perror( "Open" );
      exit( -1 );
    }
    printf( "[*] File %s\n", argv[optind] );
    dumpfname = malloc( strlen( argv[optind] ) + 8 );
    strcpy( dumpfname, argv[optind] );
    p = strrchr( dumpfname, '.' );
    if ( p == NULL )
      p = dumpfname + strlen( dumpfname );
    if ( sw.text )
      strcpy( p, ".lst" );
    else
      strcpy( p, ".vcd" );
    oopen = fopen;
    oclose = fclose;
    oprintf = fprintf;
#ifndef __MINGW32__
    if ( sw.compress )
    {
      strcat( p, ".gz" );
      oopen = ( FILE * ( * )( const char *, const char * ) ) gzopen;
      oclose = ( int ( * )( FILE * ) ) gzclose;
      oprintf = ( int ( * )( FILE *, const char *,... ) ) gzprintf;
    }
#endif
    if ( sw.onlylist )
      outfile = stdout;
    else
    {
      outfile = oopen( dumpfname, "w" );
      if ( outfile == NULL )
      {
	perror( "Create" );
	exit( -1 );
      }
      if ( !sw.text )
      {
	p = strrchr( dumpfname, '.' );
	*p = 0;
	if ( sw.compress )
	{
	  p = strrchr( dumpfname, '.' );
	  *p = 0;
	}
	vcd_header( dumpfname );
      }
    }
    process( tlafile );
    if ( !sw.onlylist )
      oclose( outfile );
    fclose( tlafile );
    optind++;
  }
  printf( "[*] Done (%d channels %d samples)\n", chtot, edgetot );
  exit( 0 );
}
