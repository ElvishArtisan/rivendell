//   rdmeterstrip.h
//
//   A strip of side-by-side vertical audio meters.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDMETERSTRIP_H
#define RDMETERSTRIP_H

#include <QLabel>
#include <QTimer>

#include <rdplaymeter.h>
#include <rdwidget.h>

class RDMeterStrip : public RDWidget
{
 Q_OBJECT
 public:
  RDMeterStrip(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void addInputMeter(int cardnum,int portnum,const QString &label);
  void addOutputMeter(int cardnum,int portnum,const QString &label);

 private slots:
  void pollData();
   
 protected:
  enum Type {Input=0,Output=1};
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *);

 private:
  void AddMeter(int cardnum,int portnum,const QString &label);
  QList<RDPlayMeter *> d_meters;
  QList<QLabel *> d_labels;
  QList<Type> d_types;
  QList<int> d_card_numbers;
  QList<int> d_port_numbers;
  QTimer *d_poll_timer;

  //  QLabel *d_temp_label;
};


#endif  // RDMETERSTRIP_H
