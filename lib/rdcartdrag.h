// rdcartdrag.h
//
// Stored value drag object for Rivendell carts.
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCARTDRAG_H
#define RDCARTDRAG_H

#include <QColor>
#include <QDragEnterEvent>
#include <QMap>
#include <QMimeData>
//#include <QMimeSource>
#include <QPixmap>

#include <rdcart.h>
#include <rdlog_line.h>

class RDCartDrag : public QMimeData
{
 public:
  RDCartDrag(unsigned cartnum,const QString &title,const QColor &color);
  QStringList formats() const;
  bool hasFormat(const QString &mimetype) const;
  static bool canDecode(const QMimeData *e);
  static bool decode(const QMimeData *e,unsigned *cartnum,QColor *color=NULL,
		     QString *title=NULL);
  static bool decode(const QMimeData *e,RDLogLine *ll,
		     RDLogLine::TransType next_trans=RDLogLine::Segue,
		     int log_mach=0,bool timescale=false,
		     RDLogLine::TransType trans=RDLogLine::NoTrans);

 protected:
  QVariant retrieveData(const QString &mimetype,QVariant::Type type) const;

 private:
  void SetData(unsigned cartnum,const QColor &color,const QString &title);
  QStringList d_formats;
  QMap<QString,QVariant> d_data;
};


#endif  // RDCARTDRAG_H
