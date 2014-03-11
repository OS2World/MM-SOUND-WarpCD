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


/* This contains the position information for the dialog controls */
#include "controls.h"
void  setupSmallText();
void  setupNumbers();


HBITMAP static ReadBitmap(PSZ pszFileName, HPS hpsBitmapFile, PBITMAPINFOHEADER2 bmp2FrameBitmapFile)
{

  FILESTATUS fsts;  
  ULONG  cbRead;                        /* Number of bytes read by DosRead. */
  HFILE hfile;

  APIRET     rc;                                         /* API return code */  
  ULONG ulResult;

  HBITMAP hbm=NULLHANDLE;
  PBYTE pFileBegin;
  PBITMAPINFOHEADER2 pbmp2FrameBitmapFile;

  PBITMAPFILEHEADER2 pbfh2;                   /* can address any file types */
  PBITMAPINFOHEADER2 pbmp2;                     /* address any info headers */
  
  /*
   *   Use Loop to avoid duplicate cleanup code.  If any errors, a break
   *   statement will jump directly to error cleanup code.
   */
  
  do
    {
      if(DosOpen(pszFileName, &hfile, &ulResult, 0UL, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
                 OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL))
        break;
      
      rc =    DosQueryFileInfo(hfile, 1, &fsts, sizeof(fsts));
      if (rc)
        break;                        /* jump to error code outside of loop */          
      rc =  DosAllocMem(      (PPVOID) &pFileBegin,
                              (ULONG)  fsts.cbFile,
                              (ULONG)  PAG_READ | PAG_WRITE | PAG_COMMIT);
      if (rc)
        break;                        /* jump to error code outside of loop */
      if (DosRead( hfile, (PVOID)pFileBegin, fsts.cbFile, &cbRead))
        break;                        /* jump to error code outside of loop */
      
      /*
       *   If it's a bitmap-array, point to common file header.  Otherwise,
       *   point to beginning of file.
       */
      pbfh2 = (PBITMAPFILEHEADER2) pFileBegin;
      pbmp2 = NULL;                   /* only set this when we validate type */     
      switch (pbfh2->usType)
        {
        case BFT_BITMAPARRAY:
          /*
           *   If this is a Bitmap-Array, adjust pointer to the normal
           *   file header.  We'll just use the first bitmap in the
           *   array and ignore other device forms.
           */
          pbfh2 = &(((PBITMAPARRAYFILEHEADER2) pFileBegin)->bfh2);
          pbmp2 = &pbfh2->bmp2;    /* pointer to info header (readability) */
          break;
        case BFT_BMAP:
          pbmp2 = &pbfh2->bmp2;    /* pointer to info header (readability) */
          break;
        default:      /* these formats aren't supported; don't set any ptrs */
        case BFT_ICON:
        case BFT_POINTER:
        case BFT_COLORICON:
        case BFT_COLORPOINTER:
          break;
        }   /* end switch (pbfh2->usType) */
      if (pbmp2 == NULL)
        break;        /* File format NOT SUPPORTED: break out to error code */
      /*
       *   Check to see if BMP file has an old structure, a new structure, or
       *   Windows structure.  Capture the common data and treat all bitmaps
       *   generically with pointer to new format.  API's will determine format
       *   using cbFixed field.
       *
       *   Windows bitmaps have the new format, but with less data fields
       *   than PM.  The old strucuture has some different size fields,
       *   though the field names are the same.
       *
       *
       *   NOTE: bitmap data is located by offsetting the beginning of the file
       *         by the offset contained in pbfh2->offBits.  This value is in
       *         the same relatie location for different format bitmap files.
       */
      
      pbmp2FrameBitmapFile=bmp2FrameBitmapFile;
      memcpy(                      /* copy bitmap info into global structure */
             (PVOID) pbmp2FrameBitmapFile,
             (PVOID) pbmp2,
             pbmp2->cbFix);      /* only copy specified size (varies per format) */
      
      hbm = GpiCreateBitmap(
                            hpsBitmapFile,                         /* presentation-space handle */
                            pbmp2FrameBitmapFile,            /* address of structure for format data */
                            CBM_INIT,                                                      /* options */
                            (PBYTE)pbfh2 + pbfh2->offBits,                            /* address of buffer of image data */
                            (PBITMAPINFO2)(PVOID)&pbfh2->bmp2);                 /* address of structure for color and format */
      
      //   GpiDeleteBitmap(hbm);
      
      if (!hbm)
        {
          break;                        /* jump to error code outside of loop */
        }
      DosClose( hfile);
      DosFreeMem( pFileBegin);
          
      return hbm;                                    /* function successful */
    } while (FALSE); /* fall through loop first time */
  
  /*
   *   Close the file, free the buffer space and leave.  This is an error exit
   *   point from the function.  Cleanup code is here to avoid duplicate code
   *   after each operation.
   */
  if (pFileBegin != NULL)
    DosFreeMem( pFileBegin);
  if(hfile)   DosClose( hfile);
  
  return NULLHANDLE;                                         /* function failed */
}   /* end ReadBitmap() */

freeBitmaps()
{
  if(hbmMain) {
    GpiDeleteBitmap(hbmMain);
    hbmMain=NULLHANDLE;
  }

  if(hbmButtons) {
    GpiDeleteBitmap(hbmButtons);
    hbmButtons=NULLHANDLE;
  }

  if(hbmTitleBar) {
    GpiDeleteBitmap(  hbmTitleBar);
    hbmTitleBar=NULLHANDLE;
  }

  if(hbmText) {
    GpiDeleteBitmap(hbmText);
    hbmText=NULLHANDLE;
  }

  if(hbmNumbers) {
    GpiDeleteBitmap(hbmNumbers);
    hbmNumbers=NULLHANDLE;
  }

  if(hbmStereo) {
    GpiDeleteBitmap(hbmStereo);
    hbmStereo=NULLHANDLE;
  }

  if(hbmShufrep) {
    GpiDeleteBitmap(hbmShufrep);
    hbmShufrep=NULLHANDLE;
  }

  if(hbmPosition) {
    GpiDeleteBitmap(hbmPosition);
    hbmPosition=NULLHANDLE;
  }
}

void showMsg(char *text)
{
  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,text,"",1234,MB_OK);  
}

BOOL loadBitmaps(char * bmpDirectory)

{
  char chrPath[CCHMAXPATH];
      
  freeBitmaps();
  
  /* LoadBitMaps */
  do {
    sprintf(chrPath,"%s\\main.bmp",bmpDirectory);
    hbmMain=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2MainBitmapFile);// Main Bitmap
    pbmpi2MainBitmapFile=&bmpi2MainBitmapFile;
    if(!hbmMain) {
      showMsg("Can't load main.bmp");;
      break;
    }
    
    sprintf(chrPath,"%s\\cbuttons.bmp",bmpDirectory);
    hbmButtons=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2ButtonsBitmapFile);// Button-Bitmap
    pbmpi2ButtonsBitmapFile=&bmpi2ButtonsBitmapFile;
    if(!hbmButtons) {
      showMsg("Can't load cbuttons.bmp");
     break;
    }

    sprintf(chrPath,"%s\\titlebar.bmp",bmpDirectory);
    hbmTitleBar=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2TitleBarBitmapFile);
    pbmpi2TitleBarBitmapFile=&bmpi2TitleBarBitmapFile;
    if(!hbmTitleBar) {
      showMsg("Can't titlebar.bmp");
     break;
    }

    sprintf(chrPath,"%s\\text.bmp",bmpDirectory);
    hbmText=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2TextBitmapFile);
    pbmpi2TextBitmapFile=&bmpi2TextBitmapFile;
    if(!hbmText) {
      showMsg("Can't load text.bmp");
     break;
    }

    sprintf(chrPath,"%s\\numbers.bmp",bmpDirectory);  
    hbmNumbers=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2NumbersBitmapFile);
    pbmpi2NumbersBitmapFile=&bmpi2NumbersBitmapFile;
    if(!hbmNumbers) {
      sprintf(chrPath,"%s\\nums_ex.bmp",bmpDirectory);
      hbmNumbers=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2NumbersBitmapFile);
      pbmpi2NumbersBitmapFile=&bmpi2NumbersBitmapFile;
      if(!hbmNumbers) {
        showMsg("Can't load numbers.bmp or nums_ex.bmp");
        break;
      }
    }
    
    sprintf(chrPath,"%s\\monoster.bmp",bmpDirectory);
    hbmStereo=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2StereoBitmapFile);
    pbmpi2StereoBitmapFile=&bmpi2StereoBitmapFile;
    if(!hbmStereo) {
      showMsg("Can't load monoster.bmp");
      break;
    }
   
    sprintf(chrPath,"%s\\shufrep.bmp",bmpDirectory);  
    hbmShufrep=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2ShufrepBitmapFile);
    pbmpi2ShufrepBitmapFile=&bmpi2ShufrepBitmapFile;
    if(!hbmShufrep) {
      showMsg("Can't load shufrep.bmp");
      break;
    }

    sprintf(chrPath,"%s\\posbar.bmp",bmpDirectory);  
    hbmPosition=ReadBitmap(chrPath, WinGetPS(HWND_DESKTOP),& bmpi2PositionBitmapFile);
    pbmpi2PositionBitmapFile=&bmpi2PositionBitmapFile;
    if(!hbmPosition) {
      showMsg("Can't load posbar.bmp");
      break;
    }

    setupControlInfo();
    setupSmallText();
    setupNumbers();
    
    return TRUE;
  }
  while(TRUE);
  freeBitmaps();
  return FALSE;
}















