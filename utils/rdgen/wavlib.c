/* wavlib.c 
 *
 * A C Library for abstracting WAV files
 *
 * (C) Copyright 1997,1999-2003 Fred Gleason <fredg@paravelsystems.com>
 *
 *      $Id: wavlib.c,v 1.4 2007/02/14 21:59:12 fredg Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/soundcard.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include "wavlib.h"

/* #define BUFFER_SIZE 65536 */
#define BUFFER_SIZE 8192
#define DEFAULT_WAV_MODE 0644

/*
 * Global Variables
 */
int iWav=0;
int dWavPause=0;
int hGlobalAudio;

int IsWav(int hFilename)
{
  int i;
  char cBuffer[5];

  /* Is this a riff file? */
  lseek(hFilename,0,SEEK_SET);
  i=read(hFilename,cBuffer,4);
  if(i==4) {
    cBuffer[4]=0;
    if(strcmp("RIFF",cBuffer)!=0) {
      return FALSE;
    }
  }
  else {
    return FALSE;
  }
  /* Is this a WAVE file? */
  if(lseek(hFilename,8,SEEK_SET)!=8) {
    return FALSE;
  }
  i=read(hFilename,cBuffer,4);
  if(i==4) {
    cBuffer[4]=0;
    if(strcmp("WAVE",cBuffer)!=0) {
      return FALSE;
    }
  }
  else {
    return FALSE;
  }
  return TRUE;
}


int OpenWav(char *sFilename,struct wavHeader *wavFilename)
{
  int hFilename;
  struct wavChunk wavChunk;
  unsigned char cBuffer[16];
  int i;
  off_t oDataChunk;

  hFilename=open(sFilename,O_RDWR);
  if(hFilename<0) {
    return -1;
  }
  if(!IsWav(hFilename)) {
    return -2;
  }

  /*
   * Find the format chunk
   */
  strcpy(wavChunk.sName,"fmt ");
  if(FindChunk(hFilename,&wavChunk)<0) {
    close(hFilename);
    return -1;
  }

  /* 
   * Found the format chunk, now get the data
   */
  lseek(hFilename,8,SEEK_CUR);  
  i=read(hFilename,cBuffer,14);
  if(i<14) {
    return -1;
  }
  wavFilename->wFormatTag=cBuffer[0]+256*cBuffer[1];
  wavFilename->wChannels=cBuffer[2]+256*cBuffer[3];
  wavFilename->dwSamplesPerSec=cBuffer[4]+256*cBuffer[5]+
    65536*cBuffer[6]+16777216*cBuffer[7];
  wavFilename->dwAvgBytesPerSec=cBuffer[8]+256*cBuffer[9]+
    65536*cBuffer[10]+16777216*cBuffer[11];
  wavFilename->wBlockAlign=cBuffer[12]+256*cBuffer[13];
  if(wavFilename->wFormatTag==WAVE_FORMAT_PCM) { /*Get bits per sample*/
    i=read(hFilename,cBuffer,2);
    if(i<2) {
      close(hFilename);
      return -1;
    }
    wavFilename->wBitsPerSample=cBuffer[0]+256*cBuffer[1];
  }

  /* 
   * Find the data chunk
   */
  strcpy(wavChunk.sName,"data");
  if(FindChunk(hFilename,&wavChunk)<0) {
    close(hFilename);
    return -1;
  }
  wavFilename->dwWaveDataSize=wavChunk.oSize;
  oDataChunk=wavChunk.oOffset;  /* Save this for restoration later */

  /*
   * Determine total number of samples (preferably from the fact chunk)
   */
  strcpy(wavChunk.sName,"fact");
  lseek(hFilename,12,SEEK_SET);
  if(FindChunk(hFilename,&wavChunk)<0) {  /* Not present, calculate the size */
    wavFilename->dwFileSize=wavFilename->dwWaveDataSize/
      wavFilename->wBlockAlign;
  }
  else {   /* Pull it from the fact chunk */
    lseek(hFilename,8,SEEK_CUR);
    i=read(hFilename,cBuffer,4);
    if(i!=4) {
      close(hFilename);
      return -1;
    }
    wavFilename->dwFileSize=cBuffer[0]+256*cBuffer[1]+65536*cBuffer[2]+
      16777216*cBuffer[3];
  }

  /*
   * Calculate audio length in seconds
   */
  wavFilename->tWavLength=wavFilename->dwFileSize/wavFilename->dwSamplesPerSec;
  lseek(hFilename,oDataChunk+8,SEEK_SET);
  return hFilename;
}



int CreateWav(char *sFilename,struct wavHeader *wavHeader)
{
  int hFilename;

  /*
   * Some sanity checks
   */
  if(wavHeader->wFormatTag!=WAVE_FORMAT_PCM) {
    return -1;
  }
  /*
   * Don't use O_TRUNC here, it can cause a significant (several second or 
   * more) delay on large files.  We'll come back and trim the fat after the 
   * recording's done.
   */
  hFilename=open(sFilename,O_RDWR|O_CREAT,DEFAULT_WAV_MODE);
  if(hFilename<0) {
    return -1;
  }

  /*
   * Write the WAVE Header
   */
  write(hFilename,"RIFF",4);
  WriteDword(hFilename,0);
  write(hFilename,"WAVE",4);

  /*
   * Write the format chunk
   */
  write(hFilename,"fmt ",4);
  WriteDword(hFilename,16);
  WriteSword(hFilename,wavHeader->wFormatTag);
  WriteSword(hFilename,wavHeader->wChannels);
  WriteDword(hFilename,wavHeader->dwSamplesPerSec);
  WriteDword(hFilename,wavHeader->dwAvgBytesPerSec);
  WriteSword(hFilename,wavHeader->wBlockAlign);
  WriteSword(hFilename,wavHeader->wBitsPerSample);

  /*
   * Write the fact chunk
   */
  write(hFilename,"fact",4);
  WriteDword(hFilename,4);
  WriteDword(hFilename,0);

  /*
   * Write the data chunk
   */
  write(hFilename,"data",4);
  WriteDword(hFilename,0);

  return hFilename;
}



int FixWav(int hFilename,unsigned dSamples,unsigned dBytes)
{
  struct stat statData;
  struct wavChunk wavChunk;

  /*
   * Get the file size
   */
  if(fstat(hFilename,&statData)<0) {
    return -1;
  }

  /*
   * Update the file-size counter 
   */
  lseek(hFilename,4,SEEK_SET);
  WriteDword(hFilename,statData.st_size-8);

  /*
   * Update the fact chunk
   */ 
  strcpy(wavChunk.sName,"fact");
  if(FindChunk(hFilename,&wavChunk)>0) {
    lseek(hFilename,8,SEEK_CUR);
    WriteDword(hFilename,dSamples);
  }

  /*
   * Update the data chunk
   */
  strcpy(wavChunk.sName,"data");
  FindChunk(hFilename,&wavChunk);
  lseek(hFilename,4,SEEK_CUR);
  WriteDword(hFilename,dBytes);

  return 0;
}



int FindChunk(int hFilename,struct wavChunk *wavChunk)
{
  int i;
  char sName[5]={0,0,0,0,0};
  unsigned char dBuffer[4];
  off_t oSize;

  lseek(hFilename,12,SEEK_SET);
  i=read(hFilename,sName,4);
  i=read(hFilename,dBuffer,4);
  oSize=dBuffer[0]+(256*dBuffer[1])+(65536*dBuffer[2])+(16777216*dBuffer[3]);
  while(i==4) {
    if(strcasecmp(wavChunk->sName,sName)==0) {
      wavChunk->oOffset=lseek(hFilename,-8,SEEK_CUR);
      wavChunk->oSize=oSize;
      return 0;
    }
    lseek(hFilename,oSize,SEEK_CUR);
    i=read(hFilename,sName,4);
    i=read(hFilename,dBuffer,4);
    oSize=dBuffer[0]+(256*dBuffer[1])+(65536*dBuffer[2])+(16777216*dBuffer[3]);
  }
  return -1;
}
  


int GetNextChunk(int hFilename,struct wavChunk *wavChunk)
{
  int i;
  char sName[5]={0,0,0,0,0};
  unsigned char dBuffer[4];
  off_t oSize;

  i=read(hFilename,sName,4);
  i=read(hFilename,dBuffer,4);
  oSize=dBuffer[0]+(dBuffer[1]*256)+(dBuffer[2]*65536)+(dBuffer[3]*16777216);
  if(i==4) {
    strcpy(wavChunk->sName,sName);
    wavChunk->oOffset=lseek(hFilename,0,SEEK_CUR);
    wavChunk->oSize=oSize;
    lseek(hFilename,oSize,SEEK_CUR);
    return 0;
  }
  return -1;
}



int GetListChunk(int hFilename,struct wavList *wavList)
{
  struct wavChunk wavChunk;
  int i;
  char cBuffer[LIST_SIZE];
  off_t oLimit,oOffset;
  char sName[5]={0,0,0,0,0};
  unsigned dBuffer[4];
  char sBuffer[LIST_SIZE];
  off_t oTemp,oSize;
  int dReturn=1;

  /*
   * Initialize Data Structures 
   */
  memset(wavList,0,sizeof(struct wavList));

  /*
   * Find the list chunk
   */
  strcpy(wavChunk.sName,"LIST");
  if(FindChunk(hFilename,&wavChunk)<0) {
    return -1;
  }
  oLimit=wavChunk.oOffset+wavChunk.oSize;

  /*
   * Setup to start & do some basic sanity checking 
   */
  oOffset=lseek(hFilename,8,SEEK_CUR);
  i=read(hFilename,cBuffer,4);
  oOffset+=i;
  cBuffer[4]=0;
  if(strcasecmp(cBuffer,"INFO")!=0) {  /* I don't understand this format! */
    return -1;
  }

  /*
   * Scan through the chunk
   */
  while(oOffset<oLimit) {
    oOffset+=read(hFilename,sName,4);
    oOffset+=read(hFilename,dBuffer,4);
    oSize=dBuffer[0]+(dBuffer[1]<<8)+(dBuffer[2]<<16)+(dBuffer[3]<<24);
    oTemp=read(hFilename,sBuffer,dBuffer[0]);
    lseek(hFilename,oSize-oTemp,SEEK_CUR);
    read(hFilename,dBuffer,1);
    if(dBuffer[0]!=0) {
      lseek(hFilename,-1,SEEK_CUR);
    }

    /* 
     * Assign the field value
     */
    if(strcasecmp(sName,"ICRD")==0) {
      strcpy(wavList->sIcrd,sBuffer);
      dReturn=0;
    }
    if(strcasecmp(sName,"IART")==0) {
      strcpy(wavList->sIart,sBuffer);
    }
    if(strcasecmp(sName,"ICMT")==0) {
      strcpy(wavList->sIcmt,sBuffer);
    }
    if(strcasecmp(sName,"ICOP")==0) {
      strcpy(wavList->sIcop,sBuffer);
    }
    if(strcasecmp(sName,"IENG")==0) {
      strcpy(wavList->sIeng,sBuffer);
    }
    if(strcasecmp(sName,"IGNR")==0) {
      strcpy(wavList->sIgnr,sBuffer);
    }
    if(strcasecmp(sName,"IKEY")==0) {
      strcpy(wavList->sIkey,sBuffer);
    }
    if(strcasecmp(sName,"IMED")==0) {
      strcpy(wavList->sImed,sBuffer);
    }
    if(strcasecmp(sName,"INAM")==0) {
      strcpy(wavList->sInam,sBuffer);
    }
    if(strcasecmp(sName,"ISFT")==0) {
      strcpy(wavList->sIsft,sBuffer);
    }
    if(strcasecmp(sName,"ISRC")==0) {
      strcpy(wavList->sIsrc,sBuffer);
    }
    if(strcasecmp(sName,"ITCH")==0) {
      strcpy(wavList->sItch,sBuffer);
    }
    if(strcasecmp(sName,"ISBJ")==0) {
      strcpy(wavList->sIsbj,sBuffer);
    }
    if(strcasecmp(sName,"ISRF")==0) {
      strcpy(wavList->sIsrf,sBuffer);
    }
  }
  return dReturn;
}  



int SetDspDesc(int hAudio,struct wavHeader *wavHeader)
{
  int dRequest;

  /* 
   * Set sampling format & bits per sample 
   */
  if(wavHeader->wFormatTag==WAVE_FORMAT_PCM) {
    switch(wavHeader->wBitsPerSample) {
    case 8:
      dRequest=AFMT_U8;
      ioctl(hAudio,SNDCTL_DSP_SETFMT,&dRequest);
      if(dRequest!=AFMT_U8) {
	return -1;
      }
      break;
    case 16:
      dRequest=AFMT_S16_LE;
      ioctl(hAudio,SNDCTL_DSP_SETFMT,&dRequest);
      if(dRequest!=AFMT_S16_LE) {
	return -1;
      }
      break;
    default:
      return -1;
      break;
    }
  }
  else {
    return -1;
  }
  /* Set number of channels */
  if(wavHeader->wChannels>0 && wavHeader->wChannels<3) {
    dRequest=wavHeader->wChannels-1;
    ioctl(hAudio,SNDCTL_DSP_STEREO,&dRequest);
    if(dRequest!=wavHeader->wChannels-1) {
      return -1;
    }
  }
  else {
    return -1;
  }

  /* 
   * Set sampling rate 
   */
  dRequest=(int)wavHeader->dwSamplesPerSec;
  ioctl(hAudio,SNDCTL_DSP_SPEED,&dRequest);
  if((dRequest<(int)(.99*wavHeader->dwSamplesPerSec)) ||
     (dRequest>(int)(1.01*wavHeader->dwSamplesPerSec))) {
    return -1;
  }

  return 0;
}




int PlayWavFile(char *sFilename,char *sAudioDev,unsigned dOptions)
{
  int hAudio;
  int i;

  if((hAudio=open(sAudioDev,O_WRONLY))<0) {
    return -1;
  }
  i=PlayWavDesc(sFilename,hAudio,dOptions);
  close(hAudio);
  if(i<0) {
    return -1;
  }
  return 0;
}




int PlayWavDesc(char *sFilename,int hAudio,unsigned dOptions)
{
  return PlayWavOffsetDesc(sFilename,hAudio,0,dOptions);
}




int PlayWavOffsetDesc(char *sFilename,int hAudio,int dOffset,unsigned dOptions)
{
  struct wavHeader wavFilename;
  struct wavChunk wavChunk;
  int hFilename;
  int j,k;
  unsigned char cBuffer[BUFFER_SIZE];
  unsigned dWaveOffset=0;

  /*
   * Pass the device fd to a global so the signal handlers can get it
   */
  hGlobalAudio=hAudio;

  /*
   * Signal Handling Stuff
   */
  if((dOptions&WAVLIB_PAUSEABLE)!=0) {
    (void) signal(SIGTERM,SigStopWav);
    (void) signal(SIGUSR1,SigPauseWav);
    (void) signal(SIGUSR2,SigResumeWav);
  }
    
  hFilename=OpenWav(sFilename,&wavFilename);
  if(hFilename<0) {
    return -1;
  }

  if(SetDspDesc(hAudio,&wavFilename)<0) {
    return -1;
  }

  /*
   * Calculate Wave Data Offset
   */
  dWaveOffset=dOffset*wavFilename.wBlockAlign;

  /* 
   * OK -- time to play the file! 
   */
  strcpy(wavChunk.sName,"data");
  if(FindChunk(hFilename,&wavChunk)<0) {
    return -1;
  }
  lseek(hFilename,8+dWaveOffset,SEEK_CUR);
  if((dOptions & WAVLIB_TEST)==0) {
    j=(wavFilename.dwWaveDataSize-dWaveOffset)/BUFFER_SIZE;
    k=(wavFilename.dwWaveDataSize-dWaveOffset)-j*BUFFER_SIZE;
    for(iWav=0;iWav<j;iWav++) {
      read(hFilename,cBuffer,BUFFER_SIZE);
      if(dWavPause==1) {
	select(0,NULL,NULL,NULL,NULL);    /* Sleep till we get a signal */
      }
      write(hAudio,cBuffer,BUFFER_SIZE);
    }
    memset(cBuffer,0,BUFFER_SIZE);
    read(hFilename,cBuffer,k);
    if(dWavPause==1) {
      select(0,NULL,NULL,NULL,NULL);    /* Sleep till we get a signal */
    }
    write(hAudio,cBuffer,k);
  }
  /* Finish up */
  close(hFilename);
  return 0;    
}

void SigStopWav(int sig)
{
  ioctl(hGlobalAudio,SNDCTL_DSP_RESET,NULL);
  exit(0);
}

void SigPauseWav(int sig)
{
  dWavPause=1;
  ioctl(hGlobalAudio,SNDCTL_DSP_RESET,NULL);
  (void) signal(SIGUSR1,SigPauseWav);
}

void SigResumeWav(int sig)
{     
  dWavPause=0;
  (void) signal(SIGUSR2,SigResumeWav);
}


int RecWavFile(char *sFilename,char *sAudioDev,unsigned dSeconds,
	   unsigned short dChannels,unsigned ldSamplesPerSec,
           unsigned short dBitsPerSample,unsigned dOptions)
{
  int hAudio;
  int i;

  hAudio=open(sAudioDev,O_RDONLY);
  if(hAudio<0) {
    return -1;
  }
  i=RecWavDesc(sFilename,hAudio,dSeconds,dChannels,ldSamplesPerSec,
	     dBitsPerSample,dOptions);
  close(hAudio);
  if(i<0) {
    return -1;
  }
  return 0;
}



int RecWavDesc(char *sFilename,int hAudio,unsigned dSeconds,
	   unsigned short dChannels,unsigned ldSamplesPerSec,
           unsigned short dBitsPerSample,unsigned dOptions)
{
  int hFilename=0;
  int dRequest;
  char cBuffer[BUFFER_SIZE];
  unsigned dBytesPerSec,dBytesPerSample=0;
  unsigned dBufferWrites;
  unsigned dTotalSampleBytes;
  unsigned dTotalWavSize;
  unsigned i;
  unsigned short dFormatTag=WAVE_FORMAT_PCM;
  unsigned short dBlockAlign;

  /* Check for format encoding type */
  if(dFormatTag!=WAVE_FORMAT_PCM) {
    return -1;
  }
  /* Set sampling format & bits per sample */
  if(dFormatTag==WAVE_FORMAT_PCM) {
    switch(dBitsPerSample) {
    case 8:
      dRequest=AFMT_U8;
      ioctl(hAudio,SNDCTL_DSP_SETFMT,&dRequest);
      if(dRequest!=AFMT_U8) {
	close(hFilename);
	return -1;
      }
      break;
    case 16:
      dRequest=AFMT_S16_LE;
      ioctl(hAudio,SNDCTL_DSP_SETFMT,&dRequest);
      if(dRequest!=AFMT_S16_LE) {
	close(hFilename);
	return -1;
      }
      break;
    default:
      close(hFilename);
      return -1;
      break;
    }
  }
  else {
    close(hFilename);
    return -1;
  }
  /* Set number of channels */
  if(dChannels>0 && dChannels<3) {
    dRequest=dChannels-1;
    ioctl(hAudio,SNDCTL_DSP_STEREO,&dRequest);
    if(dRequest!=dChannels-1) {
      close(hFilename);
      return -1;
    }
  }
  else {
    close(hFilename);
    return -1;
  }

  /* Set sampling rate */
  dRequest=(int)ldSamplesPerSec;
  ioctl(hAudio,SNDCTL_DSP_SPEED,&dRequest);
  /*      if(dRequest<(int).99*ldSamplesPerSec ||
	  dRequest>(int)1.01*ldSamplesPerSec) {
	  close(hFilename);
	  return -1;
	  }  */

  /* OK, we know the soundcard can handle it */
  /* If this is just a test, exit */
  if((dOptions & WAVLIB_TEST)!=0) {
    return 0;
  }

  /* Calculate actual bytes/sec overall */
  if(dBitsPerSample<=8) {
    dBytesPerSample=1;
  }
  if(dBitsPerSample>8 && dBitsPerSample<=16) {
    dBytesPerSample=2;
  }
  if(dBitsPerSample>16) {
    dBytesPerSample=3;
  }
  dBlockAlign=dBytesPerSample*dChannels;
  dBytesPerSec=ldSamplesPerSec*dBytesPerSample*dChannels;
  /* Calculate buffer writes and total sample bytes needed */
  dTotalSampleBytes=dBytesPerSec*dSeconds;
  dBufferWrites=dTotalSampleBytes/BUFFER_SIZE+1;
  dTotalSampleBytes=dBufferWrites*BUFFER_SIZE;
  dTotalWavSize=dTotalSampleBytes+36;

  /* 
   * Open the file and write the header 
   *
   * First, open the file, using the appropriate permissions 
   *
   * Don't use O_TRUNC here, it can cause a significant (several second or 
   * more) delay on large files.  We'll come back and trim the fat after the 
   * recording's done.
   */
  hFilename=open(sFilename,O_WRONLY|O_CREAT,DEFAULT_WAV_MODE);
  if(hFilename<0) {
    perror("open");
    return -1;
  }

  /* The 'RIFF' field */
  strcpy(cBuffer,"RIFF");
  write(hFilename,cBuffer,4);

  /* The size pointer */
  /* wav files are little endian! */
  cBuffer[0]=dTotalWavSize & 0x000000FF;
  cBuffer[1]=(dTotalWavSize & 0x0000FF00) >> 8;
  cBuffer[2]=(dTotalWavSize & 0x00FF0000) >> 16;
  cBuffer[3]=(dTotalWavSize & 0xFF000000) >> 24;
  write(hFilename,cBuffer,4);  

  /* The WAVE field */
  strcpy(cBuffer,"WAVE");
  write(hFilename,cBuffer,4);

  /* Now the format chunk */
  /* First the chunk header */
  strcpy(cBuffer,"fmt ");
  write(hFilename,cBuffer,4);

  /* The chunk size field */
  cBuffer[0]=0x10;
  cBuffer[1]=0;
  cBuffer[2]=0;
  cBuffer[3]=0;
  write(hFilename,cBuffer,4);

  /* The Format Category */
  cBuffer[0]=dFormatTag & 0x00FF;
  cBuffer[1]=(dFormatTag & 0xFF00) >> 8;
  write(hFilename,cBuffer,2);

  /* Number of Channels */
  cBuffer[0]=dChannels & 0x00FF;
  cBuffer[1]=(dChannels & 0xFF00) >> 8;
  write(hFilename,cBuffer,2);

  /* Samples/second/channel */
  cBuffer[0]=ldSamplesPerSec & 0x000000FF;
  cBuffer[1]=(ldSamplesPerSec & 0x0000FF00) >> 8;
  cBuffer[2]=(ldSamplesPerSec & 0x00FF0000) >> 16;
  cBuffer[3]=(ldSamplesPerSec & 0xFF000000) >> 24;
  write(hFilename,cBuffer,4);

  /* Average bytes per second */
  cBuffer[0]=dBytesPerSec & 0x000000FF;
  cBuffer[1]=(dBytesPerSec & 0x0000FF00) >> 8;
  cBuffer[2]=(dBytesPerSec & 0x00FF0000) >> 16;
  cBuffer[3]=(dBytesPerSec & 0xFF000000) >> 24;
  write(hFilename,cBuffer,4);

  /* Data Block Alignment */
  cBuffer[0]=dBlockAlign & 0x00FF;
  cBuffer[1]=(dBlockAlign & 0xFF00) >> 8;
  write(hFilename,cBuffer,2);

  /* Bits per Sample */
  /* NOTE: This field pertains only to WAVE_FORMAT_PCM format encoding */
  cBuffer[0]=dBitsPerSample & 0x00FF;
  cBuffer[1]=(dBitsPerSample & 0xFF00) >> 8;
  write(hFilename,cBuffer,2);

  /* Now, the data chunk */
  /* The 'data' field */
  strcpy(cBuffer,"data");
  write(hFilename,cBuffer,4);

  /* The data size field */
  cBuffer[0]=dTotalSampleBytes & 0x000000FF;
  cBuffer[1]=(dTotalSampleBytes & 0x0000FF00) >> 8;
  cBuffer[2]=(dTotalSampleBytes & 0x00FF0000) >> 16;
  cBuffer[3]=(dTotalSampleBytes & 0xFF000000) >> 24;
  write(hFilename,cBuffer,4);

  /* Move the audio data */
  for(i=0;i<dBufferWrites;i++) {
    read(hAudio,cBuffer,BUFFER_SIZE);
    write(hFilename,cBuffer,BUFFER_SIZE);
  }
  
  /* Finish up */
  ftruncate(hFilename,lseek(hFilename,0,SEEK_CUR));
  close(hFilename);

  return 0;
}



int RecordWavFile(char *sFilename,char *sAudio,int dSeconds,
		  struct wavHeader *wavHeader,struct wavProcess *wavProcess,
		  unsigned dOptions)
{
  int hAudio;

  hAudio=open(sAudio,O_RDWR);
  if(hAudio<0) {
    return -1;
  }
  return RecordWavDesc(sFilename,hAudio,dSeconds,
		       wavHeader,wavProcess,dOptions);
}



int RecordWavDesc(char *sFilename,int hAudio,int dSeconds,
		  struct wavHeader *wavHeader,struct wavProcess *wavProcess,
		  unsigned dOptions)
{
  int i;
  int hFilename;
  int dBufferWrites,dExcessWrites;
  int dSense=0,dStartWrite=BUFFER_SIZE,dPosThreshold,dNegThreshold;
  unsigned char cBuffer[BUFFER_SIZE];
  short *dBuffer;
  int dBufferCount=0;

  /*
   * Do we support this format?
   */
  if(wavHeader->wFormatTag!=WAVE_FORMAT_PCM) {
    return -1;
  }

  /*
   * Flesh out the wavHeader Structure
   */
  wavHeader->dwAvgBytesPerSec=wavHeader->wChannels*wavHeader->dwSamplesPerSec*
    (wavHeader->wBitsPerSample/8);
  wavHeader->wBlockAlign=wavHeader->wChannels*(wavHeader->wBitsPerSample/8);
  wavHeader->dwWaveDataSize=wavHeader->dwAvgBytesPerSec*dSeconds;
  wavHeader->dwFileSize=wavHeader->dwWaveDataSize/wavHeader->wBlockAlign;
  wavHeader->tWavLength=dSeconds;

  if(SetDspDesc(hAudio,wavHeader)<0) {   /* Format not supported by hardware */
    return -1;
  }
  if((dOptions&WAVLIB_TEST)!=0) {   /* Just a test, exit */
    return 0;
  }

  /*
   * Look for silence sense option
   */
  if(wavProcess!=NULL) {
    if(wavProcess->dSenseTimeout!=0) {
      if(wavHeader->wBitsPerSample==8) {
	dPosThreshold=(int)(128+(128*DbToLinear(-wavProcess->dfThreshold)));
	dNegThreshold=(int)(128-(128*DbToLinear(-wavProcess->dfThreshold)));
	dBufferCount=wavHeader->dwAvgBytesPerSec*wavProcess->dSenseTimeout/
	  BUFFER_SIZE;
	while((dSense==0)&&((dBufferCount--)>0)) {
	  read(hAudio,cBuffer,BUFFER_SIZE);
	  for(i=0;i<BUFFER_SIZE;i++) {
	    if((cBuffer[i]>dPosThreshold)||(cBuffer[i]<dNegThreshold)) {
	      dSense=1;
	      dStartWrite=i;
	      i=BUFFER_SIZE;
	    }
	  }
	}
      }
      if(wavHeader->wBitsPerSample==16) {
	dBuffer=(short *)cBuffer;
	dPosThreshold=(int)(32768*DbToLinear(-wavProcess->dfThreshold));
	dNegThreshold=(int)(-32768*DbToLinear(-wavProcess->dfThreshold));
	dBufferCount=wavHeader->dwAvgBytesPerSec*wavProcess->dSenseTimeout/
	  BUFFER_SIZE;
	while((dSense==0)&&((dBufferCount--)>0)) {
	  read(hAudio,cBuffer,BUFFER_SIZE);
	  for(i=0;i<BUFFER_SIZE/2;i++) {
	    if((dBuffer[i]>dPosThreshold)||(dBuffer[i]<dNegThreshold)) {
	      dSense=1;
	      dStartWrite=i*2;
	      i=BUFFER_SIZE;
	    }
	  }
	}
      }
      if(dBufferCount<=0) {
	return -1;
      }
    }
  }

  /*
   * Write the wav file
   */
  hFilename=CreateWav(sFilename,wavHeader);
  if(hFilename<0) {
    return -1;
  }
  dBufferWrites=(wavHeader->dwWaveDataSize-(BUFFER_SIZE-dStartWrite))/
    BUFFER_SIZE;
  dExcessWrites=(wavHeader->dwWaveDataSize-(BUFFER_SIZE-dStartWrite))-
    (dBufferWrites*BUFFER_SIZE);
  if(dSense==1) {
    write(hFilename,cBuffer+dStartWrite,BUFFER_SIZE-dStartWrite);
  }
  for(i=0;i<dBufferWrites;i++) {
    read(hAudio,cBuffer,BUFFER_SIZE);
    write(hFilename,cBuffer,BUFFER_SIZE);
  }
  read(hAudio,cBuffer,dExcessWrites);
  write(hFilename,cBuffer,dExcessWrites);

  /*
   * Finish Up
   */
  ftruncate(hFilename,lseek(hFilename,0,SEEK_CUR));
  FixWav(hFilename,wavHeader->dwFileSize,wavHeader->dwWaveDataSize);
  close(hFilename);

  return 0;
}



int SoundConfigFile(char *sAudioDev,int dParam,int dRequest)

     /* This function performs an ioctl call on the audio device 
        named by sAudioDev, using the ioctl dParam and the argument
	dRequest.  The ioctls are those defined in the OSS API. 
	If successful, the function returns the updated value of
	dRequest, otherwise, it returns a -1. 

	It is anticipated that this function will be useful mainly
	as a way for manipulating the soundcard mixer settings */

{     
  int dAudioDev;
  int i;

  dAudioDev=open(sAudioDev,O_RDONLY);
  if(dAudioDev<0) {
    return -1;
  }
  i=SoundConfigDesc(dAudioDev,dParam,dRequest);
  close(dAudioDev);
  if(i<0) {
    return -1;
  }
  else {
    return dRequest;
  }
}



double DbToLinear(double dfDb)
{
  return pow(10,dfDb/20);
}



double LinearToDb(double dfLinear)
{
  if(dfLinear==0) {
    return -1;
  }
  return 20*log10(dfLinear);
}



int SoundConfigDesc(int dAudioDev,int dParam,int dRequest)

     /* This function performs an ioctl call on the audio device 
        open on hAudioDev, using the ioctl dParam and the argument
	dRequest.  The ioctls are those defined in the OSS API. 
	If successful, the function returns the updated value of
	dRequest, otherwise, it returns a -1. 

	It is anticipated that this function will be useful mainly
	as a way for manipulating the soundcard mixer settings */

{     
  int i;

  i=ioctl(dAudioDev,dParam,&dRequest);
  if(i<0) {
    return -1;
  }
  else {
    return dRequest;
  }
}


     
int Pattern(int dFilename,char *sPattern)

     /* This function finds the looks for the string sPattern
in the file pointed to by dFilename, starting at the current 
pointer position.  If found, it returns the pointer offset and
with the pointer set to the byte immediately following the pattern.
If the pattern is not found, it returns -1. */

{
  int i,j;
  int dOffset=0;  /* file pointer offset */
  unsigned char cBuffer;
  int dPatLength;

  dPatLength=strlen(sPattern);
  i=read(dFilename,&cBuffer,1);
  if(i<0) {
    perror("read");
    return -1;
  }
  dOffset++;
  while(i==1) {
    j=0;
    while(cBuffer==sPattern[j++] && j<=dPatLength) {
      if(j==dPatLength) {   /* it's a match! */
	return dOffset;
      }
      i=read(dFilename,&cBuffer,1);
      dOffset++;
    }
    lseek(dFilename,-(j-1),SEEK_CUR);
    dOffset-=(j-1);
    i=read(dFilename,&cBuffer,1);
    dOffset++;
  }
  lseek(dFilename,-dOffset,SEEK_CUR);
  printf("Offset: %d\n",dOffset);
  return (-1);
}




ssize_t WriteSword(int hFilename,unsigned dValue)
{
  unsigned char cBuffer[2];

  cBuffer[0]=dValue&0xFF;
  cBuffer[1]=(dValue>>8)&0xFF;

  return write(hFilename,cBuffer,2);
}




ssize_t WriteDword(int hFilename,unsigned dValue)
{
  unsigned char cBuffer[4];

  cBuffer[0]=dValue&0xFF;
  cBuffer[1]=(dValue>>8)&0xFF;
  cBuffer[2]=(dValue>>16)&0xFF;
  cBuffer[3]=(dValue>>24)&0xFF;

  return write(hFilename,cBuffer,4);
}




int TailTrim(char *sFilename,int dThreshold)
{
  int i,j;
  int hFilename;
  struct wavHeader wavHeader;
  struct wavChunk wavChunk;
  long dEnd;
  unsigned char cBuffer[BUFFER_SIZE];
  short *dBuffer;

  /*
   * Initialize
   */
  dBuffer=(short *)cBuffer;

  /*
   * Open the file
   */
  hFilename=OpenWav(sFilename,&wavHeader);
  if(hFilename<0) {
    return hFilename;
  }
  
  /*
   * Find the end of the audio data
   */
  memset(&wavChunk,0,sizeof(struct wavChunk));
  strcpy(wavChunk.sName,"data");
  if(FindChunk(hFilename,&wavChunk)<0) {
    return -1;
  }

  /*
  lseek(hFilename,wavChunk.oOffset+wavChunk.oSize-(BUFFER_SIZE*500),SEEK_SET);
  read(hFilename,cBuffer,BUFFER_SIZE);
  for(j=BUFFER_SIZE/2;j>0;j--) {
    printf("VALUE: %d\n",dBuffer[j]);
  }
  exit(0);
  */

  /*
   * Scan backwards through the data looking for audio
   */
  lseek(hFilename,wavChunk.oOffset+wavChunk.oSize+6,SEEK_SET);
  for(i=0;i<wavChunk.oSize/BUFFER_SIZE;i++) {
    lseek(hFilename,-BUFFER_SIZE,SEEK_CUR);
    read(hFilename,cBuffer,BUFFER_SIZE);
    lseek(hFilename,-BUFFER_SIZE,SEEK_CUR);
    for(j=BUFFER_SIZE/2-100;j>=0;j--) {
      /*      printf("Threshold: %d   Value: %d\n",dThreshold,dBuffer[j]); */
      if(abs(dBuffer[j])>=dThreshold) {
	TruncWav(hFilename,&wavHeader,&wavChunk,lseek(hFilename,0,SEEK_CUR)+j*2); 
	close(hFilename);
	exit(0); 
      }
    }
  }
  dEnd=wavChunk.oSize-BUFFER_SIZE*(wavChunk.oSize/BUFFER_SIZE);
  lseek(hFilename,-dEnd,SEEK_CUR);
  read(hFilename,cBuffer,dEnd);
  lseek(hFilename,-dEnd,SEEK_CUR);
  for(j=dEnd-1;j>=0;j--) {
    if(abs(dBuffer[j])>=dThreshold) {
      TruncWav(hFilename,&wavHeader,&wavChunk,lseek(hFilename,0,SEEK_CUR)+j);
    }
  }
  
  /*
   * Finish up
   */
  close(hFilename);
  return 0;
}





int TruncWav(int hFilename,struct wavHeader *wavHeader,
	     struct wavChunk *wavChunk,unsigned dTruncPoint)
{
  unsigned dBytes;

  if(ftruncate(hFilename,dTruncPoint)<0) {
    return -1;
  }
  dBytes=dTruncPoint-wavChunk->oOffset;
  if(FixWav(hFilename,dBytes/(wavHeader->wBitsPerSample/8),dBytes)<0) {
    return -1;
  }
  return 0;
}

