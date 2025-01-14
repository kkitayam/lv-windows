/*
 * ichar.h
 *
 * All rights reserved. Copyright (C) 1994,1997 by NARITA Tomio
 */

#ifndef __ICHAR_H__
#define __ICHAR_H__

/*
 * international character
 */

typedef unsigned short		ic_t;

#define ICHAR_WIDTH		2

#define CNTRLWIDTH_SHORTFORM	1	/* 0x00 .. 0x1f */
#define CNTRLWIDTH_MIDDLEFORM	2	/* 0x20 .. 0x7e */
#define CNTRLWIDTH_LONGFORM	4	/* 0x7f .. 0xff */

#define HTAB_WIDTH		8
#define HTAB_INTERNAL_WIDTH	2

#define MakeByte1( ic )		( (char)( (ic) >> 8 ) )
#define MakeByte2( ic )		( (char)( 0x00ff & (ic) ) )
#define MakeIchar( c1, c2 )	( ( (ic_t)(c1) << 8 ) | (ic_t)(c2) )

typedef struct {
  char  charset;
  char  attr;
  ic_t  c;
} i_str_t;

#ifdef MSDOS
#define i_str_t i_str_t far
#else
#ifndef far
#define far
#endif /* far */
#endif /* MSDOS */

#endif /* __ICHAR_H__ */
