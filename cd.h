#ifndef _CD_H_INCL_
#define _CD_H_INCL_

#pragma pack(1)

typedef struct{
	UCHAR   ucFirstTrack;
	UCHAR 	ucLastTrack;
	ULONG   ulLeadOut;
}CDINFO;

typedef struct{
	ULONG   ulLeadOut;
	UCHAR   ucFirstTrack;
	UCHAR 	ucLastTrack;
}CDINFO2;

typedef struct
{
	ULONG ulTrackAddress;
	UCHAR ucTCInfo;
}CDTRACKINFO;

typedef struct
{
	UCHAR signature[4];
	UCHAR  ucTrackNum;
}TINFOPARAM;

typedef struct
{
	UCHAR ucFrames;
	UCHAR ucSeconds;
	UCHAR ucMinutes;
	UCHAR ucNotUsed;
}MSF;

typedef struct
{
  UCHAR signature[4];
  UCHAR  ucAddressMode;
  UCHAR ucStartSector[4];
  UCHAR ucEndSector[4];
}PLAYPARAM;

typedef struct
{
  UCHAR signature[4];
  UCHAR  ucAddressMode;
  ULONG ulStartSector;
  ULONG ulEndSector;
}PLAYPARAM2;

typedef struct
{
  USHORT usStatusBits;
  ULONG ulStarting;
  ULONG ulEnding;
}PAUSEINFO;

typedef struct
{
  UCHAR ucCtrlAdr;
  UCHAR ucTrack;
  UCHAR ucIndex;
  UCHAR ucTrackMins;
  UCHAR ucTrackSecs;
  UCHAR UCTrackFrames;
  UCHAR ucNull;
  UCHAR ucDiskMins;
  UCHAR ucDiskSecs;
  UCHAR ucDiskFrames;
}SUBCHANNELQ;

typedef struct
{
  UCHAR ucInOut0;
  UCHAR ucVolume0;
  UCHAR ucInOut1;
  UCHAR ucVolume1;
  UCHAR ucInOut2;
  UCHAR ucVolume2;
  UCHAR ucInOut3;
  UCHAR ucVolume3;
}CDGETCHANNEL;

#pragma pack()

#endif
