// deleteaudio.cpp
//
// Rivendell web service portal -- DeleteAudio service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: deleteaudio.cpp,v 1.6.2.1 2012/07/17 19:29:43 cvs Exp $
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

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::DeleteAudio()
{
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
  // Process Request
  //
  if((!xport_user->deleteCarts())&&(!xport_user->adminConfig())) {
    XmlExit("User not authorized",401);
  }
  RDCut *cut=new RDCut(cartnum,cutnum);
  if(!cut->exists()) {
    delete cut;
    XmlExit("No such cut",404);
  }
  unlink(RDCut::pathName(cartnum,cutnum));
  unlink(RDCut::pathName(cartnum,cutnum)+".energy");
  syslog(LOG_NOTICE,"unlink(%s): %s",(const char *)RDCut::pathName(cartnum,cutnum),strerror(errno));
  delete cut;
  XmlExit("OK",200);
}
