// add_recording.cpp
//
// Add a Rivendell RDCatch Event
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

#include <QKeyEvent>

#include "add_recording.h"
#include "edit_recording.h"
#include "edit_playout.h"
#include "edit_cartevent.h"
#include "edit_switchevent.h"
#include "edit_download.h"
#include "edit_upload.h"

extern RDStation *rdstation_conf;

AddRecording::AddRecording(QString *filter,QWidget *parent)
  : RDDialog(parent,Qt::CustomizeWindowHint|Qt::WindowTitleHint)
{
  add_id=-1;
  add_filter=filter;

  setWindowTitle("RDCatch");

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Title Label
  //
  QLabel *label=new QLabel(tr("Schedule a:"),this);
  label->setGeometry(0,0,sizeHint().width(),30);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  //  Recording Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(10,30,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Recording"));
  button->setDisabled(true);
  QString sql=QString("select `CHANNEL` from `DECKS` where ")+
    "(`CARD_NUMBER`>=0)&&(`CHANNEL`>0)&&(`CHANNEL`<=9)";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    button->setEnabled(true);
  }
  delete q;
  connect(button,SIGNAL(clicked()),this,SLOT(recordingData()));

  //
  //  Playout Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,80,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Playout"));
  button->setDisabled(true);
  sql=QString("select `CHANNEL` from `DECKS` where (`CARD_NUMBER`>=0)&&")+
    "(`PORT_NUMBER`>=0)&&(`CHANNEL`>128)&&(`CHANNEL`<=137)";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    button->setEnabled(true);
  }
  delete q;
  connect(button,SIGNAL(clicked()),this,SLOT(playoutData()));

  //
  //  Download Event Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,130,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Download"));
  connect(button,SIGNAL(clicked()),this,SLOT(downloadData()));

  //
  //  Upload Event Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,180,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Upload"));
  connect(button,SIGNAL(clicked()),this,SLOT(uploadData()));

  //
  //  Macro Event Cart Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,230,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Macro Cart"));
  connect(button,SIGNAL(clicked()),this,SLOT(macroData()));

  //
  //  Switch Event Cart Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,280,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Switch Event"));
  connect(button,SIGNAL(clicked()),this,SLOT(switchData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,350,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddRecording::~AddRecording()
{
}


QSize AddRecording::sizeHint() const
{
  return QSize(200,400);
} 


QSizePolicy AddRecording::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int AddRecording::exec(RDRecording::Type *type,int rec_id)
{
  add_id=rec_id;
  add_type=type;

  return QDialog::exec();
}


void AddRecording::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void AddRecording::recordingData()
{
  EditRecording *recording=new EditRecording(add_id,NULL,add_filter,this);
  if(!recording->exec()) {
    delete recording;
    done(false);
    return;
  }
  delete recording;
  *add_type=RDRecording::Recording;
  done(true);
}


void AddRecording::playoutData()
{
  EditPlayout *playout=new EditPlayout(add_id,NULL,add_filter,this);
  if(!playout->exec()) {
    delete playout;
    done(false);
    return;
  }
  delete playout;
  *add_type=RDRecording::Playout;
  done(true);
}


void AddRecording::downloadData()
{
  EditDownload *recording=
    new EditDownload(add_id,NULL,add_filter,this);
  if(!recording->exec()) {
    delete recording;
    done(false);
    return;
  }
  delete recording;
  *add_type=RDRecording::Download;
  done(true);
}


void AddRecording::uploadData()
{
  EditUpload *recording=new EditUpload(add_id,NULL,add_filter,this);
  if(!recording->exec()) {
    delete recording;
    done(false);
    return;
  }
  delete recording;
  *add_type=RDRecording::Upload;
  done(true);
}


void AddRecording::macroData()
{
  EditCartEvent *recording=new EditCartEvent(add_id,NULL,this);
  if(!recording->exec()) {
    delete recording;
    done(false);
    return;
  }
  delete recording;
  *add_type=RDRecording::MacroEvent;
  done(true);
}


void AddRecording::switchData()
{
  EditSwitchEvent *recording=new EditSwitchEvent(add_id,NULL,this);
  if(!recording->exec()) {
    delete recording;
    done(false);
    return;
  }
  delete recording;
  *add_type=RDRecording::SwitchEvent;
  done(true);
}


void AddRecording::cancelData()
{
  done(false);
}


void AddRecording::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Qt::Key_Escape:
	cancelData();
	break;

      default:
	QWidget::keyPressEvent(e);
	break;
  }
}
