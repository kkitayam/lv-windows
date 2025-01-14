/*
 * dfa.h
 *
 * All rights reserved. Copyright (C) 1994,1997 by NARITA Tomio
 */

#ifndef __DFA_H__
#define __DFA_H__

#include <istr.h>

public boolean_t regexp_short_cut;
public ic_t regexp_first_letter;

public char *ReMakeDFA( i_str_t *istr );
public boolean_t ReFreeDFA();

public boolean_t ReRun( i_str_t *istr, int *ptr );

public void ReShowNFA();
public void ReShowDFA();

#endif /* __DFA_H__ */
