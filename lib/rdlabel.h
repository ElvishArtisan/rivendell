//   rdlabel.h
//
//   An label widget with word wrap.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdlabel.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDLABEL_H
#define RDLABEL_H

#include <qwidget.h>
#include <qlabel.h>
#include <qrangecontrol.h>
#include <qcolor.h>
#include <qpalette.h>
#include <qsize.h>
#include <qpixmap.h>


class RDLabel : public QLabel
{
  Q_OBJECT

 public:
  RDLabel(QWidget *parent,const char *name,WFlags f=0);
  RDLabel(const QString &text,QWidget *parent,const char *name,WFlags f=0);
  RDLabel(QWidget *buddy,const QString &text,QWidget *parent,const char *name,
	  WFlags f=0);
  QString text() const;
  void setFont(const QFont &font);
  bool wordWrapEnabled() const;
  void setWordWrapEnabled(bool state);

 public slots:
  void setText(const QString &string);

 private:
 QString WrapText();
 QString label_text;
 QFont label_font;
 bool label_wrap;
};


#endif  // RDLABEL_H
