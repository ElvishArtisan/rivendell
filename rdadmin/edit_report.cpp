// edit_report.cpp
//
// Edit a Rivendell Report
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCheckBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>

#include <rd.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_report.h"
#include "test_import.h"
#include "autofill_carts.h"
#include "edit_svc_perms.h"

EditReport::EditReport(QString rptname,QWidget *parent)
  : QDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  QString str;
  bool ok=false;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  edit_report=new RDReport(rptname);
  setWindowTitle("RDAdmin - "+tr("Edit Report")+" - "+rptname);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont check_font=QFont("Helvetica",12,QFont::Normal);
  check_font.setPixelSize(12);
  QFont section_font=QFont("Helvetica",14,QFont::Bold);
  section_font.setPixelSize(14);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Report Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setGeometry(170,10,sizeHint().width()-180,19);
  edit_description_edit->setMaxLength(64);
  QLabel *label=
    new QLabel(edit_description_edit,tr("&Report Description:"),this);
  label->setGeometry(10,10,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Export Filter Type
  //
  edit_filter_box=new QComboBox(this);
  edit_filter_box->setGeometry(170,31,sizeHint().width()-180,19);
  for(int i=0;i<(int)RDReport::LastFilter;i++) {
    edit_filter_box->
      insertItem(RDReport::filterText((RDReport::ExportFilter)i));
  }
  label=new QLabel(edit_description_edit,tr("Export &Filter:"),this);
  label->setGeometry(10,31,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Station ID
  //
  edit_stationid_edit=new QLineEdit(this);
  edit_stationid_edit->setGeometry(170,52,180,19);
  edit_stationid_edit->setMaxLength(16);
  edit_stationid_edit->setValidator(validator);
  label=new QLabel(edit_stationid_edit,tr("Station ID:"),this);
  label->setGeometry(10,52,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Cart Number Parameters
  //
  label=new QLabel(tr("Cart Number Parameters:"),this);
  label->setGeometry(10,73,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);
  
  edit_cartzeros_box=new QCheckBox(this);
  edit_cartzeros_box->setGeometry(170,75,15,15);
  connect(edit_cartzeros_box,SIGNAL(toggled(bool)),
	  this,SLOT(leadingZerosToggled(bool)));
  label=new QLabel(edit_cartzeros_box,tr("Use Leading Zeros"),this);
  label->setGeometry(187,73,120,19);
  label->setFont(check_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);

  edit_cartdigits_spin=new QSpinBox(this);
  edit_cartdigits_spin->setGeometry(350,73,40,19);
  edit_cartdigits_spin->setRange(1,6);
  edit_cartdigits_label=new QLabel(edit_cartdigits_spin,tr("Digits:"),this);
  edit_cartdigits_label->setGeometry(300,73,45,19);
  edit_cartdigits_label->setFont(check_font);
  edit_cartdigits_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Station Type
  //
  edit_stationtype_box=new QComboBox(this);
  edit_stationtype_box->setGeometry(170,94,70,19);
  for(int i=0;i<RDReport::TypeLast;i++) {
    edit_stationtype_box->
      insertItem(RDReport::stationTypeText((RDReport::StationType)i));
  }
  label=new QLabel(edit_stationtype_box,tr("Station Type:"),this);
  label->setGeometry(10,94,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Lines Per Page
  //
  edit_linesperpage_spin=new QSpinBox(this);
  edit_linesperpage_spin->setGeometry(360,94,50,19);
  edit_linesperpage_spin->setRange(10,200);
  label=new QLabel(edit_linesperpage_spin,tr("Lines per Page:"),this);
  label->setGeometry(255,94,100,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);
  edit_linesperpage_spin->hide();
  label->hide();

  //
  // Service Name
  //
  edit_servicename_edit=new QLineEdit(this);
  edit_servicename_edit->setGeometry(170,115,sizeHint().width()-180,19);
  edit_servicename_edit->setMaxLength(64);
  label=new QLabel(edit_servicename_edit,tr("Ser&vice Name:"),this);
  label->setGeometry(10,115,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Station Format
  //
  edit_stationformat_edit=new QLineEdit(this);
  edit_stationformat_edit->setGeometry(170,136,sizeHint().width()-180,19);
  edit_stationformat_edit->setMaxLength(64);
  label=new QLabel(edit_stationformat_edit,tr("Station &Format:"),this);
  label->setGeometry(10,136,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Linux Export Path
  //
  edit_path_edit=new QLineEdit(this);
  edit_path_edit->setGeometry(170,157,sizeHint().width()-180,19);
  edit_path_edit->setMaxLength(255);
  edit_path_edit->setValidator(validator);
  label=new QLabel(edit_path_edit,tr("Linux Export Path:"),this);
  label->setGeometry(10,157,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Linux Post Export Command
  //
  edit_postexport_cmd_edit=new QLineEdit(this);
  edit_postexport_cmd_edit->setGeometry(170,178,sizeHint().width()-180,19);
  edit_postexport_cmd_edit->setValidator(validator);
  label=new QLabel(edit_path_edit,tr("Linux Post Export Cmd:"),this);
  label->setGeometry(10,178,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Windows Export Path
  //
  edit_winpath_edit=new QLineEdit(this);
  edit_winpath_edit->setGeometry(170,199,sizeHint().width()-180,19);
  edit_winpath_edit->setMaxLength(255);
  label=new QLabel(edit_winpath_edit,tr("Windows Export Path:"),this);
  label->setGeometry(10,199,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Windows Post Export Command
  //
  edit_winpostexport_cmd_edit=new QLineEdit(this);
  edit_winpostexport_cmd_edit->setGeometry(170,219,sizeHint().width()-180,19);
  edit_winpostexport_cmd_edit->setValidator(validator);
  label=new QLabel(edit_path_edit,tr("Windows Post Export Cmd:"),this);
  label->setGeometry(10,219,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Type Selectors
  //
  label=new QLabel(tr("Export Event Types:"),this);
  label->setGeometry(10,240,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  edit_traffic_box=new QCheckBox(this);
  edit_traffic_box->setGeometry(170,242,15,15);
  edit_traffic_label=new QLabel(tr("Traffic"),this);
  edit_traffic_label->setGeometry(187,241,80,19);
  edit_traffic_label->setFont(check_font);
  edit_traffic_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);
  
  edit_music_box=new QCheckBox(this);
  edit_music_box->setGeometry(270,243,15,15);
  edit_music_label=new QLabel(tr("Music"),this);
  edit_music_label->setGeometry(287,241,80,19);
  edit_music_label->setFont(check_font);
  edit_music_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);

  edit_generic_box=new QCheckBox(this);
  edit_generic_box->setGeometry(370,243,15,15);
  label=new QLabel(tr("All"),this);
  label->setGeometry(387,241,80,19);
  label->setFont(check_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);
  connect(edit_generic_box,SIGNAL(toggled(bool)),
	  this,SLOT(genericEventsToggledData(bool)));

  //
  // Force Event Source Selectors
  //
  label=new QLabel(tr("Export Events From:"),this);
  label->setGeometry(10,262,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  edit_forcetraffic_box=new QCheckBox(this);
  edit_forcetraffic_box->setGeometry(170,264,15,15);
  label=new QLabel(tr("Traffic Log"),this);
  label->setGeometry(187,262,80,19);
  label->setFont(check_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);
  
  edit_forcemusic_box=new QCheckBox(this);
  edit_forcemusic_box->setGeometry(270,264,15,15);
  label=new QLabel(tr("Music Log"),this);
  label->setGeometry(287,262,80,19);
  label->setFont(check_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Include Only On Air Events
  //
  edit_onairflag_box=new QComboBox(this);
  edit_onairflag_box->setGeometry(170,283,60,19);
  edit_onairflag_box->insertItem(tr("No"));
  edit_onairflag_box->insertItem(tr("Yes"));
  label=new QLabel(edit_onairflag_box,tr("Include Only OnAir Events:"),this);
  label->setGeometry(10,283,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  //
  // Daypart Filter
  //
  edit_daypart_check=new QCheckBox(this);
  edit_daypart_check->setGeometry(60,313,15,15);
  edit_daypart_label=
    new QLabel(edit_daypart_check,tr("Filter by Daypart"),this);
  edit_daypart_label->setGeometry(edit_daypart_check->geometry().x()+20,313,155,19);
  edit_daypart_label->setFont(font);
  edit_daypart_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);

  edit_starttime_edit=new QDateTimeEdit(this);
  edit_starttime_edit->setGeometry(150,334,80,20);
  edit_starttime_edit->setDisplayFormat("hh:mm:ss");
  edit_starttime_label=new QLabel(edit_starttime_edit,tr("Start Time:"),this);
  edit_starttime_label->setGeometry(65,334,80,20);
  edit_starttime_label->setFont(font);
  edit_starttime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

  edit_endtime_edit=new QDateTimeEdit(this);
  edit_endtime_edit->setGeometry(335,334,80,20);
  edit_endtime_edit->setDisplayFormat("hh:mm:ss");
  edit_endtime_label=new QLabel(edit_endtime_edit,tr("End Time:"),this);
  edit_endtime_label->setGeometry(250,334,80,20);
  edit_endtime_label->setFont(font);
  edit_endtime_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::ShowPrefix);

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
  edit_service_sel->
    setGeometry((sizeHint().width()-edit_service_sel->sizeHint().width())/2,
		373,edit_service_sel->sizeHint().width(),
		edit_service_sel->sizeHint().height());
  edit_service_sel->sourceSetLabel(tr("Available Services"));
  edit_service_sel->destSetLabel(tr("Source Services"));
  
  //
  // Station Selector
  //
  edit_station_sel=new RDListSelector(this);
  edit_station_sel->
    setGeometry((sizeHint().width()-edit_station_sel->sizeHint().width())/2,
		477,edit_station_sel->sizeHint().width(),
		edit_station_sel->sizeHint().height());
  edit_station_sel->sourceSetLabel(tr("Available Hosts"));
  edit_station_sel->destSetLabel(tr("Source Hosts"));

  //
  // Group Selector
  //
  edit_group_sel=new RDListSelector(this);
  edit_group_sel->
    setGeometry((sizeHint().width()-edit_group_sel->sizeHint().width())/2,
		593,edit_group_sel->sizeHint().width(),
		edit_group_sel->sizeHint().height());
  edit_group_sel->sourceSetLabel(tr("Available Groups"));
  edit_group_sel->destSetLabel(tr("Allowed Groups"));

  edit_group_box=new QCheckBox(this);
  edit_group_box->setGeometry(60,574,15,15);
  label=new QLabel(edit_group_box,tr("Filter by Groups"),this);
  label->setGeometry(edit_group_box->geometry().x()+20,573,155,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::ShowPrefix);
  connect(edit_group_box,SIGNAL(toggled(bool)),
	  edit_group_sel,SLOT(setEnabled(bool)));


  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  edit_description_edit->setText(edit_report->description());
  edit_filter_box->setCurrentItem((int)edit_report->filter());
  edit_stationid_edit->setText(edit_report->stationId());
  edit_cartzeros_box->setChecked(edit_report->useLeadingZeros());
  leadingZerosToggled(edit_cartzeros_box->isChecked());
  edit_cartdigits_spin->setValue(edit_report->cartDigits());
  edit_stationtype_box->setCurrentItem((int)edit_report->stationType());
  edit_servicename_edit->setText(edit_report->serviceName());
  edit_stationformat_edit->setText(edit_report->stationFormat());
  edit_linesperpage_spin->setValue(edit_report->linesPerPage());
  edit_path_edit->setText(edit_report->exportPath(RDReport::Linux));
  edit_postexport_cmd_edit->
    setText(edit_report->postExportCommand(RDReport::Linux));
  edit_winpath_edit->setText(edit_report->exportPath(RDReport::Windows));
  edit_winpostexport_cmd_edit->
    setText(edit_report->postExportCommand(RDReport::Windows));
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
    edit_onairflag_box->setCurrentItem(1);
  }
  else {
    edit_onairflag_box->setCurrentItem(0);
  }
  edit_starttime_edit->setTime(edit_report->startTime(&ok));
  edit_starttime_label->setDisabled(ok);
  edit_starttime_edit->setDisabled(ok);
  edit_endtime_edit->setTime(edit_report->endTime(&ok));
  edit_endtime_label->setDisabled(ok);
  edit_endtime_edit->setDisabled(ok);
  edit_daypart_check->setChecked(!ok);

  sql=QString("select SERVICE_NAME from REPORT_SERVICES where ")+
    "REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_service_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString("select NAME from SERVICES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(edit_service_sel->destFindItem(q->value(0).toString())==0) {
      edit_service_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;

  sql=QString("select STATION_NAME from REPORT_STATIONS where ")+
    "REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_station_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString("select NAME from STATIONS");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(edit_station_sel->destFindItem(q->value(0).toString())==0) {
      edit_station_sel->sourceInsertItem(q->value(0).toString());
    }
  }
  delete q;

  edit_group_box->setChecked(edit_report->filterGroups());
  edit_group_sel->setEnabled(edit_report->filterGroups());
  sql=QString("select GROUP_NAME from REPORT_GROUPS where ")+
    "REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_group_sel->destInsertItem(q->value(0).toString());
  }
  delete q;

  sql=QString("select NAME from GROUPS");
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
  return QSize(500,749);
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
    setFilter((RDReport::ExportFilter)edit_filter_box->currentItem());
  edit_report->setStationId(edit_stationid_edit->text());
  edit_report->setCartDigits(edit_cartdigits_spin->value());
  edit_report->setUseLeadingZeros(edit_cartzeros_box->isChecked());
  edit_report->setLinesPerPage(edit_linesperpage_spin->value());
  edit_report->
    setStationType((RDReport::StationType)edit_stationtype_box->currentItem());
  edit_report->setServiceName(edit_servicename_edit->text());
  edit_report->setStationFormat(edit_stationformat_edit->text());
  edit_report->setExportPath(RDReport::Linux,edit_path_edit->text());
  edit_report->
    setPostExportCommand(RDReport::Linux,edit_postexport_cmd_edit->text());
  edit_report->setExportPath(RDReport::Windows,edit_winpath_edit->text());
  edit_report->
    setPostExportCommand(RDReport::Windows,edit_winpostexport_cmd_edit->text());
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
  edit_report->setFilterOnairFlag(edit_onairflag_box->currentItem()==1);
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
    sql=QString("select SERVICE_NAME from REPORT_SERVICES where ")+
      "(REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\")&&"+
      "(SERVICE_NAME=\""+RDEscapeString(edit_service_sel->destText(i))+"\")";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into REPORT_SERVICES set ")+
	"REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\","+
	"SERVICE_NAME=\""+RDEscapeString(edit_service_sel->destText(i))+"\"";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Services
  //
  sql=QString("delete from REPORT_SERVICES where ")+
    "(REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\")";
  for(unsigned i=0;i<edit_service_sel->destCount();i++) {
    sql+=QString("&&(SERVICE_NAME<>")+"\""+
      RDEscapeString(edit_service_sel->destText(i))+"\")";
  }
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Add New Stations
  //
  for(unsigned i=0;i<edit_station_sel->destCount();i++) {
    sql=QString("select STATION_NAME from REPORT_STATIONS where ")+
      "(REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\")&&"+
      "(STATION_NAME=\""+RDEscapeString(edit_station_sel->destText(i))+"\")";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into REPORT_STATIONS set ")+
	"REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\","+
	"STATION_NAME=\""+RDEscapeString(edit_station_sel->destText(i))+"\"";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Stations
  //
  sql=QString("delete from REPORT_STATIONS where ")+
    "(REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\")";
  for(unsigned i=0;i<edit_station_sel->destCount();i++) {
    sql+=QString("&&(STATION_NAME<>")+"\""+
      RDEscapeString(edit_station_sel->destText(i))+"\"";
  }
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Add New Groups
  //
  for(unsigned i=0;i<edit_group_sel->destCount();i++) {
    sql=QString("select GROUP_NAME from REPORT_GROUPS where ")+
      "(REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\")&&"+
      "(GROUP_NAME=\""+RDEscapeString(edit_group_sel->destText(i))+"\")";
    q=new RDSqlQuery(sql);
    if(q->size()==0) {
      delete q;
      sql=QString("insert into REPORT_GROUPS set ")+
	"REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\","+
	"GROUP_NAME=\""+RDEscapeString(edit_group_sel->destText(i))+"\"";
      q=new RDSqlQuery(sql);
    }
    delete q;
  }

  //
  // Delete Old Groups
  //
  sql=QString("delete from REPORT_GROUPS where ")+
    "(REPORT_NAME=\""+RDEscapeString(edit_report->name())+"\")";
  for(unsigned i=0;i<edit_group_sel->destCount();i++) {
    sql+=QString("&&(GROUP_NAME<>")+"\""+
      RDEscapeString(edit_group_sel->destText(i))+"\")";
  }
  q=new RDSqlQuery(sql);
  delete q;

  done(0);
}


void EditReport::cancelData()
{
  done(-1);
}
