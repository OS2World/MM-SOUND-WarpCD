/*
 * This file is (C) Chris Wohlgemuth 2000
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _COMMON_H_INCL_
#define _COMMON_H_INCL_

#define SKIP_RC 

#include "cd.h"
#define CDPLAYER_TIMER 1

int iNumCD;
char cFirstCD;
BOOL bNoTracks;
int iNumTracks;
int iCurrentTrack;
int iXPos;
int iYPos;

BOOL bStereo;
BOOL bShuffle;
BOOL bRepeat;
BOOL bTBSize;
BOOL bPlaying;

MSF msfStart;
MSF msfEnd;

int track[100];

char choosenCD[4];

char installDir[CCHMAXPATH];
char skinDir[CCHMAXPATH];
char currentSkin[CCHMAXPATH];

HPOINTER hptrIcon;

PFNWP oldButtonProc;
PFNWP oldStaticProc;

PBITMAPINFOHEADER2 pbmpi2MainBitmapFile;
BITMAPINFOHEADER2 bmpi2MainBitmapFile;
HBITMAP hbmMain;

PBITMAPINFOHEADER2 pbmpi2ButtonsBitmapFile;
BITMAPINFOHEADER2 bmpi2ButtonsBitmapFile;
HBITMAP hbmButtons;

PBITMAPINFOHEADER2 pbmpi2TitleBarBitmapFile;
BITMAPINFOHEADER2 bmpi2TitleBarBitmapFile;
HBITMAP hbmTitleBar;

PBITMAPINFOHEADER2 pbmpi2TextBitmapFile;
BITMAPINFOHEADER2 bmpi2TextBitmapFile;
HBITMAP hbmText;

PBITMAPINFOHEADER2 pbmpi2NumbersBitmapFile;
BITMAPINFOHEADER2 bmpi2NumbersBitmapFile;
HBITMAP hbmNumbers;

PBITMAPINFOHEADER2 pbmpi2StereoBitmapFile;
BITMAPINFOHEADER2 bmpi2StereoBitmapFile;
HBITMAP hbmStereo;

PBITMAPINFOHEADER2 pbmpi2ShufrepBitmapFile;
BITMAPINFOHEADER2 bmpi2ShufrepBitmapFile;
HBITMAP hbmShufrep;

PBITMAPINFOHEADER2 pbmpi2PositionBitmapFile;
BITMAPINFOHEADER2 bmpi2PositionBitmapFile;
HBITMAP hbmPosition;

typedef struct
{
  char chr;
  RECTL rclSource;
  HBITMAP hbmSource;
}TEXTBMPINFO;

TEXTBMPINFO  tbiNumbers[10];
TEXTBMPINFO  tbiSmallText[0x5a];
#define NUM_SKINS_IN_MENU 20
char skinArray[NUM_SKINS_IN_MENU][CCHMAXPATH];

HWND hwndObject;

#endif	






