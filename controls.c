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

#define BTNX 16
#define BTNY 11


CONTROLINFO ciPlayButton={IDPB_PLAY,{23,18,45,35},NULLHANDLE, {BTNX+23,BTNY,BTNX+23+23,BTNY+18} };
CONTROLINFO ciPlayButtonSel={IDPB_PLAY,{23,0,45,17},NULLHANDLE, {BTNX+23,BTNY,BTNX+23+23,BTNY+18} };

CONTROLINFO ciPauseButton={IDPB_PAUSE,{46,18,68,35},NULLHANDLE, {BTNX+46,BTNY,BTNX+46+23,BTNY+18} };
CONTROLINFO ciPauseButtonSel={IDPB_PAUSE,{46,0,68,17},NULLHANDLE, {BTNX+46,BTNY,BTNX+46+23,BTNY+18} };

CONTROLINFO ciStopButton={IDPB_STOP,{69,18,91,35},NULLHANDLE, {BTNX+69,BTNY,BTNX+69+23,BTNY+18} };
CONTROLINFO ciStopButtonSel={IDPB_STOP,{69,0,91,17},NULLHANDLE, {BTNX+69,BTNY,BTNX+69+23,BTNY+18} };

CONTROLINFO ciBackButton={IDPB_PREVIOUS,{0,18,22,35},NULLHANDLE, {BTNX,BTNY,BTNX+0+23,BTNY+18} };
CONTROLINFO ciBackButtonSel={IDPB_PREVIOUS,{0,0,22,17},NULLHANDLE, {BTNX,BTNY,BTNX+0+23,BTNY+18} };

CONTROLINFO ciNextButton={IDPB_NEXT,{92,18,114,35},NULLHANDLE, {BTNX+92,BTNY,BTNX+92+23,BTNY+18} };
CONTROLINFO ciNextButtonSel={IDPB_NEXT,{92,0,114,17},NULLHANDLE, {BTNX+92,BTNY,BTNX+92+23,BTNY+18} };

CONTROLINFO ciTitleBar={IDPB_TITLEBAR,{28,73,302,86},NULLHANDLE, {1,0,1+TITLEBARCX,0+14} };
CONTROLINFO ciTitleBar2={IDPB_TITLEBAR,{28,58,302,71},NULLHANDLE, {1,0,1+TITLEBARCX,0+14} };

CONTROLINFO ciTitleBarPlayer={IDPB_TITLEPLAYER,{28,45,302,56},NULLHANDLE, {1,0,1+TITLEBARCX,0+12} };

#define EJECTX 136
#define EJECTY 12

CONTROLINFO ciEjectButton={IDPB_EJECT,{114,20,135,35},NULLHANDLE, {EJECTX,EJECTY,EJECTX+22,EJECTY+16} };
CONTROLINFO ciEjectButtonSel={IDPB_EJECT,{114,4,135,19},NULLHANDLE, {EJECTX,EJECTY,EJECTX+22,EJECTY+16} };

CONTROLINFO ciCloseButton={IDPB_CLOSE,{18,78,27,87},NULLHANDLE, {0,0,0,0} };
CONTROLINFO ciCloseButtonSel={IDPB_CLOSE,{18,69,27,78},NULLHANDLE, {0,0,0,0} };

CONTROLINFO ciMinButton={IDPB_MIN,{9,78,18,87},NULLHANDLE, {0,0,0,0} };
CONTROLINFO ciMinButtonSel={IDPB_MIN,{9,69,18,78},NULLHANDLE, {0,0,0,0} };

CONTROLINFO ciShrinkButton={IDPB_SHRINK,{0,60,8,68},NULLHANDLE, {0,0,0,0} };
CONTROLINFO ciShrinkButtonSel={IDPB_SHRINK,{9,60,17,68},NULLHANDLE, {0,0,0,0} };


CONTROLINFO ciCloseButtonTB={IDPB_CLOSETBPLAYER,{18,78,26,86},NULLHANDLE, {0,0,0,0} };
CONTROLINFO ciCloseButtonTBSel={IDPB_CLOSETBPLAYER,{18,69,26,77},NULLHANDLE, {0,0,0,0} };

CONTROLINFO ciExpandButton={IDPB_EXPANDTBPLAYER,{0,51,8,59},NULLHANDLE, {0,0,0,0} };
CONTROLINFO ciExpandButtonSel={IDPB_EXPANDTBPLAYER,{9,51,17,59},NULLHANDLE, {0,0,0,0} };

CONTROLINFO ciMinButtonTB={IDPB_MINTBPLAYER,{9,78,17,86},NULLHANDLE, {0,0,0,0} };
CONTROLINFO ciMinButtonTBSel={IDPB_MINTBPLAYER,{9,69,17,77},NULLHANDLE, {0,0,0,0} };

#define STEREOX 239
#define STEREOY 63

CONTROLINFO ciStereoButton={IDPB_STEREO,{0,12,29,24},NULLHANDLE, {STEREOX,STEREOY,STEREOX+29,STEREOY+12} };
CONTROLINFO ciStereoButtonSel={IDPB_STEREO,{0,0,29,12},NULLHANDLE, {STEREOX,STEREOY,STEREOX+29,STEREOY+20} };

CONTROLINFO ciMonoButton={IDPB_MONO,{29,12,58-2,24},NULLHANDLE, {STEREOX-27-1,STEREOY,STEREOX-2,STEREOY+12} };
CONTROLINFO ciMonoButtonSel={IDPB_MONO,{29,0,58-2,12},NULLHANDLE, {STEREOX-27-1,STEREOY,STEREOX-2,STEREOY+12} };

CONTROLINFO ciNoShuffleButton={IDPB_SHUFFLE,{28,70,75,85},NULLHANDLE, {164,12,164+47,12+15} };
CONTROLINFO ciNoShuffleButtonSel={IDPB_SHUFFLE,{28,55,75,70},NULLHANDLE, {164,12,164+47,12+15} };

CONTROLINFO ciShuffleButton={IDPB_SHUFFLE,{28,40,75,55},NULLHANDLE, {164,12,164+47,12+15} };
CONTROLINFO ciShuffleButtonSel={IDPB_SHUFFLE,{28,25,75,40},NULLHANDLE, {164,12,164+47,12+15} };

CONTROLINFO ciNoRepeatButton={IDPB_REPEAT,{0,70,28,85},NULLHANDLE, {210,12,210+28,12+15} };
CONTROLINFO ciNoRepeatButtonSel={IDPB_REPEAT,{0,55,28,70},NULLHANDLE, {210,12,210+28,12+15} };

CONTROLINFO ciRepeatButton={IDPB_REPEAT,{0,40,28,55},NULLHANDLE, {210,12,210+28,12+15} };
CONTROLINFO ciRepeatButtonSel={IDPB_REPEAT,{0,25,28,40},NULLHANDLE, {210,12,210+28,12+15} };

CONTROLINFO ciPLButton={IDPB_PL,{23,12,46,24},NULLHANDLE, {242,46,242+23,46+12} };
CONTROLINFO ciPLButtonSel={IDPB_PL,{23,0,46,12},NULLHANDLE, {242,46,242+23,46+12} };

CONTROLINFO ciNoEqButton={IDPB_EQ,{46,12,69,24},NULLHANDLE, {242-23,46,242,46+12} };
CONTROLINFO ciNoEqButtonSel={IDPB_EQ,{46,0,69,12},NULLHANDLE, {242-30,46,242-30+21,46+12} };

CONTROLINFO ciEqButton={IDPB_EQ,{0,12,23,24},NULLHANDLE, {242-23,46,242,46+12} };
CONTROLINFO ciEqButtonSel={IDPB_EQ,{0,0,23,12},NULLHANDLE, {242-30,46,242-30+21,46+12} };

CONTROLINFO ciPosSlider={IDPB_POSBAR,{0,0,248,10},NULLHANDLE, {16,34,16+249,34+10} };
CONTROLINFO ciPosSliderArm={IDSL_POSITION,{278,0,308,10},NULLHANDLE, {0,0,30,10} };

void setupControlInfo()
{
  ciPlayButton.hbmSource=hbmButtons;
  ciPlayButtonSel.hbmSource=hbmButtons;
  ciPauseButton.hbmSource=hbmButtons;
  ciPauseButtonSel.hbmSource=hbmButtons;
  ciStopButton.hbmSource=hbmButtons;
  ciStopButtonSel.hbmSource=hbmButtons;
  ciBackButton.hbmSource=hbmButtons;
  ciBackButtonSel.hbmSource=hbmButtons;
  ciNextButton.hbmSource=hbmButtons;
  ciNextButtonSel.hbmSource=hbmButtons;
  ciEjectButton.hbmSource=hbmButtons;
  ciEjectButtonSel.hbmSource=hbmButtons;
  ciTitleBar.hbmSource=hbmTitleBar;
  ciTitleBar2.hbmSource=hbmTitleBar;
  ciCloseButton.hbmSource=hbmTitleBar;
  ciCloseButtonSel.hbmSource=hbmTitleBar;
  ciMinButton.hbmSource=hbmTitleBar;
  ciMinButtonSel.hbmSource=hbmTitleBar;
  ciShrinkButton.hbmSource=hbmTitleBar;
  ciShrinkButtonSel.hbmSource=hbmTitleBar;

  ciTitleBarPlayer.hbmSource=hbmTitleBar;

  ciExpandButton.hbmSource=hbmTitleBar;
  ciExpandButtonSel.hbmSource=hbmTitleBar;
  ciCloseButtonTB.hbmSource=hbmTitleBar;
  ciCloseButtonTBSel.hbmSource=hbmTitleBar;
  ciMinButtonTB.hbmSource=hbmTitleBar;
  ciMinButtonTBSel.hbmSource=hbmTitleBar;

  ciStereoButton.hbmSource=hbmStereo;
  ciStereoButtonSel.hbmSource=hbmStereo;
  ciMonoButton.hbmSource=hbmStereo;
  ciMonoButtonSel.hbmSource=hbmStereo;

  ciNoShuffleButton.hbmSource=hbmShufrep;
  ciNoShuffleButtonSel.hbmSource=hbmShufrep;
  ciShuffleButton.hbmSource=hbmShufrep;
  ciShuffleButtonSel.hbmSource=hbmShufrep;
  ciNoRepeatButton.hbmSource=hbmShufrep;
  ciNoRepeatButtonSel.hbmSource=hbmShufrep;
  ciRepeatButton.hbmSource=hbmShufrep;
  ciRepeatButtonSel.hbmSource=hbmShufrep;
  ciPLButton.hbmSource=hbmShufrep;
  ciPLButtonSel.hbmSource=hbmShufrep;
  ciNoEqButton.hbmSource=hbmShufrep;
  ciNoEqButtonSel.hbmSource=hbmShufrep;
  ciEqButton.hbmSource=hbmShufrep;
  ciEqButtonSel.hbmSource=hbmShufrep;

  ciPosSlider.hbmSource=hbmPosition;
  ciPosSliderArm.hbmSource=hbmPosition;

}












