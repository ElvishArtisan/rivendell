// audioinfo.cpp
//
// Rivendell web service portal -- AudioInfo service
//
//   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: audioinfo.cpp,v 1.4 2012/02/13 23:01:50 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdwavefile.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::AudioInfo()
{
  RDWaveFile::Format format=RDWaveFile::Pcm16;;

  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    XmlExit("Missing CUT_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cartnum)) {
    XmlExit("No such cart",404);
  }

  //
  // Open Audio File
  //
  RDWaveFile *wave=new RDWaveFile(RDCut::pathName(cartnum,cutnum));
  if(!wave->openWave()) {
    XmlExit("No such audio",404);
  }

  //
  // Send Data
  //
  printf("Content-type: application/xml\n\n");

  switch(wave->getFormatTag()) {
  case WAVE_FORMAT_PCM:
    format=RDWaveFile::Pcm16;
    break;

  case WAVE_FORMAT_IEEE_FLOAT:
    format=RDWaveFile::Float32;
    break;

  case WAVE_FORMAT_MPEG:
    switch(wave->getHeadLayer()) {
    case 1:
      format=RDWaveFile::MpegL1;
      break;

    case 2:
      format=RDWaveFile::MpegL2;
      break;

    case 3:
      format=RDWaveFile::MpegL3;
      break;
    }
    break;

  default:
    XmlExit("Unknown audio format",400);
    break;
  }
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<audioInfo>\n");
  printf("  <cartNumber>%u</cartNumber>\n",cartnum);
  printf("  <cutNumber>%u</cutNumber>\n",cutnum);
  printf("  <format>%d</format>\n",format);
  printf("  <channels>%d</channels>\n",wave->getChannels());
  printf("  <sampleRate>%d</sampleRate>\n",wave->getSamplesPerSec());
  printf("  <frames>%u</frames>\n",wave->getSampleLength());
  printf("  <length>%u</length>\n",wave->getExtTimeLength());
  printf("</audioInfo>\n");
  delete wave;
  Exit(0);
}
