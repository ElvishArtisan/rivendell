// disk_gauge.cpp
//
// Disk Gauge Widget for RDLibrary.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: disk_gauge.cpp,v 1.7.8.1 2014/01/08 02:08:38 cvs Exp $
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

#include <sys/vfs.h>

#include <qtimer.h>
#include <qfontmetrics.h>

#include <rd.h>
#include <disk_gauge.h>
#include <rdconfig.h>

DiskGauge::DiskGauge(int samp_rate,int chans,QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  disk_sample_rate=samp_rate;
  disk_channels=chans;

  //
  // Generate Fonts
  //
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  disk_label=new QLabel("Free:",this,"free_label");
  disk_label->setGeometry(0,0,50,sizeHint().height());
  disk_label->setFont(label_font);
  disk_label->setAlignment(AlignRight|AlignVCenter);

  disk_bar=new QProgressBar(this);
  disk_bar->setPercentageVisible(false);
  disk_bar->setGeometry(55,0,sizeHint().width()-55,sizeHint().height());

  disk_space_label=new QLabel(this);
  disk_space_label->setFont(label_font);
  disk_space_label->setAlignment(AlignCenter);

  struct statfs diskstat;
  statfs(RDConfiguration()->audioRoot().ascii(),&diskstat);
  disk_bar->setTotalSteps(GetMinutes(diskstat.f_blocks,diskstat.f_bsize));
  update();

  QTimer *timer=new QTimer(this,"update_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(update()));
  timer->start(DISK_GAUGE_UPDATE_INTERVAL);
}


QSize DiskGauge::sizeHint() const
{
  return QSize(160,40);
}


QSizePolicy DiskGauge::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void DiskGauge::update()
{
  struct statfs diskstat;
  statfs(RDConfiguration()->audioRoot().ascii() ,&diskstat);
  int mins=GetMinutes(diskstat.f_bavail,diskstat.f_bsize);
  disk_bar->setProgress(mins);
  disk_space_label->
    setText(QString().sprintf("%dh %02dm",mins/60,mins-60*(mins/60)));
}


void DiskGauge::resizeEvent(QResizeEvent *e)
{
  QFontMetrics *fm=new QFontMetrics(disk_label->font());
  disk_label->setGeometry(0,0,fm->width(disk_label->text()),size().height()/2);
  disk_bar->setGeometry(fm->width(disk_label->text())+5,0,
			size().width()-fm->width(disk_label->text())-10,size().height()/2);
  disk_space_label->
    setGeometry(0,size().height()/2,size().width(),size().height()/2);
  delete fm;
}


int DiskGauge::GetMinutes(long blocks,long block_size)
{
  return (int)(((double)blocks*(double)block_size)/
	       (disk_sample_rate*disk_channels*120.0));
}
