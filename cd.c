/*
 * This file is (C) Chris Wohlgemuth 1999
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

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "cddb.h"

#include "cd.h"

#ifndef CDROMDISK_CLOSETRAY
#define CDROMDISK_CLOSETRAY 0x0045
#endif



HFILE extern openDrive(char* drive);
ULONG extern closeDrive(HFILE hfDrive);
LONG static _CDCalculateSize(MSF* msf1,MSF* msf2);
/******************************************/
/* Result:  
	 0:   Error
	 -1: CD is Data Disk
	 other: # Audio tracks */
/******************************************/
int extern CDQueryAudioCDTracks(HFILE hfDrive);
ULONG extern CDQueryUPC(UCHAR* ucUPC, char * drive);
/* Returns the size in bytes of a track on drive 'drive' (e.g. O:) */
LONG extern CDQueryTrackSize(ULONG numTrack, char * drive);
BOOL extern CDPlayTrack(ULONG numTrack, char * drive);
BOOL extern CDStop(char * drive);
BOOL extern CDResume(char * drive);
int extern CDQueryPaused(char * drive);
ULONG extern CDQueryTrayOpen(char * drive);
ULONG extern CDQueryIsPlaying(char * drive);
LONG extern queryFreeDriveSpace(ULONG diskNum) ;
/**************************************************************/
/* This funktion returns the CD-Drives in the system          */
/*                                                            */
/* iNumCD (output): # of CD-Drives                            */
/* cFirstDrive (output): first cd-Drive letter                */
/* returns TRUE: We have cd-drives                            */
/**************************************************************/
BOOL extern CDQueryCDDrives(int *iNumCD, char * cFirstDrive);
/* Returns a CDDB disc-ID for quering the database */
/* Returns 0 if error.                             */
LONG extern CDDBDiscID(char * drive, CDDBINFO * cddbInfo); 


LONG static _CDCalculateSize(MSF* msf1,MSF* msf2)
{
 	return ((msf2->ucMinutes*60+msf2->ucSeconds)*75+msf2->ucFrames)*2352-
		((msf1->ucMinutes*60+msf1->ucSeconds)*75+msf1->ucFrames)*2352;	
}

LONG static _CDCalculateSector(MSF* msf1)
{
 	return ((msf1->ucMinutes*60+msf1->ucSeconds)*75+msf1->ucFrames);	
}

BOOL static _CDQueryDeviceStatus(ULONG  *ulStatus , char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG ulDeviceStatus;
	ULONG ulDataLen;
	ULONG rc;

    ulDeviceStatus=0;

    hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;


	/* Get status */
    ulDataLen=sizeof(ulDeviceStatus);
    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMDISK, CDROMDISK_DEVICESTATUS,
                     "CD01", 4, &ulParamLen, &ulDeviceStatus,
                     sizeof(ulDeviceStatus), &ulDataLen);		
    if(rc) {
      closeDrive(hfDrive);
      return FALSE;//Error
    }

    closeDrive(hfDrive);
    *ulStatus=ulDeviceStatus;

    return TRUE;
}

BOOL extern CDSetChannel(CDGETCHANNEL  * Cgc , char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG ulDeviceStatus;
	ULONG ulDataLen;
	ULONG rc;
    CDGETCHANNEL cgc;
    char text[100];
    char signature[4];

    hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;

    memcpy(&cgc,Cgc,sizeof(cgc));

    sprintf(text," vol0: %d vol1: %d vol2: %d vol3: %d\n io0: %d io1: %d io2: %d io3: %d",
            cgc.ucVolume0, cgc.ucVolume1,cgc.ucVolume2, cgc.ucVolume3,
            cgc.ucInOut0, cgc.ucInOut1, cgc.ucInOut2, cgc.ucInOut3);
    
    
	/* Set channel data  */
    ulDataLen=sizeof(cgc);
    ulParamLen=4;
    signature[0]='C';
	signature[1]='D';	
	signature[2]='0';
	signature[3]='1';

    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_SETCHANNELCTRL,
                     &signature, 4, &ulParamLen, &cgc,
                     sizeof(cgc), &ulDataLen);		

    sprintf(text," rc: %d %x",rc, rc);
      WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,text,"",1234,MB_OK);
 
   if(rc) {
      closeDrive(hfDrive);
      return FALSE;//Error
    }
    DosBeep(5000,200); 
    closeDrive(hfDrive);

    return TRUE;
}

BOOL extern CDQueryChannel(CDGETCHANNEL  * Cgc , char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG ulDeviceStatus;
	ULONG ulDataLen;
	ULONG rc;
    CDGETCHANNEL cgc;
    char signature[4];

    hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;

    memset(&cgc,0,sizeof(cgc));
	/* Get channel data  */
    ulDataLen=sizeof(cgc);
    ulParamLen=4;
    signature[0]='C';
	signature[1]='D';	
	signature[2]='0';
	signature[3]='1';

    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETCHANNEL,
                     &signature, 4, &ulParamLen, &cgc,
                     sizeof(cgc), &ulDataLen);		
    if(rc) {
      closeDrive(hfDrive);
      return FALSE;//Error
    }

    closeDrive(hfDrive);
    memcpy(Cgc,&cgc,sizeof(cgc));

    return TRUE;
}

BOOL extern CDQuerySubChannelQ(SUBCHANNELQ  * Scq , char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG ulDeviceStatus;
	ULONG ulDataLen;
	ULONG rc;
    SUBCHANNELQ scq;


    hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;


	/* Get status */
    ulDataLen=sizeof(scq);
    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETSUBCHANNELQ,
                     "CD01", 4, &ulParamLen, &scq,
                     sizeof(scq), &ulDataLen);		
    if(rc) {
      closeDrive(hfDrive);
      return FALSE;//Error
    }

    closeDrive(hfDrive);
    memcpy(Scq,&scq,sizeof(scq));

    return TRUE;
}


/****************************************/
/* Input: drive (e.g. o:)               */
/****************************************/
HFILE extern openDrive(char* drive)
{
	HFILE hfDrive = 0;
	ULONG ulAction;
	ULONG rc;

	rc = DosOpen(drive, &hfDrive, &ulAction, 0,
					 FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
					 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY |
					 OPEN_FLAGS_DASD|OPEN_FLAGS_FAIL_ON_ERROR, NULL);	
    
	if(rc)
      return 0;//Error
    
	return hfDrive;
}

ULONG extern closeDrive(HFILE hfDrive)
{
	return DosClose(hfDrive);	
}


BOOL extern CDEject(char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG rc;

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;
    
    do {
      /* Stop CD */
      ulParamLen=4;
      rc = DosDevIOCtl(hfDrive, IOCTL_CDROMDISK, CDROMDISK_EJECTDISK,
                       "CD01", 4, &ulParamLen,0,
                       0, 0);		
      if(rc)
        break;//Error
      closeDrive(hfDrive);
      return TRUE;
    }while(TRUE);
	closeDrive(hfDrive);
    return FALSE;
}

BOOL extern CDCloseTray(char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG rc;

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;
    
    do {
      /* Stop CD */
      ulParamLen=4;
      rc = DosDevIOCtl(hfDrive, IOCTL_CDROMDISK, CDROMDISK_CLOSETRAY,
                       "CD01", 4, &ulParamLen,0,
                       0, 0);		
      if(rc)
        break;//Error
      closeDrive(hfDrive);
      return TRUE;
    }while(TRUE);
	closeDrive(hfDrive);
    return FALSE;
}

/*****************************************/
/*                                       */
/* Plays track #numTrack on              */
/* CD-Drive 'drive'                      */
/*                                       */
/* Returns TRUE if successful            */
/*                                       */
/*****************************************/
BOOL extern CDPlayRange(PLAYPARAM* PlayParam, char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
	ULONG ulDataLen;
	ULONG rc;
	CDINFO cdInfo;
	TINFOPARAM tip;
	CDTRACKINFO trackInfo[2];
    PLAYPARAM playParam;

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;

    memcpy(&playParam,PlayParam, sizeof(playParam));
    /* Play the Track... */
    ulParamLen=sizeof(PLAYPARAM);
    playParam.signature[0]='C';
    playParam.signature[1]='D';	
    playParam.signature[2]='0';
    playParam.signature[3]='1';
    //playParam.ucAddressMode=01;
        
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_PLAYAUDIO,
                     &playParam, sizeof(playParam), &ulParamLen,0,
                     0, 0);

    closeDrive(hfDrive);

    if(rc) 
      return FALSE;

    return TRUE;    
}


/*****************************************/
/*                                       */
/* Plays track #numTrack on              */
/* CD-Drive 'drive'                      */
/*                                       */
/* Returns TRUE if successful            */
/*                                       */
/*****************************************/
BOOL extern CDPlayTrack(ULONG numTrack, char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
	ULONG ulDataLen;
	ULONG rc;
	CDINFO cdInfo;
	TINFOPARAM tip;
	CDTRACKINFO trackInfo[2];
    PLAYPARAM playParam;

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;
    do {
	/* Get cd info */
		ulDataLen=sizeof(cdInfo);
		ulParamLen=4;
		rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
										 "CD01", 4, &ulParamLen, &cdInfo,
										 sizeof(cdInfo), &ulDataLen);		
		if(rc)
			break;//Error
		ulDataLen=sizeof(trackInfo);
		ulParamLen=sizeof(TINFOPARAM);
		tip.signature[0]='C';
		tip.signature[1]='D';	
		tip.signature[2]='0';
		tip.signature[3]='1';
		/* Get information about our track */
		tip.ucTrackNum=numTrack;
		rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
										 &tip, sizeof(tip), &ulParamLen, &trackInfo[0],
										 sizeof(trackInfo[0]), &ulDataLen);
		if(rc) 
			break;//Error
		/* Get information about next track */
		tip.ucTrackNum=numTrack+1;
		if(tip.ucTrackNum<=cdInfo.ucLastTrack) {
			rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
											 &tip, sizeof(tip), &ulParamLen, &trackInfo[1],
											 sizeof(trackInfo[1]), &ulDataLen);
			if(rc) 
				break;//Error
		}
		else {
          BYTE *cdInfoBytes;
          cdInfoBytes=(BYTE*)&cdInfo;
          cdInfoBytes+=2;
          trackInfo[1].ulTrackAddress=*((ULONG*)cdInfoBytes);
        }

              /* Play the Track... */
        ulParamLen=sizeof(PLAYPARAM);
		playParam.signature[0]='C';
		playParam.signature[1]='D';	
		playParam.signature[2]='0';
		playParam.signature[3]='1';
        playParam.ucAddressMode=01;
        memcpy(&playParam.ucStartSector,&trackInfo[0].ulTrackAddress,4);
        memcpy(&playParam.ucEndSector,&trackInfo[1].ulTrackAddress,4);
        
        rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_PLAYAUDIO,
                         &playParam, sizeof(playParam), &ulParamLen,0,
                         0, 0);
        if(rc)
          break;
        closeDrive(hfDrive);
        return TRUE;
    }while(TRUE);
	closeDrive(hfDrive);
    return FALSE;
}

BOOL extern CDStop(char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG rc;

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;
    
    do {
      /* Stop CD */
      ulParamLen=4;
      rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_STOPAUDIO,
                       "CD01", 4, &ulParamLen,0,
                       0, 0);		
      if(rc)
        break;//Error
      closeDrive(hfDrive);
      return TRUE;
    }while(TRUE);
	closeDrive(hfDrive);
    return FALSE;
}

BOOL extern CDResume(char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
    ULONG rc;

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;
    
    do {
      /* Stop CD */
      ulParamLen=4;
      rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_RESUMEAUDIO,
                       "CD01", 4, &ulParamLen,0,
                       0, 0);		
      if(rc)
        break;//Error
      closeDrive(hfDrive);
      return TRUE;
    }while(TRUE);
	closeDrive(hfDrive);
    return FALSE;
}

LONG extern queryFreeDriveSpace(ULONG diskNum) 
{
	FSALLOCATE infoBuf;
	
	if(DosQueryFSInfo(diskNum,FSIL_ALLOC,&infoBuf,sizeof(infoBuf)))
		return 0;
	
	return infoBuf.cUnitAvail*infoBuf.cbSector*infoBuf.cSectorUnit;
}

/******************************************/
/* Input: drive (e.g. o:)                 */
/* Result:  
	 2:   Error
	 1:   is playing
	 0:   not playing                     */
/******************************************/
ULONG extern CDQueryCurrentPlayingTrack(char * drive)
{
  ULONG ulStatus;
  SUBCHANNELQ scq;

  if(! CDQuerySubChannelQ( &scq ,  drive))
     return 2;

  return (scq.ucTrack & 0xf )+(scq.ucTrack>>4)*10;
}

/******************************************/
/* Input: drive (e.g. o:)                 */
/* Result:  
	 2:   Error
	 1:   is playing
	 0:   not playing                     */
/******************************************/
ULONG extern CDQueryIsPlaying(char * drive)
{
  ULONG ulStatus;

  if(! _CDQueryDeviceStatus( &ulStatus ,drive))
     return 2;

  return (ulStatus>>12)&1;
}


/******************************************/
/* Input: drive (e.g. o:)                 */
/* Result:  
	 2:   Error
	 1:   Tray is open
	 0:   Tray is closed                  */
/******************************************/
ULONG extern CDQueryTrayOpen(char * drive)
{
  ULONG ulStatus;

  if(! _CDQueryDeviceStatus( &ulStatus ,drive))
     return 2;

  return ulStatus & 1;
}

/******************************************/
/* Result:  
	 0:   Error
	 -1: CD is Data Disk
	 other: # Audio tracks */
/******************************************/
int extern CDQueryAudioCDTracks(HFILE hfDrive)
{
	ULONG ulParamLen;
	ULONG ulDataLen;
	ULONG rc;
	CDINFO cdInfo;
	TINFOPARAM tip;
	CDTRACKINFO trackInfo;
	
	ulDataLen=sizeof(cdInfo);
	ulParamLen=4;
		
	if(!hfDrive)
		return 0;
	
	rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
						  "CD01", 4, &ulParamLen, &cdInfo,
						  sizeof(cdInfo), &ulDataLen);

	if(rc) {
		return 0;//Error
	}

	ulDataLen=sizeof(trackInfo);
	ulParamLen=sizeof(TINFOPARAM);
	tip.signature[0]='C';
	tip.signature[1]='D';	
	tip.signature[2]='0';
	tip.signature[3]='1';
	tip.ucTrackNum=1;
	
	/* We have a disc. Check if it's audio */
	rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
						  &tip, sizeof(tip), &ulParamLen, &trackInfo,
						  sizeof(trackInfo), &ulDataLen);

	if(rc) {
		return 0;//Error
	}
		
	if(trackInfo.ucTCInfo & 0x40) {
		return -1;//It's a data track
	}
		
	return cdInfo.ucLastTrack-cdInfo.ucFirstTrack+1;
	
}

/* Returns the UPC of CD in drive 'drive' (e.g. O:) */
/* Size of ucUCP must be 10!                        */
ULONG extern CDQueryUPC(UCHAR* ucUPC, char * drive)
{
  HFILE hfDrive;
  ULONG ulParamLen;
  ULONG ulDataLen;
  ULONG rc;
  char setup[100]; 
 
  hfDrive=openDrive(drive);
  if(!hfDrive) return -1;

  do {
    DosBeep(1000,200);
    /* Get UPC info */
    ulDataLen=10;
    ulParamLen=4;
    rc = DosDevIOCtl(hfDrive, IOCTL_CDROMDISK, CDROMDISK_GETUPC,
                     "CD01", 4, &ulParamLen, ucUPC,
                     10, &ulDataLen);
    sprintf(setup,"rc is: %x",rc);

    WinMessageBox(  HWND_DESKTOP,   HWND_DESKTOP,  
                  setup, "GET_UPC", 0UL, MB_OK | MB_ICONEXCLAMATION|MB_MOVEABLE );

    if(rc)
      break;
    DosBeep(2000,200);
    closeDrive(hfDrive);
    return 0;

  }while(TRUE);

  /* No error */
  closeDrive(hfDrive);
  return -1;
}

BOOL extern CDQueryTrackMSF(ULONG numTrack, MSF *msfStart, MSF *msfEnd, char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
	ULONG ulDataLen;
	ULONG rc;
	CDINFO cdInfo;
	TINFOPARAM tip;
	CDTRACKINFO trackInfo[2];

	hfDrive=openDrive(drive);
	if(!hfDrive) return FALSE;
	
	do {
		/* Get cd info */
		ulDataLen=sizeof(cdInfo);
		ulParamLen=4;
		rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
										 "CD01", 4, &ulParamLen, &cdInfo,
										 sizeof(cdInfo), &ulDataLen);		
		if(rc)
			break;//Error
		ulDataLen=sizeof(trackInfo);
		ulParamLen=sizeof(TINFOPARAM);
		tip.signature[0]='C';
		tip.signature[1]='D';	
		tip.signature[2]='0';
		tip.signature[3]='1';
		/* Get information about our track */
		tip.ucTrackNum=numTrack;
		rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
										 &tip, sizeof(tip), &ulParamLen, &trackInfo[0],
										 sizeof(trackInfo[0]), &ulDataLen);
		if(rc) 
			break;//Error

		/* Get information about next track */
		tip.ucTrackNum=numTrack+1;
		if(tip.ucTrackNum<=cdInfo.ucLastTrack) {
			rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
											 &tip, sizeof(tip), &ulParamLen, &trackInfo[1],
											 sizeof(trackInfo[1]), &ulDataLen);
			if(rc) 
				break;//Error
		}
		else {
          trackInfo[1].ulTrackAddress=cdInfo.ulLeadOut;
        }
        closeDrive(hfDrive);
        memcpy(msfStart, (MSF*)&trackInfo[0].ulTrackAddress, sizeof(MSF));
        memcpy(msfEnd, (MSF*)&trackInfo[1].ulTrackAddress, sizeof(MSF));
        return TRUE;

	}while(TRUE);
	closeDrive(hfDrive);
	return FALSE;
}


/* Returns the size in bytes of a track on drive 'drive' (e.g. O:) */
LONG extern CDQueryTrackSize(ULONG numTrack, char * drive)
{
	HFILE hfDrive;
	ULONG ulParamLen;
	ULONG ulDataLen;
	ULONG rc;
	CDINFO cdInfo;
	TINFOPARAM tip;
	CDTRACKINFO trackInfo[2];

	hfDrive=openDrive(drive);
	if(!hfDrive) return 0;
	
	do {
		/* Get cd info */
		ulDataLen=sizeof(cdInfo);
		ulParamLen=4;
		rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
										 "CD01", 4, &ulParamLen, &cdInfo,
										 sizeof(cdInfo), &ulDataLen);		
		if(rc)
			break;//Error
		ulDataLen=sizeof(trackInfo);
		ulParamLen=sizeof(TINFOPARAM);
		tip.signature[0]='C';
		tip.signature[1]='D';	
		tip.signature[2]='0';
		tip.signature[3]='1';
		/* Get information about our track */
		tip.ucTrackNum=numTrack;
		rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
										 &tip, sizeof(tip), &ulParamLen, &trackInfo[0],
										 sizeof(trackInfo[0]), &ulDataLen);
		if(rc) 
			break;//Error

		/* Get information about next track */
		tip.ucTrackNum=numTrack+1;
		if(tip.ucTrackNum<=cdInfo.ucLastTrack) {
			rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
											 &tip, sizeof(tip), &ulParamLen, &trackInfo[1],
											 sizeof(trackInfo[1]), &ulDataLen);
			if(rc) 
				break;//Error
		}
		else {
          trackInfo[1].ulTrackAddress=cdInfo.ulLeadOut;
          /* #pragma pack(1) eingebaut */

        }
        closeDrive(hfDrive);
		return _CDCalculateSize((MSF*)&trackInfo[0].ulTrackAddress, (MSF*)&trackInfo[1].ulTrackAddress);
	}while(TRUE);
	closeDrive(hfDrive);
	return 0;
}

/* Returns sector info of track #numTrack */
/* Starting with track 0 */
LONG extern CDQueryTrackStartSector(ULONG numTrack, HFILE hfDrive)
{	
	ULONG ulParamLen;
	ULONG ulDataLen;
	ULONG rc;
	CDINFO cdInfo;
	TINFOPARAM tip;
	CDTRACKINFO trackInfo[2];

	do {
      /* Get cd info */
      ulDataLen=sizeof(cdInfo);
      ulParamLen=4;
      rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIODISK,
                       "CD01", 4, &ulParamLen, &cdInfo,
                       sizeof(cdInfo), &ulDataLen);		
      if(rc)
        break;//Error
      
      ulDataLen=sizeof(trackInfo);
		ulParamLen=sizeof(TINFOPARAM);
		tip.signature[0]='C';
		tip.signature[1]='D';	
		tip.signature[2]='0';
		tip.signature[3]='1';
		/* Get information about our track */
		tip.ucTrackNum=numTrack+1;
		if(tip.ucTrackNum<=cdInfo.ucLastTrack) {
          rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOTRACK,
                           &tip, sizeof(tip), &ulParamLen, &trackInfo[0],
                           sizeof(trackInfo[0]), &ulDataLen);
          if(rc) 
            break;//Error
          }else 
            return _CDCalculateSector((MSF*)&cdInfo.ulLeadOut);
        

        return _CDCalculateSector((MSF*)&trackInfo[0].ulTrackAddress);

	}while(TRUE);
	
    /* error */

	return 0;
}

int extern CDQueryPaused(char * drive)
{
  PAUSEINFO piPause;
  HFILE hfDrive;
  ULONG ulParamLen;
  ULONG rc;
  ULONG ulDataLen;
  
  hfDrive=openDrive(drive);
  if(!hfDrive) return -1;
 
  ulDataLen=sizeof(piPause);
  ulParamLen=4;
  
  /* Get information */
  rc = DosDevIOCtl(hfDrive, IOCTL_CDROMAUDIO, CDROMAUDIO_GETAUDIOSTATUS,
                   "CD01", 4, &ulParamLen, &piPause,
                   sizeof(piPause), &ulDataLen);
  if(rc) {
    closeDrive(hfDrive);
    return -1;
  }
   
  closeDrive(hfDrive);
  return piPause.usStatusBits & 1;
}

/**************************************************************/
/*                                                            */
/* This funktion returns the CD-Drives in the system          */
/*                                                            */
/* iNumCD (output): # of CD-Drives                            */
/* cFirstDrive (output): first cd-Drive letter                */
/* returns TRUE: We have cd-drives                            */
/*                                                            */
/**************************************************************/
BOOL extern CDQueryCDDrives(int *iNumCD, char * cFirstDrive)
{
  HFILE hfDevice;
  ULONG ulAction;
  ULONG ulLen;
  static char cFirst=0;
  static int iNumCDLocal=0;
  static BOOL haveCD=FALSE;
  static BOOL done=FALSE;			
  struct
  {
    USHORT usCountCD;
    USHORT usFirstCD;
  } CDInfo;
  
  if(!done){
    ulLen = sizeof(CDInfo);
    if(!DosOpen("\\DEV\\CD-ROM2$", &hfDevice, &ulAction, 0,
                FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY, NULL))
      {
        if(!DosDevIOCtl(hfDevice, 0x82, 0x60, NULL, 0, NULL, &CDInfo, ulLen, &ulLen))
          {
            if(CDInfo.usCountCD) {
              haveCD=TRUE;
              iNumCDLocal=CDInfo.usCountCD;
              cFirst='A'+ CDInfo.usFirstCD;
            }										 
          }
        DosClose(hfDevice);
      }
    done=TRUE;
  }
  *iNumCD=iNumCDLocal;
  *cFirstDrive=cFirst;
  return haveCD;				
}


/*****************************************/
/*                                       */
/* Query if a CD is avaiable in          */
/* CD-Drive 'drive'                      */
/*                                       */
/* Input: drive num (starting with 0)    */
/* Returns TRUE if media is avaiable     */
/*                                       */
/*****************************************/
BOOL CDQueryMedia(ULONG  driveNum)
{
  return FALSE;
}


int cddb_sum(int n)
{
  int ret;

  for (ret = 0; n > 0; n /= 10) {
    ret += (n % 10);
  }

  return ret;
}

/* Returns a CDDB disc-ID for quering the database */
/* Returns 0 if error.                             */
LONG extern CDDBDiscID(char * drive, CDDBINFO * cddbInfo)
{
  int numTracks,a;
  HFILE hfDrive;
  LONG sum;
  int t;

  do {
    hfDrive=openDrive(drive);
    if(!hfDrive)
      return 0;

    numTracks=CDQueryAudioCDTracks(hfDrive);
    if(numTracks==0||numTracks==-1)
      break;
    cddbInfo->numTracks=numTracks;

    sum=0;
    /* Calculating the id */
    for(a=0;a<numTracks;a++) {
      sum+= cddb_sum(CDQueryTrackStartSector(a, hfDrive)/75 );

      cddbInfo->startSector[a]=CDQueryTrackStartSector(a, hfDrive);
    }
    t=CDQueryTrackStartSector(a, hfDrive)/75-CDQueryTrackStartSector(0, hfDrive)/75;

    cddbInfo->discLength=(CDQueryTrackStartSector(a, hfDrive)-CDQueryTrackStartSector(0, hfDrive))/75;
    cddbInfo->discid=(sum % 0xff) << 24 | (t << 8) | numTracks;
    closeDrive(hfDrive);
    return cddbInfo->discid;
  }while(TRUE);

  /* error */
  closeDrive(hfDrive);
  return 0;
}



