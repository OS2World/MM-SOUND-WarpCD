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

HPOINTER hptrHand;

PFNWP oldStaticTextProc;

MRESULT EXPENTRY urlProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY optionsProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  int a;
  char chrCD[4];
  FILEDLG fd = { 0 };

   switch (msg) 
    {  
    case WM_INITDLG:
      oldStaticTextProc=WinSubclassWindow(WinWindowFromID(hwnd, IDST_CUSTOMIZE), urlProc);
      oldStaticTextProc=WinSubclassWindow(WinWindowFromID(hwnd, IDST_WINAMP), urlProc);
      if(!hptrHand)      
        hptrHand=WinLoadPointer(HWND_DESKTOP,0,IDPT_HAND);
      for( a=0; a<iNumCD ;a++) {
        chrCD[0]=cFirstCD+a;
        chrCD[1]=':';
        chrCD[2]=0;
        WinSendMsg(WinWindowFromID(hwnd,IDLB_DRIVE),LM_INSERTITEM,MPFROMSHORT(LIT_END),(MPARAM)chrCD);
      }
      WinSetWindowText(WinWindowFromID(hwnd,IDLB_DRIVE),choosenCD);
      WinSendMsg(WinWindowFromID(hwnd,IDEF_SKINDIRECTORY),EM_SETTEXTLIMIT,MPFROMSHORT((SHORT)CCHMAXPATH),0);
      WinSetWindowText(WinWindowFromID(hwnd,IDEF_SKINDIRECTORY),skinDir);
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
          WinQueryWindowText(WinWindowFromID(hwnd,IDEF_SKINDIRECTORY),sizeof(skinDir),skinDir);
          WinQueryWindowText(WinWindowFromID(hwnd,IDLB_DRIVE),sizeof(choosenCD),choosenCD);
          WinDismissDlg(hwnd,0);
          break;
        case IDPB_CANCEL:
          WinDismissDlg(hwnd,0);
          break;
        case IDPB_BROWSE:
          fd.cbSize = sizeof( fd );
          fd.fl = FDS_SAVEAS_DIALOG|FDS_CENTER;
          fd.pszTitle = "Search skin directory";
          sprintf(fd.szFullFile,"%s","Select directory and press OK");
          
          if( WinFileDlg( HWND_DESKTOP, hwnd, &fd ) == NULLHANDLE )
            {
              break;
            }
          if( fd.lReturn == DID_OK )
            {
              *(strrchr(fd.szFullFile,'\\'))=0;
              WinSetWindowText( WinWindowFromID(hwnd,IDEF_SKINDIRECTORY), fd.szFullFile );
            }
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

