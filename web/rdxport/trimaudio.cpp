// trimaudio.cpp
//
// Rivendell web service portal -- TrimAudio service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: trimaudio.cpp,v 1.4 2012/02/13 23:01:50 cvs Exp $
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
#include <rdaudioconvert.h>
#include <rdsettings.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::TrimAudio()
{
  int point;

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

  int trim_level=0;
  if(!xport_post->getValue("TRIM_LEVEL",&trim_level)) {
    XmlExit("Missing TRIM_LEVEL",400);
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
  if(!wave->hasEnergy()) {
    XmlExit("No peak data available",400);
  }

  //
  // Send Data
  //
  printf("Content-type: application/xml\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<trimPoint>\n");
  printf("  <cartNumber>%u</cartNumber>\n",cartnum);
  printf("  <cutNumber>%d</cutNumber>\n",cutnum);
  printf("  <trimLevel>%d</trimLevel>\n",trim_level);
  point=wave->startTrim(REFERENCE_LEVEL-trim_level);
  if(point>=0) {
    point=(double)point*1000.0/(double)wave->getSamplesPerSec();
  }
  printf("  <startTrimPoint>%d</startTrimPoint>\n",point);
  point=wave->endTrim(REFERENCE_LEVEL-trim_level);
  if(point>=0) {
    point=(double)point*1000.0/(double)wave->getSamplesPerSec();
  }
  printf("  <endTrimPoint>%d</endTrimPoint>\n",point);
  printf("</trimPoint>\n");
  Exit(0);
}
