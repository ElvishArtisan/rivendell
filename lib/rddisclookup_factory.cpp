//   rddisclookup_factory.cpp
//
//   Factory for making RDDiscLookup instances.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include "rdcddblookup.h"
#include "rddisclookup_factory.h"
#include "rddummylookup.h"
#include "rdmblookup.h"

RDDiscLookup *RDDiscLookupFactory(RDLibraryConf::CdServerType type,
				  const QString &caption,
				  FILE *profile_msgs,QWidget *parent)
{
  RDDiscLookup *ret=NULL;

  switch(type) {
  case RDLibraryConf::CddbType:
    ret=new RDCddbLookup(caption,profile_msgs,parent);
    break;

  case RDLibraryConf::DummyType:
    ret=new RDDummyLookup(caption,profile_msgs,parent);
    break;

  case RDLibraryConf::MusicBrainzType:
    ret=new RDMbLookup(caption,profile_msgs,parent);
    break;

  case RDLibraryConf::LastType:
    break;
  }

  return ret;
}
