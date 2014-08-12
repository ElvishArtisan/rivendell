// rdget_ath.cpp
//
// Get an Aggreggate Tuning Hours (ATH) Figure.
//
//   (C) Copyright 2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdget_ath.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qvalidator.h>

#include <rdget_ath.h>


RDGetAth::RDGetAth(double *ath,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  ath_ath=ath;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Enter ATH"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Validator
  //
  QDoubleValidator *validator=new QDoubleValidator(this,"validator");
  validator->setBottom(0.0);

  //
  // Label
  //
  QLabel *label=new QLabel(tr("Enter the agreggate tuning hours (ATH)\nfigure for the report period.\n(Supplied by your streaming provider)."),this);
  label->setGeometry(10,5,sizeHint().width()-20,65);

  //
  // ATH
  //
  ath_ath_edit=new QLineEdit(this,"ath_ath_edit");
  ath_ath_edit->setGeometry(55,70,50,19);
  ath_ath_edit->setValidator(validator);
  QLabel *ath_ath_label=new QLabel(ath_ath_edit,tr("ATH:"),this,
				       "ath_ath_label");
  ath_ath_label->setGeometry(10,70,40,19);
  ath_ath_label->setFont(font);
  ath_ath_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDGetAth::~RDGetAth()
{
}


QSize RDGetAth::sizeHint() const
{
  return QSize(250,160);
} 


QSizePolicy RDGetAth::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDGetAth::okData()
{
  if(ath_ath_edit->text().isEmpty()) {
    QMessageBox::warning(NULL,tr("Invalid ATH"),
			 tr("You must provide a valid ATH figure!"));
    return;
  }
  *ath_ath=ath_ath_edit->text().toDouble();
  done(0);
}


void RDGetAth::cancelData()
{
  done(-1);
}
