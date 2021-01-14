// rd3cartdrag.h
//
// Stored value drag object for Rivendell carts.
//
//   (C) Copyright 2013-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RD3CARTDRAG_H
#define RD3CARTDRAG_H

#include <qcolor.h>
#include <qpixmap.h>

#include <q3dragobject.h>

#include <rdcart.h>
#include <rdlog_line.h>

class RD3CartDrag : public Q3StoredDrag
{
 public:
  RD3CartDrag(unsigned cartnum,const QPixmap *icon,const QColor &color,
	      QWidget *src=0);
  RD3CartDrag(unsigned cartnum,const QString &title,const QColor &color,
	      QWidget *src=0);
  static bool canDecode(QMimeSource *e);
  static bool decode(QMimeSource *e,unsigned *cartnum,QColor *color=NULL,
		     QString *title=NULL);
  static bool decode(QMimeSource *e,RDLogLine *ll,
		     RDLogLine::TransType next_trans=RDLogLine::Segue,
		     int log_mach=0,bool timescale=false,
		     RDLogLine::TransType trans=RDLogLine::NoTrans);

 private:
  void SetData(unsigned cartnum,const QColor &color,const QString &title);
};


#endif  // RD3CARTDRAG_H
