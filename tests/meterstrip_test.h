// meterstrip_test.h
//
// Test harness for RDMeterStrip
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef METERSTRIP_TEST_H
#define METERSTRIP_TEST_H

#include <QLabel>
#include <QStringList>

#include <rdmeterstrip.h>

#define METERSTRIP_TEST_USAGE "--input-meter=<cardnum>:<portnum>:<label> | --output-meter=<cardnum>:<portnum>:<label> [...]\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  enum Type {Input=0,Output=1};
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void caeConnectedData(bool state);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QList<Type> d_types;
  QList<int> d_card_numbers;
  QList<int> d_port_numbers;
  QStringList d_labels;
  RDMeterStrip *d_meter_stack;
};


#endif  // METERSTRIP_TEST_H
