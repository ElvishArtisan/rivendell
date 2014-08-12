// rdcueeditdialog.cpp
//
// A Dialog Box for using an RDCueEdit widget.
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdcueeditdialog.cpp,v 1.1.2.1 2013/07/05 22:44:17 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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


#include <qapplication.h>
#include <qpushbutton.h>

#include <rdcueeditdialog.h>

RDCueEditDialog::RDCueEditDialog(RDCae *cae,int play_card,int play_port,
				 const QString &caption,QWidget *parent)
  :QDialog(parent,"",true)
{
  QFont font;

  font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  setCaption(caption+" - "+tr("Set Cue Point"));

  //
  // Cue Editor
  //
  cue_edit=new RDCueEdit(cae,play_card,play_port,this);
  cue_edit->setGeometry(15,10,
			cue_edit->sizeHint().width(),
			cue_edit->sizeHint().height());

  //
  // OK Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-170,sizeHint().height()-60,80,50);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDCueEditDialog::~RDCueEditDialog()
{
  delete cue_edit;
}


QSize RDCueEditDialog::sizeHint() const
{
  return QSize(cue_edit->sizeHint().width(),
	       cue_edit->sizeHint().height()+10);
}


QSizePolicy RDCueEditDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDCueEditDialog::exec(RDLogLine *logline)
{
  edit_logline=logline;
  cue_edit->initialize(logline);
  return QDialog::exec();
}


void RDCueEditDialog::okData()
{
  if(cue_edit->playPosition(RDMarkerBar::Start)!=
     edit_logline->playPosition()) {
    edit_logline->
      setPlayPosition(cue_edit->playPosition(RDMarkerBar::Start));
    edit_logline->setPlayPositionChanged(true);
  }
  if(cue_edit->playPosition(RDMarkerBar::End)!=
     (unsigned)edit_logline->endPoint()) {
    edit_logline->setEndPoint(cue_edit->playPosition(RDMarkerBar::End),
			      RDLogLine::LogPointer);
    edit_logline->setPlayPositionChanged(true);
  }
  done(0);
}


void RDCueEditDialog::cancelData()
{
  done(-1);
}
