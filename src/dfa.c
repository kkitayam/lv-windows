/*
 * dfa.c
 *
 * All rights reserved. Copyright (C) 1994,1997 by NARITA Tomio
 */

#include <stdio.h>
#include <stdlib.h>

#include <import.h>
#include <itable.h>
#include <uty.h>
#include <re.h>
#include <nfa.h>
#include <find.h>
#include <begin.h>
#include <dfa.h>

/*#define SHOWFUNC*/

#define DSTATES_SIZE	128

private int dsIdx = 0;

typedef struct DTRAN_T {
  re_t           *re;
  int            newState;
  struct DTRAN_T *next;
} dtran_t;

typedef struct {
  set_t     *state;
  dtran_t   *dtran;
  boolean_t final;
  boolean_t dollar;
} dstates_t;

private re_t *reRoot = NULL;
private dstates_t dStates[ DSTATES_SIZE ];

#define RE_MATCHED		-1
#define RE_NOT_MATCHED		-2

private boolean_t ReIncluded( set_t *set, re_t *re )
{
  for( ; set ; set = set->next )
    if( set->re == re )		/* equality about location */
      return TRUE;

  return FALSE;
}

private boolean_t SetIncluded( set_t *s, set_t *t )
{
  for( ; t ; t = t->next )
    if( !ReIncluded( s, t->re ) )
      return FALSE;

  return TRUE;
}

private boolean_t SetEqual( set_t *s, set_t *t )
{
  if( SetIncluded( s, t ) && SetIncluded( t, s ) )
    return TRUE;
  else
    return FALSE;
}

private dtran_t *DtranAlloc( re_t *re, int newState, dtran_t *interim )
{
  dtran_t *dtran;

  dtran = (dtran_t *)Malloc( sizeof( dtran_t ) );

  dtran->re       = re;
  dtran->newState = newState;
  dtran->next     = interim;

  return dtran;
}

private void DtranFreeAll( dtran_t *dtran )
{
  dtran_t *next;

  while( dtran ){
    next = dtran->next;
    free( dtran );
    dtran = next;
  }
}

public boolean_t ReFreeDFA()
{
  int i;

  ReFreeAll( reRoot );

  for( i = 0 ; i < dsIdx ; i++ )
    DtranFreeAll( dStates[ i ].dtran );
  dsIdx = 0;

  return TRUE;
}

private int LeafCondRange( re_t *setRe, re_t *mojiRe )
{
  return
    ( setRe->left->ic->charset == mojiRe->ic->charset
     && setRe->left->ic->c <= mojiRe->ic->c
     && setRe->right->ic->c >= mojiRe->ic->c );
}

private int ReLeafAdditionalCondition( re_t *setRe, re_t *mojiRe )
{
  /*
   * NOTE: OP_LEAF and OP_SIMPLE_LEAF is mutually exclusive.
   */
  return
    ( ( OP_RANGE == setRe->op && LeafCondRange( setRe, mojiRe ) )
     || ( OP_COMPLEMENT == setRe->op && !( setRe->ic == mojiRe->ic ) )
     || ( OP_COMPRANGE == setRe->op && !LeafCondRange( setRe, mojiRe ) )
     || ( OP_CATEGORY1 == setRe->op
	 && 1 == IcharWidth( mojiRe->ic->charset, mojiRe->ic->c ) )
     || ( OP_CATEGORY2 == setRe->op
	 && 2 == IcharWidth( mojiRe->ic->charset, mojiRe->ic->c ) )
     || ( OP_PERIOD == setRe->op ) );
}

private int ReRangeCondition( re_t *setRe, re_t *mojiRe )
{
  return
    ( OP_RANGE == setRe->op
     && setRe->left->ic->charset == mojiRe->left->ic->charset
     && setRe->left->ic->c <= mojiRe->left->ic->c
     && setRe->right->ic->c >= mojiRe->right->ic->c )
      || ( OP_PERIOD == setRe->op );
}

private int ReCompRangeCondition( re_t *setRe, re_t *mojiRe )
{
  return
    ( OP_COMPRANGE == setRe->op
     && setRe->left->ic->charset == mojiRe->left->ic->charset
     && setRe->left->ic->c >= mojiRe->left->ic->c
     && setRe->right->ic->c <= mojiRe->right->ic->c )
      || ( OP_PERIOD == setRe->op );
}

private boolean_t LeafIncluded( set_t *set, re_t *re )
{
  if( NULL == re )
    return FALSE;

  if( OP_LEAF == re->op || OP_SIMPLE_LEAF == re->op ){
    for( ; set ; set = set->next )
      if( ( OP_LEAF == set->re->op || OP_SIMPLE_LEAF == set->re->op )
	 && set->re->ic == re->ic )
	return TRUE;
  }

  return FALSE;
}

public char *ReMakeDFA( i_str_t *istr )
{
  int i, idx, newState = RE_MATCHED;
  set_t *set, *moji, *used, *included;

  if( dsIdx > 0 )
    ReFreeDFA();

  if( NULL == (reRoot = ReMakeTree( istr )) )
    return reMessage;

  ReMakeFollowpos( reRoot );

  set = NULL;
  SetInclude( (set_t **)&set, ReFirstpos( reRoot ) );

  dStates[ 0 ].state = set;
  dStates[ 0 ].dtran = NULL;
  dStates[ 0 ].final = FALSE;
  dsIdx = 1;

  for( idx = 0 ; idx < dsIdx ; idx++ ){
    used = NULL;
    for( moji = dStates[ idx ].state ; moji ; moji = moji->next ){
      if( OP_IGETA == moji->re->op )
	dStates[ idx ].final = TRUE;
      if( TRUE == LeafIncluded( used, moji->re ) )
	continue;
      ReInclude( (set_t **)&used, moji->re );
      /*
       * new character
       */
      included = NULL;

      if( OP_IGETA == moji->re->op )
	;
      else if( OP_HAT == moji->re->op || OP_DOLLAR == moji->re->op )
	SetInclude( (set_t **)&included, moji->re->followpos );
      else if( OP_LEAF == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( ( OP_LEAF == set->re->op && set->re->ic == moji->re->ic )
	     || ReLeafAdditionalCondition( set->re, moji->re ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_SIMPLE_LEAF == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( ( OP_SIMPLE_LEAF == set->re->op && set->re->ic == moji->re->ic )
	     || ReLeafAdditionalCondition( set->re, moji->re ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_RANGE == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( ReRangeCondition( set->re, moji->re ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_COMPLEMENT == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( ( OP_COMPLEMENT == set->re->op && set->re->ic == moji->re->ic )
	     || ( OP_PERIOD == set->re->op ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_COMPRANGE == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( ReCompRangeCondition( set->re, moji->re ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_CATEGORY1 == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( OP_CATEGORY1 == set->re->op
	     || ( OP_PERIOD == set->re->op ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_CATEGORY2 == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( OP_CATEGORY2 == set->re->op
	     || ( OP_PERIOD == set->re->op ) )
	    SetInclude( (set_t **)&included, set->re->followpos );
      } else if( OP_PERIOD == moji->re->op ){
	for( set = dStates[ idx ].state ; set ; set = set->next )
	  if( OP_PERIOD == set->re->op )
	    SetInclude( (set_t **)&included, set->re->followpos );
      }

      set = NULL;
      if( NULL != included ){
	/*
	 * check if it is new state
	 */
	for( i = 0 ; i < dsIdx ; i++ ){
	  if( SetEqual( included, dStates[ i ].state ) ){
	    newState = i;
	    break;
	  }
	}
	if( i == dsIdx ){
	  /*
	   * new states
	   */
	  if( dsIdx < DSTATES_SIZE ){
	    set = included;
	    newState = dsIdx;
	    dStates[ dsIdx ].state = included;
	    dStates[ dsIdx ].dtran = NULL;
	    dStates[ dsIdx ].final = FALSE;

	    dsIdx++;
	  } else {
	    SetFreeAll( included );
	    SetFreeAll( used );
	    ReFreeAll( reRoot );

	    for( i = 0 ; i < dsIdx ; i++ )
	      SetFreeAll( dStates[ i ].state );

	    reMessage = "too many states";

	    return reMessage;
	  }
	}
      } else {
	newState = RE_MATCHED;
      }
      /*
       * add dtran
       */
      dStates[ idx ].dtran = DtranAlloc( moji->re,
					newState,
					dStates[ idx ].dtran );
      if( NULL == set )
	SetFreeAll( included );
    }
    SetFreeAll( used );
  }

  for( i = 0 ; i < dsIdx ; i++ ){
    dtran_t *tran;

    for( tran = dStates[ i ].dtran ; tran ; tran = tran->next ){
      if( OP_DOLLAR == tran->re->op ){
	dStates[ i ].dollar = TRUE;
	break;
      }
    }
    if( NULL == tran )
      dStates[ i ].dollar = FALSE;

    SetFreeAll( dStates[ i ].state );
  }

  if( NULL == dStates[ 0 ].dtran->next
     && OP_SIMPLE_LEAF == dStates[ 0 ].dtran->re->op ){
    regexp_short_cut = TRUE;
    regexp_first_letter = dStates[ 0 ].dtran->re->ic->c;
  } else
    regexp_short_cut = FALSE;

  return NULL;
}

/*
 * DFA execution
 */

private boolean_t complementFailed = FALSE;
private dtran_t *dTran;
private i_str_t *iStr;
private int *iPtr;

private int ReDoLeaf( re_t *re, char charset, ic_t c )
{
  if( charset < PSEUDO ){
    return ( re->ic->c == c
	    && re->ic->charset == charset );
  } else {
    return ( re->ic->c == ( 0x00ff & c )
	    && re->ic->charset == charset );
  }
}

private int ReDoSimpleLeaf( re_t *re, char charset, ic_t c )
{
  return ( re->ic->c == c && re->ic->charset == charset );
}

private int ReDoHat( re_t *re, char charset, ic_t c )
{
  if( 0 == *iPtr ){
    (*iPtr)--;
    return TRUE;
  } else
    return FALSE;
}

private int ReDoDollar( re_t *re, char charset, ic_t c )
{
  return FALSE;
}

private int ReDoRange( re_t *re, char charset, ic_t c )
{
  return re->left->ic->charset == charset
    && re->left->ic->c <= c
      && re->right->ic->c >= c;
}

private int ReDoComplement( re_t *re, char charset, ic_t c )
{
  if( TRUE == complementFailed || ReDoLeaf( re, charset, c ) ){
    if( NULL == dTran->next ||
       ( OP_COMPLEMENT != dTran->next->re->op
	&& OP_COMPRANGE != dTran->next->re->op ) )
      complementFailed = FALSE;
    else
      complementFailed = TRUE;

    return FALSE;
  }

  if( NULL == dTran->next ||
     ( OP_COMPLEMENT != dTran->next->re->op
      && OP_COMPRANGE != dTran->next->re->op ) )
    return TRUE;
  else
    return FALSE;
}

private int ReDoCompRange( re_t *re, char charset, ic_t c )
{
  if( TRUE == complementFailed || ReDoRange( re, charset, c ) ){
    if( NULL == dTran->next ||
       ( OP_COMPLEMENT != dTran->next->re->op
	&& OP_COMPRANGE != dTran->next->re->op ) )
      complementFailed = FALSE;
    else
      complementFailed = TRUE;

    return FALSE;
  }

  if( NULL == dTran->next ||
     ( OP_COMPLEMENT != dTran->next->re->op
      && OP_COMPRANGE != dTran->next->re->op ) )
    return TRUE;
  else
    return FALSE;
}

private int ReDoCategory1( re_t *re, char charset, ic_t c )
{
  return 1 == IcharWidth( charset, c );
}

private int ReDoCategory2( re_t *re, char charset, ic_t c )
{
  return 2 == IcharWidth( charset, c );
}

private int ReDoCategory3( re_t *re, char charset, ic_t c )
{
  return FALSE;
}

private int ReDoPeriod( re_t *re, char charset, ic_t c )
{
  return TRUE;
}

private int ReDoIgeta( re_t *re, char charset, ic_t c )
{
  return FALSE;
}

typedef int (*re_do_func_t)( re_t *, char, ic_t );

private re_do_func_t reDoFuncTable[ OP_LEAF_MAX ] = {
  ReDoLeaf,
  ReDoSimpleLeaf,
  ReDoHat,
  ReDoDollar,
  ReDoRange,
  ReDoComplement,
  ReDoCompRange,
  ReDoCategory1,
  ReDoCategory2,
  ReDoCategory3,
  ReDoPeriod,
  ReDoIgeta
};

private int ReDo( int state )
{
  for( dTran = dStates[ state ].dtran ; dTran ; dTran = dTran->next ){
    if( (*reDoFuncTable[ dTran->re->op ])( dTran->re,
					  iStr[ *iPtr ].charset,
					  ToLower( iStr[*iPtr].charset,
						  iStr[ *iPtr ].c ) ) )
      return dTran->newState;
  }

  return RE_NOT_MATCHED;
}

public boolean_t ReRun( i_str_t *istr, int *iptr )
{
  int state, lastptr = 0;
  boolean_t matched;

  iStr = istr;
  iPtr = iptr;

  if( 0 > (state = ReDo( 0 )) )
    return FALSE;

  for( matched = FALSE ; ;  ){
    if( TRUE == dStates[ state ].final ){
      matched = TRUE;
      lastptr = *iPtr;
    }
    (*iPtr)++;
    if( NOSET == iStr[ *iPtr ].charset ){
      if( TRUE == matched ){
	*iPtr = lastptr;
	return TRUE;
      } else if( TRUE == dStates[ state ].dollar ){
	(*iPtr)--;
	return TRUE;
      } else
	return FALSE;
    }
    if( 0 > (state = ReDo( state )) ){
      if( TRUE == matched ){
	*iPtr = lastptr;
	return TRUE;
      } else
	return FALSE;
    }
  }
}

#ifdef SHOWFUNC
private void ReIndent( int indent )
{
  while( indent-- > 0 )
    putchar( ' ' );
}

private void ReShowFollowpos( re_t *re, int indent )
{
  set_t *set;

  if( NULL == re )
    return;

  ReIndent( indent );

  switch( re->op ){
  case OP_LEAF:		printf( "op:leaf(%c:%d:%04x)",
			       0x00ff & re->ic->c,
			       re->ic->charset,
			       re->ic->c ); break;
  case OP_SIMPLE_LEAF:	printf( "op:s-leaf(%c:%d:%04x)",
			       0x00ff & re->ic->c,
			       re->ic->charset,
			       re->ic->c ); break;
  case OP_HAT:		printf( "op:hat" ); break;
  case OP_DOLLAR:	printf( "op:dollar" ); break;
  case OP_PERIOD:	printf( "op:period" ); break;
  case OP_COMPLEMENT:	printf( "op:complement" ); break;
  case OP_RANGE:	printf( "op:range" ); break;
  case OP_COMPRANGE:	printf( "op:comprange" ); break;
  case OP_OR:		printf( "op:or" ); break;
  case OP_CLOSURE:	printf( "op:closure" ); break;
  case OP_QUESTION:	printf( "op:question" ); break;
  case OP_CAT:		printf( "op:cat" ); break;
  case OP_IGETA:	printf( "op:igeta" ); break;
  }

  if( re->followpos ){
    printf( ", follow:" );
    for( set = re->followpos ; set ; set = set->next )
      printf( "%08x ", (int)set->re );
  }

  if( re->firstpos ){
    printf( ", first:" );
    for( set = re->firstpos ; set ; set = set->next )
      printf( "%08x ", (int)set->re );
  }

  if( re->lastpos ){
    printf( ", last:" );
    for( set = re->lastpos ; set ; set = set->next )
      printf( "%08x ", (int)set->re );
  }

  printf( "\r\n" );

  ReShowFollowpos( re->left, indent + 2 );
  ReShowFollowpos( re->right, indent + 2 );
}

public void ReShowNFA()
{
  ReShowFollowpos( reRoot, 0 );
}

private void ReShowdtran( dtran_t *dtran )
{
  re_t *re;

  for( ; dtran ; dtran = dtran->next ){
    re = dtran->re;
    printf( "\t" );
    switch( re->op ){
    case OP_LEAF:	printf( "op:leaf(%c:%d:%04x)",
			       0x00ff & re->ic->c,
			       re->ic->charset,
			       re->ic->c ); break;
    case OP_SIMPLE_LEAF:	printf( "op:s-leaf(%c:%d:%04x)",
			       0x00ff & re->ic->c,
			       re->ic->charset,
			       re->ic->c ); break;
    case OP_HAT:	printf( "op:hat" ); break;
    case OP_DOLLAR:	printf( "op:dollar" ); break;
    case OP_PERIOD:	printf( "op:period" ); break;
    case OP_COMPLEMENT:	printf( "op:complement" ); break;
    case OP_RANGE:	printf( "op:range" ); break;
    case OP_COMPRANGE:	printf( "op:comprange" ); break;
    case OP_OR:		printf( "op:or" ); break;
    case OP_CLOSURE:	printf( "op:closure" ); break;
    case OP_QUESTION:	printf( "op:question" ); break;
    case OP_CAT:	printf( "op:cat" ); break;
    case OP_IGETA:	printf( "op:igeta" ); break;
    }
    printf( "->(%d)\r\n", dtran->newState );
  }
}

public void ReShowDFA()
{
  int i;

  for( i = 0 ; i < dsIdx ; i++ ){
    printf( "%sT(%d):\r\n", ( TRUE == dStates[ i ].final ? "*" : " " ), i );
    ReShowdtran( dStates[ i ].dtran );
  }
}
#endif /* SHOWFUNC */
