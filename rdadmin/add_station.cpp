// add_station.cpp
//
// Add a Rivendell Workstation
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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

AddStation::AddStation(QString *stationname,QWidget *parent)
  : QDialog(parent,"",true)
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
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Station Name
  //
  add_name_edit=new QLineEdit(this);
  add_name_edit->setGeometry(130,10,sizeHint().width()-140,19);
  add_name_edit->setMaxLength(64);
  add_name_edit->setValidator(validator);
  QLabel *label=new QLabel(add_name_edit,tr("New &Host Name:"),this);
  label->setGeometry(10,10,115,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Exemplar
  //
  add_exemplar_box=new QComboBox(this);
  add_exemplar_box->setGeometry(130,35,sizeHint().width()-140,19);
  label=new QLabel(add_exemplar_box,tr("Base Host On:"),this);
  label->setGeometry(10,35,115,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
			 80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
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
    sql=QString("insert into STATIONS set ")+
      "NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
      "DESCRIPTION=\""+tr("Host")+" "+
      RDEscapeString(add_name_edit->text())+"\","+
      "USER_NAME=\"user\","+
      "DEFAULT_NAME=\"user\"";
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
      sql=QString("insert into SERVICE_PERMS set ")+
	"SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
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
    for(unsigned i=0;i<RD_CUT_EVENT_ID_QUAN;i++) {
      for(unsigned j=0;j<MAX_DECKS;j++) {
	sql=QString("insert into DECK_EVENTS set ")+
	  "STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	  QString().sprintf("CHANNEL=%u,",j+129)+
	  QString().sprintf("NUMBER=%u",i+1);
	q=new RDSqlQuery(sql);
	delete q;
      }
    }
  }
  else {    // Use Specified Config

    sql=QString("select ")+
      "DEFAULT_NAME,"+        // 00
      "STARTUP_CART,"+        // 01
      "TIME_OFFSET,"+         // 02
      "HEARTBEAT_CART,"+      // 03
      "HEARTBEAT_INTERVAL,"+  // 04
      "EDITOR_PATH,"+         // 05
      "FILTER_MODE,"+         // 06
      "SYSTEM_MAINT,"+        // 07
      "HTTP_STATION,"+        // 08
      "CAE_STATION "+         // 09
      "from STATIONS where "+
      "NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into STATIONS set ")+
	"NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	"DESCRIPTION=\""+
	tr("Host")+" "+RDEscapeString(add_name_edit->text())+"\","+
	"USER_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"DEFAULT_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	QString().sprintf("STARTUP_CART=%u,",q->value(1).toUInt())+
	QString().sprintf("TIME_OFFSET=%d,",q->value(2).toInt())+
	QString().sprintf("HEARTBEAT_CART=%u,",q->value(3).toUInt())+
	QString().sprintf("HEARTBEAT_INTERVAL=%u,",q->value(4).toUInt())+
	"EDITOR_PATH=\""+RDEscapeString(q->value(5).toString())+"\","+
	QString().sprintf("FILTER_MODE=%d,",q->value(6).toInt())+
	"SYSTEM_MAINT=\""+RDEscapeString(q->value(7).toString())+"\","+
	"HTTP_STATION=\""+RDEscapeString(q->value(8).toString())+"\","+
	"CAE_STATION=\""+RDEscapeString(q->value(9).toString())+"\"";
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
    sql=QString("select SERVICE_NAME from SERVICE_PERMS where ")+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into SERVICE_PERMS set ")+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDLibrary Config
    //
    sql=QString("select ")+
      "INPUT_CARD,"+           // 00
      "INPUT_PORT,"+           // 01
      "INPUT_TYPE,"+           // 02
      "OUTPUT_CARD,"+          // 03
      "OUTPUT_PORT,"+          // 04
      "VOX_THRESHOLD,"+        // 05
      "TRIM_THRESHOLD,"+       // 06
      "DEFAULT_FORMAT,"+       // 07
      "DEFAULT_CHANNELS,"+     // 08
      "DEFAULT_SAMPRATE,"+     // 09
      "DEFAULT_LAYER,"+        // 10
      "DEFAULT_BITRATE,"+      // 11
      "DEFAULT_RECORD_MODE,"+  // 12
      "DEFAULT_TRIM_STATE,"+   // 13
      "MAXLENGTH,"+            // 14
      "TAIL_PREROLL,"+         // 15
      "RIPPER_DEVICE,"+        // 16
      "PARANOIA_LEVEL,"+       // 17
      "RIPPER_LEVEL,"+         // 18
      "CDDB_SERVER "+          // 19
      "from RDLIBRARY where "+
      "STATION=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into RDLIBRARY set ")+
	QString().sprintf("INPUT_CARD=%d,",q->value(0).toInt())+
	QString().sprintf("INPUT_PORT=%d,",q->value(1).toInt())+
	QString().sprintf("INPUT_TYPE=%d,",q->value(2).toInt())+
	QString().sprintf("OUTPUT_CARD=%d,",q->value(3).toInt())+
	QString().sprintf("OUTPUT_PORT=%d,",q->value(4).toInt())+
	QString().sprintf("VOX_THRESHOLD=%d,",q->value(5).toInt())+
	QString().sprintf("TRIM_THRESHOLD=%d,",q->value(6).toInt())+
	QString().sprintf("DEFAULT_FORMAT=%u,",q->value(7).toUInt())+
	QString().sprintf("DEFAULT_CHANNELS=%u,",q->value(8).toUInt())+
	QString().sprintf("DEFAULT_SAMPRATE=%u,",q->value(9).toUInt())+
	QString().sprintf("DEFAULT_LAYER=%u,",q->value(10).toUInt())+
	QString().sprintf("DEFAULT_BITRATE=%u,",q->value(11).toUInt())+
	QString().sprintf("DEFAULT_RECORD_MODE=%u,",q->value(12).toUInt())+
	"DEFAULT_TRIM_STATE=\""+RDEscapeString(q->value(13).toString())+"\","+
	QString().sprintf("MAXLENGTH=%d,",q->value(14).toInt())+
	QString().sprintf("TAIL_PREROLL=%u,",q->value(15).toUInt())+
	"RIPPER_DEVICE=\""+RDEscapeString(q->value(16).toString())+"\","+
	QString().sprintf("PARANOIA_LEVEL=%d,",q->value(17).toInt())+
	QString().sprintf("RIPPER_LEVEL=%d,",q->value(18).toInt())+
	"CDDB_SERVER=\""+RDEscapeString(q->value(19).toString())+"\","+
	"STATION=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDLogEdit Config
    //
    sql=QString("select ")+
      "INPUT_CARD,"+          // 00
      "INPUT_PORT,"+          // 01
      "OUTPUT_CARD,"+         // 02
      "OUTPUT_PORT,"+         // 03
      "FORMAT,"+              // 04
      "DEFAULT_CHANNELS,"+    // 05
      "SAMPRATE,"+            // 06
      "LAYER,"+               // 07
      "BITRATE,"+             // 08
      "MAXLENGTH,"+           // 09
      "TAIL_PREROLL,"+        // 10
      "START_CART,"+          // 11
      "END_CART,"+            // 12
      "REC_START_CART,"+      // 13
      "REC_END_CART,"+        // 14
      "TRIM_THRESHOLD,"+      // 15
      "RIPPER_LEVEL,"+        // 16
      "DEFAULT_TRANS_TYPE "+  // 17
      "from RDLOGEDIT where "+
      "STATION=\""+RDEscapeString(add_exemplar_box->currentText())+"\"",
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into RDLOGEDIT set ")+
	QString().sprintf("INPUT_CARD=%d,",q->value(0).toInt())+
	QString().sprintf("INPUT_PORT=%d,",q->value(1).toInt())+
	QString().sprintf("OUTPUT_CARD=%d,",q->value(2).toInt())+
	QString().sprintf("OUTPUT_PORT=%d,",q->value(3).toInt())+
	QString().sprintf("FORMAT=%u,",q->value(4).toUInt())+
	QString().sprintf("DEFAULT_CHANNELS=%u,",q->value(5).toUInt())+
	QString().sprintf("SAMPRATE=%u,",q->value(6).toUInt())+
	QString().sprintf("LAYER=%u,",q->value(7).toUInt())+
	QString().sprintf("BITRATE=%u,",q->value(8).toUInt())+
	QString().sprintf("MAXLENGTH=%d,",q->value(9).toInt())+
	QString().sprintf("TAIL_PREROLL=%u,",q->value(10).toUInt())+
	"STATION=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("START_CART=%u,",q->value(11).toUInt())+
	QString().sprintf("END_CART=%d,",q->value(12).toInt())+
	QString().sprintf("REC_START_CART=%u,",q->value(13).toUInt())+
	QString().sprintf("REC_END_CART=%u,",q->value(14).toUInt())+
	QString().sprintf("TRIM_THRESHOLD=%d,",q->value(15).toInt())+
	QString().sprintf("RIPPER_LEVEL=%d,",q->value(16).toInt())+
	QString().sprintf("DEFAULT_TRANS_TYPE=%d",q->value(17).toInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDCatch Config
    //
    sql=QString("select ")+
      "CHANNEL,"+             // 00
      "CARD_NUMBER,"+         // 01
      "PORT_NUMBER,"+         // 02
      "MON_PORT_NUMBER,"+     // 03
      "PORT_TYPE,"+           // 04
      "DEFAULT_FORMAT,"+      // 05
      "DEFAULT_CHANNELS,"+    // 06
      "DEFAULT_SAMPRATE,"+    // 07
      "DEFAULT_BITRATE,"+     // 08
      "DEFAULT_THRESHOLD,"+   // 09
      "DEFAULT_MONITOR_ON "+  // 10
      "from DECKS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into DECKS set ")+
	QString().sprintf("CHANNEL=%u,",q->value(0).toUInt())+
	QString().sprintf("CARD_NUMBER=%d,",q->value(1).toInt())+
	QString().sprintf("PORT_NUMBER=%d,",q->value(2).toInt())+
	QString().sprintf("MON_PORT_NUMBER=%d,",q->value(3).toInt())+
	"PORT_TYPE=\""+RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("DEFAULT_FORMAT=%d,",q->value(5).toInt())+
	QString().sprintf("DEFAULT_CHANNELS=%d,",q->value(6).toInt())+
	QString().sprintf("DEFAULT_SAMPRATE=%d,",q->value(7).toInt())+
	QString().sprintf("DEFAULT_BITRATE=%d,",q->value(8).toInt())+
	QString().sprintf("DEFAULT_THRESHOLD=%d,",q->value(9).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	"DEFAULT_MONITOR_ON=\""+RDEscapeString(q->value(10).toString())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
 
    //
    // Clone Deck Events
    //
    sql=QString("select ")+
      "CHANNEL,"+
      "NUMBER,"+
      "CART_NUMBER "+
      "from DECK_EVENTS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into DECK_EVENTS set ")+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("CHANNEL=%u,",q->value(0).toUInt())+
	QString().sprintf("NUMBER=%d,",q->value(1).toInt())+
	QString().sprintf("CART_NUMBER=%u",q->value(2).toUInt());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone RDAirPlay Config
    //
    sql=QString("select ")+
      "SEGUE_LENGTH,"+           // 00
      "TRANS_LENGTH,"+           // 01
      "OP_MODE,"+                // 02
      "START_MODE,"+             // 03
      "PIE_COUNT_LENGTH,"+       // 04
      "PIE_COUNT_ENDPOINT,"+     // 05
      "CHECK_TIMESYNC,"+         // 06
      "STATION_PANELS,"+         // 07
      "USER_PANELS,"+            // 08
      "SHOW_AUX_1,"+             // 09
      "SHOW_AUX_2,"+             // 10
      "CLEAR_FILTER,"+           // 11
      "DEFAULT_TRANS_TYPE,"+     // 12
      "BAR_ACTION,"+             // 13
      "FLASH_PANEL,"+            // 14
      "PAUSE_ENABLED,"+          // 15
      "UDP_ADDR0,"+              // 16
      "UDP_PORT0,"+              // 17
      "UDP_STRING0,"+            // 18
      "UDP_ADDR1,"+              // 19
      "UDP_PORT1,"+              // 20
      "UDP_STRING1,"+            // 21
      "UDP_ADDR2,"+              // 22
      "UDP_PORT2,"+              // 23
      "UDP_STRING2,"+            // 24
      "DEFAULT_SERVICE,"+        // 25
      "LOG_RML0,"+               // 26
      "LOG_RML1,"+               // 27
      "LOG_RML2,"+               // 28
      "BUTTON_LABEL_TEMPLATE,"+  // 29
      "EXIT_PASSWORD "+          // 30
      "from RDAIRPLAY where "+
      "(STATION=\""+RDEscapeString(add_exemplar_box->currentText())+"\")";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into RDAIRPLAY set ")+
	QString().sprintf("SEGUE_LENGTH=%d,",q->value(0).toInt())+
	QString().sprintf("TRANS_LENGTH=%d,",q->value(1).toInt())+
	QString().sprintf("OP_MODE=%d,",q->value(2).toInt())+
	QString().sprintf("START_MODE=%d,",q->value(3).toInt())+
	QString().sprintf("PIE_COUNT_LENGTH=%d,",q->value(4).toInt())+
	QString().sprintf("PIE_COUNT_ENDPOINT=%d,",q->value(5).toInt())+
	"CHECK_TIMESYNC=\""+RDEscapeString(q->value(6).toString())+"\","+
	QString().sprintf("STATION_PANELS=%d,",q->value(7).toInt())+
	QString().sprintf("USER_PANELS=%d,",q->value(8).toInt())+
	"SHOW_AUX_1=\""+RDEscapeString(q->value(9).toString())+"\","+
	"SHOW_AUX_2=\""+RDEscapeString(q->value(10).toString())+"\","+
	"CLEAR_FILTER=\""+RDEscapeString(q->value(11).toString())+"\","+
	QString().sprintf("DEFAULT_TRANS_TYPE=%u,",q->value(12).toUInt())+
	QString().sprintf("BAR_ACTION=%u,",q->value(13).toUInt())+
	"FLASH_PANEL=\""+RDEscapeString(q->value(14).toString())+"\","+
	"PAUSE_ENABLED=\""+RDEscapeString(q->value(15).toString())+"\","+
	"UDP_ADDR0=\""+RDEscapeString(q->value(16).toString())+"\","+
	QString().sprintf("UDP_PORT0=%u,",q->value(17).toUInt())+
	"UDP_STRING0=\""+RDEscapeString(q->value(18).toString())+"\","+
	"UDP_ADDR1=\""+RDEscapeString(q->value(19).toString())+"\","+
	QString().sprintf("UDP_PORT1=%u,",q->value(20).toUInt())+
	"UDP_STRING1=\""+RDEscapeString(q->value(21).toString())+"\","+
	"UDP_ADDR2=\""+RDEscapeString(q->value(22).toString())+"\","+
	QString().sprintf("UDP_PORT2=%u,",q->value(23).toUInt())+
	"UDP_STRING2=\""+RDEscapeString(q->value(24).toString())+"\","+
	"STATION=\""+RDEscapeString(add_name_edit->text())+"\","+
	"DEFAULT_SERVICE=\""+RDEscapeString(q->value(25).toString())+"\","+
	"LOG_RML0=\""+RDEscapeString(q->value(26).toString())+"\",0"+
	"LOG_RML1=\""+RDEscapeString(q->value(27).toString())+"\","+
	"LOG_RML2=\""+RDEscapeString(q->value(28).toString())+"\","+
	"BUTTON_LABEL_TEMPLATE=\""+
	RDEscapeString(q->value(29).toString())+"\","+
	"EXIT_PASSWORD=\""+RDEscapeString(q->value(30).toString())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql=QString("select ")+
      "INSTANCE,"+          // 00
      "CARD,"+              // 01
      "PORT,"+              // 02
      "START_RML,"+         // 03
      "STOP_RML,"+          // 04
      "START_GPI_MATRIX,"+  // 05
      "START_GPI_LINE,"+    // 06
      "START_GPO_MATRIX,"+  // 07
      "START_GPO_LINE,"+    // 08
      "STOP_GPI_MATRIX,"+   // 09
      "STOP_GPI_LINE,"+     // 10
      "STOP_GPO_MATRIX,"+   // 11
      "STOP_GPO_LINE "+     // 12
      "from RDAIRPLAY_CHANNELS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+
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
    sql=QString("select ")+
      "STATION_PANELS,"+
      "USER_PANELS,"+
      "CLEAR_FILTER,"+
      "FLASH_PANEL,"+
      "DEFAULT_SERVICE,"+
      "BUTTON_LABEL_TEMPLATE "+
      "from RDPANEL where "+
      "STATION=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("insert into RDPANEL set ")+
	QString().sprintf("STATION_PANELS=%d,",q->value(0).toInt())+
	QString().sprintf("USER_PANELS=%d,",q->value(1).toInt())+
	"CLEAR_FILTER=\""+RDEscapeString(q->value(2).toString())+"\","+
	"FLASH_PANEL=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STATION=\""+RDEscapeString(add_name_edit->text())+"\","+
	"DEFAULT_SERVICE=\""+RDEscapeString(q->value(4).toString())+"\","+
	"BUTTON_LABEL_TEMPLATE=\""+RDEscapeString(q->value(5).toString())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql=QString("select ")+
      "INSTANCE,"+          // 00
      "CARD,"+              // 01
      "PORT,"+              // 02
      "START_RML,"+         // 03
      "STOP_RML,"+          // 04
      "START_GPI_MATRIX,"+  // 05
      "START_GPI_LINE,"+    // 06
      "START_GPO_MATRIX,"+  // 07
      "START_GPO_LINE,"+    // 08
      "STOP_GPI_MATRIX,"+   // 09
      "STOP_GPI_LINE,"+     // 10
      "STOP_GPO_MATRIX,"+   // 11
      "STOP_GPO_LINE "+     // 12
      "from RDPANEL_CHANNELS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
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
    sql=QString("select ")+
      "CARD_NUMBER,"+
      "CLOCK_SOURCE,";
    for(int i=0;i<RD_MAX_PORTS;i++) {
      sql+=QString().sprintf("INPUT_%d_LEVEL,INPUT_%d_MODE,INPUT_%d_TYPE,\
                              OUTPUT_%d_LEVEL,",
			     i,i,i,i);
    }
    sql=sql.left(sql.length()-1);
    sql+=QString(" from AUDIO_PORTS where ")+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into AUDIO_PORTS set ")+
	QString().sprintf("CARD_NUMBER=%u,",q->value(0).toUInt())+
	QString().sprintf("CLOCK_SOURCE=%d,",q->value(1).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\",";
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
    sql=QString("select ")+
      "PORT_ID,"+      // 00
      "ACTIVE,"+       // 01
      "PORT,"+         // 02
      "BAUD_RATE,"+    // 03
      "DATA_BITS,"+    // 04
      "STOP_BITS,"+    // 05
      "PARITY,"+       // 06
      "TERMINATION "+  // 07
      "from TTYS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into TTYS set ")+
	QString().sprintf("PORT_ID=%u,",q->value(0).toUInt())+
	"ACTIVE=\""+RDEscapeString(q->value(1).toString())+"\","+
	"PORT=\""+RDEscapeString(q->value(2).toString())+"\","+
	QString().sprintf("BAUD_RATE=%d,",q->value(3).toInt())+
	QString().sprintf("DATA_BITS=%d,",q->value(4).toInt())+
	QString().sprintf("STOP_BITS=%d,",q->value(5).toInt())+
	QString().sprintf("PARITY=%d,",q->value(6).toInt())+
	QString().sprintf("TERMINATION=%d,",q->value(7).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Matrices
    //
    sql=QString("select ")+
      "NAME,"+          // 00
      "MATRIX,"+        // 01
      "TYPE,"+          // 02
      "PORT_TYPE,"+     // 03
      "CARD,"+          // 04
      "PORT,"+          // 05
      "IP_ADDRESS,"+    // 06
      "IP_PORT,"+       // 07
      "USERNAME,"+      // 08
      "PASSWORD,"+      // 09
      "GPIO_DEVICE,"+   // 10
      "INPUTS,"+        // 11
      "OUTPUTS,"+       // 12
      "GPIS,"+          // 13
      "GPOS,"+          // 14
      "DISPLAYS,"+      // 15
      "FADERS,"+        // 16
      "PORT_TYPE_2,"+   // 17
      "PORT_2,"+        // 18
      "IP_ADDRESS_2,"+  // 19
      "IP_PORT_2,"+     // 20
      "USERNAME_2,"+    // 21
      "PASSWORD_2 "+    // 22
      "from MATRICES where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into MATRICES set ")+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	QString().sprintf("MATRIX=%d,",q->value(1).toInt())+
	QString().sprintf("TYPE=%d,",q->value(2).toInt())+
	QString().sprintf("PORT_TYPE=%d,",q->value(3).toInt())+
	QString().sprintf("CARD=%d,",q->value(4).toInt())+
	QString().sprintf("PORT=%d,",q->value(5).toInt())+
	"IP_ADDRESS=\""+RDEscapeString(q->value(6).toString())+"\","+
	QString().sprintf("IP_PORT=%d,",q->value(7).toInt())+
	"USERNAME=\""+RDEscapeString(q->value(8).toString())+"\","+
	"PASSWORD=\""+RDEscapeString(q->value(9).toString())+"\","+
	"GPIO_DEVICE=\""+RDEscapeString(q->value(10).toString())+"\","+
	QString().sprintf("INPUTS=%d,",q->value(11).toInt())+
	QString().sprintf("OUTPUTS=%d,",q->value(12).toInt())+
	QString().sprintf("GPIS=%d,",q->value(13).toInt())+
	QString().sprintf("GPOS=%d,",q->value(14).toInt())+
	QString().sprintf("DISPLAYS=%d,",q->value(15).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\","+
	QString().sprintf("FADERS=%d,",q->value(16).toInt())+
	QString().sprintf("PORT_TYPE_2=%d,",q->value(17).toInt())+
	QString().sprintf("PORT_2=%d,",q->value(18).toInt())+
	"IP_ADDRESS_2=\""+RDEscapeString(q->value(19).toString())+"\","+
	QString().sprintf("IP_PORT_2=%d,",q->value(20).toInt())+
	"USERNAME_2=\""+RDEscapeString(q->value(21).toString())+"\","+
	"PASSWORD_2=\""+RDEscapeString(q->value(22).toString())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Matrix Inputs
    //
    sql=QString("select ")+
      "MATRIX,"+
      "NUMBER,"+
      "NAME,"+
      "FEED_NAME,"+
      "CHANNEL_MODE,"
      "ENGINE_NUM,"+
      "DEVICE_NUM "+
      "from INPUTS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into INPUTS set ")+
	QString().sprintf("MATRIX=%d,",q->value(0).toInt())+
	QString().sprintf("NUMBER=%d,",q->value(1).toInt())+
	"NAME=\""+RDEscapeString(q->value(2).toString())+"\","+
	"FEED_NAME=\""+RDEscapeString(q->value(3).toString())+"\","+
	QString().sprintf("CHANNEL_MODE=%d,",q->value(4).toInt())+
	QString().sprintf("ENGINE_NUM=%d,",q->value(5).toInt())+
	QString().sprintf("DEVICE_NUM=%d,",q->value(6).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Matrix Outputs
    //
    sql=QString("select ")+
      "MATRIX,"+
      "NUMBER,"+
      "NAME,"+
      "ENGINE_NUM,"+
      "DEVICE_NUM "+
      "from OUTPUTS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into OUTPUTS set ")+
	QString().sprintf("MATRIX=%d,",q->value(0).toInt())+
	QString().sprintf("NUMBER=%d,",q->value(1).toInt())+
	"NAME=\""+RDEscapeString(q->value(2).toString())+"\","+
	QString().sprintf("ENGINE_NUM=%d,",q->value(3).toInt())+
	QString().sprintf("DEVICE_NUM=%d,",q->value(4).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone GPIs
    //
    sql=QString("select ")+
      "MATRIX,"+
      "NUMBER,"+
      "MACRO_CART "+
      "from GPIS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into GPIS set ")+
	QString().sprintf("MATRIX=%d,",q->value(0).toInt())+
	QString().sprintf("NUMBER=%d,",q->value(1).toInt())+
	QString().sprintf("MACRO_CART=%d,",q->value(2).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Close vGuest Settings
    //
    sql=QString("select ")+
      "MATRIX_NUM,"+   // 00
      "VGUEST_TYPE,"+  // 01
      "NUMBER,"+       // 02
      "ENGINE_NUM,"+   // 03
      "DEVICE_NUM,"+   // 04
      "SURFACE_NUM,"+  // 05
      "RELAY_NUM,"+    // 06
      "BUSS_NUM "+     // 07
      "from VGUEST_RESOURCES where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into VGUEST_RESOURCES set ")+
	QString().sprintf("MATRIX_NUM=%d,",q->value(0).toInt())+
	QString().sprintf("VGUEST_TYPE=%d,",q->value(1).toInt())+
	QString().sprintf("NUMBER=%d,",q->value(2).toInt())+
	QString().sprintf("ENGINE_NUM=%d,",q->value(3).toInt())+
	QString().sprintf("DEVICE_NUM=%d,",q->value(4).toInt())+
	QString().sprintf("SURFACE_NUM=%d,",q->value(5).toInt())+
	QString().sprintf("RELAY_NUM=%d,",q->value(6).toInt())+
	QString().sprintf("BUSS_NUM=%d,",q->value(7).toInt())+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Host Variables
    //
    sql=QString("select ")+
      "NAME,"+
      "VARVALUE,"+
      "REMARK "+
      "from HOSTVARS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into HOSTVARS set ")+
	"NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	"VARVALUE=\""+RDEscapeString(q->value(1).toString())+"\","+
	"REMARK=\""+RDEscapeString(q->value(2).toString())+"\","+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone System Panels
    //
    sql=QString("select ")+
      "PANEL_NO,"+
      "ROW_NO,"+
      "COLUMN_NO,"+
      "LABEL,CART,"+
      "DEFAULT_COLOR "+
      "from PANELS where "+
      QString().sprintf("(TYPE=%d)&&",RDAirPlayConf::StationPanel)+
      "(OWNER=\""+RDEscapeString(add_exemplar_box->currentText())+"\")";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into PANELS set ")+
	QString().sprintf("PANEL_NO=%d,",q->value(0).toInt())+
	QString().sprintf("ROW_NO=%d,",q->value(1).toInt())+
	QString().sprintf("COLUMN_NO=%d,",q->value(2).toInt())+
	"LABEL=\""+RDEscapeString(q->value(3).toString())+"\","+
	QString().sprintf("CART=%u,",q->value(4).toUInt())+
	"DEFAULT_COLOR=\""+RDEscapeString(q->value(5).toString())+"\","+
	QString().sprintf("TYPE=%d,",q->value(6).toInt())+
	"OWNER=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    sql=QString("select ")+
      "PANEL_NO,"+
      "ROW_NO,"+
      "COLUMN_NO,"+
      "LABEL,"+
      "CART,"+
      "DEFAULT_COLOR "+
      "from EXTENDED_PANELS where "+
      QString().sprintf("(TYPE=%d)&&",RDAirPlayConf::StationPanel)+
      "(OWNER=\""+RDEscapeString(add_exemplar_box->currentText())+"\")";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into EXTENDED_PANELS set ")+
	QString().sprintf("PANEL_NO=%d,",q->value(0).toInt())+
	QString().sprintf("ROW_NO=%d,",q->value(1).toInt())+
	QString().sprintf("COLUMN_NO=%d,",q->value(2).toInt())+
	"LABEL=\""+RDEscapeString(q->value(3).toString())+"\","+
	QString().sprintf("CART=%u,",q->value(4).toInt())+
	"DEFAULT_COLOR=\""+RDEscapeString(q->value(5).toString())+"\","+
	QString().sprintf("TYPE=%d,",RDAirPlayConf::StationPanel)+
	"OWNER=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;

    //
    // Clone Encoders
    //
    /*
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
    */

    //
    // Clone Hotkeys
    //
    sql=QString("select ")+
      "MODULE_NAME,"+
      "KEY_ID,"+
      "KEY_VALUE,"+
      "KEY_LABEL "+
      "from RDHOTKEYS where "+
      "STATION_NAME=\""+RDEscapeString(add_exemplar_box->currentText())+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into RDHOTKEYS set ")+
	"MODULE_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	QString().sprintf("KEY_ID=%d,",q->value(1).toInt())+
	"KEY_VALUE=\""+RDEscapeString(q->value(2).toString())+"\","+
	"KEY_LABEL=\""+RDEscapeString(q->value(3).toString())+"\","+
	"STATION_NAME=\""+RDEscapeString(add_name_edit->text())+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }
  *add_name=add_name_edit->text();

  EditStation *station=new EditStation(add_name_edit->text(),this);
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
  /*
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("select ")+paramname+" from ENCODER_%s where ENCODER_ID=%d",
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
  */
}
