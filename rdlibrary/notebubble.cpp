// notebubble.cpp
//
// Bubble for displaying a cart note
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rddb.h>

#include "notebubble.h"

NoteBubble::NoteBubble(QWidget *parent)
  : QLabel(parent)
{
  note_cart_number=0;

  setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  setWordWrap(true);
  setMargin(5);
  setStyleSheet(QString("background-color:")+RD_STATUS_BACKGROUND_COLOR);

  note_show_timer=new QTimer(this);
  note_show_timer->setSingleShot(true);
  connect(note_show_timer,SIGNAL(timeout()),this,SLOT(show()));
}


unsigned NoteBubble::cartNumber() const
{
  return note_cart_number;
}


bool NoteBubble::setCartNumber(unsigned cartnum)
{
  bool ret=false;

  note_show_timer->stop();
  hide();
  QString sql=QString("select `NOTES` from `CART` where ")+
    QString().sprintf("`NUMBER`=%u",cartnum);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()&&(!q->value(0).toString().trimmed().isEmpty())) {
    setText(q->value(0).toString());
    note_show_timer->start(1000);
    ret=true;
  }
  delete q;
  note_cart_number=cartnum;

  return ret;
}
