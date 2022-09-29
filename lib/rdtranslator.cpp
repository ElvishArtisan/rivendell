//   rdtranslate.cpp
//
//   Load translation objects for Rivendell 
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include "rdapplication.h"
#include "rdtranslator.h"

RDTranslator *rdt=NULL;

RDTranslator::RDTranslator(const QString &cmdname,QObject *parent)
  : QObject(parent)
{
  d_command_name=cmdname;

  QString loc=RDApplication::locale().left(2)+".qm";
  if(loc.left(2)!="en") {  // There are no English translations
    LoadTranslation("qt_"+loc,"/usr/share/qt5/translations");
    LoadTranslation("librd_"+loc,"/usr/share/rivendell");
    LoadTranslation("rdhpi_"+loc,"/usr/share/rivendell");
    LoadTranslation(d_command_name+"_"+loc,"/usr/share/rivendell");
  }
}


bool RDTranslator::LoadTranslation(const QString &filename,
				   const QString &dirname)
{
  QTranslator *qt=new QTranslator(0);
  if(!qt->load(filename,dirname)) {
    fprintf(stderr,"%s: failed to load translation file \"%s/%s\"\n",
	    d_command_name.toUtf8().constData(),
	    dirname.toUtf8().constData(),
	    filename.toUtf8().constData());
    delete qt;
    return false;
  }
  return qApp->installTranslator(qt);
}
