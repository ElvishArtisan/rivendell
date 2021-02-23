// disk_gauge.cpp
//
// Disk Gauge Widget for RDLibrary.
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

#include <rd.h>
#include <rdapplication.h>

#include <rdaudiostore.h>

#include "disk_gauge.h"
#include "globals.h"

DiskGauge::DiskGauge(int samp_rate,int chans,QWidget *parent)
  : RDWidget(parent)
{
  disk_sample_rate=samp_rate;
  disk_channels=chans;

  disk_label=new QLabel(tr("Free")+":",this);
  disk_label->setGeometry(0,0,50,sizeHint().height());
  disk_label->setFont(labelFont());
  disk_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  disk_label->setDisabled(true);

  disk_bar=new QProgressBar(this);
  disk_bar->setGeometry(55,0,sizeHint().width()-55,sizeHint().height());
  disk_bar->setDisabled(true);

  disk_space_label=new QLabel(this);
  disk_space_label->setFont(labelFont());
  disk_space_label->setAlignment(Qt::AlignCenter);
  disk_space_label->setDisabled(true);

  /*
  update();
  */

  disk_timer=new QTimer(this);
  disk_timer->setSingleShot(true);
  connect(disk_timer,SIGNAL(timeout()),this,SLOT(update()));
  disk_timer->start(100);
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
  if(rda->user()->name().isEmpty()) {
    return;
  }
  RDAudioStore::ErrorCode conv_err;
  RDAudioStore *conv=new RDAudioStore(rda->station(),rda->config(),this);
  if((conv_err=conv->runStore(rda->user()->name(),rda->user()->password()))==
     RDAudioStore::ErrorOk) {
    uint64_t free_min=GetMinutes(conv->freeBytes());
    uint64_t total_min=GetMinutes(conv->totalBytes());
    disk_bar->setMaximum(total_min);
    disk_bar->setValue(free_min);
    disk_space_label->setText(QString().sprintf("%luh %02lum",free_min/60,
						free_min-60*(free_min/60)));
    disk_label->setEnabled(true);
    disk_bar->setEnabled(true);
    disk_space_label->setEnabled(true);
  }
  delete conv;
  disk_timer->stop();
  disk_timer->start(DISK_GAUGE_UPDATE_INTERVAL);
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


unsigned DiskGauge::GetMinutes(uint64_t bytes)
{
  unsigned ret=0;

  switch(rda->libraryConf()->defaultFormat()) {
  case 1:   // MPEG Layer 2
    ret=bytes*2/(rda->libraryConf()->defaultChannels()*
		 rda->libraryConf()->defaultBitrate()*15);
    break;

  default:  // PCM16
    ret=bytes/(rda->libraryConf()->defaultChannels()*2*rda->system()->sampleRate()*60);
    break;
  }
  return ret;
}

