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
#include "cd.h"

/* This contains the position information for the dialog controls */
#include "controls.h"

static void printSmallText(HPS hps, POINTL ptlDest,char *text)
{
  char * chrPtr;
  int num;
  chrPtr=text;
  num=0;

  /* ToDo: double buffering! */ 
    while(*chrPtr!=0)
    {
      ptlDest.x=num*5;
      ptlDest.y=0; 
      if(*chrPtr>=0x30 && *chrPtr<=0x3a) {
        WinDrawBitmap(hps, tbiSmallText[*chrPtr-0x30].hbmSource,
                      &tbiSmallText[*chrPtr-0x30].rclSource, 
                      &ptlDest,
                      0, 0,
                      DBM_IMAGEATTRS);    
      }
      if(*chrPtr>=0x41 && *chrPtr<=0x5a) {
        WinDrawBitmap(hps, tbiSmallText[*chrPtr-0x30].hbmSource,
                      &tbiSmallText[*chrPtr-0x30].rclSource, 
                      &ptlDest,
                      0, 0,
                      DBM_IMAGEATTRS);            
      }
      chrPtr++;
      num++;      
    }  
}

static void printNumbers(HPS hps, POINTL ptlDest,char *text)
{
  char * chrPtr;
  int num;
  chrPtr=text;
  num=0;

  /* ToDo: double buffering! */ 
    while(*chrPtr!=0)
    {
      ptlDest.x=num*12;
      ptlDest.y=0; 
      if(*chrPtr>=0x30 && *chrPtr<=0x39) {
        WinDrawBitmap(hps, tbiNumbers[*chrPtr-0x30].hbmSource,
                      &tbiNumbers[*chrPtr-0x30].rclSource, 
                      &ptlDest,
                      0, 0,
                      DBM_IMAGEATTRS);    
      }
      chrPtr++;
      num++;      
    }
}


#define dx -25
MRESULT EXPENTRY buttonProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SHORT sX;

  switch (msg) 
    {  
    case WM_BUTTON1CLICK  :
      if(WinQueryWindowUShort(hwnd,QWS_ID)==IDPB_TITLEPLAYER) {
        sX=SHORT1FROMMP(mp1);
        if(sX>=195+dx && sX<=202+dx) 
          cdSkipBack(WinQueryWindow(hwnd,QW_PARENT),choosenCD);
        else 
          if(sX>=204+dx && sX<=211+dx) 
            cdPlay(WinQueryWindow(hwnd,QW_PARENT), choosenCD);
          else
            if(sX>=214+dx && sX<=221+dx) 
              cdPause(WinQueryWindow(hwnd,QW_PARENT), choosenCD);
            else
              if(sX>=224+dx && sX<=231+dx) 
                cdStop(WinQueryWindow(hwnd,QW_PARENT),choosenCD);
              else
                if(sX>=233+dx && sX<=240+dx) 
                  cdSkipForward(WinQueryWindow(hwnd,QW_PARENT),choosenCD);
                else
                  if(sX>=243+dx && sX<=250+dx) 
                    cdEject(WinQueryWindow(hwnd,QW_PARENT), choosenCD);//DosBeep(500,30);
        return (MRESULT)TRUE;
      }
      break;

    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:   
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:   
    case WM_BUTTON1DOWN   :
    case WM_MOUSEMOVE     :   
    case WM_BUTTON1UP     :
    case WM_BUTTON1DBLCLK :
    case WM_BUTTON2DOWN   :
    case WM_BUTTON2UP     :
    case WM_BUTTON2CLICK  :
    case WM_BUTTON2DBLCLK :
    case WM_BUTTON3DOWN   :
    case WM_BUTTON3UP     :
    case WM_BUTTON3CLICK  :
    case WM_BUTTON3DBLCLK :
    case WM_CHAR          :
    case WM_VIOCHAR       :
    case WM_BEGINDRAG     :
    case WM_ENDDRAG       :
    case WM_SINGLESELECT  :
    case WM_OPEN          :
    case WM_CONTEXTMENU   :
    case WM_CONTEXTHELP   :
    case WM_TEXTEDIT      :
    case WM_BEGINSELECT   :
    case WM_ENDSELECT     :
      return WinSendMsg(WinQueryWindow(hwnd,QW_PARENT), msg, mp1, mp2) ;
    }
  return oldButtonProc(hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY sliderMainProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SHORT sX;
  HWND hwndSlider;

  switch (msg) 
    {  
    case SLM_SETSLIDERINFO:
      hwndSlider=(HWND)WinQueryWindowULong(hwnd,QWL_USER);
      if(!(WinQueryCapture(HWND_DESKTOP)==WinWindowFromID(hwnd,IDSL_POSITION)))
        WinSetWindowPos(hwndSlider,NULLHANDLE,SHORT1FROMMP(mp2),0,0,0,SWP_MOVE);
      return (MRESULT)FALSE;


    case WM_BUTTON1CLICK  :
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:   
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:   
    case WM_BUTTON1DOWN   :
      return (MRESULT)FALSE;
    case WM_MOUSEMOVE     :   
    case WM_BUTTON1UP     :
    case WM_BUTTON1DBLCLK :
    case WM_BUTTON2DOWN   :
    case WM_BUTTON2UP     :
    case WM_BUTTON2CLICK  :
    case WM_BUTTON2DBLCLK :
    case WM_BUTTON3DOWN   :
    case WM_BUTTON3UP     :
    case WM_BUTTON3CLICK  :
    case WM_BUTTON3DBLCLK :
    case WM_CHAR          :
    case WM_VIOCHAR       :
    case WM_BEGINDRAG     :
    case WM_ENDDRAG       :
    case WM_SINGLESELECT  :
    case WM_OPEN          :
    case WM_CONTEXTMENU   :
    case WM_CONTEXTHELP   :
    case WM_TEXTEDIT      :
    case WM_BEGINSELECT   :
    case WM_ENDSELECT     :
      return oldButtonProc(hwnd, msg, mp1, mp2) ;
    }
  return oldButtonProc(hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY sliderProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SWP swp,swp2;
  SHORT sX;
  POINTL ptl;
  ULONG ulWork;
  LONG lWork;

  switch (msg) 
    {  
    case WM_BUTTON1DOWN   :
      WinSetCapture(HWND_DESKTOP,hwnd);
      WinSetWindowULong(hwnd,QWL_USER,(ULONG)SHORT1FROMMP(mp1));
      break;

    case WM_BUTTON1UP     :
      WinSetCapture(HWND_DESKTOP,NULLHANDLE);
      WinQueryWindowPos(hwnd,&swp);
      lWork=swp.x;
      WinSendMsg(WinQueryWindow(WinQueryWindow(hwnd,QW_PARENT),QW_PARENT),WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hwnd,QWS_ID),SLN_CHANGE),MPFROMLONG(lWork));
      return FALSE;
      break;

    case WM_MOUSEMOVE     :   
      if(WinQueryCapture(HWND_DESKTOP)==hwnd) {
        ptl.x=SHORT1FROMMP(mp1);
        ptl.y=SHORT2FROMMP(mp1);
        WinQueryWindowPos(hwnd, &swp);
        ulWork=WinQueryWindowULong(hwnd,QWL_USER);
        WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp2);
        WinMapWindowPoints(hwnd,WinQueryWindow(hwnd,QW_PARENT),&ptl,2);
        lWork=ptl.x-((LONG)ulWork);
        if(lWork>=0 && lWork<swp2.cx-swp.cx) {
          WinSetWindowPos(hwnd, NULLHANDLE, ptl.x-(LONG)ulWork, swp.y, 0, 0, SWP_MOVE);
        }
      }
      break;
    case WM_BUTTON1CLICK  :
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:   
    
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:   

    case WM_BUTTON1DBLCLK :
    case WM_BUTTON2DOWN   :
    case WM_BUTTON2UP     :
    case WM_BUTTON2CLICK  :
    case WM_BUTTON2DBLCLK :
    case WM_BUTTON3DOWN   :
    case WM_BUTTON3UP     :
    case WM_BUTTON3CLICK  :
    case WM_BUTTON3DBLCLK :
    case WM_CHAR          :
    case WM_VIOCHAR       :
    case WM_BEGINDRAG     :
    case WM_ENDDRAG       :
    case WM_SINGLESELECT  :
    case WM_OPEN          :
    case WM_CONTEXTMENU   :
    case WM_CONTEXTHELP   :
    case WM_TEXTEDIT      :
    case WM_BEGINSELECT   :
    case WM_ENDSELECT     :
      return oldButtonProc(hwnd, msg, mp1, mp2) ;
    }
  return oldButtonProc(hwnd, msg, mp1, mp2) ;
}


MRESULT EXPENTRY staticProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

  HPS hps;
  RECTL rcl;
  POINTL ptl;
  char text[200];

  switch (msg) 
    {  
    case WM_PAINT:
      hps = WinBeginPaint( hwnd, NULLHANDLE, NULL );
      WinQueryWindowRect(hwnd, &rcl);
   
      WinQueryWindowText(hwnd,sizeof(text),text);
      ptl.x=rcl.xLeft;
      ptl.y=rcl.yBottom;
      printSmallText(hps,ptl,text);
      WinEndPaint( hps );        
      return (MRESULT) 0;
    case WM_SETWINDOWPARAMS:
      
      if(((PWNDPARAMS)PVOIDFROMMP(mp1))->fsStatus==WPM_TEXT)
        oldStaticProc(hwnd, msg, mp1, mp2);
      /* Force repainting of background */
      WinShowWindow(hwnd,FALSE);
      WinShowWindow(hwnd,TRUE);
      return (MRESULT)0;
        
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:   
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:   
    case WM_BUTTON1DOWN   :
    case WM_MOUSEMOVE     :   
    case WM_BUTTON1UP     :
    case WM_BUTTON1CLICK  :
    case WM_BUTTON1DBLCLK :
    case WM_BUTTON2DOWN   :
    case WM_BUTTON2UP     :
    case WM_BUTTON2CLICK  :
    case WM_BUTTON2DBLCLK :
    case WM_BUTTON3DOWN   :
    case WM_BUTTON3UP     :
    case WM_BUTTON3CLICK  :
    case WM_BUTTON3DBLCLK :
    case WM_CHAR          :
    case WM_VIOCHAR       :
    case WM_BEGINDRAG     :
    case WM_ENDDRAG       :
    case WM_SINGLESELECT  :
    case WM_OPEN          :
    case WM_CONTEXTMENU   :
    case WM_CONTEXTHELP   :
    case WM_TEXTEDIT      :
    case WM_BEGINSELECT   :
    case WM_ENDSELECT     :
      return WinSendMsg(WinQueryWindow(hwnd,QW_PARENT), msg, mp1, mp2) ;
    }
  return oldStaticProc(hwnd, msg, mp1, mp2) ;
}

MRESULT EXPENTRY staticNumbersProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

  HPS hps;
  RECTL rcl;
  POINTL ptl;
  char text[200];

  switch (msg) 
    {  
    case WM_PAINT:
      hps = WinBeginPaint( hwnd, NULLHANDLE, NULL );
      WinQueryWindowRect(hwnd, &rcl);
   
      WinQueryWindowText(hwnd,sizeof(text),text);
      ptl.x=rcl.xLeft;
      ptl.y=rcl.yBottom;
      printNumbers(hps,ptl,text);
      WinEndPaint( hps );        
      return (MRESULT) 0;
    case WM_SETWINDOWPARAMS:
      
      if(((PWNDPARAMS)PVOIDFROMMP(mp1))->fsStatus==WPM_TEXT)
        oldStaticProc(hwnd, msg, mp1, mp2);
      /* Force repainting of background */
      WinShowWindow(hwnd,FALSE);
      WinShowWindow(hwnd,TRUE);
      return (MRESULT)0;
        
    case WM_BUTTON1MOTIONSTART:
    case WM_BUTTON1MOTIONEND:   
    case WM_BUTTON2MOTIONSTART:
    case WM_BUTTON2MOTIONEND:   
    case WM_BUTTON1DOWN   :
    case WM_MOUSEMOVE     :   
    case WM_BUTTON1UP     :
    case WM_BUTTON1CLICK  :
    case WM_BUTTON1DBLCLK :
    case WM_BUTTON2DOWN   :
    case WM_BUTTON2UP     :
    case WM_BUTTON2CLICK  :
    case WM_BUTTON2DBLCLK :
    case WM_BUTTON3DOWN   :
    case WM_BUTTON3UP     :
    case WM_BUTTON3CLICK  :
    case WM_BUTTON3DBLCLK :
    case WM_CHAR          :
    case WM_VIOCHAR       :
    case WM_BEGINDRAG     :
    case WM_ENDDRAG       :
    case WM_SINGLESELECT  :
    case WM_OPEN          :
    case WM_CONTEXTMENU   :
    case WM_CONTEXTHELP   :
    case WM_TEXTEDIT      :
    case WM_BEGINSELECT   :
    case WM_ENDSELECT     :
      return WinSendMsg(WinQueryWindow(hwnd,QW_PARENT), msg, mp1, mp2) ;
    }
  return oldStaticProc(hwnd, msg, mp1, mp2) ;
}















