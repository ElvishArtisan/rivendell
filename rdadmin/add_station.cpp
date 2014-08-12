// add_station.cpp
//
// Add a Rivendell Workstation
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_station.cpp,v 1.32.6.1 2013/03/09 00:21:11 cvs Exp $
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
#include <rdairplay_conf.h>
#include <rdescape_string.h>

#include <edit_station.h>
#include <add_station.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>


AddStation::AddStation(QString *stationname,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  add_name=stationname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Add Host"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Station Name
  //
  add_name_edit=new QLineEdit(this,"add_name_edit");
  add_name_edit->setGeometry(130,10,sizeHint().width()-140,19);
  add_name_edit->setMaxLength(64);
  add_name_edit->setValidator(validator);
  QLabel *label=new QLabel(add_name_edit,tr("New &Host Name:"),this,"label");
  label->setGeometry(10,10,115,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Exemplar
  //
  add_exemplar_box=new QComboBox(this,"add_exemplar_box");
  add_exemplar_box->setGeometry(130,35,sizeHint().width()-140,19);
  label=new QLabel(add_exemplar_box,tr("Base Host On:"),this,"label");
  label->setGeometry(10,35,115,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
			 80,50);
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

  //
  // Fill Exemplar List
  //
  add_exemplar_box->insertItem(tr("Empty Host Config"));
  QString sql="select NAME from STATIONS";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    add_exemplar_box->insertItem(q->value(0).toString());
  }
  delete q;
}


AddStation::~AddStation() 
{
  delete add_name_edit;
}


QSize AddStation::sizeHint() const
{
  return QSize(380,130);
} 


QSizePolicy AddStation::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddStation::okData()
{
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString sql;

  if(add_name_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Invalid Name"),
			 tr("You must give the host a name!"));
    return;
  }

  if(add_exemplar_box->currentItem()==0) {  // Create Blank Host Config
    sql=QString().sprintf("insert into STATIONS set NAME=\"%s\",\
                           DESCRIPTION=\"Workstation %s\",USER_NAME=\"user\",\
                           DEFAULT_NAME=\"user\"",
			  (const char *)add_name_edit->text(),
			  (const char *)add_name_edit->text());
    q=new RDSqlQuery(sql);
    if(!q->isActive()) {
      QMessageBox::warning(this,tr("Host Exists"),tr("Host Already Exists!"),
			   1,0,0);
      delete q;
      return;
    }
    delete q;

    //
    // Create Service Perms
    //
    sql="select NAME from SERVICES";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into SERVICE_PERMS set\
                           SERVICE_NAME=\"%s\",STATION_NAME=\"%s\"",
			    (const char *)q->value(0).toString(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // RDAirPlay/RDPanel Channel Data
    //
    for(unsigned i=0;i<10;i++) {
      sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("INSTANCE=%u",i);
      q=new RDSqlQuery(sql);
      delete q;

      sql=QString("insert into RDPANEL_CHANNELS set ")+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("INSTANCE=%u",i);
      q=new RDSqlQuery(sql);
      delete q;
    }
  }
  else {    // Use Specified Config

    sql=QString().sprintf("select DEFAULT_NAME,STARTUP_CART,TIME_OFFSET,\
                           BROADCAST_SECURITY,HEARTBEAT_CART,\
                           HEARTBEAT_INTERVAL,EDITOR_PATH,FILTER_MODE,\
                           SYSTEM_MAINT,HTTP_STATION,CAE_STATION from STATIONS\
                           where NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into STATIONS set NAME=\"%s\",\
                             DESCRIPTION=\"Workstation %s\",\
                             USER_NAME=\"%s\",\
                             DEFAULT_NAME=\"%s\",\
                             STARTUP_CART=%u,\
                             TIME_OFFSET=%d,\
                             BROADCAST_SECURITY=%u,\
                             HEARTBEAT_CART=%u,\
                             HEARTBEAT_INTERVAL=%u,\
                             EDITOR_PATH=\"%s\",\
                             FILTER_MODE=%d,\
                             SYSTEM_MAINT=\"%s\",\
                             HTTP_STATION=\"%s\",\
                             CAE_STATION=\"%s\"",
			    (const char *)RDEscapeString(add_name_edit->text()),
			    (const char *)RDEscapeString(add_name_edit->text()),
			    (const char *)RDEscapeString(q->value(0).
							 toString()),
			    (const char *)RDEscapeString(q->value(0).
							 toString()),
			    q->value(1).toUInt(),
			    q->value(2).toInt(),
			    q->value(3).toUInt(),
			    q->value(4).toUInt(),
			    q->value(5).toUInt(),
			    (const char *)RDEscapeString(q->value(6).
							 toString()),
			    q->value(7).toInt(),
			    (const char *)q->value(8).toString(),
			    (const char *)RDEscapeString(q->value(9).
							 toString()),
			    (const char *)RDEscapeString(q->value(10).
							 toString()));
      q1=new RDSqlQuery(sql);
      if(!q1->isActive()) {
	QMessageBox::warning(this,tr("Host Exists"),tr("Host Already Exists!"),
			     1,0,0);
	delete q1;
	delete q;
	return;
      }
      delete q1;
    }
    delete q;

    //
    // Clone Service Perms
    //
    sql=QString().sprintf("select SERVICE_NAME from SERVICE_PERMS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into SERVICE_PERMS set\
                             STATION_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			    (const char *)add_name_edit->text(),
			    (const char *)q->value(0).toString());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDLibrary Config
    //
    sql=QString().sprintf("select INPUT_CARD,INPUT_PORT,INPUT_TYPE,\
                           OUTPUT_CARD,OUTPUT_PORT,VOX_THRESHOLD,\
                           TRIM_THRESHOLD,DEFAULT_FORMAT,DEFAULT_CHANNELS,\
                           DEFAULT_SAMPRATE,DEFAULT_LAYER,DEFAULT_BITRATE,\
                           DEFAULT_RECORD_MODE,DEFAULT_TRIM_STATE,MAXLENGTH,\
                           TAIL_PREROLL,RIPPER_DEVICE,PARANOIA_LEVEL,\
                           RIPPER_LEVEL,CDDB_SERVER from RDLIBRARY\
                           where (STATION=\"%s\")&&(INSTANCE=0)",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into RDLIBRARY set\
                             INPUT_CARD=%d,INPUT_PORT=%d,INPUT_TYPE=%d,\
                             OUTPUT_CARD=%d,OUTPUT_PORT=%d,VOX_THRESHOLD=%d,\
                             TRIM_THRESHOLD=%d,DEFAULT_FORMAT=%u,\
                             DEFAULT_CHANNELS=%u,DEFAULT_SAMPRATE=%u,\
                             DEFAULT_LAYER=%u,DEFAULT_BITRATE=%u,\
                             DEFAULT_RECORD_MODE=%u,DEFAULT_TRIM_STATE=\"%s\",\
                             MAXLENGTH=%d,TAIL_PREROLL=%u,\
                             RIPPER_DEVICE=\"%s\",PARANOIA_LEVEL=%d,\
                             RIPPER_LEVEL=%d,CDDB_SERVER=\"%s\",\
                             STATION=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    q->value(3).toInt(),
			    q->value(4).toInt(),
			    q->value(5).toInt(),
			    q->value(6).toInt(),
			    q->value(7).toUInt(),
			    q->value(8).toUInt(),
			    q->value(9).toUInt(),
			    q->value(10).toUInt(),
			    q->value(11).toUInt(),
			    q->value(12).toUInt(),
			    (const char *)q->value(13).toString(),
			    q->value(14).toInt(),
			    q->value(15).toUInt(),
			    (const char *)q->value(16).toString(),
			    q->value(17).toInt(),
			    q->value(18).toInt(),
			    (const char *)q->value(19).toString(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDLogEdit Config
    //
    sql=QString().sprintf("select INPUT_CARD,INPUT_PORT,\
                           OUTPUT_CARD,OUTPUT_PORT,\
                           FORMAT,DEFAULT_CHANNELS,\
                           SAMPRATE,LAYER,BITRATE,MAXLENGTH,\
                           TAIL_PREROLL,START_CART,END_CART,\
                           REC_START_CART,REC_END_CART,TRIM_THRESHOLD,\
                           RIPPER_LEVEL,DEFAULT_TRANS_TYPE from RDLOGEDIT\
                           where (STATION=\"%s\")",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into RDLOGEDIT set\
                             INPUT_CARD=%d,INPUT_PORT=%d,\
                             OUTPUT_CARD=%d,OUTPUT_PORT=%d,\
                             FORMAT=%u,\
                             DEFAULT_CHANNELS=%u,SAMPRATE=%u,\
                             LAYER=%u,BITRATE=%u,\
                             MAXLENGTH=%d,TAIL_PREROLL=%u,\
                             STATION=\"%s\",\
                             START_CART=%u,\
                             END_CART=%d,\
                             REC_START_CART=%u,\
                             REC_END_CART=%u,\
                             TRIM_THRESHOLD=%d,\
                             RIPPER_LEVEL=%d,\
                             DEFAULT_TRANS_TYPE=%d",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    q->value(3).toInt(),
			    q->value(4).toUInt(),
			    q->value(5).toUInt(),
			    q->value(6).toUInt(),
			    q->value(7).toUInt(),
			    q->value(8).toUInt(),
			    q->value(9).toInt(),
			    q->value(10).toUInt(),
			    (const char *)add_name_edit->text(),
			    q->value(11).toUInt(),
			    q->value(12).toUInt(),
			    q->value(13).toUInt(),
			    q->value(14).toUInt(),
			    q->value(15).toInt(),
			    q->value(16).toInt(),
			    q->value(17).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDCatch Config
    //
    sql=QString().sprintf("select CHANNEL,CARD_NUMBER,PORT_NUMBER,\
                           MON_PORT_NUMBER,PORT_TYPE,DEFAULT_FORMAT,\
                           DEFAULT_CHANNELS,DEFAULT_SAMPRATE,DEFAULT_BITRATE,\
                           DEFAULT_THRESHOLD,DEFAULT_MONITOR_ON from DECKS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into DECKS set\
                             CHANNEL=%u,CARD_NUMBER=%d,PORT_NUMBER=%d,\
                             MON_PORT_NUMBER=%d,PORT_TYPE=\"%s\",\
                             DEFAULT_FORMAT=%d,DEFAULT_CHANNELS=%d,\
                             DEFAULT_SAMPRATE=%d,DEFAULT_BITRATE=%d,\
                             DEFAULT_THRESHOLD=%d,STATION_NAME=\"%s\",\
                             DEFAULT_MONITOR_ON=\"%s\"",
			    q->value(0).toUInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    q->value(3).toInt(),
			    (const char *)q->value(4).toString(),
			    q->value(5).toInt(),
			    q->value(6).toInt(),
			    q->value(7).toInt(),
			    q->value(8).toInt(),
			    q->value(9).toInt(),
			    (const char *)add_name_edit->text(),
                            (const char *)q->value(10).toString());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDAirPlay Config
    //
    sql=QString().sprintf("select SEGUE_LENGTH,\
                           TRANS_LENGTH,OP_MODE,START_MODE,PIE_COUNT_LENGTH,\
                           PIE_COUNT_ENDPOINT,CHECK_TIMESYNC,STATION_PANELS,\
                           USER_PANELS,SHOW_AUX_1,SHOW_AUX_2,CLEAR_FILTER,\
                           DEFAULT_TRANS_TYPE,BAR_ACTION,FLASH_PANEL,\
                           PAUSE_ENABLED,UDP_ADDR0,UDP_PORT0,UDP_STRING0,\
                           UDP_ADDR1,UDP_PORT1,UDP_STRING1,UDP_ADDR2,\
                           UDP_PORT2,UDP_STRING2,DEFAULT_SERVICE,\
                           LOG_RML0,LOG_RML1,LOG_RML2,\
                           BUTTON_LABEL_TEMPLATE,EXIT_PASSWORD from RDAIRPLAY \
                           where (STATION=\"%s\")",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into RDAIRPLAY set\
                             SEGUE_LENGTH=%d,TRANS_LENGTH=%d,OP_MODE=%d,\
                             START_MODE=%d,PIE_COUNT_LENGTH=%d,\
                             PIE_COUNT_ENDPOINT=%d,CHECK_TIMESYNC=\"%s\",\
                             STATION_PANELS=%d,USER_PANELS=%d,\
                             SHOW_AUX_1=\"%s\",SHOW_AUX_2=\"%s\",\
                             CLEAR_FILTER=\"%s\",DEFAULT_TRANS_TYPE=%u,\
                             BAR_ACTION=%u,FLASH_PANEL=\"%s\",\
                             PAUSE_ENABLED=\"%s\",\
                             UDP_ADDR0=\"%s\",UDP_PORT0=%u,UDP_STRING0=\"%s\",\
                             UDP_ADDR1=\"%s\",UDP_PORT1=%u,UDP_STRING1=\"%s\",\
                             UDP_ADDR2=\"%s\",UDP_PORT2=%u,UDP_STRING2=\"%s\",\
                             STATION=\"%s\",DEFAULT_SERVICE=\"%s\",\
                             LOG_RML0=\"%s\",\
                             LOG_RML1=\"%s\",\
                             LOG_RML2=\"%s\",\
                             BUTTON_LABEL_TEMPLATE=\"%s\",\
                             EXIT_PASSWORD=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    q->value(3).toInt(),
			    q->value(4).toInt(),
			    q->value(5).toInt(),
			    (const char *)q->value(6).toString(),
			    q->value(7).toInt(),
			    q->value(8).toInt(),
			    (const char *)q->value(9).toString(),
			    (const char *)q->value(10).toString(),
			    (const char *)q->value(11).toString(),
			    q->value(12).toUInt(),
			    q->value(13).toUInt(),
			    (const char *)q->value(14).toString(),
			    (const char *)q->value(15).toString(),
			    (const char *)q->value(16).toString(),
			    q->value(17).toUInt(),
			    (const char *)q->value(18).toString(),
			    (const char *)q->value(19).toString(),
			    q->value(20).toUInt(),
			    (const char *)q->value(21).toString(),
			    (const char *)q->value(22).toString(),
			    q->value(23).toUInt(),
			    (const char *)q->value(24).toString(),
			    (const char *)add_name_edit->text(),
			    (const char *)q->value(25).toString(),
			    (const char *)q->value(26).toString(),
			    (const char *)q->value(27).toString(),
			    (const char *)q->value(28).toString(),
			    (const char *)q->value(29).toString(),
			    (const char *)q->value(30).toString());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql=QString("select INSTANCE,CARD,PORT,START_RML,STOP_RML,")+
      "START_GPI_MATRIX,"+
      "START_GPI_LINE,START_GPO_MATRIX,START_GPO_LINE,STOP_GPI_MATRIX,"+
      "STOP_GPI_LINE,STOP_GPO_MATRIX,STOP_GPO_LINE from RDAIRPLAY_CHANNELS "+
      "where STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+
      "\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into RDAIRPLAY_CHANNELS set ")+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("INSTANCE=%u,",q->value(0).toUInt())+
	QString().sprintf("CARD=%d,",q->value(1).toInt())+
	QString().sprintf("PORT=%d,",q->value(2).toInt())+
	"START_RML=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STOP_RML=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("START_GPI_MATRIX=%d,",q->value(5).toInt())+
	QString().sprintf("START_GPI_LINE=%d,",q->value(6).toInt())+
	QString().sprintf("START_GPO_MATRIX=%d,",q->value(7).toInt())+
	QString().sprintf("START_GPO_LINE=%d,",q->value(8).toInt())+
	QString().sprintf("STOP_GPI_MATRIX=%d,",q->value(9).toInt())+
	QString().sprintf("STOP_GPI_LINE=%d,",q->value(10).toInt())+
	QString().sprintf("STOP_GPO_MATRIX=%d,",q->value(11).toInt())+
	QString().sprintf("STOP_GPO_LINE=%d",q->value(12).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDPanel Config
    //
    sql=QString().sprintf("select STATION_PANELS,\
                           USER_PANELS,CLEAR_FILTER,\
                           FLASH_PANEL,\
                           DEFAULT_SERVICE,\
                           BUTTON_LABEL_TEMPLATE from RDPANEL \
                           where (STATION=\"%s\")",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("insert into RDPANEL set\
                             STATION_PANELS=%d,\
                             USER_PANELS=%d,\
                             CLEAR_FILTER=\"%s\",\
                             FLASH_PANEL=\"%s\",\
                             STATION=\"%s\",\
                             DEFAULT_SERVICE=\"%s\",\
                             BUTTON_LABEL_TEMPLATE=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    (const char *)q->value(2).toString(),
			    (const char *)q->value(3).toString(),
			    (const char *)add_name_edit->text(),
			    (const char *)q->value(4).toString(),
			    (const char *)q->value(5).toString());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql=QString("select INSTANCE,CARD,PORT,START_RML,STOP_RML,")+
      "START_GPI_MATRIX,"+
      "START_GPI_LINE,START_GPO_MATRIX,START_GPO_LINE,STOP_GPI_MATRIX,"+
      "STOP_GPI_LINE,STOP_GPO_MATRIX,STOP_GPO_LINE from RDPANEL_CHANNELS "+
      "where STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+
      "\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into RDPANEL_CHANNELS set ")+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("INSTANCE=%u,",q->value(0).toUInt())+
	QString().sprintf("CARD=%d,",q->value(1).toInt())+
	QString().sprintf("PORT=%d,",q->value(2).toInt())+
	"START_RML=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STOP_RML=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("START_GPI_MATRIX=%d,",q->value(5).toInt())+
	QString().sprintf("START_GPI_LINE=%d,",q->value(6).toInt())+
	QString().sprintf("START_GPO_MATRIX=%d,",q->value(7).toInt())+
	QString().sprintf("START_GPO_LINE=%d,",q->value(8).toInt())+
	QString().sprintf("STOP_GPI_MATRIX=%d,",q->value(9).toInt())+
	QString().sprintf("STOP_GPI_LINE=%d,",q->value(10).toInt())+
	QString().sprintf("STOP_GPO_MATRIX=%d,",q->value(11).toInt())+
	QString().sprintf("STOP_GPO_LINE=%d",q->value(12).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Audio Port Settings
    //
    sql="select CARD_NUMBER,CLOCK_SOURCE,";
    for(int i=0;i<RD_MAX_PORTS;i++) {
      sql+=QString().sprintf("INPUT_%d_LEVEL,INPUT_%d_MODE,INPUT_%d_TYPE,\
                              OUTPUT_%d_LEVEL,",
			     i,i,i,i);
    }
    sql=sql.left(sql.length()-1);
    sql+=QString().sprintf(" from AUDIO_PORTS where STATION_NAME=\"%s\"",
			   (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into AUDIO_PORTS set\
                             CARD_NUMBER=%u,CLOCK_SOURCE=%d,\
                             STATION_NAME=\"%s\",",
			    q->value(0).toUInt(),
			    q->value(1).toInt(),
			    (const char *)add_name_edit->text());
      for(int i=0;i<RD_MAX_PORTS;i++) {
	sql+=QString().sprintf("INPUT_%d_LEVEL=%d,INPUT_%d_MODE=%d,\
                                INPUT_%d_TYPE=%d,OUTPUT_%d_LEVEL=%d,",
			       i,q->value(2+3*i).toInt(),
			       i,q->value(3+3*i).toInt(),
			       i,q->value(4+3*i).toInt(),
			       i,q->value(5+3*i).toInt());
      }
      sql=sql.left(sql.length()-1);
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone the Serial Setups
    //
    sql=QString().sprintf("select PORT_ID,ACTIVE,PORT,BAUD_RATE,DATA_BITS,\
                           STOP_BITS,PARITY,TERMINATION from TTYS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into TTYS set\
                             PORT_ID=%u,ACTIVE=\"%s\",PORT=\"%s\",\
                             BAUD_RATE=%d,DATA_BITS=%d,STOP_BITS=%d,\
                             PARITY=%d,TERMINATION=%d,STATION_NAME=\"%s\"",
			    q->value(0).toUInt(),
			    (const char *)q->value(1).toString(),
			    (const char *)q->value(2).toString(),
			    q->value(3).toInt(),
			    q->value(4).toInt(),
			    q->value(5).toInt(),
			    q->value(6).toInt(),
			    q->value(7).toInt(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Matrices
    //
    sql=QString().sprintf("select NAME,MATRIX,TYPE,PORT_TYPE,CARD,PORT,\
                           IP_ADDRESS,IP_PORT,USERNAME,PASSWORD,GPIO_DEVICE,\
                           INPUTS,OUTPUTS,GPIS,GPOS,DISPLAYS,FADERS,\
                           PORT_TYPE_2,PORT_2,IP_ADDRESS_2,IP_PORT_2,\
                           USERNAME_2,PASSWORD_2 \
                           from MATRICES where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into MATRICES set\
                             NAME=\"%s\",MATRIX=%d,TYPE=%d,PORT_TYPE=%d,\
                             CARD=%d,PORT=%d,IP_ADDRESS=\"%s\",IP_PORT=%d,\
                             USERNAME=\"%s\",PASSWORD=\"%s\",\
                             GPIO_DEVICE=\"%s\",INPUTS=%d,OUTPUTS=%d,GPIS=%d,\
                             GPOS=%d,DISPLAYS=%d,STATION_NAME=\"%s\",\
                             FADERS=%d,PORT_TYPE_2=%d,PORT_2=%d,\
                             IP_ADDRESS_2=\"%s\",IP_PORT_2=%d,\
                             USERNAME_2=\"%s\",PASSWORD_2=\"%s\"",
			    (const char *)q->value(0).toString(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    q->value(3).toInt(),
			    q->value(4).toInt(),
			    q->value(5).toInt(),
			    (const char *)q->value(6).toString(),
			    q->value(7).toInt(),
			    (const char *)q->value(8).toString(),
			    (const char *)q->value(9).toString(),
			    (const char *)q->value(10).toString(),
			    q->value(11).toInt(),
			    q->value(12).toInt(),
			    q->value(13).toInt(),
			    q->value(14).toInt(),
			    q->value(15).toInt(),
			    (const char *)add_name_edit->text(),
			    q->value(16).toInt(),
			    q->value(17).toInt(),
			    q->value(18).toInt(),
			    (const char *)q->value(19).toString(),
			    q->value(20).toInt(),
			    (const char *)q->value(21).toString(),
			    (const char *)q->value(22).toString());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Matrix Inputs
    //
    sql=QString().sprintf("select MATRIX,NUMBER,NAME,FEED_NAME,CHANNEL_MODE,\
                           ENGINE_NUM,DEVICE_NUM from INPUTS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into INPUTS set\
                             MATRIX=%d,NUMBER=%d,NAME=\"%s\",FEED_NAME=\"%s\",\
                             CHANNEL_MODE=%d,ENGINE_NUM=%d,DEVICE_NUM=%d,\
                             STATION_NAME=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    (const char *)q->value(2).toString(),
			    (const char *)q->value(3).toString(),
			    q->value(4).toInt(),
			    q->value(5).toInt(),
			    q->value(6).toInt(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Matrix Outputs
    //
    sql=QString().sprintf("select MATRIX,NUMBER,NAME,\
                           ENGINE_NUM,DEVICE_NUM from OUTPUTS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into OUTPUTS set\
                             MATRIX=%d,NUMBER=%d,NAME=\"%s\",\
                             ENGINE_NUM=%d,DEVICE_NUM=%d,\
                             STATION_NAME=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    (const char *)q->value(2).toString(),
			    q->value(3).toInt(),
			    q->value(4).toInt(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone GPIs
    //
    sql=QString().sprintf("select MATRIX,NUMBER,MACRO_CART from GPIS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into GPIS set\
                             MATRIX=%d,NUMBER=%d,MACRO_CART=%d,\
                             STATION_NAME=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Close vGuest Settings
    //
    sql=QString().sprintf("select MATRIX_NUM,VGUEST_TYPE,NUMBER,ENGINE_NUM,\
                           DEVICE_NUM,SURFACE_NUM,RELAY_NUM,BUSS_NUM\
                           from VGUEST_RESOURCES where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into VGUEST_RESOURCES set\
                             MATRIX_NUM=%d,VGUEST_TYPE=%d,NUMBER=%d,\
                             ENGINE_NUM=%d,DEVICE_NUM=%d,SURFACE_NUM=%d,\
                             RELAY_NUM=%d,BUSS_NUM=%d,STATION_NAME=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    q->value(3).toInt(),
			    q->value(4).toInt(),
			    q->value(5).toInt(),
			    q->value(6).toInt(),
			    q->value(7).toInt(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Host Variables
    //
    sql=QString().sprintf("select NAME,VARVALUE,REMARK from HOSTVARS\
                           where STATION_NAME=\"%s\"",
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into HOSTVARS set\
                             NAME=\"%s\",VARVALUE=\"%s\",REMARK=\"%s\",\
                             STATION_NAME=\"%s\"",
			    (const char *)q->value(0).toString(),
			    (const char *)q->value(1).toString(),
			    (const char *)q->value(2).toString(),
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone System Panels
    //
    sql=QString().sprintf("select PANEL_NO,ROW_NO,COLUMN_NO,LABEL,CART,\
                           DEFAULT_COLOR from PANELS where \
                           (TYPE=%d && OWNER=\"%s\")",
			  RDAirPlayConf::StationPanel,
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into PANELS set PANEL_NO=%d,ROW_NO=%d,\
                             COLUMN_NO=%d,LABEL=\"%s\",CART=%u,\
                             DEFAULT_COLOR=\"%s\",TYPE=%d,OWNER=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    (const char *)q->value(3).toString(),
			    q->value(4).toUInt(),
			    (const char *)q->value(5).toString(),
			    RDAirPlayConf::StationPanel,
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql=QString().sprintf("select PANEL_NO,ROW_NO,COLUMN_NO,LABEL,CART,\
                           DEFAULT_COLOR from EXTENDED_PANELS where \
                           (TYPE=%d && OWNER=\"%s\")",
			  RDAirPlayConf::StationPanel,
			  (const char *)add_exemplar_box->currentText());
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into EXTENDED_PANELS set PANEL_NO=%d,\
                             ROW_NO=%d,COLUMN_NO=%d,LABEL=\"%s\",CART=%u,\
                             DEFAULT_COLOR=\"%s\",TYPE=%d,OWNER=\"%s\"",
			    q->value(0).toInt(),
			    q->value(1).toInt(),
			    q->value(2).toInt(),
			    (const char *)q->value(3).toString(),
			    q->value(4).toUInt(),
			    (const char *)q->value(5).toString(),
			    RDAirPlayConf::StationPanel,
			    (const char *)add_name_edit->text());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Encoders
    //
    sql=QString().sprintf("select ID,NAME,COMMAND_LINE from ENCODERS \
                           where STATION_NAME=\"%s\"",
			  (const char *)RDEscapeString(add_exemplar_box->
						       currentText()));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().
	sprintf("insert into ENCODERS set NAME=\"%s\",\
                 COMMAND_LINE=\"%s\",STATION_NAME=\"%s\"",
		(const char *)RDEscapeString(q->value(1).toString()),
		(const char *)RDEscapeString(q->value(2).toString()),
		(const char *)RDEscapeString(add_name_edit->text()));
      q1=new RDSqlQuery(sql);
      delete q1;
      sql=QString().
	sprintf("select ID from ENCODERS \
                where (NAME=\"%s\")&&(STATION_NAME=\"%s\")",
		(const char *)RDEscapeString(q->value(1).toString()),
		(const char *)RDEscapeString(add_name_edit->text()));
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	CloneEncoderValues("CHANNELS",q->value(0).toInt(),q1->value(0).toInt());
	CloneEncoderValues("SAMPLERATES",q->value(0).toInt(),
			   q1->value(0).toInt());
	CloneEncoderValues("BITRATES",q->value(0).toInt(),q1->value(0).toInt());
      }
      delete q1;
    }
    delete q;

    //
    // Clone Hotkeys
    //
    sql=QString().sprintf("select MODULE_NAME,KEY_ID,KEY_VALUE,KEY_LABEL\
                           from RDHOTKEYS where STATION_NAME=\"%s\"",
		(const char *)RDEscapeString(add_exemplar_box->currentText()));
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString().sprintf("insert into RDHOTKEYS set \
                             MODULE_NAME=\"%s\",\
                             KEY_ID=%d,\
                             KEY_VALUE=\"%s\",\
                             KEY_LABEL=\"%s\",\
                             STATION_NAME=\"%s\"",
			    (const char *)RDEscapeString(q->value(0).
							 toString()),
			    q->value(1).toInt(),
			    (const char *)RDEscapeString(q->value(2).
							 toString()),
			    (const char *)RDEscapeString(q->value(3).
							 toString()),
			    (const char *)RDEscapeString(add_name_edit->
							 text()));
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }
  *add_name=add_name_edit->text();

  EditStation *station=new EditStation(add_name_edit->text(),this,"station");
  int res=station->exec();
  delete station;

  done(res);
}


void AddStation::cancelData()
{
  done(-1);
}


void AddStation::CloneEncoderValues(const QString &paramname,
				    int src_id,int dest_id)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select %s from ENCODER_%s where ENCODER_ID=%d",
			(const char *)RDEscapeString(paramname),
			(const char *)RDEscapeString(paramname),
			src_id);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("insert into ENCODER_%s set %s=%d,ENCODER_ID=%d",
			  (const char *)RDEscapeString(paramname),
			  (const char *)RDEscapeString(paramname),
			  q->value(0).toInt(),
			  dest_id);
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}
