// deleteaudio.cpp
//
// Rivendell web service portal -- DeleteAudio service
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rdformpost.h>
#include <rdweb.h>

#include <rdxport.h>

void Xport::DeleteAudio()
{
  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    XmlExit("Missing CART_NUMBER",400,"deleteaudio.cpp",LINE_NUMBER);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    XmlExit("Missing CUT_NUMBER",400,"deleteaudio.cpp",LINE_NUMBER);
  }

  //
  // Process Request
  //
  if((!rda->user()->deleteCarts())&&(!rda->user()->adminConfig())) {
    XmlExit("User not authorized",404,"deleteaudio.cpp",LINE_NUMBER);
  }
  RDCut *cut=new RDCut(cartnum,cutnum);
  if(!cut->exists()) {
    delete cut;
    XmlExit("No such cut",404,"deleteaudio.cpp",LINE_NUMBER);
  }
  unlink(RDCut::pathName(cartnum,cutnum));
  unlink(RDCut::pathName(cartnum,cutnum)+".energy");
  QString sql=QString("delete from CUT_EVENTS where ")+
    "CUT_NAME=\""+RDCut::cutName(cartnum,cutnum)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  SendNotification(RDNotification::CartType,RDNotification::ModifyAction,
		   QVariant(cartnum));
  syslog(LOG_NOTICE,"unlink(%s): %s",(const char *)RDCut::pathName(cartnum,cutnum),strerror(errno));
  delete cut;
  XmlExit("OK",200,"deleteaudio.cpp",LINE_NUMBER);
}
