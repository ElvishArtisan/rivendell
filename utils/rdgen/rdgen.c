/* rdgen.c
 *
 *  A WAV file generator for test tones
 *
 * (C) Copyright 1997-2003 Fred Gleason <fredg@paravelsystems.com>
 *
 *      $Id: rdgen.c,v 1.4 2010/07/29 19:32:40 cvs Exp $
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
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <wavlib.h>


#define BUFFER_SIZE 16384
#define SAMPLE_RATE 48000
#define SAMPLE_SIZE 16
#define USAGE "rdgen [-f freq][-l level][-c left|right|both][-p now|rev][-t secs][-v] <wavfile>\n"

int main(int argc,char *argv[])
{
  int i;
  int hFilename;
  unsigned char cBuffer[BUFFER_SIZE];
  float fFreq=1000;
  float fLevel=0;
  float fRatio;
  int dChan=0;     /* 0=Both, 1=Left, 2=Right */
  char sChan[10];
  int dPhase=0;    /* 0=Normal, 1=Reverse */
  char sPhase[10];
  int dTime=0;
  int dMatch=0;
  int dDebug=0;
  int dSampleRate=0;
  char sFilename[128];  /* Name of audio device */
  float fGain;
  float fAngle;
  unsigned long ldCount,ldTimeline=0,ldLimit=BUFFER_SIZE;
  short int dSample;
  time_t tmTimestamp,tmTime;
  struct wavHeader wavHeader;
  unsigned dBytes=0,dTotalBytes;

  if(argc<2) {
    printf(USAGE);
    exit(0);
  }

  if(argc==2) {
    if(strcmp(argv[1],"-v")==0) {
      printf("wavgen v");
      printf(VERNUM);
      printf("\n");
      exit(0);
    }
  }

  /* Get device name */
  if(sscanf(argv[argc-1],"%s",sFilename)!=1) {
    printf("wavgen: invalid file anem\n");
    exit(1);
  }

  /* Get options */
  for(i=1;i<argc-1;i++) {
    dMatch=0;
    if(strcmp(argv[i],"-v")==0) {     /* Version */
      printf("wavgen v");
      printf(VERNUM);
      printf("\n");
      exit(0);
    }
    if(strcmp(argv[i],"-d")==0) {     /* Debug Mode */
      dMatch=1;
      dDebug=1;
    }
    
    if(strcmp(argv[i],"-f")==0) {     /* Frequency */
      dMatch=1;
      if(sscanf(argv[++i],"%f",&fFreq)!=1) {
	printf("wavgen: invalid frequency\n");
	exit(1);
      }
    }
    if(strcmp(argv[i],"-l")==0) {    /* Level */
      dMatch=1;
      if(sscanf(argv[++i],"%f",&fLevel)!=1) {
	printf("wavgen: invalid level\n");
	exit(1);
      }
      fLevel=-fLevel;
    }
    if(strcmp(argv[i],"-t")==0) {    /* Time */
      dMatch=1;
      if(sscanf(argv[++i],"%d",&dTime)!=1) {
	printf("wavgen: invalid time interval\n");
	exit(1);
      }
    }
    if(strcmp(argv[i],"-c")==0) {    /* Channel */
      dMatch=1;
      if(sscanf(argv[++i],"%s",sChan)!=1) {
	printf("wavgen: invalid time interval\n");
	exit(1);
      }
      if(strcasecmp(sChan,"both")==0) {
	dChan=0;
      }
      else {
	if(strcasecmp(sChan,"left")==0) {
	  dChan=1;
	}
	else {
	  if(strcasecmp(sChan,"right")==0) {
	    dChan=2;
	  }
	  else {
	    printf("wavgen: invalid channel\n");
	    exit(1);
	  }
	}
      }  
    }
    if(strcmp(argv[i],"-p")==0) {    /* Phase */
      dMatch=1;
      if(sscanf(argv[++i],"%s",sPhase)!=1) {
	printf("wavgen: invalid phase setting\n");
	exit(1);
      }
      if(strcasecmp(sPhase,"norm")==0) {
	dPhase=0;
      }
      else {
	if(strcasecmp(sPhase,"rev")==0) {
	  dPhase=1;
	}
	else {
	  printf("wavgen: invalid phase setting\n");
	  exit(1);
	}
      }  
    }
    if(dMatch==0) {
      printf("wavgen: invalid option\n");
      exit(1);
    }
    if(dTime==0) {
      printf("wavgen: missing time argument\n");
      exit(1);
    }
  }

  /* Convert db to ratio */
  fRatio=pow(10,(fLevel/20));

  /* Set audio characteristics */
  wavHeader.wFormatTag=WAVE_FORMAT_PCM;
  wavHeader.wChannels=2;
  wavHeader.dwSamplesPerSec=SAMPLE_RATE;
  wavHeader.dwAvgBytesPerSec=SAMPLE_RATE*(SAMPLE_SIZE/8)*2;
  wavHeader.wBlockAlign=SAMPLE_SIZE/4;
  wavHeader.wBitsPerSample=SAMPLE_SIZE;
  
  /* Open the wav file */
  hFilename=CreateWav(sFilename,&wavHeader);
  if(hFilename<0) {
    printf("wavgen: can't open wav file\n");
    exit(1);
  }

  /* Display Settings (if requested) */
  if(dDebug==1) {
    printf("--Audio Generator Settings--\n");
    printf("Frequency: %5.0f Hz\n",fFreq);
    printf("Level: %3.1f dB\n",fLevel);
    printf("Channel(s): ");
    switch(dChan) {
    case 0:
      printf("BOTH\n");
      break;
    case 1:
      printf("LEFT\n");
      break;
    case 2:
      printf("RIGHT\n");
      break;
    }
    printf("Phasing: ");
    switch(dPhase) {
    case 0:
      printf("NORMAL\n");
      break;
    case 1:
      printf("REVERSE\n");
      break;
    }
    printf("Effective Sample Rate: %d samples/sec\n",dSampleRate);
  }

  /* Setup time data */
  time(&tmTimestamp);
  tmTime=tmTimestamp;
  if(dTime>0) {
    tmTimestamp+=dTime;
  }
  else {
    tmTimestamp--;
  }

  /* Output audio */
  dTotalBytes=wavHeader.dwAvgBytesPerSec*dTime;

  switch(SAMPLE_SIZE) {
  case 8:
    fGain=127*fRatio;
    ldTimeline=0;
    ldLimit=BUFFER_SIZE/2;
    while(dBytes<dTotalBytes) { 
      i=0;
      for(ldCount=ldTimeline;ldCount<ldLimit;ldCount++) {
	fAngle=2*PI*ldCount*fFreq/SAMPLE_RATE;
	if(dChan==0 || dChan==1) {
	  cBuffer[i++]=fGain*sin(fAngle)+128;
	}
	else {
	  cBuffer[i++]=128;
	}
	if(dChan==0 || dChan==2) {
	  if(dPhase==0) {
	    cBuffer[i++]=fGain*sin(fAngle)+128;
	  }
	  else {
	    cBuffer[i++]=-fGain*sin(fAngle)+128;
	  }
	}
	else {
	  cBuffer[i++]=128;
	}
      }
      write(hFilename,cBuffer,BUFFER_SIZE);
      dBytes+=BUFFER_SIZE;
      ldLimit+=(BUFFER_SIZE/2);
      ldTimeline+=(BUFFER_SIZE/2);
      time(&tmTime);
    }  
    break;
  case 16:
    fGain=32767*fRatio;
    ldTimeline=0;
    ldLimit=BUFFER_SIZE/4;
    while(dBytes<dTotalBytes) { 
      i=0;
      for(ldCount=ldTimeline;ldCount<ldLimit;ldCount++) {
	fAngle=2*PI*ldCount*fFreq/SAMPLE_RATE;
	dSample=fGain*sin(fAngle);
	if(dChan==0 || dChan==1) {
	  cBuffer[i++]=0xFF&dSample;
	  cBuffer[i++]=(0xFF&(dSample>>8));  
	}
	else {
	  cBuffer[i++]=0;
	  cBuffer[i++]=0;
	}
	if(dChan==0 || dChan==2) {
	  if(dPhase==1) {
	    dSample=-dSample;
	  }
	  cBuffer[i++]=0xFF&dSample;
	  cBuffer[i++]=(0xFF&(dSample>>8));
	} 
	else {
	  cBuffer[i++]=0;
	  cBuffer[i++]=0;
	}
      }
      write(hFilename,cBuffer,BUFFER_SIZE);
      dBytes+=BUFFER_SIZE;
      ldLimit+=(BUFFER_SIZE/4);
      ldTimeline+=(BUFFER_SIZE/4);
      time(&tmTime);
    }  
  break;
  }

  /* close files and finish */
  FixWav(hFilename,dBytes/wavHeader.wBlockAlign,dBytes);
  close(hFilename);


  exit(0);
}
