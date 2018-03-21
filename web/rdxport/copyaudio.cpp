// copyaudio.cpp
//
// Rivendell web service portal -- CopyAudio service
//
//   (C) Copyright 2010-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <errno.h>
#include <unistd.h>

#include <rdapplication.h>
#include <rdaudioconvert.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rdformpost.h>
#include <rdsettings.h>
#include <rdweb.h>

#include <rdxport.h>

void Xport::CopyAudio()
{
  //
  // Verify Post
  //
  int source_cartnum=0;
  if(!xport_post->getValue("SOURCE_CART_NUMBER",&source_cartnum)) {
    XmlExit("Missing SOURCE_CART_NUMBER",400,"copyaudio.cpp",LINE_NUMBER);
  }
  int source_cutnum=0;
  if(!xport_post->getValue("SOURCE_CUT_NUMBER",&source_cutnum)) {
    XmlExit("Missing SOURCE_CUT_NUMBER",400,"copyaudio.cpp",LINE_NUMBER);
  }

  int destination_cartnum=0;
  if(!xport_post->getValue("DESTINATION_CART_NUMBER",&destination_cartnum)) {
    XmlExit("Missing DESTINATION_CART_NUMBER",400,"copyaudio.cpp",LINE_NUMBER);
  }
  int destination_cutnum=0;
  if(!xport_post->getValue("DESTINATION_CUT_NUMBER",&destination_cutnum)) {
    XmlExit("Missing DESTINATION_CUT_NUMBER",400,"copyaudio.cpp",LINE_NUMBER);
  }

  //
  // Verify User Perms
  //
  if(!rda->user()->cartAuthorized(source_cartnum)) {
    XmlExit("No such cart",404,"copyaudio.cpp",LINE_NUMBER);
  }
  if(!rda->user()->cartAuthorized(destination_cartnum)) {
    XmlExit("No such cart",404,"copyaudio.cpp",LINE_NUMBER);
  }

  //
  // Make the copy
  //
  unlink(RDCut::pathName(destination_cartnum,destination_cutnum));
  if(link(RDCut::pathName(source_cartnum,source_cutnum),
	  RDCut::pathName(destination_cartnum,destination_cutnum))!=0) {
    XmlExit(strerror(errno),400,"copyaudio.cpp",LINE_NUMBER);
  }
  SendNotification(RDNotification::CartType,RDNotification::ModifyAction,
		   QVariant(destination_cartnum));
  XmlExit("OK",200,"copyaudio.cpp",LINE_NUMBER);
}
