// rdcardselector.h
//
// Audio Channel Assignments Widget for Rivendell
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCARDSELECTOR_H
#define RDCARDSELECTOR_H

#include <QLabel>
#include <QSpinBox>

#include <rd.h>
#include <rdwidget.h>

class RDCardSelector : public RDWidget
{
  Q_OBJECT
 public:
  RDCardSelector(QWidget *parent=0);
  ~RDCardSelector();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool isDisabled() const;
  int id() const;
  void setId(int id);
  QString title() const;
  void setTitle(QString title);
  int card() const;
  void setCard(int card);
  int port() const;
  void setPort(int port);
  int maxCards() const;
  void setMaxCards(int num);
  int maxPorts(int card) const;
  void setMaxPorts(int card,int num);

 signals:
  void settingsChanged(int id,int card,int port);
  void cardChanged(int card);
  void portChanged(int port);

 private slots:
  void cardData(int);
  void portData(int);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *card_card_label;
  QSpinBox *card_card_box;
  QLabel *card_port_label;
  QSpinBox *card_port_box;
  QLabel *card_title;
  int yoffset;
  int card_max_ports[RD_MAX_CARDS];
  int card_id;
};


#endif  // RDCARDSELECTOR_H
