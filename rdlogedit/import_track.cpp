// import_track.cpp
//
// Insert Audio for a Voice Track
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: import_track.cpp,v 1.7 2010/07/29 19:32:37 cvs Exp $
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

#include <import_track.h>


extern RDStation *rdstation_conf;


ImportTrack::ImportTrack(QString *filter,QString *group,
			 QWidget *parent,const char *name)
  : QDialog(parent,name,true,Qt::WStyle_Customize|Qt::WStyle_DialogBorder)
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

  add_filter=filter;
  add_group=group;

  //
  // Title Label
  //
  QLabel *label=new QLabel(tr("Insert audio from a:"),this,"title_label");
  label->setGeometry(0,0,sizeHint().width(),30);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);

  //
  //  Cart Button
  //
  QPushButton *button=new QPushButton(this,"cart_button");
  button->setGeometry(10,30,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Cart"));
  button->setDisabled(true);
  QString sql=QString("select CHANNEL from DECKS \
                       where (CARD_NUMBER>=0)&&(CHANNEL>0)&&(CHANNEL<=9)");
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    button->setEnabled(true);
  }
  delete q;
  connect(button,SIGNAL(clicked()),this,SLOT(cartData()));

  //
  //  Import Button
  //
  button=new QPushButton(this,"import_button");
  button->setGeometry(10,80,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&File"));
  button->setDisabled(true);
  sql=QString("select CHANNEL from DECKS \
               where (CARD_NUMBER>=0)&&(CHANNEL>128)&&(CHANNEL<=137)");
  q=new RDSqlQuery(sql);
  if(q->first()) {
    button->setEnabled(true);
  }
  delete q;
  connect(button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(10,140,sizeHint().width()-20,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


ImportTrack::~ImportTrack()
{
}


QSize ImportTrack::sizeHint() const
{
  return QSize(200,200);
} 


QSizePolicy ImportTrack::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ImportTrack::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void ImportTrack::cartData()
{
/*
  EditRecording *recording=
    new EditRecording(add_id,NULL,add_filter,this,"recording");
  if(recording->exec()<0) {
    delete recording;
    done(-1);
    return;
  }
  delete recording;
  done((int)RDRecording::Recording);
*/
}


void ImportTrack::importData()
{
/*
  EditPlayout *playout=
    new EditPlayout(add_id,NULL,add_filter,this,"playout");
  if(playout->exec()<0) {
    delete playout;
    done(-1);
    return;
  }
  delete playout;
  done((int)RDRecording::Playout);
*/
}


void ImportTrack::cancelData()
{
  done(-1);
}


void ImportTrack::keyPressEvent(QKeyEvent *e)
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
