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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "cddb.h"
#include "cdplayerres.h"
#include "cd.h"

/* This contains the information for the dialog controls */
#include "controls.h"

BOOL initData();
MRESULT EXPENTRY cdPlayerDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY selectTrackProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/* This Proc handles the main dialog */
MRESULT EXPENTRY cdPlayerObjectProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  char text[CCHMAXPATH];
  static BOOL bExecuting;

  switch (msg)
    {      
    case WM_INITDLG:   
      return (MRESULT) TRUE;
    case WM_CLOSE:
      return FALSE;
    case WM_APPTERMINATENOTIFY:
      switch(SHORT1FROMMP(mp2))
        {
        case IDPB_EJECT:
          cdEject(LONGFROMMP(mp1), choosenCD);
          break;
        case IDPB_PLAY:
          cdPlay(LONGFROMMP(mp1), choosenCD);
          break;
        case IDPB_STOP:
          cdStop( LONGFROMMP(mp1), choosenCD);
          break;
        case IDPB_PAUSE:
          cdPause(LONGFROMMP(mp1), choosenCD);
          break;
        case IDPB_NEXT:
          cdSkipForward(LONGFROMMP(mp1), choosenCD);
          break;
        case IDPB_PREVIOUS:
          cdSkipBack(LONGFROMMP(mp1), choosenCD);
          break;
        default:
          if(SHORT1FROMMP(mp2)==IDSL_POSITION) {
            cdPlayPos(LONGFROMMP(mp1),SHORT2FROMMP(mp2) ,choosenCD);
          }
          break;
        }
      return (MRESULT)FALSE;
    default:
      break;
    }
  return (MRESULT)FALSE;

}


void _Optlink objectThreadFunc (void *arg)
{
  HWND hwnd;
  HAB  hab;
  HMQ  hmq;
  QMSG qmsg;

  hab=WinInitialize(0);
  if(hab) {
    hmq=WinCreateMsgQueue(hab,0);
    if(hmq) {
      hwnd=WinCreateWindow(HWND_OBJECT,WC_STATIC,"cdPlayerObj",0,0,0,0,0,NULLHANDLE,HWND_BOTTOM,12343,NULL,NULL);
      if(hwnd) {
        WinSubclassWindow(hwnd,&cdPlayerObjectProc);
        /* Window created. */             
        hwndObject=hwnd;
        while(WinGetMsg(hab,&qmsg,(HWND)NULL,0,0))
          WinDispatchMsg(hab,&qmsg);
        WinDestroyWindow(hwnd);
      }
      WinDestroyMsgQueue(hmq);
    }
    WinTerminate(hab);
  }
}


int main (int argc, char *argv[])
{
  HAB  hab;
  HMQ  hmq;
  QMSG qmsg;

  hab=WinInitialize(0);
  if(hab) {
    hmq=WinCreateMsgQueue(hab,0);
    if(hmq) {  
      if(initData()) {
        if(_beginthread(objectThreadFunc,NULL, 8192*16,NULL)!=-1) {  
          if( WinDlgBox( HWND_DESKTOP, NULLHANDLE, cdPlayerDialogProc, NULLHANDLE, IDDLG_MAINDLG, 0) == DID_ERROR )
            {
              WinDestroyMsgQueue( hmq );
              WinTerminate( hab );
              DosBeep(100,600);
              return( 1 );
            }
          /* Save config to ini */
          saveData();
        }
      }/* if(initData) */
      WinDestroyMsgQueue(hmq);
    }
    WinTerminate(hab);
  }
  return 0;
}











