/*
 * keybind.h
 *
 * All rights reserved. Copyright (C) 1994,1997 by NARITA Tomio
 */

#ifndef __KEYBIND_H__
#define __KEYBIND_H__

typedef void (*key_table_t)( unsigned int );

private key_table_t keyTable[ 128 ] = {
  CommandNull,		/* C-@ */	/* 0/0 */
  CommandNull,		/* C-a */
  CommandPrevPage,	/* C-b */
  CommandNull,		/* C-c */
  CommandNextHalf,	/* C-d */
  CommandNextLine,	/* C-e */
  CommandNextPage,	/* C-f */
  CommandFileStatus,	/* C-g */
  CommandPrevLine,	/* C-h */
  CommandNull,		/* C-i */
  CommandNextLine,	/* C-j */
  CommandPrevLine,	/* C-k */
  CommandRedisplay,	/* C-l */
  CommandNextLine,	/* C-m */
  CommandNextLine,	/* C-n */
  CommandNull,		/* C-o */
  CommandPrevLine,	/* C-p */	/* 1/0 */
  CommandNull,		/* C-q */
  CommandRefresh,	/* C-r */
  CommandNull,		/* C-s */
  CommandNull,		/* C-t */
  CommandPrevHalf,	/* C-u */
  CommandNextPage,	/* C-v */
  CommandNull,		/* C-w */
  CommandNull,		/* C-x */
  CommandPrevLine,	/* C-y */
  CommandShellEscape,	/* C-z */
  CommandNull,		/* C-[ */
  CommandNull,		/* C-\ */
  CommandNull,		/* C-] */
  CommandNull,		/* C-^ */
  CommandNull,		/* C-_ */
  CommandNextPage,	/* ' ' */	/* 2/0 */
  CommandNull,		/* '!' */
  CommandNull,		/* '"' */
  CommandNull,		/* '#' */
  CommandNull,		/* '$' */
  CommandNull,		/* '%' */
  CommandNull,		/* '&' */
  CommandNull,		/* ''' */
  CommandNull,		/* '(' */
  CommandNull,		/* ')' */
  CommandNull,		/* '*' */
  CommandNull,		/* '+' */
  CommandNull,		/* ',' */
  CommandNull,		/* '-' */
  CommandNull,		/* '.' */
  CommandFindForward,	/* '/' */
  CommandNull,		/* '0' */	/* 3/0 */
  CommandNull,		/* '1' */
  CommandNull,		/* '2' */
  CommandNull,		/* '3' */
  CommandNull,		/* '4' */
  CommandNull,		/* '5' */
  CommandNull,		/* '6' */
  CommandNull,		/* '7' */
  CommandNull,		/* '8' */
  CommandNull,		/* '9' */
  CommandNull,		/* ':' */
  CommandNull,		/* ';' */
  CommandTopOfFile,	/* '<' */
  CommandNull,		/* '=' */
  CommandBottomOfFile,	/* '>' */
  CommandFindBackward,	/* '?' */
  CommandNull,		/* '@' */	/* 4/0 */
  CommandNull,		/* 'A' */
  CommandNull,		/* 'B' */
  CommandNull,		/* 'C' */
  CommandNull,		/* 'D' */
  CommandNull,		/* 'E' */
  CommandNull,		/* 'F' */
  CommandBottomOfFile,	/* 'G' */
  CommandNull,		/* 'H' */
  CommandNull,		/* 'I' */
  CommandNull,		/* 'J' */
  CommandNull,		/* 'K' */
  CommandNull,		/* 'L' */
  CommandNull,		/* 'M' */
  CommandRepeatBackward,/* 'N' */
  CommandNull,		/* 'O' */
  CommandNull,		/* 'P' */	/* 5/0 */
  CommandQuit,		/* 'Q' */
  CommandReload,	/* 'R' */
  CommandNull,		/* 'S' */
  CommandNull,		/* 'T' */
  CommandNull,		/* 'U' */
  CommandVersion,	/* 'V' */
  CommandNull,		/* 'W' */
  CommandNull,		/* 'X' */
  CommandNull,		/* 'Y' */
  CommandNull,		/* 'Z' */
  CommandNull,		/* '[' */
  CommandNull,		/* '\' */
  CommandNull,		/* ']' */
  CommandNull,		/* '^' */
  CommandNull,		/* '_' */
  CommandNull,		/* '`' */	/* 6/0 */
  CommandNull,		/* 'a' */
  CommandPrevPage,	/* 'b' */
  CommandNull,		/* 'c' */
  CommandNextHalf,	/* 'd' */
  CommandNextLine,	/* 'e' */
  CommandNextPage,	/* 'f' */
  CommandTopOfFile,	/* 'g' */
  CommandNull,		/* 'h' */
  CommandNull,		/* 'i' */
  CommandNextLine,	/* 'j' */
  CommandPrevLine,	/* 'k' */
  CommandNull,		/* 'l' */
  CommandNull,		/* 'm' */
  CommandRepeatForward,	/* 'n' */
  CommandNull,		/* 'o' */
  CommandPercent,	/* 'p' */	/* 7/0 */
  CommandQuit,		/* 'q' */
  CommandRefresh,	/* 'r' */
#ifdef REGEXPTEST
  CommandRegexpNFA,	/* 's' */
  CommandRegexpDFA,	/* 't' */
#else
  CommandNull,		/* 's' */
  CommandNull,		/* 't' */
#endif /* REGEXPTEST */
  CommandPrevHalf,	/* 'u' */
  CommandNull,		/* 'v' */
  CommandNull,		/* 'w' */
  CommandNull,		/* 'x' */
  CommandPrevLine,	/* 'y' */
  CommandNull,		/* 'z' */
  CommandNull,		/* '{' */
  CommandNull,		/* '|' */
  CommandNull,		/* '}' */
  CommandNull,		/* '~' */
  CommandNull		/* 7/15 */
};

#endif /* __KEYBIND_H__ */
