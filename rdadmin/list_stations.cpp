// list_stations.cpp
//
// List Rivendell Workstations
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_stations.cpp,v 1.28.6.1 2013/03/09 00:21:12 cvs Exp $
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
#include <qbuttongroup.h>

#include <rdairplay_conf.h>
#include <rdescape_string.h>
#include <rddb.h>
#include <list_stations.h>
#include <edit_station.h>
#include <add_station.h>


ListStations::ListStations(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Rivendell Workstation List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  //  Add Button
  //
  QPushButton *add_button=new QPushButton(this,"add_button");
  add_button->setGeometry(410,30,80,50);
  add_button->setFont(font);
  add_button->setText(tr("&Add"));
  connect(add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  QPushButton *edit_button=new QPushButton(this,"edit_button");
  edit_button->setGeometry(410,90,80,50);
  edit_button->setFont(font);
  edit_button->setText(tr("&Edit"));
  connect(edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  QPushButton *delete_button=new QPushButton(this,"delete_button");
  delete_button->setGeometry(410,150,80,50);
  delete_button->setFont(font);
  delete_button->setText(tr("&Delete"));
  connect(delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(410,240,80,50);
  close_button->setDefault(true);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Station List Box
  //
  list_box=new QListBox(this,"list_box");
  list_box->setGeometry(10,30,390,260);
  QLabel *list_box_label=new QLabel(list_box,tr("Ho&sts:"),
				    this,"list_box_label");
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,10,85,19);
  connect(list_box,SIGNAL(doubleClicked(QListBoxItem *)),
	  this,SLOT(doubleClickedData(QListBoxItem *)));

  RefreshList();
}


ListStations::~ListStations()
{
  delete list_box;
}


QSize ListStations::sizeHint() const
{
  return QSize(500,300);
} 


QSizePolicy ListStations::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListStations::addData()
{
  QString stationname;

  AddStation *add_station=new AddStation(&stationname,this,"add_station");
  if(add_station->exec()<0) {
    DeleteStation(stationname);
    delete add_station;
    return;
  }
  delete add_station;
  RefreshList(stationname);
}


void ListStations::editData()
{
  if(list_box->currentItem()<0) {
    return;
  }
  EditStation *edit_station=new EditStation(list_box->currentText(),
					    this,"edit_station");
  edit_station->exec();
  delete edit_station;
}


void ListStations::deleteData()
{
  QString str;

  str=QString(tr("Are you sure you want to delete host"));
  if(QMessageBox::warning(this,tr("Delete Station"),
			  QString().sprintf(
			    "%s %s?",(const char *)str,
			    (const char *)list_box->currentText()),
			  QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::Yes) {
    DeleteStation(list_box->currentText());
    list_box->removeItem(list_box->currentItem());
    if(list_box->currentItem()>=0) {
      list_box->setSelected(list_box->currentItem(),true);
    }
  }
}


void ListStations::closeData()
{
  done(0);
}


void ListStations::doubleClickedData(QListBoxItem *item)
{
  editData();
}


void ListStations::RefreshList(QString stationname)
{
  QString sql;
  RDSqlQuery *q;

  list_box->clear();
  q=new RDSqlQuery("select NAME from STATIONS",0);
  while (q->next()) {
    list_box->insertItem(q->value(0).toString());
    if(stationname==list_box->text(list_box->count()-1)) {
      list_box->setCurrentItem(list_box->count()-1);
    }
  }
  delete q;
}


void ListStations::DeleteStation(QString name)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("delete from DECKS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from TTYS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from AUDIO_PORTS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RECORDINGS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from SERVICE_PERMS where STATION_NAME=\"%s\"",
		      (const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDAIRPLAY where STATION=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDAIRPLAY_CHANNELS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDPANEL where STATION=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDPANEL_CHANNELS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDLOGEDIT where STATION=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from MATRICES where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from INPUTS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from OUTPUTS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from VGUEST_RESOURCES where\
                           STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDLIBRARY where STATION=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from GPIS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from HOSTVARS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from STATIONS where NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=
    QString().sprintf("delete from REPORT_STATIONS where STATION_NAME=\"%s\"",
		      (const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from PANELS where (TYPE=%d && OWNER=\"%s\")",
			RDAirPlayConf::StationPanel,
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().
    sprintf("delete from EXTENDED_PANELS where (TYPE=%d && OWNER=\"%s\")",
	    RDAirPlayConf::StationPanel,(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString().sprintf("select ID from ENCODERS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("delete from ENCODER_CHANNELS where ENCODER_ID=%d",
			  q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    delete q1;
    sql=QString().sprintf("delete from ENCODER_SAMPLERATES where ENCODER_ID=%d",
			  q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    delete q1;
    sql=QString().sprintf("delete from ENCODER_BITRATES where ENCODER_ID=%d",
			  q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
  sql=QString().sprintf("delete from ENCODERS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from RDHOTKEYS where STATION_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
}

