// eventlight.h
//
// Indicator light for cut events in rdcatch(1);
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EVENTLIGHT_H
#define EVENTLIGHT_H

#include <QLabel>
#include <QTimer>

class EventLight : public QLabel
{
  Q_OBJECT
 public:
  EventLight(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void trigger(int num);
  void setEnabled(bool state);
  void setDisabled(bool state);

 private slots:
  void reset();

 private:
  QTimer *d_timer;
};


#endif  // EVENTLIGHT_H
