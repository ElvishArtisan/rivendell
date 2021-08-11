// edit_report.cpp
//
// Edit a Rivendell Report
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "autofill_carts.h"
#include "edit_report.h"
#include "edit_svc_perms.h"
#include "globals.h"
#include "test_import.h"

EditReport::EditReport(QString rptname,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  bool ok=false;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  edit_report=new RDReport(rptname,rda->station(),rda->config());
  setWindowTitle("RDAdmin - "+tr("Edit Report")+" "+rptname);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Report Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setGeometry(200,10,sizeHint().width()-210,19);
  edit_description_edit->setMaxLength(64);
  QLabel *label=new QLabel(tr("Report Description:"),this);
  label->setGeometry(10,10,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Export Filter Type
  //
  edit_filter_box=new QComboBox(this);
  edit_filter_box->setGeometry(200,31,sizeHint().width()-210,19);
  for(int i=0;i<(int)RDReport::LastFilter;i++) {
    edit_filter_box->
      insertItem(i,RDReport::filterText((RDReport::ExportFilter)i));
  }
  label=new QLabel(tr("Export Filter:"),this);
  label->setGeometry(10,31,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station ID
  //
  edit_stationid_edit=new QLineEdit(this);
  edit_stationid_edit->setGeometry(200,52,180,19);
  edit_stationid_edit->setMaxLength(16);
  edit_stationid_edit->setValidator(validator);
  label=new QLabel(tr("Station ID:"),this);
  label->setGeometry(10,52,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart Number Parameters
  //
  label=new QLabel(tr("Cart Number Parameters:"),this);
  label->setGeometry(10,73,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  
  edit_cartzeros_box=new QCheckBox(this);
  edit_cartzeros_box->setGeometry(200,75,15,15);
  connect(edit_cartzeros_box,SIGNAL(toggled(bool)),
	  this,SLOT(leadingZerosToggled(bool)));
  label=new QLabel(tr("Use Leading Zeros"),this);
  label->setGeometry(217,73,120,19);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  edit_cartdigits_spin=new QSpinBox(this);
  edit_cartdigits_spin->setGeometry(380,73,40,19);
  edit_cartdigits_spin->setRange(1,6);
  edit_cartdigits_label=new QLabel(tr("Digits:"),this);
  edit_cartdigits_label->setGeometry(330,73,45,19);
  edit_cartdigits_label->setFont(subLabelFont());
  edit_cartdigits_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station Type
  //
  edit_stationtype_box=new QComboBox(this);
  edit_stationtype_box->setGeometry(200,94,70,19);
  for(int i=0;i<RDReport::TypeLast;i++) {
    edit_stationtype_box->
      insertItem(edit_stationtype_box->count(),
		 RDReport::stationTypeText((RDReport::StationType)i));
  }
  label=new QLabel(tr("Station Type:"),this);
  label->setGeometry(10,94,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Lines Per Page
  //
  edit_linesperpage_spin=new QSpinBox(this);
  edit_linesperpage_spin->setGeometry(360,94,50,19);
  edit_linesperpage_spin->setRange(10,200);
  label=new QLabel(tr("Lines per Page:"),this);
  label->setGeometry(255,94,100,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_linesperpage_spin->hide();
  label->hide();

  //
  // Service Name
  //
  edit_servicename_edit=new QLineEdit(this);
  edit_servicename_edit->setGeometry(200,115,sizeHint().width()-210,19);
  edit_servicename_edit->setMaxLength(64);
  label=new QLabel(tr("Service Name:"),this);
  label->setGeometry(10,115,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Station Format
  //
  edit_stationformat_edit=new QLineEdit(this);
  edit_stationformat_edit->setGeometry(200,136,sizeHint().width()-210,19);
  edit_stationformat_edit->setMaxLength(64);
  label=new QLabel(tr("Station Format:"),this);
  label->setGeometry(10,136,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Linux Export Path
  //
  edit_path_edit=new QLineEdit(this);
  edit_path_edit->setGeometry(200,157,sizeHint().width()-210,19);
  edit_path_edit->setMaxLength(255);
  edit_path_edit->setValidator(validator);
  label=new QLabel(tr("Export Path:"),this);
  label->setGeometry(10,157,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Linux Post Export Command
  //
  edit_postexport_cmd_edit=new QLineEdit(this);
  edit_postexport_cmd_edit->setGeometry(200,178,sizeHint().width()-210,19);
  edit_postexport_cmd_edit->setValidator(validator);
  label=new QLabel(tr("Post Export Cmd:"),this);
  label->setGeometry(10,178,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Type Selectors
  //
  label=new QLabel(tr("Export Event Types:"),this);
  label->setGeometry(10,200,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_traffic_box=new QCheckBox(this);
  edit_traffic_box->setGeometry(200,202,15,15);
  edit_traffic_label=new QLabel(tr("Traffic"),this);
  edit_traffic_label->setGeometry(217,201,80,19);
  edit_traffic_label->setFont(subLabelFont());
  edit_traffic_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  
  edit_music_box=new QCheckBox(this);
  edit_music_box->setGeometry(300,203,15,15);
  edit_music_label=new QLabel(tr("Music"),this);
  edit_music_label->setGeometry(317,201,80,19);
  edit_music_label->setFont(subLabelFont());
  edit_music_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  edit_generic_box=new QCheckBox(this);
  edit_generic_box->setGeometry(400,203,15,15);
  label=new QLabel(tr("All"),this);
  label->setGeometry(417,201,80,19);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(edit_generic_box,SIGNAL(toggled(bool)),
	  this,SLOT(genericEventsToggledData(bool)));

  //
  // Force Event Source Selectors
  //
  label=new QLabel(tr("Export Events From:"),this);
  label->setGeometry(10,222,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_forcetraffic_box=new QCheckBox(this);
  edit_forcetraffic_box->setGeometry(200,224,15,15);
  label=new QLabel(tr("Traffic Log"),this);
  label->setGeometry(217,222,80,19);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  
  edit_forcemusic_box=new QCheckBox(this);
  edit_forcemusic_box->setGeometry(300,224,15,15);
  label=new QLabel(tr("Music Log"),this);
  label->setGeometry(317,222,80,19);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Include Only On Air Events
  //
  edit_onairflag_box=new QComboBox(this);
  edit_onairflag_box->setGeometry(200,243,60,19);
  edit_onairflag_box->insertItem(0,tr("No"));
  edit_onairflag_box->insertItem(1,tr("Yes"));
  label=new QLabel(tr("Include Only OnAir Events:"),this);
  label->setGeometry(10,243,185,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Daypart Filter
  //
  edit_daypart_check=new QCheckBox(this);
  edit_daypart_check->setGeometry(60,273,15,15);
  edit_daypart_label=
    new QLabel(tr("Filter by Daypart"),this);
  edit_daypart_label->
    setGeometry(edit_daypart_check->geometry().x()+20,270,155,19);
  edit_daypart_label->setFont(labelFont());
  edit_daypart_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  edit_starttime_edit=new RDTimeEdit(this);
  edit_starttime_edit->setGeometry(110,294,120,20);
  edit_starttime_label=new QLabel(tr("Start Time:"),this);
  edit_starttime_label->setGeometry(25,294,80,20);
  edit_starttime_label->setFont(labelFont());
  edit_starttime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  edit_endtime_edit=new RDTimeEdit(this);
  edit_endtime_edit->setGeometry(335,294,120,20);
  edit_endtime_label=new QLabel(tr("End Time:"),this);
  edit_endtime_label->setGeometry(250,294,80,20);
  edit_endtime_label->setFont(labelFont());
  edit_endtime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  connect(edit_daypart_check,SIGNAL(toggled(bool)),
  	  edit_starttime_label,SLOT(setEnabled(bool)));
  connect(edit_daypart_check,SIGNAL(toggled(bool)),
  	  edit_starttime_edit,SLOT(setEnabled(bool)));
  connect(edit_daypart_check,SIGNAL(toggled(bool)),
  	  edit_endtime_label,SLOT(setEnabled(bool)));
  connect(edit_daypart_check,SIGNAL(toggled(bool)),
  	  edit_endtime_edit,SLOT(setEnabled(bool)));

  //
  // Service Selector
  //
  edit_service_sel=new RDListSelector(this);
  edit_service_sel->setGeometry(75,333,380,80);
  edit_service_sel->sourceSetLabel(tr("Available Services"));
  edit_service_sel->destSetLabel(tr("Source Services"));
  
  //
  // Station Selector
  //
  edit_station_sel=new RDListSelector(this);
  edit_station_sel->setGeometry(75,437,380,80);
    /*
    setGeometry((sizeHint().width()-edit_station_sel->sizeHint().width())/2,
		437,edit_station_sel->sizeHint().width(),
		edit_station_sel->sizeHint().height());
    */
  edit_station_sel->sourceSetLabel(tr("Available Hosts"));
  edit_station_sel->destSetLabel(tr("Source Hosts"));

  //
  // Group Selector
  //
  edit_group_sel=new RDListSelector(this);
  edit_group_sel->setGeometry(75,553,380,80);
  /*
    setGeometry((sizeHint().width()-edit_group_sel->sizeHint().width())/2,
		553,edit_group_sel->sizeHint().width(),
		edit_group_sel->sizeHint().height());
  */
  edit_group_sel->sourceSetLabel(tr("Available Groups"));
  edit_group_sel->destSetLabel(tr("Allowed Groups"));

  edit_group_box=new QCheckBox(this);
  edit_group_box->setGeometry(60,531,15,15);
  label=new QLabel(tr("Filter by Groups"),this);
  label->setGeometry(edit_group_box->geometry().x()+20,532,155,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(edit_group_box,SIGNAL(toggled(bool)),
	  edit_group_sel,SLOT(setEnabled(bool)));


  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  edit_description_edit->setText(edit_report->description());
  edit_filter_box->setCurrentIndex((int)edit_report->filter());
  edit_stationid_edit->setText(edit_report->stationId());
  edit_cartzeros_box->setChecked(edit_report->useLeadingZeros());
  leadingZerosToggled(edit_cartzeros_box->isChecked());
  edit_cartdigits_spin->setValue(edit_report->cartDigits());
  edit_stationtype_box->setCurrentIndex((int)edit_report->stationType());
  edit_servicename_edit->setText(edit_report->serviceName());
  edit_stationformat_edit->setText(edit_report->stationFormat());
  edit_linesperpage_spin->setValue(edit_report->linesPerPage());
  edit_path_edit->setText(edit_report->exportPath(RDReport::Linux));
  edit_postexport_cmd_edit->
    setText(edit_report->postExportCommand(RDReport::Linux));
  edit_traffic_box->
    setChecked(edit_report->exportTypeEnabled(RDReport::Traffic));
  edit_music_box->
    setChecked(edit_report->exportTypeEnabled(RDReport::Music));
  edit_generic_box->
    setChecked(edit_report->exportTypeEnabled(RDReport::Generic));
  edit_forcetraffic_box->
    setChecked(edit_report->exportTypeForced(RDReport::Traffic));
  edit_forcemusic_box->
    setChecked(edit_report->exportTypeForced(RDReport::Music));
  genericEventsToggledData(edit_generic_box->isChecked());
  if(edit_report->filterOnairFlag()) {
    edit_onairflag_box->setCurrentIndex(1);
  }
  else {
    edit_onairflag_box->setCurrentIndex(0);
  }
  edit_starttime_edit->setTime(edit_report->startTime(&ok));
  edit_starttime_label->setDisabled(ok);
  edit_starttime_edit->setDisabled(ok);
  edit_endtime_edit->setTime(edit_report->endTime(&ok));
  edit_endtime_label->setDisabled(ok);
  edit_endtime_edit->setDisabled(ok);
  edit_daypart_check->setChecked(!ok);

  sql=QString("select `SERVICE_NAME` from `REPORT_SERVICES` where ")+
    "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_service_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select `NAME` from `SERVICES`");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(edit_service_sel->destFindItem(q->value(0).toString())==0) {
      edit_service_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;

  sql=QString("select `STATION_NAME` from `REPORT_STATIONS` where ")+
    "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_station_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select `NAME` from `STATIONS`");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(edit_station_sel->destFindItem(q->value(0).toString())==0) {
      edit_station_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;

  edit_group_box->setChecked(edit_report->filterGroups());
  edit_group_sel->setEnabled(edit_report->filterGroups());
  sql=QString("select `GROUP_NAME` from `REPORT_GROUPS` where ")+
    "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_group_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString().sprintf("select `NAME` from `GROUPS`");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(edit_group_sel->destFindItem(q->value(0).toString())==0) {
      edit_group_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;
}


EditReport::~EditReport()
{
}


QSize EditReport::sizeHint() const
{
  return QSize(530,709);
} 


QSizePolicy EditReport::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditReport::leadingZerosToggled(bool state)
{
  edit_cartdigits_label->setEnabled(state);
  edit_cartdigits_spin->setEnabled(state);
}


void EditReport::genericEventsToggledData(bool state)
{
  edit_traffic_label->setDisabled(state);
  edit_traffic_box->setDisabled(state);
  edit_music_label->setDisabled(state);
  edit_music_box->setDisabled(state);
}


void EditReport::okData()
{
  QString sql;
  RDSqlQuery *q;

  edit_report->setDescription(edit_description_edit->text());
  edit_report->
    setFilter((RDReport::ExportFilter)edit_filter_box->currentIndex());
  edit_report->setStationId(edit_stationid_edit->text());
  edit_report->setCartDigits(edit_cartdigits_spin->value());
  edit_report->setUseLeadingZeros(edit_cartzeros_box->isChecked());
  edit_report->setLinesPerPage(edit_linesperpage_spin->value());
  edit_report->
    setStationType((RDReport::StationType)edit_stationtype_box->currentIndex());
  edit_report->setServiceName(edit_servicename_edit->text());
  edit_report->setStationFormat(edit_stationformat_edit->text());
  edit_report->setExportPath(RDReport::Linux,edit_path_edit->text());
  edit_report->
    setPostExportCommand(RDReport::Linux,edit_postexport_cmd_edit->text());
  edit_report->
    setExportTypeEnabled(RDReport::Traffic,edit_traffic_box->isChecked());
  edit_report->
    setExportTypeEnabled(RDReport::Music,edit_music_box->isChecked());
  edit_report->
    setExportTypeForced(RDReport::Traffic,edit_forcetraffic_box->isChecked());
  edit_report->
    setExportTypeForced(RDReport::Music,edit_forcemusic_box->isChecked());
  edit_report->
    setExportTypeEnabled(RDReport::Generic,edit_generic_box->isChecked());
  edit_report->setFilterOnairFlag(edit_onairflag_box->currentIndex()==1);
  edit_report->setFilterGroups(edit_group_box->isChecked());
  if(edit_daypart_check->isChecked()) {
    edit_report->setStartTime(edit_starttime_edit->time());
    edit_report->setEndTime(edit_endtime_edit->time());
  }
  else {
    edit_report->setStartTime();
    edit_report->setEndTime();
  }

  //
  // Add New Services
  //
  for(unsigned i=0;i<edit_service_sel->destCount();i++) {
    sql=QString("select `SERVICE_NAME` from `REPORT_SERVICES` where ")+
      "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"' && "+
      "`SERVICE_NAME`='"+RDEscapeString(edit_service_sel->destText(i))+"'";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into `REPORT_SERVICES` (`REPORT_NAME`,`SERVICE_NAME`) ")+
	"values ('"+RDEscapeString(edit_report->name())+"',"+
	"'"+RDEscapeString(edit_service_sel->destText(i))+"')";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Services
  //
  sql=QString("delete from `REPORT_SERVICES` where ")+
    "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"'";
  for(unsigned i=0;i<edit_service_sel->destCount();i++) {
    sql+=QString(" && `SERVICE_NAME`<>'")+
      RDEscapeString(edit_service_sel->destText(i))+"'";
  }
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Add New Stations
  //
  for(unsigned i=0;i<edit_station_sel->destCount();i++) {
    sql=QString("select `STATION_NAME` from `REPORT_STATIONS` where ")+
      "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"' && "+
      "`STATION_NAME`='"+RDEscapeString(edit_station_sel->destText(i))+"'";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into `REPORT_STATIONS` (`REPORT_NAME`,`STATION_NAME`) ")+
	"values ('"+RDEscapeString(edit_report->name())+"',"+
	"'"+RDEscapeString(edit_station_sel->destText(i))+"')";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Stations
  //
  sql=QString("delete from `REPORT_STATIONS` where ")+
    "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"'";
  for(unsigned i=0;i<edit_station_sel->destCount();i++) {
    sql+=QString(" && `STATION_NAME`<>'")+
      RDEscapeString(edit_station_sel->destText(i))+"'";
  }
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Add New Groups
  //
  for(unsigned i=0;i<edit_group_sel->destCount();i++) {
    sql=QString("select `GROUP_NAME` from `REPORT_GROUPS` where ")+
      "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"' && "+
      "`GROUP_NAME`='"+RDEscapeString(edit_group_sel->destText(i))+"'";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into `REPORT_GROUPS` (`REPORT_NAME`,`GROUP_NAME`) ")+
	"values ('"+RDEscapeString(edit_report->name())+"',"+
	"'"+RDEscapeString(edit_group_sel->destText(i))+"')";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Groups
  //
  sql=QString("delete from `REPORT_GROUPS` where ")+
    "`REPORT_NAME`='"+RDEscapeString(edit_report->name())+"'";
  for(unsigned i=0;i<edit_group_sel->destCount();i++) {
    sql+=QString(" && `GROUP_NAME`<>'")+
      RDEscapeString(edit_group_sel->destText(i))+"'";
  }
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditReport::cancelData()
{
  done(-1);
}
