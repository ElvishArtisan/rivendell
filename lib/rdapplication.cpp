// rdapplication.cpp
//
// Base GUI Application Class
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdapplication.h"

RDApplication *rda=NULL;

RDApplication::RDApplication(const QString &module_name,const QString &cmdname,
			     const QString &usage,QObject *parent)
  : RDCoreApplication(module_name,cmdname,usage,parent)
{
  app_icon_engine=new RDIconEngine();
}


RDApplication::~RDApplication()
{
  delete app_icon_engine;
}


RDIconEngine *RDApplication::iconEngine() const
{
  return app_icon_engine;
}
