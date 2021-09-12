// rdget_ath.cpp
//
// Get an Aggreggate Tuning Hours (ATH) Figure.
//
//   (C) Copyright 2006-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QValidator>

#include "rdget_ath.h"

RDGetAth::RDGetAth(double *ath,QWidget *parent)
  : RDDialog(parent)
{
  ath_ath=ath;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle(tr("Enter ATH"));

  //
  // Validator
  //
  QDoubleValidator *validator=new QDoubleValidator(this);
  validator->setBottom(0.0);

  //
  // Label
  //
  QLabel *label=new QLabel(tr("Enter the agreggate tuning hours (ATH)\nfigure for the report period.\n(Supplied by your streaming provider)."),this);
  label->setGeometry(10,5,sizeHint().width()-20,65);

  //
  // ATH
  //
  ath_ath_edit=new QLineEdit(this);
  ath_ath_edit->setGeometry(55,70,50,19);
  ath_ath_edit->setValidator(validator);
  QLabel *ath_ath_label=new QLabel(tr("ATH:"),this);
  ath_ath_label->setGeometry(10,70,40,19);
  ath_ath_label->setFont(labelFont());
  ath_ath_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
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
