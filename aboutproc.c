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
#include "cdplayerres.h"

PFNWP oldStaticTextProc;

HPOINTER hptrHand=NULLHANDLE;

MRESULT EXPENTRY urlProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  STARTDATA startData;
  char chrLoadError[CCHMAXPATH];
  ULONG ulSessionID;
  PID pid;
  APIRET rc;
  char chrURL[500];
  ULONG attrFound;
  ULONG attrValue[32];
  static ULONG ulFgColor;

   switch (msg) 
    { 
    case WM_BUTTON1DOWN:
      WinQueryPresParam(hwnd, PP_FOREGROUNDCOLORINDEX, PP_FOREGROUNDCOLOR,&attrFound,sizeof(ulFgColor),
                        &ulFgColor,QPF_ID1COLORINDEX|QPF_NOINHERIT|QPF_PURERGBCOLOR);
      attrValue[0]=0xff0000;
      WinSetPresParam(hwnd,PP_FOREGROUNDCOLOR,sizeof(ULONG),&attrValue);
      break;
    case WM_BUTTON1UP:
      if(ulFgColor)
        WinSetPresParam(hwnd,PP_FOREGROUNDCOLOR,sizeof(ULONG),&ulFgColor);
      else {
        ulFgColor=SYSCLR_WINDOWSTATICTEXT;
        WinSetPresParam(hwnd,PP_FOREGROUNDCOLOR,sizeof(ULONG),&ulFgColor);
      }
      break;
    case WM_BUTTON1CLICK:
      WinQueryWindowText(hwnd,sizeof(chrURL),chrURL);
      memset(&startData,0,sizeof(startData));
      startData.Length=sizeof(startData);
      startData.Related=SSF_RELATED_INDEPENDENT;
      startData.FgBg=SSF_FGBG_FORE;
      startData.TraceOpt=SSF_TRACEOPT_NONE;
      startData.PgmTitle="";
      startData.PgmName="netscape.exe";
      startData.InheritOpt=SSF_INHERTOPT_SHELL;
      startData.SessionType=SSF_TYPE_DEFAULT;
      startData.PgmControl=SSF_CONTROL_VISIBLE;
      startData.InitXPos=30;
      startData.InitYPos=30;
      startData.InitXSize=500;
      startData.InitYSize=400;
      startData.ObjectBuffer=chrLoadError;
      startData.ObjectBuffLen=(ULONG)sizeof(chrLoadError);
      startData.PgmInputs=chrURL;
      rc=DosStartSession(&startData,&ulSessionID,&pid);					
     
      return (MRESULT) FALSE;; 
    case WM_MOUSEMOVE:
      if(hptrHand) {
          WinSetPointer(HWND_DESKTOP,hptrHand);
        }
        return (MRESULT)FALSE;
    default:
      break;
    }
   return oldStaticTextProc(hwnd, msg, mp1, mp2) ;
  return WinDefWindowProc(hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY aboutProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) 
    {  
    case WM_INITDLG:
      oldStaticTextProc=WinSubclassWindow(WinWindowFromID(hwnd, IDST_HOMEPAGE), urlProc);
      if(!hptrHand)      
        hptrHand=WinLoadPointer(HWND_DESKTOP,0,IDPT_HAND);
      return (MRESULT) FALSE;
    case WM_DESTROY:
      if(hptrHand) {
        WinDestroyPointer(hptrHand);
        hptrHand=NULLHANDLE;
      }
      break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1))
        {
        case IDPB_OK:
          WinDismissDlg(hwnd,0);
          break;
        default:
          break;
        }
      return (MRESULT) FALSE;
    default:
      break;
    }
  return WinDefDlgProc(hwnd, msg, mp1, mp2) ;
}

