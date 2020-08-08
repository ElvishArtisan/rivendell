// rddatapacer.h
//
// Pace a stream of data messages.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDATAPACER_H
#define RDDATAPACER_H

#include <QByteArray>
#include <QObject>
#include <QQueue>
#include <QTimer>

#define RDDATAPACER_DEFAULT_PACE_INTERVAL 100

class RDDataPacer : public QObject
{
  Q_OBJECT;
 public:
  RDDataPacer(QObject *parent=0);
  ~RDDataPacer();
  int paceInterval() const;
  void setPaceInterval(int msecs);

 signals:
  void dataSent(const QByteArray &data);

 public slots:
  void send(const QByteArray &data);

 private slots:
  void timeoutData();

 private:
  QQueue<QByteArray> d_data_queue;
  QTimer *d_timer;
  int d_pace_interval;
};


#endif  // RDDATAPACER_H
