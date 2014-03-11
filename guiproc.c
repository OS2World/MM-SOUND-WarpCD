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
#include <sys\types.h>
#include <sys\stat.h>
#include <dirent.h>

#include "common.h"
#include "cddb.h"
#include "cdplayerres.h"

/* This contains the position information for the dialog controls */
#include "controls.h"

BOOL CDQueryCDDrives(int *iNumCD, char * cFirstDrive);
int CDQueryAudioCDTracks(HFILE hfDrive);
HFILE openDrive(char* drive);
ULONG closeDrive(HFILE hfDrive);
BOOL CDPlayTrack(ULONG numTrack, char * drive);
BOOL CDStop(char * drive);

MRESULT paintControls(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void setupControlInfo();

MRESULT EXPENTRY buttonProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY staticNumbersProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY staticProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY sliderProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY sliderMainProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY aboutProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY optionsProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

static void getBmpSize(ULONG *cx, ULONG *cy,PBITMAPINFOHEADER2 pbmpih2)
{
    if(!pbmpih2) {/* Catch error */
        *cx=(ULONG)0;
        *cy=(ULONG)0;
        return;
    }    
    if (pbmpih2->cbFix == sizeof(BITMAPINFOHEADER))           /* old format? */
        {
            *cx=(ULONG)((PBITMAPINFOHEADER)pbmpih2)->cx;
            *cy=(ULONG)((PBITMAPINFOHEADER)pbmpih2)->cy;
        }
    else                               /* new PM format, Windows, or other */
        {
            *cx=pbmpih2->cx;
            *cy=pbmpih2->cy;
        }               
}

static void adjustDialogControlPos(HWND hwnd)
{
  CONTROLINFO ci;
  char text[CCHMAXPATH];
  ULONG ulCx;
  ULONG ulCy;
  ULONG ulStyle;

      /* Get size of Main bmp */
      getBmpSize(& ulCx,  &ulCy,pbmpi2MainBitmapFile);


      /* Move Buttons and set to BS_USERBUTTON to receive paint msg */
      ci=ciPlayButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);
      ci=ciPauseButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciStopButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciBackButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciNextButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciEjectButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciStereoButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);
      oldButtonProc=WinSubclassWindow(WinWindowFromID(hwnd, ci.id),buttonProc);

      ci=ciMonoButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);
      oldButtonProc=WinSubclassWindow(WinWindowFromID(hwnd, ci.id),buttonProc);
      
      ci=ciNoShuffleButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciNoRepeatButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);
   
      ci=ciPLButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      ci=ciEqButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft,ci.rclDest.yTop-ci.rclDest.yBottom,SWP_MOVE|SWP_SIZE);

      /* ----------- Setup titlebar -------------*/      
      ci=ciTitleBar;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP, ci.rclDest.xLeft, ulCy-ci.rclSource.yTop+ci.rclSource.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft, ci.rclDest.yTop-ci.rclDest.yBottom, SWP_MOVE|SWP_SIZE);
      oldButtonProc=WinSubclassWindow(WinWindowFromID(hwnd, ci.id),buttonProc);

      ci=ciCloseButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,263,2,
                      9,9,SWP_MOVE|SWP_SIZE);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_TITLEBAR),FALSE);

      ci=ciMinButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,243,2,
                      9,9,SWP_MOVE|SWP_SIZE);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_TITLEBAR),FALSE);

      ci=ciShrinkButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,253,2,
                      9,9,SWP_MOVE|SWP_SIZE);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_TITLEBAR),FALSE);

      /* --------- Setup titlebar player -------------*/      
      ci=ciTitleBarPlayer;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP, ci.rclDest.xLeft,0,
                      ci.rclDest.xRight-ci.rclDest.xLeft, ci.rclDest.yTop-ci.rclDest.yBottom, SWP_MOVE|SWP_SIZE|SWP_HIDE);
      oldButtonProc=WinSubclassWindow(WinWindowFromID(hwnd, ci.id),buttonProc);

      ci=ciCloseButtonTB;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,263,1,
                      9,9,SWP_MOVE|SWP_SIZE);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_TITLEPLAYER),FALSE);

      ci=ciExpandButton;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,253,1,
                      9,9,SWP_MOVE|SWP_SIZE);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_TITLEPLAYER),FALSE);
      
      ci=ciMinButtonTB;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP,243,1,
                      9,9,SWP_MOVE|SWP_SIZE);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_TITLEPLAYER),FALSE);

      /* Time */
      WinSetWindowPos(WinWindowFromID(hwnd, IDST_TIMETBPLAYER),HWND_TOP, 132, 3,
                      28, 8, SWP_MOVE|SWP_SIZE);
      oldStaticProc=WinSubclassWindow(WinWindowFromID(hwnd, IDST_TIMETBPLAYER),staticProc);      
      WinSetParent(WinWindowFromID(hwnd, IDST_TIMETBPLAYER),WinWindowFromID(hwnd, IDPB_TITLEPLAYER),FALSE);

      /* --------- Setup titlebar player END -----------*/      

      ci=ciPosSlider;
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP, ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft, ci.rclDest.yTop-ci.rclDest.yBottom, SWP_MOVE|SWP_SIZE);
      oldButtonProc=WinSubclassWindow(WinWindowFromID(hwnd, ci.id),sliderMainProc);

      ci=ciPosSliderArm;
      WinSetWindowULong(WinWindowFromID(hwnd, IDPB_POSBAR),QWL_USER,(ULONG)WinWindowFromID(hwnd, ci.id));
      ulStyle=WinQueryWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE);
      WinSetWindowULong(WinWindowFromID(hwnd, ci.id),QWL_STYLE,ulStyle|BS_USERBUTTON);
      WinSetWindowPos(WinWindowFromID(hwnd, ci.id),HWND_TOP, ci.rclDest.xLeft,ci.rclDest.yBottom,
                      ci.rclDest.xRight-ci.rclDest.xLeft, ci.rclDest.yTop-ci.rclDest.yBottom, SWP_MOVE|SWP_SIZE);
      WinSubclassWindow(WinWindowFromID(hwnd, ci.id), sliderProc);
      WinSetParent(WinWindowFromID(hwnd, ci.id),WinWindowFromID(hwnd, IDPB_POSBAR),FALSE);
      
      WinSetWindowPos(WinWindowFromID(hwnd, IDST_KHZ),HWND_TOP, 156, 67,
                      10, 9, SWP_MOVE|SWP_SIZE);
      oldStaticProc=WinSubclassWindow(WinWindowFromID(hwnd, IDST_KHZ),staticProc);

      WinSetWindowPos(WinWindowFromID(hwnd, IDST_TRACK),HWND_TOP, 114, 83,
                      120, 19, SWP_MOVE|SWP_SIZE);
      oldStaticProc=WinSubclassWindow(WinWindowFromID(hwnd, IDST_TRACK),staticProc);

      WinSetWindowPos(WinWindowFromID(hwnd, IDST_MINS),HWND_TOP, 48, 77,
                      22, 19, SWP_MOVE|SWP_SIZE);
      WinSubclassWindow(WinWindowFromID(hwnd, IDST_MINS),staticNumbersProc);

      WinSetWindowPos(WinWindowFromID(hwnd, IDST_SECS),HWND_TOP, 78, 77,
                      22, 19, SWP_MOVE|SWP_SIZE);
      WinSubclassWindow(WinWindowFromID(hwnd, IDST_SECS),staticNumbersProc);

      sprintf(text,"TRACK %d",iCurrentTrack);
      WinSetWindowText(WinWindowFromID(hwnd,IDST_TRACK),text);

      /* Size dialog */
      if(bTBSize) {
        WinShowWindow(WinWindowFromID(hwnd,IDPB_TITLEBAR),FALSE);
        WinShowWindow(WinWindowFromID(hwnd,IDPB_TITLEPLAYER),TRUE);
        WinSetWindowPos(hwnd,HWND_TOP,iXPos,iYPos,TITLEBARCX,12,SWP_ZORDER|SWP_ACTIVATE|SWP_SIZE|SWP_MOVE);
      }
      else
        WinSetWindowPos(hwnd,HWND_TOP,iXPos,iYPos,ulCx,ulCy,SWP_ZORDER|SWP_ACTIVATE|SWP_SIZE|SWP_MOVE);
}

#define BUTTONCX 30
#define BUTTONCY 30
#define DELTA    5
MRESULT EXPENTRY selectTrackProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SWP swp;
  SWP swpCancel;
  LONG lTitleBarCY;
  LONG lDlgBorderCY;
  LONG lDlgBorderCX;
  int iNumRows;
  int iNumColums;
  char buttonText[4];
  int a,b;
  POINTL ptl;
  LONG lScreenCX;
  LONG lScreenCY;

  switch (msg) 
    {  
    case WM_INITDLG:
      lTitleBarCY=WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR);      /* Height of titlebar */
      lDlgBorderCY=WinQuerySysValue(HWND_DESKTOP,SV_CYDLGFRAME);
      lDlgBorderCX=WinQuerySysValue(HWND_DESKTOP,SV_CXDLGFRAME);
      lScreenCX=WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN);
      lScreenCY=WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN);

      if(iNumTracks==0) {
        WinDismissDlg(hwnd,0);
        return (MRESULT) FALSE;
      }

      if(iNumTracks<=10) {
        iNumRows=1;
        iNumColums=iNumTracks;
      }
      else {
        iNumRows=iNumTracks/10+1; /* 10 tracks per row */
        iNumColums=10; 
      }
      
      /* Dialog window */
      WinQueryWindowPos(hwnd,&swp);
      /* Cancel button */
      WinQueryWindowPos(WinWindowFromID(hwnd,IDPB_CANCEL),&swpCancel);
      /* Size dialog */
      swp.cy=swpCancel.y+swpCancel.cy+lDlgBorderCY*2+lTitleBarCY+iNumRows*(BUTTONCY+DELTA)+DELTA;
      swp.cx=lDlgBorderCX*2+iNumColums*(BUTTONCX+DELTA)+DELTA;
      /* Create track buttons and position it */
      for(a=0;a<iNumTracks;a++) {    
        b=a/10;
        if(a+1==iCurrentTrack)
          snprintf(buttonText,sizeof(buttonText),">>");
        else
          snprintf(buttonText,sizeof(buttonText),"%d",a+1);
        WinCreateWindow(hwnd,WC_BUTTON,buttonText,WS_VISIBLE,(a%10)*(BUTTONCX+DELTA)+DELTA+lDlgBorderCX,
                        DELTA-swpCancel.y-swpCancel.cy-b*(BUTTONCY+DELTA)+swp.cy-lDlgBorderCY-lTitleBarCY,
                        BUTTONCX,BUTTONCY,hwnd,HWND_TOP,a+1,NULLHANDLE,NULLHANDLE);
      }
      swpCancel.x=DELTA+lDlgBorderCX;
      WinSetWindowPos(WinWindowFromID(hwnd,IDPB_CANCEL),HWND_TOP,swpCancel.x, swpCancel.y,0,0,SWP_MOVE);
      WinQueryPointerPos(HWND_DESKTOP,&ptl);
      if(ptl.x>(lScreenCX-swp.cx))
        swp.x=lScreenCX-swp.cx;
      else
        swp.x=ptl.x;
      if(ptl.y>(lScreenCY-swp.cy))
        swp.y=lScreenCY-swp.cy;
      else
        swp.y=ptl.y;
        
      WinSetWindowPos(hwnd,HWND_TOP,swp.x,swp.y,swp.cx,swp.cy,SWP_SIZE|SWP_MOVE|SWP_ZORDER|SWP_ACTIVATE);
      return (MRESULT) FALSE;
      case WM_COMMAND:
      switch(SHORT1FROMMP(mp1))
        {
        case IDPB_CANCEL:
          WinDismissDlg(hwnd,0);
          break;
        default:
          if(SHORT1FROMMP(mp1)>99)
            break;
          iCurrentTrack=SHORT1FROMMP(mp1);
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(WinQueryWindow(hwnd,QW_OWNER)),MPFROMSHORT(IDPB_PLAY));
          WinDismissDlg(hwnd,0);
          break;
        }
      return (MRESULT) FALSE;
    default:
      break;
    }
  return WinDefDlgProc(hwnd, msg, mp1, mp2) ;
}

//skinArray
BOOL scanSkinDir(int * iNumSkins)
{
  DIR * dir;
  struct dirent *dirEnt;
  int a;
  char tempName[CCHMAXPATH];
  struct stat Stat;

  dir=opendir(skinDir);
  if(!dir)
    return FALSE;

  a=0;
  do {
    dirEnt=readdir(dir);
    if(dirEnt) {
      if((dirEnt->d_attr & A_DIR)&& !(dirEnt->d_attr & A_HIDDEN))/* Only dirs */
        if(strcmp(dirEnt->d_name,".") && strcmp(dirEnt->d_name,"..")) /* skip . and .. */
          {
            snprintf(tempName,sizeof(tempName),"%s\\%s\\main.bmp",skinDir,dirEnt->d_name);
            if(!stat(tempName,&Stat)) {            
              strcpy(skinArray[a],dirEnt->d_name);
              a++;
            }
          }
    }    
  } while(dirEnt && a < NUM_SKINS_IN_MENU);
  
  closedir(dir);
  *iNumSkins=a; /* return num of skins */
  return TRUE;

}

void contextMenu(HWND hwnd)
{
  POINTL ptl;
  HWND   hwndPopup;
  char menuText[CCHMAXPATH];
  MENUITEM subMenuItem;
  HWND hwndTemp;
  int iNumSkins, a;

  hwndPopup = WinLoadMenu(hwnd, NULLHANDLE, IDM_POPUP) ;
  if (hwndPopup == NULLHANDLE) {
    DosBeep(100,1000);
    return;
  }

  WinSendMsg(hwndPopup,MM_QUERYITEM,MPFROM2SHORT(IDM_ITEMSKINMENU,TRUE),(MPARAM)&subMenuItem);
  hwndTemp=subMenuItem.hwndSubMenu;

  /* Get skins in skindir */
  scanSkinDir(&iNumSkins);

  for(a=0;a<iNumSkins;a++) {
    /*insert menuitem in popupmenu*/
    subMenuItem.iPosition=MIT_END;
    subMenuItem.afStyle=MIS_TEXT;
    subMenuItem.afAttribute=NULLHANDLE;
    subMenuItem.id=5001+a;//ITEM-ID 
    subMenuItem.hwndSubMenu=NULLHANDLE;
    subMenuItem.hItem=NULLHANDLE;
    strncpy(menuText,skinArray[a],sizeof(menuText));
    WinSendMsg(hwndTemp,MM_INSERTITEM,(MPARAM)&subMenuItem,(MPARAM)&menuText);
    /* Check selected skin */
    snprintf(menuText,sizeof(menuText),"%s\\%s",installDir,skinArray[a]);
    if(!strcmp(strlwr(currentSkin),strlwr(menuText)))
      WinSendMsg(hwndTemp,MM_SETITEMATTR,MPFROM2SHORT(5001+a,FALSE), MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
  }
  snprintf(menuText,sizeof(menuText),"%s\\Default",installDir);
  if(!strcmp(strlwr(currentSkin),strlwr(menuText)))
    WinSendMsg(hwndTemp,MM_SETITEMATTR,MPFROM2SHORT(IDM_ITEMDEFAULTSKIN,FALSE), MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));

  WinQueryPointerPos(HWND_DESKTOP, &ptl) ;
  WinMapWindowPoints(HWND_DESKTOP,hwnd,&ptl,1);
  WinPopupMenu(hwnd, hwnd, hwndPopup, 
               ptl.x, ptl.y, IDM_ITEMEXIT, PU_HCONSTRAIN | PU_VCONSTRAIN |
               PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_NONE  ); 
  
}

/* This Proc handles the main dialog */
MRESULT EXPENTRY cdPlayerDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  char text[CCHMAXPATH];
  int a;
  ULONG rc;
  SUBCHANNELQ scq;

  SWCNTRL swctl;
  PID pid;

  HPS hps;
  RECTL rcl;
  ULONG ulCx;
  ULONG ulCy;
  CONTROLINFO ci;

  PSWP pswp;
  SWP swp;
  FRAMECDATA fcd;
  LONG lPos;

  HWND hwndTB;

  switch (msg)
    {      
    case WM_INITDLG:   
      
      /* Add switch entry */
      memset(&swctl,0,sizeof(swctl));
      WinQueryWindowProcess(hwnd,&pid,NULL);
      swctl.hwnd=hwnd;
      snprintf(swctl.szSwtitle,MAXNAMEL,"WarpCD");
      swctl.uchVisibility=SWL_VISIBLE;
      swctl.idProcess=pid;
      swctl.bProgType=PROG_DEFAULT;
      swctl.fbJump=SWL_JUMPABLE;
      WinAddSwitchEntry(&swctl);

      hptrIcon=WinLoadPointer(HWND_DESKTOP,0,1);
      WinSendMsg(hwnd,WM_SETICON,(MPARAM)hptrIcon,NULL);

      adjustDialogControlPos(hwnd);

      return (MRESULT) TRUE;
    case WM_BUTTON2CLICK  :
      contextMenu(hwnd);
      break;
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON1MOTIONSTART:
      WinSendMsg(hwnd, WM_TRACKFRAME, 
                 MPFROMSHORT(TF_MOVE   /*| TF_SETPOINTERPOS */), NULL) ;
      return (MRESULT) 0 ;

    case WM_CLOSE:
      cdStop(hwnd,choosenCD);
      if(hptrIcon)
        WinDestroyPointer(hptrIcon);
      /* Save last window position */
      WinQueryWindowPos(hwnd, &swp);
      iXPos=swp.x;
      iYPos=swp.y;
      WinDismissDlg(hwnd,0);
      return FALSE;
    case WM_TIMER:
      strcpy(text,choosenCD);
      if(CDQueryPaused(text)==1)
        {
          /* We are pausing flash the display */
        if(WinIsWindowVisible(WinWindowFromID(hwnd,IDST_TRACK)))
          WinShowWindow(WinWindowFromID(hwnd,IDST_TRACK),FALSE);
        else
          WinShowWindow(WinWindowFromID(hwnd,IDST_TRACK),TRUE);
        if(WinIsWindowVisible(WinWindowFromID(hwnd,IDPB_TITLEPLAYER))) {
          hwndTB=WinWindowFromID(hwnd,IDPB_TITLEPLAYER);
          if(WinIsWindowVisible(WinWindowFromID(hwndTB,IDST_TIMETBPLAYER)))
            WinShowWindow(WinWindowFromID(hwndTB,IDST_TIMETBPLAYER),FALSE);
          else
            WinShowWindow(WinWindowFromID(hwndTB,IDST_TIMETBPLAYER),TRUE);          
        }
        }/* if(CDQueryPaused(text)==1) */
      else {
        if(CDQueryIsPlaying(text)==1) {
          if(CDQuerySubChannelQ(&scq, text)) {
            sprintf(text,"%02d",scq.ucTrackMins);
            WinSetWindowText(WinWindowFromID(hwnd,IDST_MINS),text);
            sprintf(text,"%02d",scq.ucTrackSecs);
            WinSetWindowText(WinWindowFromID(hwnd,IDST_SECS),text);
            sprintf(text,"%02d:%02d",scq.ucTrackMins,scq.ucTrackSecs);
            WinSetWindowText(WinWindowFromID(WinWindowFromID(hwnd,IDPB_TITLEPLAYER),IDST_TIMETBPLAYER),text);
            WinQueryWindowPos(WinWindowFromID(hwnd,IDPB_POSBAR),&swp);
            lPos=(scq.ucTrackMins*60+scq.ucTrackSecs)*(swp.cx-30);
            lPos/=(msfEnd.ucMinutes*60+msfEnd.ucSeconds-msfStart.ucMinutes*60-msfStart.ucSeconds);
            /* Set slider position */
            WinSendMsg(WinWindowFromID(hwnd,IDPB_POSBAR),SLM_SETSLIDERINFO,
                       MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),MPFROMLONG(lPos));                         
          }
        }
        else /* not playing */
          {
            if(bShuffle) {
              if(track[100]!=iNumTracks || bRepeat) {
                cdSkipForward(hwnd , text);
                cdPlayTrack(hwnd, iCurrentTrack, text);
              }
            }/* if(bShuffle) */
            else {
              if(iCurrentTrack<iNumTracks) {
                iCurrentTrack++;
                cdPlayTrack(hwnd, iCurrentTrack, text);
              }
              else {
                if(bRepeat) {
                  iCurrentTrack=1;
                  cdPlayTrack(hwnd, iCurrentTrack, text);
                }
                else
                  cdStop(hwnd,text);
              }
            }
          }/* not playing */
      }
      return (MRESULT)FALSE;
    case WM_PAINT:
      WinQueryWindowPos(hwnd,&swp);
      if(swp.fl&SWP_MINIMIZE)
        break;      
      hps = WinBeginPaint( hwnd, NULLHANDLE, &rcl );
      WinDrawBitmap(hps, hbmMain,
                    &rcl, 
                    (PPOINTL)&rcl,
                    0, 0,
                    DBM_IMAGEATTRS);    
      WinEndPaint( hps );        
      return FALSE;
    case WM_CONTROL:
      switch(SHORT2FROMMP(mp1))
        {
        case BN_PAINT:
          WinQueryWindowPos(hwnd,&swp);
          if(swp.fl&SWP_MINIMIZE)
            break;          
          return paintControls(hwnd, msg, mp1, mp2);
        case SLN_CHANGE:
          switch(SHORT1FROMMP(mp1))
            {
            case IDSL_POSITION:
              lPos=LONGFROMMP(mp2);
              WinStopTimer(WinQueryAnchorBlock(HWND_DESKTOP),hwnd,CDPLAYER_TIMER);
              WinQueryWindowPos(WinWindowFromID(hwnd,IDPB_POSBAR),&swp);
              WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROM2SHORT(IDSL_POSITION,
                                                                                        (SHORT) ((lPos)*100)/(swp.cx-30)));
              return (MRESULT)TRUE;
            default:
              break;
            }
          break;
        default:
          break;
        }
      break;
    case WM_MINMAXFRAME:
      pswp=PVOIDFROMMP(mp1);
      if(pswp->fl&SWP_RESTORE) {
        WinDestroyWindow(WinWindowFromID(hwnd,FID_SYSMENU));
        WinDestroyWindow(WinWindowFromID(hwnd,FID_MINMAX));
      }
      break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1))
        {
          /* Popup menu items */
        case IDM_ITEMEXIT:
          WinPostMsg(hwnd,WM_CLOSE,0,0);
          break;
        case IDM_ITEMOPTIONS:
          WinDlgBox( HWND_DESKTOP, hwnd, optionsProc, NULLHANDLE, IDDLG_OPTIONS, 0);
          break;
        case IDM_ITEMABOUT:
          WinDlgBox( HWND_DESKTOP, hwnd, aboutProc, NULLHANDLE, IDDLG_ABOUT, 0);
          break;
          /* End menu items */
        case IDPB_SHRINK:
          bTBSize=TRUE;
          WinQueryWindowPos(hwnd,&swp);
          WinShowWindow(WinWindowFromID(hwnd,IDPB_TITLEBAR),FALSE);
          WinShowWindow(WinWindowFromID(hwnd,IDPB_TITLEPLAYER),TRUE);
          WinSetWindowPos(hwnd,0,swp.x,swp.y+swp.cy-12,TITLEBARCX,12,SWP_SIZE|SWP_MOVE);
          break;
        case IDPB_EXPANDTBPLAYER:        
          bTBSize=FALSE;
          /* Get size of Main bmp */
          getBmpSize(&ulCx, &ulCy,pbmpi2MainBitmapFile);
          WinQueryWindowPos(hwnd,&swp);
          WinShowWindow(WinWindowFromID(hwnd,IDPB_TITLEPLAYER),FALSE);
          WinShowWindow(WinWindowFromID(hwnd,IDPB_TITLEBAR),TRUE);
          WinSetWindowPos(hwnd,0,swp.x,swp.y-ulCy+12,ulCx,ulCy,SWP_SIZE|SWP_MOVE);
          break;
        case IDPB_MIN:         
        case IDPB_MINTBPLAYER:
          fcd.cb=sizeof(fcd);
          fcd.flCreateFlags=FCF_SYSMENU|FCF_MINBUTTON;
          fcd.hmodResources=0;
          fcd.idResources=0;
          WinCreateFrameControls(hwnd,&fcd,"");
          WinSetWindowPos(hwnd,0,0,0,0,0,SWP_MINIMIZE);
          break;
        case IDPB_CLOSE:
        case IDPB_CLOSETBPLAYER:
          WinSendMsg(hwnd,WM_CLOSE,(MPARAM)0,(MPARAM)0);
          break;
        case IDPB_PL:
          cdQueryTracks(hwnd , choosenCD); 
          WinDlgBox( HWND_DESKTOP, hwnd, selectTrackProc, NULLHANDLE, IDDLG_SELECTTRACK, 0);
          break;
        case IDPB_SHUFFLE:
          if(bShuffle)
            bShuffle=FALSE;
          else {
            bShuffle=TRUE;
            initTrackArray();
          }
          ci=ciNoShuffleButton;
          WinInvalidateRect(WinWindowFromID(hwnd, ci.id), NULL, FALSE);
          break;
        case IDPB_REPEAT:
          if(bRepeat)
            bRepeat=FALSE;
          else
            bRepeat=TRUE;
          ci=ciNoRepeatButton;
          WinInvalidateRect(WinWindowFromID(hwnd, ci.id), NULL, FALSE);
          break;
        case IDPB_EJECT:
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROMSHORT(IDPB_EJECT));
          break;
        case IDPB_PLAY:
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROMSHORT(IDPB_PLAY));
          break;
        case IDPB_STOP:
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROMSHORT(IDPB_STOP));
          break;
        case IDPB_PAUSE:
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROMSHORT(IDPB_PAUSE));
          break;
        case IDPB_NEXT:
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROMSHORT(IDPB_NEXT));
          break;
        case IDPB_PREVIOUS:
          WinPostMsg(hwndObject,WM_APPTERMINATENOTIFY,MPFROMLONG(hwnd),MPFROMSHORT(IDPB_PREVIOUS));
          break;
          /* Skin selection */
        case IDM_ITEMDEFAULTSKIN:  /* Default skin selected */
          snprintf(currentSkin,sizeof(currentSkin),"%s\\%s",installDir,"Default");
          /* Reload skin */
          loadBitmaps(currentSkin);
          WinQueryWindowRect(hwnd,&rcl);
          WinInvalidateRect(hwnd,&rcl,TRUE);        
          break;
        default:
          if(SHORT1FROMMP(mp1)>5000 && SHORT1FROMMP(mp1)<5000+NUM_SKINS_IN_MENU-1) {
            /* Skin item choosen */
            snprintf(currentSkin,sizeof(currentSkin),"%s\\%s",skinDir,skinArray[SHORT1FROMMP(mp1)-5001]);
            /* Reload skin */
            loadBitmaps(currentSkin);
            WinQueryWindowRect(hwnd,&rcl);
            WinInvalidateRect(hwnd,&rcl,TRUE);        
            break;
          }
          break;
        }
      return (MRESULT)FALSE;
    default:
      break;
    }

  return WinDefDlgProc(hwnd, msg, mp1, mp2);    
}

