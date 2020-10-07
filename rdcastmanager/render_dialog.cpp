// render_dialog.cpp
//
// Render Log Dialog for Rivendell.
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

#include <qfiledialog.h>

#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include <rdconf.h>

#include "render_dialog.h"

RenderDialog::RenderDialog(QWidget *parent)
  : RDDialog(parent)
{
  d_start_line=NULL;
  d_end_line=NULL;
  
  setWindowTitle("RDCastManager - "+tr("Log Render Options"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Dialogs
  //
  d_log_dialog=new LogDialog(this);

  //
  // Start Time
  //
  d_start_time_box=new QComboBox(this);
  connect(d_start_time_box,SIGNAL(activated(int)),
	  this,SLOT(startTimeActivatedData(int)));
  d_start_time_box->insertItem(tr("[now]"));
  d_start_time_box->insertItem(tr("As Specified"));
  d_start_time_label=new QLabel(tr("Virtual Start Time")+":",this);
  d_start_time_label->setFont(labelFont());
  d_start_time_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_start_time_edit=new QTimeEdit(this);
  d_start_time_edit->setDisplayFormat("hh:mm:ss");
  d_start_time_edit->setDisabled(true);

  //
  // Ignore STOP
  //
  d_ignorestop_box=new QComboBox(this);
  d_ignorestop_box->insertItem(0,tr("Stop Rendering"));
  d_ignorestop_box->insertItem(1,tr("Treat as PLAY"));
  d_ignorestop_label=new QLabel(tr("At STOP transition")+":",this);
  d_ignorestop_label->setFont(labelFont());
  d_ignorestop_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Select Log Events
  //
  d_select_label_label=new QLabel(tr("Selected Log Events"),this);
  d_select_label_label->setFont(labelFont());
  d_select_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_select_label=new QLabel(this);
  d_select_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  d_select_label->setFrameStyle(QFrame::Plain);
  d_select_label->setFrameShape(QFrame::Box);
  d_select_label->setLineWidth(1);
  d_select_label->setMidLineWidth(1);
  d_select_button=new QPushButton(tr("Select"),this);
  d_select_button->setFont(subButtonFont());
  connect(d_select_button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // OK Button
  //
  d_ok_button=new QPushButton(tr("OK"),this);
  d_ok_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  d_ok_button->setFont(buttonFont());
  d_ok_button->setDefault(true);
  connect(d_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  d_cancel_button=new QPushButton(tr("&Cancel"),this);
  d_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  d_cancel_button->setFont(buttonFont());
  connect(d_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RenderDialog::~RenderDialog()
{
  delete d_log_dialog;
}


QSize RenderDialog::sizeHint() const
{
  return QSize(500,145);
}


int RenderDialog::exec(RDLogEvent *log,QTime *start_time,
		       bool *ignore_stops,int *start_line,int *end_line)
{
  d_log=log;
  d_start_time=start_time;
  d_start_time_edit->setDisabled(start_time->isNull());
  d_start_time_box->setCurrentIndex(0);
  d_ignore_stops=ignore_stops;
  if(*ignore_stops) {
    d_ignorestop_box->setCurrentIndex(1);
  }
  else {
    d_ignorestop_box->setCurrentIndex(0);
  }
  d_start_line=start_line;
  d_modified_start_line=*start_line;
  d_modified_end_line=*end_line;
  d_end_line=end_line;
  UpdateLogEventsSelection();

  return QDialog::exec();
}


void RenderDialog::startTimeActivatedData(int n)
{
  d_start_time_edit->setEnabled(n==1);
}


void RenderDialog::selectData()
{
  if(d_log_dialog->exec(d_log,&d_modified_start_line,&d_modified_end_line)) {
    UpdateLogEventsSelection();
  }
}


void RenderDialog::okData()
{
  if(d_start_time_box->currentIndex()==0) {
    *d_start_time=QTime();
  }
  else {
    *d_start_time=d_start_time_edit->time();
  }
  *d_ignore_stops=d_ignorestop_box->currentIndex();
  *d_start_line=d_modified_start_line;
  *d_end_line=d_modified_end_line;

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
  d_start_time_label->setGeometry(10,2,135,20);
  d_start_time_box->setGeometry(150,2,160,20);
  d_start_time_edit->setGeometry(315,2,80,20);

  d_ignorestop_label->setGeometry(10,24,135,20);
  d_ignorestop_box->setGeometry(150,24,160,20);

  d_select_label_label->setGeometry(10,46,135,20);
  d_select_label->setGeometry(150,46,160,20);
  d_select_button->setGeometry(315,43,70,24);

  d_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  d_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void RenderDialog::UpdateLogEventsSelection()
{
  if((d_modified_start_line==0)&&(d_modified_end_line==(d_log->size()-1))) {
    d_select_label->setText(tr("All lines"));
  }
  else {
    if(d_modified_start_line==d_modified_end_line) {
      d_select_label->setText(tr("Line")+
			      QString().sprintf(" %d",d_modified_start_line));
    }
    else {
      d_select_label->setText(tr("Lines")+
			      QString().sprintf(" %d ",d_modified_start_line)+
			      tr("through")+
			      QString().sprintf(" %d",d_modified_end_line));
    }
  }
}
