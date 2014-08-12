// rdhpicardselector.h
//
// Audio Card Selector Widget
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdhpicardselector.h,v 1.3 2010/07/29 19:32:36 cvs Exp $
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

#ifndef RDHPICARDSELECTOR_H
#define RDHPICARDSELECTOR_H

#include <qwidget.h>
#include <qspinbox.h>
#include <qlabel.h>


class RDHPICardSelector : public QWidget
{
  Q_OBJECT
  public:
   RDHPICardSelector(QWidget *parent=0,const char *name=0);
   ~RDHPICardSelector();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;
   QString title() const;
   void setTitle(QString title);
   int card() const;
   void setCard(int card);
   int port() const;
   void setPort(int port);

  signals:
   void cardChanged(int card);
   void portChanged(int port);

  private slots:
   void cardData(int);
   void portData(int);

  private:
   QLabel *card_card_label;
   QSpinBox *card_card_box;
   QLabel *card_port_label;
   QSpinBox *card_port_box;
   QLabel *card_title;
   int yoffset;
};


#endif  // RDHPICARDSELECTOR_H
