// rdgpioselector.h
//
// GPIO Pin Selector Widget for Rivendell
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgpioselector.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDGPIOSELECTOR_H
#define RDGPIOSELECTOR_H

#include <qwidget.h>
#include <qspinbox.h>
#include <qlabel.h>


class RDGpioSelector : public QWidget
{
  Q_OBJECT
  public:
   RDGpioSelector(QWidget *parent=0,const char *name=0);
   ~RDGpioSelector();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;
   int pin() const;
   void setPin(int pinno);

  signals:
   void pinChanged(int card);

  private slots:
   void pinData(int);

  private:
  QSpinBox *gpio_pin_box;
};


#endif

