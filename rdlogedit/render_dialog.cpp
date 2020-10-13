// render_dialog.cpp
//
// Render Log Dialog for Rivendell.
//
//   (C) Copyright 2017-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qfiledialog.h>

#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include <rdconf.h>
#include <rdexport_settings_dialog.h>
#include <rdrenderer.h>

#include "render_dialog.h"

RenderDialog::RenderDialog(RDStation *station,RDSystem *system,RDConfig *config,
			   QWidget *parent)
  : RDDialog(parent)
{
  render_station=station;
  render_system=system;
  render_config=config;
  render_save_path=RDHomeDir();
  render_first_line=0;
  render_last_line=0;

  setWindowTitle("RDLogEdit - "+tr("Render Log"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Dialogs
  //
  render_progress_dialog=
    new QProgressDialog(tr("Rendering Log..."),tr("Cancel"),0,0,this);
  render_progress_dialog->setWindowTitle(tr("Render Progress"));

  //
  // Settings
  //
  render_settings=new RDSettings();
  render_settings->setChannels(2);
  render_settings->setSampleRate(render_system->sampleRate());
  render_settings->setFormat(RDSettings::Pcm16);
  render_settings->setLayer(2);
  render_settings->setBitRate(256000);
  render_settings->setQuality(3);
  render_settings->setNormalizationLevel(0);

  //
  // Render To Type
  //
  render_to_box=new QComboBox(this);
  render_to_box->insertItem(tr("Cart/Cut"));
  render_to_box->insertItem(tr("File"));
  connect(render_to_box,SIGNAL(activated(int)),this,SLOT(toChangedData(int)));
  render_to_label=new QLabel(tr("Render To")+":",this);
  render_to_label->setFont(labelFont());
  render_to_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Filename
  //
  render_filename_edit=new QLineEdit(this);
  render_filename_edit->setReadOnly(true);
  connect(render_filename_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filenameChangedData(const QString &)));
  render_filename_label=new QLabel(tr("Cart/Cut")+":",this);
  render_filename_label->setFont(labelFont());
  render_filename_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  render_filename_button=new QPushButton(tr("Select"),this);
  connect(render_filename_button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Audio Settings
  //
  render_audiosettings_edit=new QLineEdit(this);
  render_audiosettings_edit->setReadOnly(true);
  render_audiosettings_edit->setText(render_settings->description());
  render_audiosettings_label=new QLabel(tr("Audio Parameters")+":",this);
  render_audiosettings_label->setFont(labelFont());
  render_audiosettings_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  render_audiosettings_button=new QPushButton(tr("Set"),this);
  connect(render_audiosettings_button,SIGNAL(clicked()),
	  this,SLOT(audiosettingsData()));

  //
  // Start Time
  //
  render_starttime_box=new QComboBox(this);
  render_starttime_box->insertItem(tr("[now]"));
  render_starttime_box->insertItem(tr("As Specified"));
  connect(render_starttime_box,SIGNAL(activated(int)),
	  this,SLOT(starttimeSourceData(int)));
  render_starttime_label=new QLabel(tr("Virtual Start Time")+":",this);
  render_starttime_label->setFont(labelFont());
  render_starttime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  render_starttime_edit=new QTimeEdit(this);
  render_starttime_edit->setDisplayFormat("hh:mm:ss");
  render_starttime_edit->setDisabled(true);

  //
  // Include Events
  //
  render_events_box=new QComboBox(this);
  render_events_box->insertItem(tr("All Events"));
  render_events_box->insertItem(tr("Only Selected Events"));
  render_events_label=new QLabel(tr("Include")+":",this);
  render_events_label->setFont(labelFont());
  render_events_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Ignore STOP
  //
  render_ignorestop_box=new QComboBox(this);
  render_ignorestop_box->insertItem(tr("Stop Rendering"));
  render_ignorestop_box->insertItem(tr("Treat as PLAY"));
  render_ignorestop_label=new QLabel(tr("At STOP transition")+":",this);
  render_ignorestop_label->setFont(labelFont());
  render_ignorestop_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Render Button
  //
  render_render_button=new QPushButton(tr("&Render"),this);
  render_render_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  render_render_button->setFont(buttonFont());
  render_render_button->setDefault(true);
  connect(render_render_button,SIGNAL(clicked()),this,SLOT(renderData()));

  //
  // Cancel Button
  //
  render_cancel_button=new QPushButton(tr("&Cancel"),this);
  render_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  render_cancel_button->setFont(buttonFont());
  render_cancel_button->setDefault(true);
  connect(render_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RenderDialog::~RenderDialog()
{
}


QSize RenderDialog::sizeHint() const
{
  return QSize(500,230);
}


int RenderDialog::exec(RDUser *user,RDLogEvent *log,
		       int first_line,int last_line)
{
  render_user=user;
  render_log=log;
  render_first_line=first_line;
  render_last_line=last_line;
  render_filename_edit->clear();
  render_render_button->setDisabled(true);

  return QDialog::exec();
}


void RenderDialog::toChangedData(int item)
{
  if(item) {
    render_filename_label->setText(tr("Filename")+":");
    render_filename_edit->setReadOnly(false);
  }
  else {
    render_filename_label->setText(tr("Cart/Cut")+":");
    render_filename_edit->setReadOnly(true);
  }
  render_filename_edit->clear();
  render_to_cartnum=0;
  render_to_cutnum=-1;
  render_render_button->setDisabled(true);
}


void RenderDialog::filenameChangedData(const QString &str)
{
  render_render_button->setDisabled(str.isEmpty());
}


void RenderDialog::selectData()
{
  if(render_to_box->currentItem()) {
    QString filename=
      QFileDialog::getSaveFileName(this,tr("Render Log"),
				   render_save_path,RD_AUDIO_FILE_FILTER);
    if(!filename.isEmpty()) {
      render_filename_edit->setText(filename);
      filenameChangedData(filename);
      render_save_path=RDGetPathPart(filename);
    }
  }
  else {
    QString cutname;
    RDCutDialog *d=
      new RDCutDialog(&cutname,"RDLogEdit",NULL,NULL,NULL,false,true,true,this);
    if(d->exec()==0) {
      render_to_cartnum=RDCut::cartNumber(cutname);
      render_to_cutnum=RDCut::cutNumber(cutname);
      render_filename_edit->setText(QString().sprintf("%06u:%03d",
				    render_to_cartnum,render_to_cutnum));
    }
    delete d;
  }
}


void RenderDialog::starttimeSourceData(int item)
{
  render_starttime_edit->setEnabled(item);
}


void RenderDialog::audiosettingsData()
{
  RDExportSettingsDialog *d=new RDExportSettingsDialog("RDLogEdit",this);
  if(d->exec(render_settings)) {
    render_audiosettings_edit->setText(render_settings->description());
  }
  delete d;
}


void RenderDialog::lineStartedData(int linno,int totallines)
{
  render_progress_dialog->setMaximum(totallines);
  render_progress_dialog->setValue(linno);
}


void RenderDialog::renderData()
{
  QString err_msg;
  bool result;

  int first_line=0;
  int last_line=render_log->size();
  if(render_events_box->currentItem()) {
    first_line=render_first_line;
    last_line=render_last_line;
  }
  QTime start_time=QTime::currentTime();
  if(render_starttime_box->currentItem()) {
    start_time=render_starttime_edit->time();
  }
  RDRenderer *r=new RDRenderer(this);
  connect(r,SIGNAL(lineStarted(int,int)),this,SLOT(lineStartedData(int,int)));
  connect(render_progress_dialog,SIGNAL(canceled()),r,SLOT(abort()));
  if(render_to_box->currentItem()) {
    result=
      r->renderToFile(render_filename_edit->text(),render_log,render_settings,
		      start_time,render_ignorestop_box->currentItem(),
		      &err_msg,first_line,last_line);
  }
  else {
    result=
      r->renderToCart(render_to_cartnum,render_to_cutnum,render_log,
		      render_settings,start_time,
		      render_ignorestop_box->currentItem(),
		      &err_msg,first_line,last_line);
  }
  if(!result) {
    QMessageBox::warning(this,"RDLogEdit - "+tr("Rendering Error"),
			 err_msg);
    delete r;
    return;
  }
  delete r;

  done(true);
}


void RenderDialog::cancelData()
{
  done(false);
}


void RenderDialog::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void RenderDialog::resizeEvent(QResizeEvent *e)
{
  render_to_label->setGeometry(10,10,65,20);
  render_to_box->setGeometry(80,10,100,20);

  render_filename_label->setGeometry(10,40,95,20);
  render_filename_edit->setGeometry(110,40,size().width()-190,20);
  render_filename_button->setGeometry(size().width()-70,38,60,25);

  render_audiosettings_label->setGeometry(10,70,135,20);
  render_audiosettings_edit->setGeometry(150,70,size().width()-230,20);
  render_audiosettings_button->setGeometry(size().width()-70,68,60,25);

  render_starttime_label->setGeometry(10,95,135,20);
  render_starttime_box->setGeometry(150,95,160,20);
  render_starttime_edit->setGeometry(315,95,80,20);

  render_events_label->setGeometry(10,117,135,20);
  render_events_box->setGeometry(150,117,160,20);

  render_ignorestop_label->setGeometry(10,139,135,20);
  render_ignorestop_box->setGeometry(150,139,160,20);

  render_render_button->
    setGeometry(size().width()-180,size().height()-60,80,50);
  render_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
