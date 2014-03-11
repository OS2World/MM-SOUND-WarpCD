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

#ifndef _CONTROL_H_INCL_
#define _CONTROL_H_INCL_


#define TITLEBARCX 275 /* Width of titlebar */

typedef struct
{
  INT id;
  RECTL rclSource;
  HBITMAP hbmSource;
  RECTL rclDest;
}CONTROLINFO;

CONTROLINFO ciPlayButton;
CONTROLINFO ciPlayButtonSel;

CONTROLINFO ciPauseButton;
CONTROLINFO ciPauseButtonSel;

CONTROLINFO ciStopButton;
CONTROLINFO ciStopButtonSel;

CONTROLINFO ciBackButton;
CONTROLINFO ciBackButtonSel;

CONTROLINFO ciNextButton;
CONTROLINFO ciNextButtonSel;

CONTROLINFO ciTitleBar;
CONTROLINFO ciTitleBar2;

CONTROLINFO ciTitleBarPlayer;

CONTROLINFO ciEjectButton;
CONTROLINFO ciEjectButtonSel;

CONTROLINFO ciCloseButton;
CONTROLINFO ciCloseButtonSel;

CONTROLINFO ciMinButton;
CONTROLINFO ciMinButtonSel;

CONTROLINFO ciShrinkButton;
CONTROLINFO ciShrinkButtonSel;


CONTROLINFO ciCloseButtonTB;
CONTROLINFO ciCloseButtonTBSel;

CONTROLINFO ciExpandButton;
CONTROLINFO ciExpandButtonSel;

CONTROLINFO ciMinButtonTB;
CONTROLINFO ciMinButtonTBSel;


CONTROLINFO ciStereoButton;
CONTROLINFO ciStereoButtonSel;

CONTROLINFO ciMonoButton;
CONTROLINFO ciMonoButtonSel;

CONTROLINFO ciNoShuffleButton;
CONTROLINFO ciNoShuffleButtonSel;

CONTROLINFO ciShuffleButton;
CONTROLINFO ciShuffleButtonSel;

CONTROLINFO ciNoRepeatButton;
CONTROLINFO ciNoRepeatButtonSel;

CONTROLINFO ciRepeatButton;
CONTROLINFO ciRepeatButtonSel;

CONTROLINFO ciPLButton;
CONTROLINFO ciPLButtonSel;

CONTROLINFO ciNoEqButton;
CONTROLINFO ciNoEqButtonSel;

CONTROLINFO ciEqButton;
CONTROLINFO ciEqButtonSel;

CONTROLINFO ciPosSlider;
CONTROLINFO ciPosSliderArm;



#endif















