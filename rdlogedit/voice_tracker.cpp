// voice_tracker.cpp
//
// Rivendell Voice Tracker Dialog
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

#include <rdconf.h>
#include <rdsvc.h>
#include <rdimport_audio.h>
#include <rdrehash.h>

#include "voice_tracker.h"

VoiceTracker::VoiceTracker(QString *import_path,QWidget *parent)
  : RDDialog(parent)
{
  d_import_path=import_path;
  setWindowTitle("RDLogEdit - "+tr("Voice Tracker"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Tracker Widget
  //
  d_tracker_widget=new RDTrackerWidget(d_import_path,this);
  
  //
  //  Close Button
  //
  d_close_button=new QPushButton(this);
  d_close_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  d_close_button->setFont(buttonFont());
  d_close_button->setText(tr("Close"));
  connect(d_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


VoiceTracker::~VoiceTracker()
{
  delete d_close_button;
  delete d_tracker_widget;
}


QSize VoiceTracker::sizeHint() const
{
  return QSize(800,700);
} 


QSizePolicy VoiceTracker::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,
		     QSizePolicy::MinimumExpanding);
}


int VoiceTracker::exec(const QString &logname)
{
  if(!d_tracker_widget->load(logname)) {
    return false;
  }
  return QDialog::exec();
}


void VoiceTracker::closeData()
{
  d_tracker_widget->unload();
  /*
  stopData();
  CheckChanges();
  if(d_size_altered) {
    d_log_model->save(rda->config());
  }
  */
  done(0);
}


void VoiceTracker::closeEvent(QCloseEvent *e)
{
  if(d_close_button->isEnabled()) {
    closeData();
  }
  else {
    e->ignore();
  }
}


void VoiceTracker::resizeEvent(QResizeEvent *e)
{
  d_tracker_widget->setGeometry(10,10,size().width()-20,size().height()-20);
  d_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
