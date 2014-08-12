// disk_gauge.h
//
// Disk Gauge Widget for RDLibrary.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: disk_gauge.h,v 1.6.8.1 2014/01/08 02:08:38 cvs Exp $
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

#ifndef DISK_GAUGE_H
#define DISK_GAUGE_H

#include <qwidget.h>
#include <qlabel.h>
#include <qprogressbar.h>

#define DISK_GAUGE_UPDATE_INTERVAL 60000

class DiskGauge : public QWidget
{
  Q_OBJECT
 public:
  DiskGauge(int samp_rate,int chans,QWidget *parent,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void update();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  int GetMinutes(long blocks,long block_size);
  QLabel *disk_label;
  QProgressBar *disk_bar;
  QLabel *disk_space_label;
  double disk_sample_rate;
  double disk_channels;
};


#endif  // DISK_GAUGE
