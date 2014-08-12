// rdmarker_button.h
//
// Marker Button for RDLibrary
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmarker_button.h,v 1.6 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDMARKER_BUTTON_H
#define RDMARKER_BUTTON_H

#include <rdpushbutton.h>
#include <qevent.h>

class RDMarkerButton : public RDPushButton
{
  public:
  RDMarkerButton(QWidget *parent,const char *name=0);
  RDMarkerButton(const QString &text,QWidget *parent,const char *name=0);
  RDMarkerButton(const QIconSet &icon,const QString &text,QWidget *parent,
	       const char *name=0);

  protected:
   void keyPressEvent(QKeyEvent *e);
};


#endif

