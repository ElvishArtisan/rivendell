// add_replicator.cpp
//
// Add a Rivendell Replicator Configuration
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_replicator.cpp,v 1.2.8.1 2013/07/05 22:44:17 cvs Exp $
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

#include <edit_replicator.h>
#include <add_replicator.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>


AddReplicator::AddReplicator(QString *rname,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  repl_name=rname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Replicator"));

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

  //
  // Replicator Name
  //
  repl_name_edit=new QLineEdit(this,"repl_name_edit");
  repl_name_edit->setGeometry(145,11,sizeHint().width()-150,19);
  repl_name_edit->setMaxLength(10);
  repl_name_edit->setValidator(validator);
  QLabel *label=new QLabel(repl_name_edit,tr("&New Replicator Name:"),
				      this,"repl_name_label");
  label->setGeometry(10,11,130,19);
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


AddReplicator::~AddReplicator()
{
  delete repl_name_edit;
}


QSize AddReplicator::sizeHint() const
{
  return QSize(250,108);
} 


QSizePolicy AddReplicator::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddReplicator::okData()
{
  RDSqlQuery *q;
  QString sql;

  if(repl_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),tr("You must give the replicator a name!"));
    return;
  }

  sql=QString().sprintf("insert into REPLICATORS set NAME=\"%s\"",
			(const char *)repl_name_edit->text());

  q=new RDSqlQuery(sql);
  if(!q->isActive()) {
    QMessageBox::warning(this,tr("Replicator Exists"),tr("A replicator with that name already exists!"));
    delete q;
    return;
  }
  delete q;

  EditReplicator *replicator=new EditReplicator(repl_name_edit->text(),this,"replicator");
  if(replicator->exec()<0) {
    sql=QString().sprintf("delete from REPLICATORS where NAME=\"%s\"",
			  (const char *)repl_name_edit->text());
    q=new RDSqlQuery(sql);
    delete q;
    delete replicator;
    done(-1);
    return;
  }
  delete replicator;
  *repl_name=repl_name_edit->text();
  done(0);
}


void AddReplicator::cancelData()
{
  done(-1);
}
