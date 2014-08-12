// gpi_label.h
//
// A Qt-based application for testing general purpose input (GPI) devices.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_label.h,v 1.4 2010/07/29 19:32:40 cvs Exp $
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


#ifndef GPI_LABEL_H
#define GPI_LABEL_H

#include <qwidget.h>
#include <qlabel.h>


class GpiLabel : public QWidget
{
  Q_OBJECT
 public:
  GpiLabel(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  int line() const;
  void setLine(int line);
  void setState(bool state);
  void setCart(unsigned off_cartnum,unsigned on_cartnum);
  void setMask(bool state);

 public slots:
  void setGeometry(int x,int y,int w,int h);

 private:
  QLabel *gpi_line_label;
  QLabel *gpi_oncart_label;
  QLabel *gpi_offcart_label;
  int gpi_line;
};


#endif  // GPI_LABEL_H
