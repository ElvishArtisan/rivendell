// rehash.cpp
//
// Rivendell web service portal -- Rehash service
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdlog_line.h>
#include <rdweb.h>
#include <rduser.h>
#include <rdgroup.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdcart_search_text.h>
#include <rdhash.h>

#include <rdxport.h>

void Xport::Rehash()
{
  int cart_number;
  int cut_number;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400,"rdhash.cpp",LINE_NUMBER);
  }
  if(!xport_post->getValue("CUT_NUMBER",&cut_number)) {
    XmlExit("Missing CUT_NUMBER",400,"rdhash.cpp",LINE_NUMBER);
  }

  //
  // Process Request
  //
  RDCut *cut=new RDCut(cart_number,cut_number);
  if(!cut->exists()) {
    delete cut;
    XmlExit("No such cut",404,"rdhash.cpp",LINE_NUMBER);
  }
  cut->setSha1Hash(RDSha1Hash(RDCut::pathName(cart_number,cut_number)));
  delete cut;
  XmlExit("OK",200,"rdhash.cpp",LINE_NUMBER);
}
