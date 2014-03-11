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
#include <stdlib.h>

#include "cddb.h"
#include "common.h"
#include "cdplayerres.h"


char text[200];

BOOL cdQueryTracks(HWND hwndListBox , char * drive) ;
BOOL cdPlayTrack(HWND hwnd, ULONG ulTrack, char * drive);

int randomTrack()
{
  int rc;
  POINTL ptl;

  WinQueryPointerPos(HWND_DESKTOP,&ptl);
  if(track[100]==iNumTracks) {
    track[100]=0;
    initTrackArray();
  }

  srand((int)ptl.x);
  while(TRUE) {
    rc=(rand()*(iNumTracks))/RAND_MAX;
    if(rc<=iNumTracks) {
      if(!track[rc-1]) {
        track[rc-1]=1;
        track[100]++;
        return rc;
      }
    }
  }

}

BOOL cdStop(HWND hwnd, char * drive)
{
  char text[200];
   
  bPlaying=FALSE;
  WinStopTimer(WinQueryAnchorBlock(HWND_DESKTOP),hwnd,CDPLAYER_TIMER);
  sprintf(text,"TRACK %d NOT PLAYING",iCurrentTrack);
  WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
  sprintf(text,"%02d",0);
  WinSetWindowText(WinWindowFromID(hwnd,IDST_MINS),text);
  sprintf(text,"%02d",0);
  WinSetWindowText(WinWindowFromID(hwnd,IDST_SECS),text);
  sprintf(text,"%02d:%02d",0,0);
  WinSetWindowText(WinWindowFromID(WinWindowFromID(hwnd,IDPB_TITLEPLAYER),IDST_TIMETBPLAYER),text);
  WinSendMsg(WinWindowFromID(hwnd,IDPB_POSBAR),SLM_SETSLIDERINFO,
             MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),MPFROMLONG(0));
  
  return CDStop(drive);
}

BOOL cdPause(HWND hwnd, char * drive)
{
  char text[200];

  if(CDQueryPaused(drive)==-1)
    return FALSE; /* An error occured */
  if(bPlaying) {
    /* Make sure we resume only if we pressed pause before */
    if(CDQueryPaused(drive)==1)
      {
        CDResume(drive);
        sprintf(text,"TRACK %d OF %d RUNNING",iCurrentTrack,iNumTracks);
        WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
      }
    else {
      sprintf(text,"TRACK %d PAUSED",iCurrentTrack);
      WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
      return CDStop(drive);
    }
  }
}


BOOL cdPlay(HWND hwnd, char * drive)
{
  int a;

  if(!cdQueryTracks( hwnd, drive))
    return FALSE; 

  if(bShuffle)
    iCurrentTrack=randomTrack();

  CDStop(drive);

  cdPlayTrack(hwnd, iCurrentTrack, drive);

  return TRUE;
}  

BOOL cdPlayPos(HWND hwnd, int iPercent, char * drive)
{
  int a;
  LONG lStart, lStartOrg, lEnd;
  PLAYPARAM2 playParam;

  if(!cdQueryTracks( hwnd, drive))
    return FALSE; 
  
  CDQueryTrackMSF(iCurrentTrack, &msfStart, &msfEnd, drive);

  lStart=(msfStart.ucMinutes*60+msfStart.ucSeconds)*75+msfStart.ucFrames;
  lStartOrg=lStart;
  lEnd=(msfEnd.ucMinutes*60+msfEnd.ucSeconds)*75+msfEnd.ucFrames;
  lStart+=((lEnd-lStart)*iPercent)/100;

  lStart-=150;
  lEnd-=150;

      
  CDStop(drive);
  sprintf(text,"PLEASE WAIT");
  WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
  sprintf(text,"%02d",(lStart-lStartOrg+150)/75/60);
  WinSetWindowText(WinWindowFromID(hwnd,IDST_MINS),text);
  sprintf(text,"%02d",((lStart-lStartOrg+150)/75)%60);
  WinSetWindowText(WinWindowFromID(hwnd,IDST_SECS),text);
      

  playParam.ulStartSector=lStart;
  playParam.ulEndSector=lEnd;
  playParam.ucAddressMode=00;
  if(CDPlayRange( (PLAYPARAM*)&playParam, drive)) {
    sprintf(text,"TRACK %d OF %d RUNNING",iCurrentTrack,iNumTracks);
    WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
    WinStartTimer(WinQueryAnchorBlock(HWND_DESKTOP),hwnd,CDPLAYER_TIMER,1000);
  }
  bPlaying=TRUE;
  return TRUE;
}  

BOOL cdEject(HWND hwnd, char * drive)
{
  ULONG rc;
  char text[200];

  rc=CDQueryTrayOpen(drive);
  if(rc==2) {
    return FALSE;
  }

  if(rc==0) {
    CDStop(drive);            
    sprintf(text,"NOT PLAYING");
    WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
    bNoTracks=TRUE;
    iCurrentTrack=1;
    return CDEject(drive);
  }
  else {
    CDCloseTray(drive);
    /* Query tracks */
    cdQueryTracks( hwnd, drive); 
  }
  return TRUE;
}

BOOL cdPlayTrack(HWND hwnd, ULONG ulTrack, char * drive)
{
  BOOL rc;
  char text[200];

  sprintf(text,"STARTING TRACK %d",ulTrack);
  WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
  rc=CDPlayTrack(ulTrack, drive); 
  if(rc) {
    WinSendMsg(WinWindowFromID(hwnd,IDPB_POSBAR),SLM_SETSLIDERINFO,
               MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),MPFROMLONG(0));    
    CDQueryTrackMSF(ulTrack, &msfStart, &msfEnd, drive);
    WinStartTimer(WinQueryAnchorBlock(HWND_DESKTOP),hwnd,CDPLAYER_TIMER,1000);
    iCurrentTrack=ulTrack;
    sprintf(text,"TRACK %d OF %d RUNNING",ulTrack,iNumTracks);
    WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
  }
  bPlaying=TRUE;
  return rc;
}

BOOL cdQueryTracks(HWND hwnd , char * drive) 
{
  HFILE hFile;
  int a, b;
  char text[100];

  bNoTracks=TRUE;

  if(!CDQueryCDDrives(&iNumCD, &cFirstCD))
    return FALSE;
  
  /* Querytracks */
  hFile=openDrive(drive);
  
  iNumTracks=0;
  if(hFile) {
    a=CDQueryAudioCDTracks(hFile);
    iNumTracks=a;
    if(a>0)
      bNoTracks=FALSE;
    else
      return FALSE;
    closeDrive(hFile);
  }
  else 
    return FALSE;

  return TRUE;    
}

void cdSkipForward(HWND hwnd , char * drive)
{
  ULONG rc;
  int a;
  char text[100];


  if((rc=CDQueryIsPlaying(drive))==2) {
    return;
  }
  else 
  {
    if(rc==0) {
      if(bShuffle)
        iCurrentTrack=randomTrack();
      else {
        if(iCurrentTrack>=iNumTracks)
          return;
        else
          iCurrentTrack++;
      }
      sprintf(text,"TRACK %d",iCurrentTrack);
     
      WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
    }
    else {


      if(bShuffle) {
        iCurrentTrack=randomTrack();
      }
      else {
        iCurrentTrack=CDQueryCurrentPlayingTrack(drive);
        if(iCurrentTrack<iNumTracks)
          iCurrentTrack++;
      }

      cdStop(hwnd,drive);
      cdPlayTrack(hwnd,iCurrentTrack, drive);      
      return;
    }
  }
}

void cdSkipBack(HWND hwnd, char * drive)
{
  ULONG rc;
  int a;
  char text[100];


  if((rc=CDQueryIsPlaying(drive))==2)
    return;
  else 
    if(rc==0) {
      if(bShuffle)
        iCurrentTrack=randomTrack();
      else {
        if(iCurrentTrack<=1)
          return;
        else
          iCurrentTrack--;
      }
      sprintf(text,"TRACK %d",iCurrentTrack);
      WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);
    }
    else {
      if(bShuffle)
        iCurrentTrack=randomTrack();
      else {
        iCurrentTrack=CDQueryCurrentPlayingTrack(drive);
        if(iCurrentTrack>1)
          iCurrentTrack--;
      }
      cdStop(hwnd, drive);
      cdPlayTrack(hwnd, iCurrentTrack, drive);
      
    }
}

