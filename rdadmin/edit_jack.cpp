// edit_jack.cpp
//
// Edit a Rivendell Jack Configuration
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_jack.h"
#include "edit_jack_client.h"

EditJack::EditJack(RDStation *station,QWidget *parent)
  : QDialog(parent)
{
  edit_station=station;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("JACK Configuration for ")+
  edit_station->name());

  //
  // Create Fonts
  //
  QFont normal_font=QFont("Helvetica",12,QFont::Normal);
  normal_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Start JACK Server
  //
  edit_start_jack_box=new QCheckBox(this);
  edit_start_jack_label=
    new QLabel(edit_start_jack_box,tr("Start JACK Server"),this);
  edit_start_jack_label->setFont(font);
  edit_start_jack_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // JACK Server Name
  //
  edit_jack_server_name_edit=new QLineEdit(this);
  edit_jack_server_name_label=
    new QLabel(edit_jack_server_name_edit,tr("JACK Server Name:"),this);
  edit_jack_server_name_label->setFont(font);
  edit_jack_server_name_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // JACK Command Line
  //
  edit_jack_command_line_edit=new QLineEdit(this);
  connect(edit_start_jack_box,SIGNAL(toggled(bool)),
	  this,SLOT(startJackData(bool)));
  edit_jack_command_line_label=
    new QLabel(edit_jack_command_line_edit,tr("JACK Command Line:"),this);
  edit_jack_command_line_label->setFont(font);
  edit_jack_command_line_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // JACK Client List
  //
  edit_jack_client_label=
    new QLabel(tr("JACK Clients to Start:"),this);
  edit_jack_client_label->setFont(font);
  edit_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "DESCRIPTION,"+
    "COMMAND_LINE "+
    "from JACK_CLIENTS where "+
    "STATION_NAME=\""+RDEscapeString(edit_station->name())+"\" "+
    "order by DESCRIPTION";
  edit_model->setQuery(sql);
  edit_model->setHeaderData(1,Qt::Horizontal,tr("Client"));
  edit_model->setHeaderData(2,Qt::Horizontal,tr("Command Line"));
  edit_view=new RDTableView(this);
  edit_view->setModel(edit_model);
  edit_view->hideColumn(0);
  edit_view->resizeColumnsToContents();
  connect(edit_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Add Button
  //
  edit_add_button=new QPushButton(this);
  edit_add_button->setFont(font);
  edit_add_button->setText(tr("&Add"));
  connect(edit_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setFont(font);
  edit_edit_button->setText(tr("&Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->setFont(font);
  edit_delete_button->setText(tr("&Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setFont(font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  edit_start_jack_box->setChecked(edit_station->startJack());
  edit_jack_server_name_edit->setText(edit_station->jackServerName());
  edit_jack_command_line_edit->setText(edit_station->jackCommandLine());
  if(edit_jack_server_name_edit->text().isEmpty()) {
    edit_jack_server_name_edit->setText(EDITJACK_DEFAULT_SERVERNAME);
  }
  startJackData(edit_station->startJack());
}


QSize EditJack::sizeHint() const
{
  return QSize(450,330);
} 


QSizePolicy EditJack::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditJack::startJackData(bool state)
{
  edit_jack_command_line_label->setEnabled(state);
  edit_jack_command_line_edit->setEnabled(state);
}


void EditJack::addData()
{
  QString sql;

  sql=QString("insert into JACK_CLIENTS set ")+
    "STATION_NAME=\""+RDEscapeString(edit_station->name())+"\","+
    "DESCRIPTION=\"\",COMMAND_LINE=\"\"";
  int id=RDSqlQuery::run(sql).toInt();
  if(id>=0) {
    EditJackClient *d=new EditJackClient(edit_station,this);
    if(d->exec(id)==0) {
      edit_model->update();
    }
    else {
      sql=QString("delete from JACK_CLIENTS where ")+
	QString().sprintf("ID=%d",id);
      RDSqlQuery::run(sql);
    }
    delete d;
  }
}


void EditJack::editData()
{
  QItemSelectionModel *s=edit_view->selectionModel();
  if(s->hasSelection()) {
    EditJackClient *d=new EditJackClient(edit_station,this);
    if(d->exec(s->selectedRows()[0].data().toInt())==0) {
      edit_model->update();
    }
    delete d;
  }
}


void EditJack::deleteData()
{
  QItemSelectionModel *s=edit_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete JACK Client"),
		       tr("Are you sure you want to delete this JACK client?"),
			  QMessageBox::No,QMessageBox::Yes)!=
      QMessageBox::Yes) {
      return;
    }
    QString sql=QString("delete from JACK_CLIENTS where ")+
      QString().sprintf("ID=%d",s->selectedRows()[0].data().toInt());
    RDSqlQuery::run(sql);
    edit_model->update();
  }
}


void EditJack::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void EditJack::okData()
{
  edit_station->setStartJack(edit_start_jack_box->isChecked());
  if(edit_jack_server_name_edit->text()==EDITJACK_DEFAULT_SERVERNAME) {
    edit_station->setJackServerName("");
  }
  else {
    edit_station->setJackServerName(edit_jack_server_name_edit->text());
  }
  edit_station->setJackCommandLine(edit_jack_command_line_edit->text());

  done(0);
}


void EditJack::cancelData()
{
  done(-1);
}


void EditJack::resizeEvent(QResizeEvent *e)
{
  edit_start_jack_box->setGeometry(10,11,15,15);
  edit_start_jack_label->setGeometry(30,10,sizeHint().width()-70,20);

  edit_jack_server_name_label->setGeometry(10,32,130,20);
  edit_jack_server_name_edit->setGeometry(145,32,size().width()-155,20);

  edit_jack_command_line_label->setGeometry(10,54,130,20);
  edit_jack_command_line_edit->setGeometry(145,54,size().width()-155,20);

  edit_jack_client_label->setGeometry(15,80,sizeHint().width()-28,20);
  edit_view->
    setGeometry(10,102,size().width()-20,size().height()-170);

  edit_add_button->setGeometry(15,size().height()-60,50,30);
  edit_edit_button->setGeometry(75,size().height()-60,50,30);
  edit_delete_button->setGeometry(135,size().height()-60,50,30);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
