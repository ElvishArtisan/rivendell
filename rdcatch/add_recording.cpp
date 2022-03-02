// add_recording.cpp
//
// Add a Rivendell RDCatch Event
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdescape_string.h>

#include "add_recording.h"
#include "edit_recording.h"
#include "edit_playout.h"
#include "edit_cartevent.h"
#include "edit_switchevent.h"
#include "edit_download.h"
#include "edit_upload.h"
#include "globals.h"

extern RDStation *rdstation_conf;

AddRecording::AddRecording(QString *filter,QWidget *parent)
  : RDDialog(parent)
{
  add_record_id=NULL;
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
  add_title_label=new QLabel(tr("Schedule a:"),this);
  add_title_label->setFont(labelFont());
  add_title_label->setAlignment(Qt::AlignCenter);

  //
  //  Recording Button
  //
  add_recording_button=new QPushButton(this);
  add_recording_button->setFont(buttonFont());
  add_recording_button->setText(tr("Recording"));
  add_recording_button->setDisabled(true);
  QString sql=QString("select `CHANNEL` from `DECKS` where ")+
    "(`CARD_NUMBER`>=0)&&(`CHANNEL`>0)&&(`CHANNEL`<=9)";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    add_recording_button->setEnabled(true);
  }
  delete q;
  connect(add_recording_button,SIGNAL(clicked()),this,SLOT(recordingData()));

  //
  //  Playout Button
  //
  add_playout_button=new QPushButton(this);
  add_playout_button->setFont(buttonFont());
  add_playout_button->setText(tr("Playout"));
  add_playout_button->setDisabled(true);
  sql=QString("select `CHANNEL` from `DECKS` where (`CARD_NUMBER`>=0)&&")+
    "(`PORT_NUMBER`>=0)&&(`CHANNEL`>128)&&(`CHANNEL`<=137)";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    add_playout_button->setEnabled(true);
  }
  delete q;
  connect(add_playout_button,SIGNAL(clicked()),this,SLOT(playoutData()));

  //
  //  Download Event Button
  //
  add_download_button=new QPushButton(this);
  add_download_button->setFont(buttonFont());
  add_download_button->setText(tr("Download"));
  connect(add_download_button,SIGNAL(clicked()),this,SLOT(downloadData()));

  //
  //  Upload Event Button
  //
  add_upload_button=new QPushButton(this);
  add_upload_button->setFont(buttonFont());
  add_upload_button->setText(tr("Upload"));
  connect(add_upload_button,SIGNAL(clicked()),this,SLOT(uploadData()));

  //
  //  Macro Event Cart Button
  //
  add_macrocart_button=new QPushButton(this);
  add_macrocart_button->setFont(buttonFont());
  add_macrocart_button->setText(tr("Macro Cart"));
  connect(add_macrocart_button,SIGNAL(clicked()),this,SLOT(macroData()));

  //
  //  Switch Event Cart Button
  //
  add_switchevent_button=new QPushButton(this);
  add_switchevent_button->setFont(buttonFont());
  add_switchevent_button->setText(tr("Switch Event"));
  connect(add_switchevent_button,SIGNAL(clicked()),this,SLOT(switchData()));

  //
  //  Cancel Button
  //
  add_cancel_button=new QPushButton(this);
  add_cancel_button->setFont(buttonFont());
  add_cancel_button->setText(tr("Cancel"));
  add_cancel_button->setDefault(true);
  connect(add_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
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


int AddRecording::exec(unsigned *rec_id,RDRecording::Type *type)
{
  add_record_id=rec_id;
  add_type=type;

  return QDialog::exec();
}


void AddRecording::recordingData()
{
  *add_record_id=AddRecord(0);

  if(!catch_editrecording_dialog->exec(*add_record_id,NULL)) {
    DeleteRecord();
    done(false);
    return;
  }
  *add_type=RDRecording::Recording;
  done(true);
}


void AddRecording::playoutData()
{
  *add_record_id=AddRecord(128);

  if(!catch_editplayout_dialog->exec(*add_record_id,NULL)) {
    DeleteRecord();
    done(false);
    return;
  }
  *add_type=RDRecording::Playout;
  done(true);
}


void AddRecording::downloadData()
{
  *add_record_id=AddRecord(0);

  if(!catch_editdownload_dialog->exec(*add_record_id,NULL)) {
    DeleteRecord();
    done(false);
    return;
  }
  *add_type=RDRecording::Download;
  done(true);
}


void AddRecording::uploadData()
{
  *add_record_id=AddRecord(0);

  if(!catch_editupload_dialog->exec(*add_record_id,NULL)) {
    DeleteRecord();
    done(false);
    return;
  }
  *add_type=RDRecording::Upload;
  done(true);
}


void AddRecording::macroData()
{
  *add_record_id=AddRecord(0);

  if(!catch_editcartevent_dialog->exec(*add_record_id,NULL)) {
    DeleteRecord();
    done(false);
    return;
  }
  *add_type=RDRecording::MacroEvent;
  done(true);
}


void AddRecording::switchData()
{
  *add_record_id=AddRecord(0);

  if(!catch_editswitchevent_dialog->exec(*add_record_id,NULL)) {
    DeleteRecord();
    done(false);
    return;
  }
  *add_type=RDRecording::SwitchEvent;
  done(true);
}


void AddRecording::cancelData()
{
  done(false);
}


void AddRecording::resizeEvent(QResizeEvent *e)
{
  add_title_label->setGeometry(0,0,sizeHint().width(),30);

  add_recording_button->setGeometry(10,30,sizeHint().width()-20,50);
  add_playout_button->setGeometry(10,80,sizeHint().width()-20,50);
  add_download_button->setGeometry(10,130,sizeHint().width()-20,50);
  add_upload_button->setGeometry(10,180,sizeHint().width()-20,50);
  add_macrocart_button->setGeometry(10,230,sizeHint().width()-20,50);
  add_switchevent_button->setGeometry(10,280,sizeHint().width()-20,50);

  add_cancel_button->setGeometry(10,340,sizeHint().width()-20,50);
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


void AddRecording::closeEvent(QCloseEvent *e)
{
  cancelData();
}


unsigned AddRecording::AddRecord(unsigned chan) const
{
  QString sql;

  sql=QString("insert into `RECORDINGS` set ")+
    "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"',"+
    QString::asprintf("`CHANNEL`=%u,",chan)+
    "`CUT_NAME`=''";
  return RDSqlQuery::run(sql).toUInt();
}


void AddRecording::DeleteRecord()
{
  QString sql=QString("delete from `RECORDINGS` where ")+
    QString::asprintf("`ID`=%u",*add_record_id);
  RDSqlQuery::apply(sql);
  *add_record_id=0;
  add_record_id=NULL;
}
