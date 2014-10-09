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

#include <qfontmetrics.h>

#include <globals.h>
#include <rd.h>
#include <disk_gauge.h>
#include <rdconfig.h>
#include <rdaudiostore.h>

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

  disk_label=new QLabel("Free:",this);
  disk_label->setGeometry(0,0,50,sizeHint().height());
  disk_label->setFont(label_font);
  disk_label->setAlignment(AlignRight|AlignVCenter);
  disk_label->setDisabled(true);

  disk_bar=new QProgressBar(this);
  disk_bar->setPercentageVisible(false);
  disk_bar->setGeometry(55,0,sizeHint().width()-55,sizeHint().height());
  disk_bar->setDisabled(true);

  disk_space_label=new QLabel(this);
  disk_space_label->setFont(label_font);
  disk_space_label->setAlignment(AlignCenter);
  disk_space_label->setDisabled(true);

  /*
  update();
  */

  disk_timer=new QTimer(this);
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
  if(lib_user==NULL) {
    return;
  }
  RDAudioStore::ErrorCode conv_err;
  RDAudioStore *conv=new RDAudioStore(rdstation_conf,lib_config,this);
  if((conv_err=conv->runStore(lib_user->name(),lib_user->password()))==
     RDAudioStore::ErrorOk) {
    uint64_t free_min=conv->freeBytes()/
      (60*rdlibrary_conf->defaultChannels()*lib_system->sampleRate());
    uint64_t total_min=conv->totalBytes()/
      (60*rdlibrary_conf->defaultChannels()*lib_system->sampleRate());
    disk_bar->setTotalSteps(total_min);
    disk_bar->setProgress(free_min);
    disk_space_label->setText(QString().sprintf("%luh %02lum",free_min/60,
						free_min-60*(free_min/60)));
    disk_label->setEnabled(true);
    disk_bar->setEnabled(true);
    disk_space_label->setEnabled(true);
  }
  delete conv;
  disk_timer->stop();
  disk_timer->start(DISK_GAUGE_UPDATE_INTERVAL,true);
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
