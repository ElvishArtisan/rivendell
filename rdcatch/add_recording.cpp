// add_recording.cpp
//
// Add a Rivendell RDCatch Event
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_recording.cpp,v 1.26.8.1 2014/01/07 23:40:36 cvs Exp $
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

#include <vector>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>

#include <rddb.h>
#include <rd.h>
#include <rdstation.h>
#include <rdcut_dialog.h>
#include <rdcut_path.h>

#include <add_recording.h>
#include <edit_recording.h>
#include <edit_playout.h>
#include <edit_cartevent.h>
#include <edit_switchevent.h>
#include <edit_download.h>
#include <edit_upload.h>


extern RDStation *rdstation_conf;


AddRecording::AddRecording(int id,QString *filter,QWidget *parent)
  : QDialog(parent,"",true,Qt::WStyle_Customize|Qt::WStyle_DialogBorder)
{
  setCaption("");

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont day_font=QFont("Helvetica",12,QFont::Normal);
  day_font.setPixelSize(12);

  add_id=id;
  add_filter=filter;

  //
  // Title Label
  //
  QLabel *label=new QLabel(tr("Schedule a:"),this,"title_label");
  label->setGeometry(0,0,sizeHint().width(),30);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);

  //
  //  Recording Button
  //
  QPushButton *button=new QPushButton(this,"recording_button");
  button->setGeometry(10,30,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Recording"));
  button->setDisabled(true);
  QString sql=QString("select CHANNEL from DECKS \
                       where (CARD_NUMBER>=0)&&(CHANNEL>0)&&(CHANNEL<=9)");
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    button->setEnabled(true);
  }
  delete q;
  connect(button,SIGNAL(clicked()),this,SLOT(recordingData()));

  //
  //  Playout Button
  //
  button=new QPushButton(this,"playout_button");
  button->setGeometry(10,80,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Playout"));
  button->setDisabled(true);
  sql=QString("select CHANNEL from DECKS where (CARD_NUMBER>=0)&&")+
    "(PORT_NUMBER>=0)&&(CHANNEL>128)&&(CHANNEL<=137)";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    button->setEnabled(true);
  }
  delete q;
  connect(button,SIGNAL(clicked()),this,SLOT(playoutData()));

  //
  //  Download Event Button
  //
  button=new QPushButton(this,"download_button");
  button->setGeometry(10,130,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Download"));
  connect(button,SIGNAL(clicked()),this,SLOT(downloadData()));

  //
  //  Upload Event Button
  //
  button=new QPushButton(this,"upload_button");
  button->setGeometry(10,180,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Upload"));
  connect(button,SIGNAL(clicked()),this,SLOT(uploadData()));

  //
  //  Macro Event Cart Button
  //
  button=new QPushButton(this,"macro_button");
  button->setGeometry(10,230,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Macro Cart"));
  connect(button,SIGNAL(clicked()),this,SLOT(macroData()));

  //
  //  Switch Event Cart Button
  //
  button=new QPushButton(this,"switch_button");
  button->setGeometry(10,280,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Switch Event"));
  connect(button,SIGNAL(clicked()),this,SLOT(switchData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(10,350,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
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


void AddRecording::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void AddRecording::recordingData()
{
  EditRecording *recording=new EditRecording(add_id,NULL,add_filter,this);
  if(recording->exec()<0) {
    delete recording;
    done(-1);
    return;
  }
  delete recording;
  done((int)RDRecording::Recording);
}


void AddRecording::playoutData()
{
  EditPlayout *playout=new EditPlayout(add_id,NULL,add_filter,this);
  if(playout->exec()<0) {
    delete playout;
    done(-1);
    return;
  }
  delete playout;
  done((int)RDRecording::Playout);
}


void AddRecording::downloadData()
{
  EditDownload *recording=
    new EditDownload(add_id,NULL,add_filter,this);
  if(recording->exec()<0) {
    delete recording;
    done(-1);
    return;
  }
  delete recording;
  done((int)RDRecording::Download);
}


void AddRecording::uploadData()
{
  EditUpload *recording=new EditUpload(add_id,NULL,add_filter,this);
  if(recording->exec()<0) {
    delete recording;
    done(-1);
    return;
  }
  delete recording;
  done((int)RDRecording::Upload);
}


void AddRecording::macroData()
{
  EditCartEvent *recording=new EditCartEvent(add_id,NULL,this);
  if(recording->exec()<0) {
    delete recording;
    done(-1);
    return;
  }
  delete recording;
  done((int)RDRecording::MacroEvent);
}


void AddRecording::switchData()
{
  EditSwitchEvent *recording=new EditSwitchEvent(add_id,NULL,this);
  if(recording->exec()<0) {
    delete recording;
    done(-1);
    return;
  }
  delete recording;
  done((int)RDRecording::SwitchEvent);
}


void AddRecording::cancelData()
{
  done(-1);
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
