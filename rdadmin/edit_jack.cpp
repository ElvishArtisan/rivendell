// edit_jack.cpp
//
// Edit a Rivendell Jack Configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_jack.cpp,v 1.1.4.4 2012/11/15 19:27:13 cvs Exp $
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qvalidator.h>
#include <qfiledialog.h>

#include <rdescape_string.h>

#include <globals.h>
#include <rdcart_dialog.h>
#include <rddb.h>
#include <edit_jack.h>
#include <edit_jack_client.h>


EditJack::EditJack(RDStation *station,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;

  edit_station=station;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("JACK Configuration for ")+edit_station->name());

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
  edit_start_jack_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // JACK Server Name
  //
  edit_jack_server_name_edit=new QLineEdit(this);
  edit_jack_server_name_label=
    new QLabel(edit_jack_server_name_edit,tr("JACK Server Name:"),this);
  edit_jack_server_name_label->setFont(font);
  edit_jack_server_name_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

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
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // JACK Client List
  //
  edit_jack_client_view=new RDListView(this);
  edit_jack_client_label=
    new QLabel(edit_jack_client_view,tr("JACK Clients to Start:"),this);
  edit_jack_client_label->setFont(font);
  edit_jack_client_view->setAllColumnsShowFocus(true);
  edit_jack_client_view->setItemMargin(5);
  edit_jack_client_view->addColumn(tr("Client"));
  edit_jack_client_view->setColumnAlignment(0,Qt::AlignLeft);
  edit_jack_client_view->addColumn(tr("Command Line"));
  edit_jack_client_view->setColumnAlignment(1,Qt::AlignLeft);
  connect(edit_jack_client_view,SIGNAL(doubleClicked(QListViewItem *,
						     const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

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
  edit_ok_button=new QPushButton(this,"edit_ok_button");
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this,"edit_cancel_button");
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

  RefreshList();
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
  RDSqlQuery *q;
  RDSqlQuery *q1;
  RDListViewItem *item=NULL;

  sql=QString("insert into JACK_CLIENTS set ")+
    "STATION_NAME=\""+RDEscapeString(edit_station->name())+"\","+
    "DESCRIPTION=\"\",COMMAND_LINE=\"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql="select last_insert_id() from JACK_CLIENTS";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    item=new RDListViewItem(edit_jack_client_view);
    item->setId(q->value(0).toInt());
    QString desc=tr("[New Client]");
    QString cmd="";
    EditJackClient *d=new EditJackClient(edit_station,this);
    if(d->exec(&desc,&cmd)==0) {
      item->setText(0,desc);
      item->setText(1,cmd);
    }
    else {
      sql=QString().sprintf("delete from JACK_CLIENTS where ID=%d",item->id());
      q1=new RDSqlQuery(sql);
      delete q1;
      delete item;
    }
  }
  delete q;
}


void EditJack::editData()
{
  RDListViewItem *item=(RDListViewItem *)edit_jack_client_view->selectedItem();
  if(item==NULL) {
    return;
  }
  QString desc=item->text(0);
  QString cmd=item->text(1);
  EditJackClient *d=new EditJackClient(edit_station,this);
  if(d->exec(&desc,&cmd)==0) {
    item->setText(0,desc);
    item->setText(1,cmd);
  }
  delete d;
}


void EditJack::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item=(RDListViewItem *)edit_jack_client_view->selectedItem();
  if(item==NULL) {
    return;
  }
  if(QMessageBox::question(this,tr("RDAdmin - JACK Clients"),
			   tr("Are you sure you want to delete JACK Client")+
			   " \""+item->text(0)+"\"?",QMessageBox::Yes,
			   QMessageBox::No)==QMessageBox::Yes) {
    sql=QString().sprintf("delete from JACK_CLIENTS where ID=%d",item->id());
    q=new RDSqlQuery(sql);
    delete q;
    delete item;
  }
}


void EditJack::doubleClickedData(QListViewItem *item,const QPoint &pt,int col)
{
  editData();
}


void EditJack::okData()
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item=NULL;

  edit_station->setStartJack(edit_start_jack_box->isChecked());
  if(edit_jack_server_name_edit->text()==EDITJACK_DEFAULT_SERVERNAME) {
    edit_station->setJackServerName("");
  }
  else {
    edit_station->setJackServerName(edit_jack_server_name_edit->text());
  }
  edit_station->setJackCommandLine(edit_jack_command_line_edit->text());
  item=(RDListViewItem *)edit_jack_client_view->firstChild();
  while(item!=NULL) {
    sql=QString("update JACK_CLIENTS set DESCRIPTION=\"")+
      RDEscapeString(item->text(0))+"\",COMMAND_LINE=\""+
      RDEscapeString(item->text(1))+"\" where ID="+
      QString().sprintf("%d",item->id());
    q=new RDSqlQuery(sql);
    delete q;
    item=(RDListViewItem *)item->nextSibling();
  }

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
  edit_jack_client_view->
    setGeometry(10,102,size().width()-20,size().height()-170);

  edit_add_button->setGeometry(15,size().height()-60,50,30);
  edit_edit_button->setGeometry(75,size().height()-60,50,30);
  edit_delete_button->setGeometry(135,size().height()-60,50,30);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditJack::RefreshList()
{
  RDListViewItem *l;

  edit_jack_client_view->clear();
  QString sql=QString().
    sprintf("select ID,DESCRIPTION,COMMAND_LINE from JACK_CLIENTS \
             where STATION_NAME=\"%s\" order by DESCRIPTION",
	    (const char *)edit_station->name());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new RDListViewItem(edit_jack_client_view);
    l->setId(q->value(0).toInt());
    l->setText(0,q->value(1).toString());
    l->setText(1,q->value(2).toString());
  }
  delete q;
}

