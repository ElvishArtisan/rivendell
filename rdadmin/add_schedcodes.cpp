// add_schedcodes.cpp
//
// Add scheduler codes dialog
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rddb.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include <edit_schedcodes.h>
#include <add_schedcodes.h>


AddSchedCode::AddSchedCode(QString *schedCode,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  schedCode_schedCode=schedCode;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Scheduler Code"));

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
  schedCode_name_edit=new QLineEdit(this,"schedCode_name_edit");
  schedCode_name_edit->setGeometry(105,11,sizeHint().width()-150,19);
  schedCode_name_edit->setMaxLength(10);
  schedCode_name_edit->setValidator(validator);
  QLabel *label=new QLabel(schedCode_name_edit,tr("&New Code:"),
				      this,"schedCode_name_label");
  label->setGeometry(10,11,90,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

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
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddSchedCode::~AddSchedCode()
{
  delete schedCode_name_edit;
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
  RDSqlQuery *q;
  QString sql;

  if(schedCode_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("Invalid Name!"));
    return;
  }

  sql=QString().sprintf("insert into SCHED_CODES set CODE=\"%s\"",
			(const char *)schedCode_name_edit->text());

  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    QMessageBox::warning(this,tr("Code Exists"),tr("Code Already Exists!"),
			 1,0,0);
    delete q;
    return;
  }
  delete q;

  EditSchedCode *schedCode=new EditSchedCode(schedCode_name_edit->text(),"", this,"group");
  if(schedCode->exec()<0) {
    delete schedCode;
    done(-1);
    return;
  }
  delete schedCode;
  *schedCode_schedCode=schedCode_name_edit->text();
  done(0);
}


void AddSchedCode::cancelData()
{
  done(-1);
}

