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

#define INCL_DOS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSFILEMGR
#define INCL_WIN
#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define  INCL_PM

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "cddb.h"
#include "cdplayerres.h"
#include "cd.h"

BOOL loadBitmaps(char * bmpDirectory);

void  setupSmallText()
{
  int a;
  int b;

  for(a=0x30;a<=0x30+10;a++)
    {
      b=a-0x30;
      tbiSmallText[a-0x30].chr=a;
      tbiSmallText[a-0x30].hbmSource=hbmText;
      tbiSmallText[a-0x30].rclSource.xLeft=b*5;
      tbiSmallText[a-0x30].rclSource.xRight=b*5+4;
      tbiSmallText[a-0x30].rclSource.yBottom=6;
      tbiSmallText[a-0x30].rclSource.yTop=12;
    }

  /* : */
  tbiSmallText[0xa].chr=':';
  tbiSmallText[0xa].hbmSource=hbmText;
  tbiSmallText[0xa].rclSource.xLeft=12*5;
  tbiSmallText[0xa].rclSource.xRight=12*5+4;
  tbiSmallText[0xa].rclSource.yBottom=6;
  tbiSmallText[0xa].rclSource.yTop=12;
  
  for(a=0x41;a<=0x41+26;a++)
    {
      b=a-0x41;
      tbiSmallText[a-0x30].chr=a;
      tbiSmallText[a-0x30].hbmSource=hbmText;
      tbiSmallText[a-0x30].rclSource.xLeft=b*5;
      tbiSmallText[a-0x30].rclSource.xRight=b*5+4;
      tbiSmallText[a-0x30].rclSource.yBottom=12;
      tbiSmallText[a-0x30].rclSource.yTop=18;
    }
}

void initTrackArray()
{
  int a;

  for(a=0;a<=99;a++)
    track[a]=0;
  track[100]=0;

}

void  setupNumbers()
{
  int a;
  int b;

  for(a=0x30;a<=0x30+10;a++)
    {
      b=a-0x30;
      tbiNumbers[a-0x30].chr=a;
      tbiNumbers[a-0x30].hbmSource=hbmNumbers;
      tbiNumbers[a-0x30].rclSource.xLeft=b*9;
      tbiNumbers[a-0x30].rclSource.xRight=b*9+9;
      tbiNumbers[a-0x30].rclSource.yBottom=0;
      tbiNumbers[a-0x30].rclSource.yTop=13;
    }
}

BOOL initData()
{
  char bmpDir[CCHMAXPATH];
  char *chrPtr;
  char path[CCHMAXPATH];
  HINI hini;
  ULONG keyLength;
  char chrSkin[CCHMAXPATH];
  SWP swp;

  bStereo=TRUE;
  bShuffle=FALSE;
  bRepeat=FALSE;
  iCurrentTrack=1;


  /* Get installation directory */
  _getcwd2(installDir,sizeof(installDir));/* Get current working dir */
  chrPtr=installDir;
  while(*chrPtr!=0 && (chrPtr-(char*)&installDir)<=sizeof(installDir)) {
    if(*chrPtr=='/')
      *chrPtr='\\';
    chrPtr++;
  }
  
  /* Load ini values */
  /* Build full path to ini file */
  snprintf(path,sizeof(path),"%s\\cdplayer.ini",installDir);       
  /* Open ini-file */
  hini=PrfOpenProfile(WinQueryAnchorBlock(HWND_DESKTOP),path);
  
  if(!hini) {
    showMsg("Can't open ini file!");
    return FALSE;
  }/* end of if(!hini) */
  

  keyLength=PrfQueryProfileString(hini,"skin","skinbasedir","",skinDir,sizeof(skinDir));
  /* Get skin basedirectory if not in ini file */
  if(keyLength==1){
    if(strlen(skinDir)<2)
      _getcwd2(skinDir,sizeof(skinDir));/* current dir */
    chrPtr=skinDir;
    while(*chrPtr!=0 && (chrPtr-(char*)&skinDir)<=sizeof(skinDir)) {
      if(*chrPtr=='/')
        *chrPtr='\\';
      chrPtr++;
    }
  }

  keyLength=PrfQueryProfileString(hini,"skin","choosenskin","",currentSkin,sizeof(currentSkin));
  if(keyLength==1)
    /* No entry found */
    snprintf(currentSkin,sizeof(currentSkin),"%s\\default",installDir);
  
  /* LoadBitMaps */
  strcpy(bmpDir,currentSkin);
  if(!loadBitmaps(bmpDir)) {
    snprintf(currentSkin,sizeof(currentSkin),"%s\\default",installDir);
    strcpy(bmpDir,currentSkin);
    loadBitmaps(bmpDir);
  }

  /* Query CD drives */
  CDQueryCDDrives(&iNumCD, &cFirstCD);

  keyLength=PrfQueryProfileString(hini,"drive","which","",choosenCD,sizeof(choosenCD));
  if(keyLength==1)
    sprintf(choosenCD,"%c:",cFirstCD);

  bRepeat=PrfQueryProfileInt(hini,"state","repeat",0); /* Repeat? */
  bShuffle=PrfQueryProfileInt(hini,"state","shuffle",0); /* Shuffle? */
  bTBSize=PrfQueryProfileInt(hini,"state","tbsize",0); /* Titlebar size? */

  iXPos=PrfQueryProfileInt(hini,"state","xpos",0); /* Last xpos */
  iYPos=PrfQueryProfileInt(hini,"state","ypos",0); /* Last ypos */
  if(iXPos==0 && iYPos==0) {
    /* First Start, center the Window. */
    WinQueryWindowPos(HWND_DESKTOP, &swp);
    iXPos=swp.cx/2-275/2;
    if(bTBSize)
      iYPos=swp.cy/2-12/2;
    else
      iYPos=swp.cy/2-116/2;
  }

  setupSmallText();
  setupNumbers();
  initTrackArray();

  return TRUE;
}

BOOL saveData() 
{
  HINI hini;
  char path[CCHMAXPATH];
  char *chrPtr;
  
/* Get installation directory */
  _getcwd2(installDir,sizeof(installDir));/* Get current working dir */
  chrPtr=installDir;
  while(*chrPtr!=0 && (chrPtr-(char*)&installDir)<=sizeof(installDir)) {
    if(*chrPtr=='/')
      *chrPtr='\\';
    chrPtr++;
  }
  
  /* Load ini values */
  /* Build full path for cdrecord.ini file */
  snprintf(path,sizeof(path),"%s\\cdplayer.ini",installDir);       
  /* Open ini-file */
  hini=PrfOpenProfile(WinQueryAnchorBlock(HWND_DESKTOP),path);

  if(!hini) {
    showMsg("Can't open ini file!");
    return FALSE;
  }/* end of if(!hini) */
  
  if(!PrfWriteProfileString(hini,"skin","skinbasedir",skinDir))
    return FALSE;
  if(!PrfWriteProfileString(hini,"skin","choosenskin",currentSkin))
    return FALSE;
  if(!PrfWriteProfileString(hini,"drive","which",choosenCD))
    return FALSE;

  if(bRepeat) {
    if(!PrfWriteProfileString(hini,"state","repeat","1"))
      return FALSE;
  }
  else
    if(!PrfWriteProfileString(hini,"state","repeat","0"))
      return FALSE;

  if(bShuffle) {
    if(!PrfWriteProfileString(hini,"state","shuffle","1"))
      return FALSE;
  }
  else
    if(!PrfWriteProfileString(hini,"state","shuffle","0"))
      return FALSE;

  if(bTBSize) {
    if(!PrfWriteProfileString(hini,"state","tbsize","1"))
      return FALSE;
  }
  else
    if(!PrfWriteProfileString(hini,"state","tbsize","0"))
      return FALSE;

  /* Save last position */
  snprintf(path,sizeof(path),"%d",iXPos);    
  if(!PrfWriteProfileString(hini,"state","xpos",path))
    return FALSE;
  snprintf(path,sizeof(path),"%d",iYPos);    
  if(!PrfWriteProfileString(hini,"state","ypos",path))
    return FALSE;
    
  
  return TRUE;
}




