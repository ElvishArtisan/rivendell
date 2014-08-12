/* wavlib.h
 *
 * A C Library for abstracting WAV files
 *
 * (C) Copyright 1997,1999-2003 Fred Gleason <fredg@paravelsystems.com>
 *
 *      $Id: wavlib.h,v 1.4 2007/02/14 21:59:12 fredg Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* 
 * The package version number
 */
#define VERNUM "2.4.2"

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _TIME_H
#include <sys/time.h>
#endif

#ifndef PI
#define PI 3.1415928
#endif

#define AUDIO_BUFFER 32768

/*
 * Some basic values
 */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*
 * Aliases for backward compatibility.  Use of these names is deprecated.
 */
#define PlayWav PlayWavFile
#define RecWav RecWavFile
#define SoundConfig SoundConfigFile


/* Data Structures */
struct wavHeader {
  unsigned short wFormatTag;     /* Format Category */
  unsigned short wChannels;      /* Number of Audio Channels */
  int dwSamplesPerSec;           /* Samples/sec/channel */
  int dwAvgBytesPerSec;          /* Average Bytes per Second Overall */
  unsigned short wBlockAlign;    /* Data Block Size */
  unsigned short wBitsPerSample; /* Sample Size */
  int dwWaveDataSize;            /* Data Chunk Length */
  unsigned dwFileSize;           /* Number of Audio Samples */
  time_t tWavLength;              /* Length of Audio in seconds */
};

struct wavChunk {
  char sName[5];                 /* Chunk Name */
  off_t oOffset;                 /* Offset to start of chunk */
  off_t oSize;                   /* Size of chunk */
};

struct wavProcess {
  double dfThreshold;            /* Audio Detect Threshold, in in dBd */
  int dSenseTimeout;             /* Audio Detect Timeout, in secs */
  double dfNormalLevel;          /* Normalize Level, in dBd */
}; 

#define LIST_SIZE 256
struct wavList {
                                 /* STANDARD SCHEME */     /* BDCST SCHEME */
  char sIcrd[LIST_SIZE];         /* Creation Date */       /* Not Present */
  char sIart[LIST_SIZE];         /* Original Artist */     /* Advertiser */
  char sIcmt[LIST_SIZE];         /* Comments */            /* Comments */
  char sIcop[LIST_SIZE];         /* Copyright */           /* Agency */
  char sIeng[LIST_SIZE];         /* Engineer */            /* Producer */
  char sIgnr[LIST_SIZE];         /* Genre */               /* Start Date */
  char sIkey[LIST_SIZE];         /* Key Words */           /* End Date */
  char sImed[LIST_SIZE];         /* Original Medium */     /* ???? */
  char sInam[LIST_SIZE];         /* Name */                /* Outcue */
  char sIsft[LIST_SIZE];         /* Software Package */    /* Account Exec. */
  char sIsrc[LIST_SIZE];         /* Source Supplier */     /* Category */
  char sItch[LIST_SIZE];         /* Digitizer */           /* Talent */
  char sIsbj[LIST_SIZE];         /* Subject */             /* Copy */
  char sIsrf[LIST_SIZE];         /* Digitization Source */ /* ???? */
};


/* 
 * Function Prototypes 
 *
 * API Entry Points
 */
extern int IsWav(int);
extern int OpenWav(char *,struct wavHeader *);
extern int CreateWav(char *,struct wavHeader *);
extern int FixWav(int,unsigned,unsigned);
extern int FindChunk(int,struct wavChunk *);
extern int GetNextChunk(int,struct wavChunk *);
extern int GetListChunk(int,struct wavList *);
extern int SetDspDesc(int,struct wavHeader *);
extern int PlayWavFile(char *,char *,unsigned);
extern int PlayWavDesc(char *,int,unsigned);
extern int PlayWavOffsetDesc(char *,int,int,unsigned);
extern int RecWavFile(char *,char *,unsigned,unsigned short,unsigned,
		      unsigned short,unsigned);
extern int RecordWavFile(char *,char *,int,struct wavHeader *,
			 struct wavProcess *,unsigned);
extern int RecWavDesc(char *,int,unsigned,unsigned short,unsigned,
		      unsigned short,unsigned);
extern int RecordWavDesc(char *,int,int,struct wavHeader *,
			 struct wavProcess *,unsigned);
extern double DbToLinear(double);
extern double LinearToDb(double);
extern int GetMixerCat(char *,int,unsigned);
extern int Pattern(int,char *);
extern int SoundConfigFile(char *,int,int);
extern int SoundConfigDesc(int,int,int);
extern ssize_t WriteSword(int,unsigned);
extern ssize_t WriteDword(int,unsigned);
extern int TailTrim(char *,int);
extern int TruncWav(int,struct wavHeader *,struct wavChunk *,unsigned);

/*
 * Signal Handlers
 */
extern void SigStopWav(int);
extern void SigPauseWav(int);
extern void SigResumeWav(int);


/* Microsoft WAVE Format Categories */
#define WAVE_FORMAT_PCM 0x0001
#define IBM_FORMAT_MULAW 0x0101
#define IBM_FORMAT_ALAW 0x0102
#define IBM_FORMAT_ADPCM 0x0103

/* 
 * Option Values 
 *
 * WAVLIB_TEST will cause the function to do everything it ordinarily 
 * would except actually play the file.  Useful for testing to see if
 * a particular format is supported by a device.
 */
#define WAVLIB_TEST 0x40000000

/*
 * WAVLIB_PAUSEABLE enables audio to be paused by sending SIGUSR1 to the
 * process and then resumed by sending SIGUSR2.
 */
#define WAVLIB_PAUSEABLE 0x20000000




