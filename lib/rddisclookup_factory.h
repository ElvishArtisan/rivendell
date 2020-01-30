//   rddisclookup_factory.h
//
//   Factory for making RDDiscLook instances.
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

#ifndef RDDISCLOOKUP_FACTORY_H
#define RDDISCLOOKUP_FACTORY_H

#include <rddisclookup.h>
#include <rdlibrary_conf.h>

RDDiscLookup *RDDiscLookupFactory(RDLibraryConf::CdServerType type,
				  const QString &caption,
				  FILE *profile_msgs,QWidget *parent=0);


#endif  // RDDISCLOOKUP_FACTORY_H
