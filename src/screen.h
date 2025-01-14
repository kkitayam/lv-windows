/*
 * screen.h
 *
 * All rights reserved. Copyright (C) 1994,1997 by NARITA Tomio
 */

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <file.h>

public boolean_t ScreenTop( file_t *f, unsigned int logical );
public boolean_t ScreenTopPhysical( file_t *f, position_t *pos );
public boolean_t ScreenBot( file_t *f );

public unsigned int ScreenPrev( file_t *f, int physical );
public unsigned int ScreenNext( file_t *f, int physical );

public void ScreenRefresh( file_t *f );

#endif /* __SCREEN_H__ */
