// rdmarker_button.cpp
//
// Marker Button for RDLibrary
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmarker_button.cpp,v 1.6 2010/07/29 19:32:33 cvs Exp $
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

#include <rdmarker_button.h>

RDMarkerButton::RDMarkerButton(QWidget *parent,const char *name)
  : RDPushButton(parent,name)
{
}


RDMarkerButton::RDMarkerButton(const QString &text,QWidget *parent,
			   const char *name)
  : RDPushButton(text,parent,name)
{
}


RDMarkerButton::RDMarkerButton(const QIconSet &icon,const QString &text,
			   QWidget *parent,const char *name)
  : RDPushButton(icon,text,parent,name)
{
}


void RDMarkerButton::keyPressEvent(QKeyEvent *e)
{
  e->ignore();
}

