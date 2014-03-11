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

/* This contains the position information for the dialog controls */
#include "controls.h"


MRESULT paintControls(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  USERBUTTON* ubPtr;
  CONTROLINFO ci;
  RECTL rcl;
  SWP swp;

  ubPtr=(USERBUTTON*)PVOIDFROMMP(mp2);
  switch(SHORT1FROMMP(mp1))
    {
    case IDPB_PLAY:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciPlayButton;
      else
        ci=ciPlayButtonSel;
      break;                                        
    case IDPB_PAUSE:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciPauseButton;
      else
        ci=ciPauseButtonSel;
      break;                                        
    case IDPB_STOP:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciStopButton;
      else
        ci=ciStopButtonSel;
      break;                                        
    case IDPB_PREVIOUS:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciBackButton;
      else
        ci=ciBackButtonSel;
      break;                                        
    case IDPB_NEXT:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciNextButton;
      else
        ci=ciNextButtonSel;
      break;                                        
    case IDPB_EJECT:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciEjectButton;
      else
        ci=ciEjectButtonSel;
      break;                                        
    case IDPB_STEREO:
      if(bStereo)
        ci=ciStereoButton;
      else
        ci=ciStereoButtonSel;
      break;                                        
    case IDPB_MONO:
      if(bStereo)
        ci=ciMonoButtonSel;
      else
        ci=ciMonoButton;
      break;                                        
    case IDPB_CLOSE:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciCloseButton;
      else
        ci=ciCloseButtonSel;
      break;                                        
    case IDPB_MIN:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciMinButton;
      else
        ci=ciMinButtonSel;
      break;                                        
    case IDPB_SHRINK:
      WinQueryWindowPos(hwnd,&swp);
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciShrinkButton;
      else
        ci=ciShrinkButtonSel;
      break;                                        
    case IDPB_CLOSETBPLAYER:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciCloseButtonTB;
      else
        ci=ciCloseButtonTBSel;
      break; 
    case IDPB_MINTBPLAYER:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciMinButtonTB;
      else
        ci=ciMinButtonTBSel;
      break;                                        
    case IDPB_EXPANDTBPLAYER:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciExpandButton;
      else
        ci=ciExpandButtonSel;
      break;           
    case IDPB_TITLEBAR:
      ci=ciTitleBar;
      break;                                        
    case IDPB_TITLEPLAYER:
      ci=ciTitleBarPlayer;
      break;                                        
    case IDPB_SHUFFLE:
      if(bShuffle) {
        if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
          ci=ciShuffleButton;
        else 
          ci=ciShuffleButtonSel;
      }
      else {
        if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
          ci=ciNoShuffleButton;
        else 
          ci=ciNoShuffleButtonSel;
      }
      break;                                        
    case IDPB_REPEAT:
      if(bRepeat) {
        if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
          ci=ciRepeatButton;
        else 
          ci=ciRepeatButtonSel;
      }
      else {
        if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
          ci=ciNoRepeatButton;
        else 
          ci=ciNoRepeatButtonSel;
      }
      break;                                        
    case IDPB_PL:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciPLButton;
      else
        ci=ciPLButtonSel;
      break; 
    case IDPB_EQ:
      if(ubPtr->fsState==BDS_DEFAULT||!ubPtr->fsState)
        ci=ciEqButton;
      else
        ci=ciEqButton;
      break; 
    case IDPB_POSBAR:
      ci=ciPosSlider;
      break;                                        
    case IDSL_POSITION:
      ci=ciPosSliderArm;
      break;                                        
    default:
      return (MRESULT)0;
    }
  rcl.yBottom=0;
  rcl.xLeft=0;
  WinDrawBitmap(ubPtr->hps, ci.hbmSource,
                &ci.rclSource, 
                (PPOINTL)&rcl,
                0, 0,
                DBM_IMAGEATTRS);    
  if(ubPtr->fsStateOld!=ubPtr->fsState)
    ubPtr->fsState =( ubPtr->fsStateOld==BDS_DEFAULT ? BDS_HILITED : BDS_DEFAULT);
  return (MRESULT)0;
}

