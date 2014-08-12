// vbox.h
//
// A QVBox widget with dynamic horizontal resizing.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: vbox.h,v 1.5 2010/07/29 19:32:36 cvs Exp $
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


#ifndef VBOX_H
#define VBOX_H

#include <vector>

#include <qwidget.h>


class VBox : public QWidget
{
  Q_OBJECT
 public:
  VBox(QWidget *parent=0,const char *name=0,WFlags f=0);
  void addWidget(QWidget *widget);
  void setSpacing(int space);

 public slots:
  void setGeometry(int x,int y,int w,int h);
  void setGeometry(const QRect &r);

 private:
  int box_spacing;
  std::vector<QWidget *> box_children;
};


#endif  // VBOX_H
