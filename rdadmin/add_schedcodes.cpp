// add_schedcodes.cpp
//
// Add scheduler codes dialog
//
//   Stefan Gabriel <stg@st-gabriel.de>
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

#include "rddb.h"
#include "rdescape_string.h"
#include "rdschedcode.h"
#include "rdtextvalidator.h"

#include "add_schedcodes.h"
#include "edit_schedcodes.h"

AddSchedCode::AddSchedCode(QString *schedcode,QWidget *parent)
  : QDialog(parent)
{
  schedcode_schedcode=schedcode;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle(tr("Add Scheduler Code"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont user_font=QFont("Helvetica",12,QFont::Normal);
  user_font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");
  validator->addBannedChar(' ');

  //
  // Code Name
  //
  schedcode_name_edit=new QLineEdit(this);
  schedcode_name_edit->setGeometry(105,11,sizeHint().width()-150,19);
  schedcode_name_edit->setMaxLength(10);
  schedcode_name_edit->setValidator(validator);
  QLabel *label=new QLabel(schedcode_name_edit,tr("&New Code:"),this);
  label->setGeometry(10,11,90,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddSchedCode::~AddSchedCode()
{
  delete schedcode_name_edit;
}


QSize AddSchedCode::sizeHint() const
{
  return QSize(250,120);
} 


QSizePolicy AddSchedCode::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddSchedCode::okData()
{
  if(schedcode_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("Invalid Name!"));
    return;
  }

  if(!RDSchedCode::create(schedcode_name_edit->text())) {
    QMessageBox::warning(this,tr("Code Exists"),tr("Code Already Exists!"));
    return;
  }

  EditSchedCode *schedcode=
    new EditSchedCode(schedcode_name_edit->text(),"",this);
  if(schedcode->exec()<0) {
    delete schedcode;
    done(-1);
    return;
  }
  delete schedcode;
  *schedcode_schedcode=schedcode_name_edit->text();
  done(0);
}


void AddSchedCode::cancelData()
{
  done(-1);
}

