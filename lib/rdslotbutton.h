// rdslotbutton.h
//
//   Start/stop button for rdcartslots(1).
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSLOTBUTTON_H
#define RDSLOTBUTTON_H

#include <QPushButton>

#include <rdfontengine.h>
#include <rdplay_deck.h>

class RDSlotButton : public QPushButton, public RDFontEngine
{
  Q_OBJECT
 public:
  RDSlotButton(int slotnum,QWidget *parent,RDConfig *c=NULL);
  int slotNumber() const;
  QString portLabel() const;

 public slots:
  void setState(int id,RDPlayDeck::State state);
  void setPortLabel(const QString &str);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void WriteKeycap();
  int d_slot_number;
  RDPlayDeck::State d_dstate;
  QString d_port_label;
  QPalette d_ready_color;
  QPalette d_playing_color;
};


#endif  // RDSLOTBUTTON_H
