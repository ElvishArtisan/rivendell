// edit_log.cpp
//
// Edit a Rivendell Log
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rd.h>
#include <rdadd_log.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rdcreate_log.h>
#include <rddatedialog.h>
#include <rddb.h>
#include <rddebug.h>
#include <rdlogedit_conf.h>
#include <rdtextfile.h>
#include <rdtextvalidator.h>

#include "add_meta.h"
#include "edit_chain.h"
#include "edit_log.h"
#include "edit_logline.h"
#include "edit_marker.h"
#include "edit_track.h"
#include "globals.h"

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/marker.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/music.xpm"
#include "../icons/mic16.xpm"
#include "../icons/traffic.xpm"

EditLog::EditLog(QString logname,QString *filter,QString *group,
		 QString *schedcode,vector<RDLogLine> *clipboard,
		 vector<QString> *new_logs,QWidget *parent)
  : QDialog(parent,"",true)
{
  QString sql;
  RDSqlQuery *q;
  QStringList services_list;
  QColor system_mid_color = colorGroup().mid();
  QColor system_button_color = palette().active().button();

  edit_logname=logname;
  edit_filter=filter;
  edit_group=group;
  edit_schedcode=schedcode;
  edit_clipboard=clipboard;
  edit_newlogs=new_logs;
  edit_default_trans=RDLogLine::Play;
  bool adding_allowed=rda->user()->addtoLog();
  bool deleting_allowed=rda->user()->removefromLog();
  bool editing_allowed=rda->user()->arrangeLog();
  bool saveas_allowed=rda->user()->createLog();

  setCaption(tr("Edit Log"));

  //
  // Config Data
  //
  edit_default_trans=rda->logeditConf()->defaultTransType();
  edit_output_card=rda->logeditConf()->outputCard();
  edit_output_port=rda->logeditConf()->outputPort();
  edit_start_macro=rda->logeditConf()->startCart();
  edit_end_macro=rda->logeditConf()->endCart();

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont modified_font=QFont("Helvetica",18,QFont::Bold);
  modified_font.setPixelSize(18);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont title_font=QFont("Helvetica",12,QFont::Normal);
  title_font.setPixelSize(12);
  QFont length_font=QFont("Helvetica",10,QFont::Bold);
  length_font.setPixelSize(10);
  
  //
  // Create Icons
  //
  edit_playout_map=new QPixmap(play_xpm);
  edit_macro_map=new QPixmap(rml5_xpm);
  edit_marker_map=new QPixmap(marker_xpm);
  edit_chain_map=new QPixmap(chain_xpm);
  edit_track_cart_map=new QPixmap(track_cart_xpm);
  edit_notemarker_map=new QPixmap(notemarker_xpm);
  edit_music_map=new QPixmap(music_xpm);
  edit_mic16_map=new QPixmap(mic16_xpm);
  edit_traffic_map=new QPixmap(traffic_xpm);

  //
  // Dialogs
  //
#ifndef WIN32
  edit_render_dialog=new RenderDialog(rda->station(),rda->system(),rda->config(),this);
#endif  // WIN32

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Log Header
  //
  edit_log=new RDLog(edit_logname);

  //
  // Log Data Structures
  //
  edit_log_lock=new RDLogLock(edit_logname,rda->user(),rda->station(),this);
  edit_log_event=new RDLogEvent(RDLog::tableName(edit_logname));
  edit_log_event->load(true);

  //
  // Notifications
  //
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));

  //
  // Log Name
  //
  edit_modified_label=new QLabel(this);
  edit_modified_label->setBackgroundColor(QColor(system_mid_color));
  edit_modified_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  edit_modified_label->setFont(modified_font);
  edit_logname_label=new QLabel(logname,this);
  edit_logname_label->setBackgroundColor(QColor(system_mid_color));
  edit_logname_label->setAlignment(AlignLeft|AlignVCenter);
  edit_logname_label->setFont(title_font);
  edit_logname_label_label=new QLabel(tr("Log Name:"),this);
  edit_logname_label_label->setBackgroundColor(QColor(system_mid_color));
  edit_logname_label_label->setFont(label_font);
  edit_logname_label_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Track Counts
  //
  edit_track_label=new QLabel(this);
  edit_track_label->setBackgroundColor(QColor(system_mid_color));
  edit_track_label->setAlignment(AlignLeft|AlignVCenter);
  edit_track_label->setFont(title_font);
  edit_track_label_label=new QLabel(tr("Tracks:"),this);
  edit_track_label_label->setBackgroundColor(QColor(system_mid_color));
  edit_track_label_label->setFont(label_font);
  edit_track_label_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Log Origin
  //
  edit_origin_label=
    new QLabel(edit_log->originUser()+QString(" - ")+
	       edit_log->originDatetime().toString("MM/dd/yyyy - hh:mm:ss"),
	       this);
  edit_origin_label->setBackgroundColor(QColor(system_mid_color));
  edit_origin_label->setAlignment(AlignLeft|AlignVCenter);
  edit_origin_label->setFont(title_font);
  edit_origin_label_label=new QLabel(tr("Origin:"),this);
  edit_origin_label_label->setBackgroundColor(QColor(system_mid_color));
  edit_origin_label_label->setFont(label_font);
  edit_origin_label_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setValidator(validator);
  connect(edit_description_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(descriptionChangedData(const QString &)));
  edit_description_label=
    new QLabel(edit_description_edit,tr("Description:"),this);
  edit_description_label->setFont(label_font);
  edit_description_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Purge Date
  //
  edit_purgedate_box=new QCheckBox(this);
  edit_purgedate_label=new QLabel(edit_purgedate_box,tr("Delete on"),this);
  edit_purgedate_label->setFont(label_font);
  edit_purgedate_label->setAlignment(AlignRight|AlignVCenter);
  edit_purgedate_edit=new QDateEdit(this);
  edit_purgedate_button=new QPushButton(tr("Select"),this);
  edit_purgedate_button->setFont(label_font);
  connect(edit_purgedate_box,SIGNAL(toggled(bool)),
	  this,SLOT(purgeDateToggledData(bool)));
  connect(edit_purgedate_box,SIGNAL(toggled(bool)),
	  edit_purgedate_edit,SLOT(setEnabled(bool)));
  connect(edit_purgedate_box,SIGNAL(toggled(bool)),
	  edit_purgedate_button,SLOT(setEnabled(bool)));
  connect(edit_purgedate_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(purgeDateChangedData(const QDate &)));
  connect(edit_purgedate_button,SIGNAL(clicked()),
	  this,SLOT(selectPurgeDateData()));

  //
  // Service
  //
  edit_service_box=new QComboBox(this);
  edit_service_edit=new QLineEdit(this);
  edit_service_edit->setReadOnly(true);
  edit_service_label=new QLabel(edit_service_box,tr("Service:"),this);
  edit_service_label->setFont(label_font);
  edit_service_label->setAlignment(AlignRight|AlignVCenter);  
  connect(edit_service_box,SIGNAL(activated(const QString &)),
	  this,SLOT(serviceActivatedData(const QString &)));

  //
  // Auto Refresh
  //
  edit_autorefresh_box=new QComboBox(this);
  edit_autorefresh_box->insertItem(tr("Yes"));
  edit_autorefresh_box->insertItem(tr("No"));
  edit_autorefresh_edit=new QLineEdit(this);
  edit_autorefresh_edit->setReadOnly(true);
  edit_autorefresh_label=
    new QLabel(edit_autorefresh_box,tr("Enable AutoRefresh:"),this);
  edit_autorefresh_label->setFont(label_font);
  edit_autorefresh_label->setAlignment(AlignRight|AlignVCenter);  
  connect(edit_autorefresh_box,SIGNAL(activated(int)),
	  this,SLOT(autorefreshChangedData(int)));

  //
  // Start Date
  //
  edit_startdate_edit=new QDateEdit(this);
  edit_startdate_label=new QLabel(edit_startdate_edit,tr("Start Date:"),this);
  edit_startdate_label->setFont(label_font);
  edit_startdate_label->setAlignment(AlignRight|AlignVCenter);  
  connect(edit_startdate_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(dateValueChangedData(const QDate &)));


  //
  // End Date
  //
  edit_enddate_edit=new QDateEdit(this);
  edit_enddate_label=new QLabel(edit_startdate_edit,tr("End Date:"),this);
  edit_enddate_label->setFont(label_font);
  edit_enddate_label->setAlignment(AlignRight|AlignVCenter);  
  connect(edit_enddate_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(dateValueChangedData(const QDate &)));

  //
  // Start Date Checkbox
  //
  edit_startdate_box=new QCheckBox(this);
  connect(edit_startdate_box,SIGNAL(toggled(bool)),
	  this,SLOT(startDateEnabledData(bool)));
  edit_startdate_box_label=
    new QLabel(edit_startdate_box,tr("Start Date Enabled"),this);
  edit_startdate_box_label->setFont(label_font);
  edit_startdate_box_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // End Date Checkbox
  //
  edit_enddate_box=new QCheckBox(this);
  connect(edit_enddate_box,SIGNAL(toggled(bool)),
	  this,SLOT(endDateEnabledData(bool)));
  edit_enddate_box_label=
    new QLabel(edit_enddate_box,tr("End Date Enabled"),this);
  edit_enddate_box_label->setFont(label_font);
  edit_enddate_box_label->setAlignment(AlignLeft|AlignVCenter);  

  //
  // Time Counter Section
  //
  edit_time_label=new QLabel(tr("Run Length"),this);
  edit_time_label->setFont(label_font);
  edit_time_label->setAlignment(AlignCenter);  

  //
  // Stop Time Counter
  //
  edit_stoptime_edit=new QLineEdit(this);
  edit_stoptime_label=new QLabel(edit_stoptime_edit,tr("Next Stop:"),this);
  edit_stoptime_label->setFont(label_font);
  edit_stoptime_label->setAlignment(AlignRight|AlignVCenter);  

  //
  // End Time Counter
  //
  edit_endtime_edit=new QLineEdit(this);
  edit_endtime_label=new QLabel(edit_endtime_edit,tr("Log End:"),this);
  edit_endtime_label->setFont(label_font);
  edit_endtime_label->setAlignment(AlignRight|AlignVCenter);  

  //
  // Log Event List
  //
  edit_log_list=new DropListView(this);
  edit_log_list->setAllColumnsShowFocus(true);
  edit_log_list->setSelectionMode(QListView::Extended);
  edit_log_list->setItemMargin(5);
  edit_log_list->addColumn("");
  edit_log_list->setColumnAlignment(0,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("TIME"));
  edit_log_list->setColumnAlignment(1,Qt::AlignRight);
  edit_log_list->addColumn(tr("TRANS"));
  edit_log_list->setColumnAlignment(2,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("CART"));
  edit_log_list->setColumnAlignment(3,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("GROUP"));
  edit_log_list->setColumnAlignment(4,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("LENGTH"));
  edit_log_list->setColumnAlignment(5,Qt::AlignRight);
  edit_log_list->addColumn(tr("TITLE"));
  edit_log_list->setColumnAlignment(6,Qt::AlignLeft);
  edit_log_list->addColumn(tr("ARTIST"));
  edit_log_list->setColumnAlignment(7,Qt::AlignLeft);
  edit_log_list->addColumn(tr("CLIENT"));
  edit_log_list->setColumnAlignment(8,Qt::AlignLeft);
  edit_log_list->addColumn(tr("AGENCY"));
  edit_log_list->setColumnAlignment(9,Qt::AlignLeft);
  edit_log_list->addColumn(tr("LABEL"));
  edit_log_list->setColumnAlignment(10,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("SOURCE"));
  edit_log_list->setColumnAlignment(11,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("EXT DATA"));
  edit_log_list->setColumnAlignment(12,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("LINE ID"));
  edit_log_list->setColumnAlignment(13,Qt::AlignHCenter);
  edit_log_list->addColumn(tr("COUNT"));
  edit_log_list->setColumnAlignment(14,Qt::AlignHCenter);
  edit_log_list->setHardSortColumn(14);
  edit_log_list->setColumnSortType(14,RDListView::LineSort);
  if(editing_allowed) {
    connect(edit_log_list,SIGNAL(doubleClicked(QListViewItem *)),
	    this,SLOT(doubleClickData(QListViewItem *)));
    connect(edit_log_list,SIGNAL(cartDropped(int,RDLogLine *)),
	    this,SLOT(cartDroppedData(int,RDLogLine *)));
  }
  connect(edit_log_list,SIGNAL(clicked(QListViewItem *)),
  	  this,SLOT(clickedData(QListViewItem *)));
  connect(edit_log_list,SIGNAL(selectionChanged()),
  	  this,SLOT(selectionChangedData()));

  //
  //  Insert Cart Button
  //
  edit_cart_button=new QPushButton(this);
  edit_cart_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_cart_button->setFont(button_font);
  edit_cart_button->setText(tr("Insert\nCart"));
  connect(edit_cart_button,SIGNAL(clicked()),
	  this,SLOT(insertCartButtonData()));

  //
  //  Insert Marker Button
  //
  edit_marker_button=new QPushButton(this);
  edit_marker_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_marker_button->setFont(button_font);
  edit_marker_button->setText(tr("Insert\nMeta"));
  connect(edit_marker_button,SIGNAL(clicked()),
	  this,SLOT(insertMarkerButtonData()));

  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_edit_button->setFont(button_font);
  edit_edit_button->setText(tr("Edit"));
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editButtonData()));

  //
  //  Delete Button
  //
  edit_delete_button=new QPushButton(this);
  edit_delete_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_delete_button->setFont(button_font);
  edit_delete_button->setText(tr("Delete"));
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteButtonData()));

  //
  //  Up Button
  //
  edit_up_button=new RDTransportButton(RDTransportButton::Up,this);
  edit_up_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  connect(edit_up_button,SIGNAL(clicked()),this,SLOT(upButtonData()));

  //
  //  Down Button
  //
  edit_down_button=new RDTransportButton(RDTransportButton::Down,this);
  edit_down_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  connect(edit_down_button,SIGNAL(clicked()),this,SLOT(downButtonData()));

  //
  //  Cut Button
  //
  edit_cut_button=new QPushButton(this);
  edit_cut_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_cut_button->setFont(button_font);
  edit_cut_button->setText(tr("Cut"));
  connect(edit_cut_button,SIGNAL(clicked()),this,SLOT(cutButtonData()));

  //
  //  Copy Button
  //
  edit_copy_button=new QPushButton(this);
  edit_copy_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_copy_button->setFont(button_font);
  edit_copy_button->setText(tr("Copy"));
  connect(edit_copy_button,SIGNAL(clicked()),this,SLOT(copyButtonData()));

  //
  //  Paste Button
  //
  edit_paste_button=new QPushButton(this);
  edit_paste_button->setPalette(QPalette(QColor(system_button_color),QColor(system_mid_color)));
  edit_paste_button->setFont(button_font);
  edit_paste_button->setText(tr("Paste"));
  connect(edit_paste_button,SIGNAL(clicked()),this,SLOT(pasteButtonData()));

  //
  //  Save Button
  //
  edit_save_button=new QPushButton(this);
  edit_save_button->setFont(button_font);
  edit_save_button->setText(tr("&Save"));
  edit_save_button->setDisabled(true);
  connect(edit_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  edit_saveas_button=new QPushButton(this);
  edit_saveas_button->setFont(button_font);
  edit_saveas_button->setText(tr("Save")+"\n"+tr("As"));
  connect(edit_saveas_button,SIGNAL(clicked()),this,SLOT(saveasData()));

  //
  //  Render Button
  //
#ifndef WIN32
  edit_renderas_button=new QPushButton(this);
  edit_renderas_button->setFont(button_font);
  edit_renderas_button->setText(tr("Render"));
  connect(edit_renderas_button,SIGNAL(clicked()),this,SLOT(renderasData()));
#endif  // WIN32

  //
  //  Reports Button
  //
  edit_reports_button=new QPushButton(this);
  edit_reports_button->setFont(button_font);
  edit_reports_button->setText(tr("&Reports"));
  connect(edit_reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  // Cart Player
  //
#ifdef WIN32
  edit_player=NULL;
#else
  edit_player=
    new RDSimplePlayer(rda->cae(),rda->ripc(),edit_output_card,edit_output_port,
		       edit_start_macro,edit_end_macro,this);
  edit_player->stopButton()->setOnColor(red);
#endif  // WIN32

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(button_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(button_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_description_edit->setText(edit_log->description());

  QDate purge_date=edit_log->purgeDate();
  if(purge_date.isNull()) {
    edit_purgedate_edit->setDate(QDate::currentDate().addMonths(1));
    edit_purgedate_edit->setDisabled(true);
    edit_purgedate_button->setDisabled(true);
  }
  else {
    edit_purgedate_box->setChecked(true);
    edit_purgedate_edit->setDate(purge_date);
  }
  sql=QString("select NAME from SERVICES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append( q->value(0).toString() );
  }
  delete q;

  int n=-1;
  int ncounter=0;
  QString service=edit_log->service();
  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end(); ++it ) {
    edit_service_box->insertItem(*it);
    if(*it==service) {
      n=ncounter;
      edit_service_edit->setText(*it);
    }
    ncounter++;
  }
  if(n>=0) {
    edit_service_box->setCurrentItem(n);
  }
  if(edit_log->autoRefresh()) {
    edit_autorefresh_box->setCurrentItem(0);
    edit_autorefresh_edit->setText(tr("Yes"));
  }
  else {
    edit_autorefresh_box->setCurrentItem(1);
    edit_autorefresh_edit->setText(tr("No"));
  }
  if(!edit_log->startDate().isNull()) {
    edit_startdate_box->setChecked(true);
    edit_startdate_edit->setDate(edit_log->startDate());
  }
  else {
    edit_startdate_edit->setDisabled(true);
    edit_startdate_label->setDisabled(true);
    edit_startdate_box->setChecked(false);
  }
  if(!edit_log->endDate().isNull()) {
    edit_enddate_box->setChecked(true);
    edit_enddate_edit->setDate(edit_log->endDate());
  }
  else {
    edit_enddate_edit->setDisabled(true);
    edit_enddate_label->setDisabled(true);
    edit_enddate_box->setChecked(false);
  }
  RefreshList();
  serviceActivatedData(edit_service_box->currentText());
  SetLogModified(false);
  UpdateTracks();

  //
  // Set Control Perms
  //
  edit_description_edit->setReadOnly(!editing_allowed);
  if(saveas_allowed) {
    edit_service_edit->hide();
    edit_autorefresh_edit->hide();
  }
  else {
    edit_service_box->hide();
    edit_autorefresh_box->hide();
  }
  if(!editing_allowed) {
    if(edit_startdate_box->isChecked()) {
      edit_startdate_edit->setRange(edit_startdate_edit->date(),
				    edit_startdate_edit->date());
    }
    if(edit_enddate_box->isChecked()) {
      edit_enddate_edit->setRange(edit_enddate_edit->date(),
				  edit_enddate_edit->date());
    }
  }
  edit_startdate_box->setEnabled(editing_allowed);
  edit_enddate_box->setEnabled(editing_allowed);
  edit_startdate_box->setEnabled(editing_allowed);
  edit_enddate_box->setEnabled(editing_allowed);
  edit_cart_button->setEnabled(adding_allowed&&editing_allowed);
  edit_marker_button->setEnabled(adding_allowed&&editing_allowed);
  edit_edit_button->setEnabled(editing_allowed&&editing_allowed);
  edit_delete_button->setEnabled(deleting_allowed&&editing_allowed);
  edit_up_button->setEnabled(editing_allowed);
  edit_down_button->setEnabled(editing_allowed);
  edit_cut_button->
    setEnabled(adding_allowed&&deleting_allowed&&editing_allowed);
  edit_copy_button->setEnabled(adding_allowed&&editing_allowed);
  edit_paste_button->setEnabled(adding_allowed&&editing_allowed);
  edit_saveas_button->setEnabled(saveas_allowed);
}


EditLog::~EditLog()
{
  delete edit_log_event;
  delete edit_log_lock;
}


QSize EditLog::sizeHint() const
{
  return QSize(800,600);
} 


QSizePolicy EditLog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditLog::exec()
{
  QString username;
  QString stationname;
  QHostAddress addr;

  if(!edit_log_lock->tryLock(&username,&stationname,&addr)) {
    QString msg=tr("Log already being edited by")+" "+username+"@"+stationname;
    if(stationname!=addr.toString()) {
      msg+=" ["+addr.toString()+"]";
    }
    msg+=".";
    QMessageBox::warning(this,"RDLogEdit - "+tr("Log Locked"),msg);
    return false;
  }

  return QDialog::exec();
}


void EditLog::descriptionChangedData(const QString &)
{
  SetLogModified(true);
}


void EditLog::purgeDateChangedData(const QDate &date)
{
  SetLogModified(true);
}


void EditLog::purgeDateToggledData(bool state)
{
  SetLogModified(true);
}


void EditLog::selectPurgeDateData()
{
  QDate date=edit_purgedate_edit->date();
  RDDateDialog *d=new RDDateDialog(2008,QDate::currentDate().year()+5,this);
  if(d->exec(&date)==0) {
    edit_purgedate_edit->setDate(date);
    SetLogModified(true);
  }
}


void EditLog::serviceActivatedData(const QString &svcname)
{
  SetLogModified(true);
  edit_group_list.loadSvc(svcname);
  if(!ValidateSvc()) {
    QMessageBox::warning(this,tr("Invalid Carts"),
			 tr("This log contains one or more carts\nthat are invalid for the selected service!"));
  }
}


void EditLog::startDateEnabledData(bool state)
{
  if(state) {
    edit_startdate_edit->setDate(QDate::currentDate());
  }
  edit_startdate_edit->setEnabled(state);
  edit_startdate_label->setEnabled(state);
  SetLogModified(true);
}


void EditLog::endDateEnabledData(bool state)
{
  if(state) {
    edit_enddate_edit->setDate(QDate::currentDate());
  }
  edit_enddate_edit->setEnabled(state);
  edit_enddate_label->setEnabled(state);
  SetLogModified(true);
}


void EditLog::dateValueChangedData(const QDate &)
{
  SetLogModified(true);
}


void EditLog::autorefreshChangedData(int index)
{
  SetLogModified(true);
}


void EditLog::insertCartButtonData()
{
  int line;
  int id;

  QListViewItem *item=SingleSelection();
  if(item==NULL || (line=item->text(14).toInt())<0) {
    line=edit_log_event->size();
    id=END_MARKER_ID;
  } else {
    id=item->text(13).toInt();
  }
  edit_log_event->insert(line,1);
  edit_log_event->logLine(line)->setTransType(edit_default_trans);
  edit_log_event->logLine(line)->setFadeupGain(-3000);
  edit_log_event->logLine(line)->setFadedownGain(-3000);
  EditLogLine *edit=
    new EditLogLine(edit_log_event->logLine(line),edit_filter,edit_group,
		    edit_schedcode,edit_service_box->currentText(),
		    &edit_group_list,edit_log_event,line,this);
  int ret=edit->exec();
  if(ret>=0) {
    edit_log_event->refresh(line);
    SetLogModified(true);
  }
  else {
    edit_log_event->remove(line,1);
    delete edit;
    return;
  }
  delete edit;
  RefreshList();
  SelectRecord(id);
  UpdateSelection();
}


void EditLog::insertMarkerButtonData()
{
  int line;
  int id;
  int ret;
  EditMarker *edit_marker;
  EditTrack *edit_track;
  EditChain *edit_chain;

  QListViewItem *item=SingleSelection();
  if(item==NULL) {
    return;
  }
  line=item->text(14).toInt();
  id=item->text(13).toInt();
  AddMeta *meta=new AddMeta(this);
  switch((RDLogLine::Type)meta->exec()) {
  case RDLogLine::Marker:
    edit_log_event->insert(line,1);
    edit_log_event->logLine(line)->setType(RDLogLine::Marker);
    edit_marker=new EditMarker(edit_log_event->logLine(line),this);
    ret=edit_marker->exec();
    if(ret>=0) {
      edit_log_event->refresh(line);
      SetLogModified(true);
    }
    else {
      edit_log_event->remove(line,1);
    }
    delete edit_marker;
    break;

  case RDLogLine::Track:
    edit_log_event->insert(line,1);
    edit_log_event->logLine(line)->setType(RDLogLine::Track);
    edit_log_event->logLine(line)->setTransType(RDLogLine::Segue);
    edit_log_event->logLine(line)->setMarkerComment(tr("Voice Track"));
    edit_track=new EditTrack(edit_log_event->logLine(line),this);
    ret=edit_track->exec();
    if(ret>=0) {
      edit_log_event->refresh(line);
      SetLogModified(true);
    }
    else {
      edit_log_event->remove(line,1);
    }
    delete edit_track;
    break;

  case RDLogLine::Chain:
    edit_log_event->insert(line,1);
    edit_log_event->logLine(line)->setType(RDLogLine::Chain);
    edit_chain=new EditChain(edit_log_event->logLine(line),this);
    ret=edit_chain->exec();
    if(ret>=0) {
      edit_log_event->refresh(line);
      SetLogModified(true);
    }
    else {
      edit_log_event->remove(line,1);
    }
    delete edit_chain;
    break;

  default:
    break;
  }
  RefreshList();
  UpdateTracks();
  SelectRecord(id);
  UpdateSelection();
}


void EditLog::clickedData(QListViewItem *item)
{
#ifndef WIN32
  RDListViewItem *rditem=SingleSelection();
  if(rditem==NULL) {
    edit_player->setCart(0);
    return;
  }
  else {
    edit_player->setCart(rditem->text(3).toUInt());
  }
#endif  // WIN32
}


void EditLog::selectionChangedData()
{
  UpdateSelection();
}


void EditLog::doubleClickData(QListViewItem *item)
{
  if(item->text(13).toInt()==END_MARKER_ID) {
    insertCartButtonData();
  }
  else {
    editButtonData();
  }
}


void EditLog::editButtonData()
{
  EditLogLine *edit_cart;
  EditMarker *edit_marker;
  EditTrack *edit_track;
  EditChain *edit_chain;

  RDListViewItem *item=SingleSelection();
  if(item==NULL) {
    return;
  }
  int id=item->text(13).toInt();
  int line=item->text(14).toInt();
  if(id==END_MARKER_ID) {
    return;
  }
  switch(edit_log_event->logLine(line)->type()) {
  case RDLogLine::Cart:
  case RDLogLine::Macro:
    edit_cart=
      new EditLogLine(edit_log_event->logLine(line),edit_filter,edit_group,
		      edit_schedcode,edit_service_box->currentText(),
		      &edit_group_list,edit_log_event,line,this);
    if(edit_cart->exec()>=0) {
      edit_log_event->refresh(item->text(14).toInt());
      SetLogModified(true);
    }
    delete edit_cart;
    break;

  case RDLogLine::Marker:
    edit_marker=new EditMarker(edit_log_event->logLine(line),this);
    if(edit_marker->exec()>=0) {
      SetLogModified(true);
    }
    delete edit_marker;
    break;

  case RDLogLine::Track:
    edit_track=new EditTrack(edit_log_event->logLine(line),this);
    if(edit_track->exec()>=0) {
      SetLogModified(true);
    }
    delete edit_track;
    break;

  case RDLogLine::Chain:
    edit_chain=new EditChain(edit_log_event->logLine(line),this);
    if(edit_chain->exec()>=0) {
      SetLogModified(true);
    }
    delete edit_chain;
    break;

  default:
    break;
  }
  RefreshList();
  SelectRecord(id);
  UpdateSelection();
}


void EditLog::deleteButtonData()
{
  int count=0;

  QListViewItem *next=edit_log_list->firstChild();
  int line=0;

  while(next!=NULL) {
    if(edit_log_list->isSelected(next)) {
      if(next->text(13).toInt()!=END_MARKER_ID) {
	if(count==0) {
	  line=next->text(14).toInt();
	}
	count++;
      }
    }
    next=next->nextSibling();
  }
  DeleteLines(line,count);
}


void EditLog::upButtonData()
{
  QListViewItem *item=SingleSelection();
  if((item==NULL)||(item->text(14).toInt()==0)||
     (item->text(13).toInt()==END_MARKER_ID)) {
    return;
  }
  int id=item->text(13).toInt();
  sscanf((const char *)item->text(13),"%u",&id);
  edit_log_event->move(item->text(14).toInt(),
		       item->text(14).toInt()-1);
  SetLogModified(true);
  RefreshList();
  SelectRecord(id);
  UpdateSelection();
}


void EditLog::downButtonData()
{
  QListViewItem *item=SingleSelection();

  if((item==NULL)||(item->text(14).toInt()==(edit_log_list->childCount()-2))||
     (item->text(13).toInt()==END_MARKER_ID)) {
    return;
  }
  int id=item->text(13).toInt();
  edit_log_event->move(item->text(14).toInt(),
		       item->text(14).toInt()+1);
  SetLogModified(true);
  RefreshList();
  SelectRecord(id);
  UpdateSelection();
}


void EditLog::cutButtonData()
{
  LoadClipboard(false);
  deleteButtonData();
  UpdateTracks();
  UpdateSelection();
}


void EditLog::copyButtonData()
{
  LoadClipboard(true);
}


void EditLog::pasteButtonData()
{
  QListViewItem *item=SingleSelection();
  if((item==NULL)||(edit_clipboard->size()==0)) {
    return;
  }
  int line=item->text(14).toInt();
  int id=item->text(13).toInt();
  edit_log_event->insert(line,edit_clipboard->size());
  for(unsigned i=0;i<edit_clipboard->size();i++) {
    edit_clipboard->at(i).setId(edit_log_event->logLine(line+i)->id());
    *edit_log_event->logLine(line+i)=edit_clipboard->at(i);
    edit_log_event->logLine(line+i)->setSource(RDLogLine::Manual);
    edit_clipboard->at(i).clearExternalData();
  }
  SetLogModified(true);
  RefreshList();
  UpdateTracks();
  SelectRecord(id);
  UpdateSelection();
}


void EditLog::cartDroppedData(int line,RDLogLine *ll)
{
  RDListViewItem *item=NULL;
  bool appended=false;

  if((line<0)||(line==edit_log_event->size())) {
    line=edit_log_event->size();
    appended=true;
  }
  if(ll->cartNumber()==0) {  // Empty Cart
    if(!appended) {
      DeleteLines(line,1);
    }
    return;
  }
  edit_log_event->insert(line,1);
  edit_log_event->setLogLine(line,ll);
  edit_log_event->logLine(line)->setTransType(edit_default_trans);
  edit_log_event->logLine(line)->setFadeupGain(-3000);
  edit_log_event->logLine(line)->setFadedownGain(-3000);
  edit_log_event->refresh(line);
  SetLogModified(true);
  if(appended) {
    item=(RDListViewItem *)edit_log_list->lastItem();
    item->setText(14,QString().sprintf("%d",item->text(14).toInt()+1));
  }
  else {
    item=(RDListViewItem *)edit_log_list->
      findItem(QString().sprintf("%d",line),14);
    item->setText(14,QString().sprintf("%d",item->text(14).toInt()+1));
    while((item=(RDListViewItem *)item->nextSibling())!=NULL) {
      item->setText(14,QString().sprintf("%d",item->text(14).toInt()+1));
    }
  }
  item=new RDListViewItem(edit_log_list);
  item->setText(14,QString().sprintf("%d",line));
  RefreshLine(item);
  edit_log_list->sort();
  edit_log_list->clearSelection();
  item->setSelected(true);
}


void EditLog::notificationReceivedData(RDNotification *notify)
{
  RDListViewItem *item=NULL;

  if(notify->type()==RDNotification::CartType) {
    unsigned cartnum=notify->id().toUInt();
    item=(RDListViewItem *)edit_log_list->firstChild();
    while(item!=NULL) {
      if(item->text(3).toUInt()==cartnum) {
	int line=item->text(14).toInt();
	if(line>=0) {
	  edit_log_event->refresh(line);
	  RefreshLine(item);
	}
      }
      item=(RDListViewItem *)item->nextSibling();
    }
  }
}


void EditLog::saveData()
{
  if(!ValidateSvc()) {
    if(QMessageBox::warning(this,tr("Invalid Carts"),
			    tr("The log contains carts that are disabled\nfor the selected service!\n\nDo you still want to save?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }
  if(!DeleteTracks()) {
    QMessageBox::warning(this,tr("RDLogEdit"),
			 tr("Unable to save log, audio deletion error!"));
    return;
  }
  SaveLog();
  SetLogModified(false);
  edit_log->setAutoRefresh(edit_autorefresh_box->currentItem()==0);
  edit_log->
    setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
}


void EditLog::saveasData()
{
  if(!ValidateSvc()) {
    if(QMessageBox::warning(this,tr("Invalid Carts"),
			    tr("The log contains carts that are disabled\nfor the selected service!\n\nDo you still want to save?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }
  QString logname;
  QString svcname=edit_service_box->currentText();
  RDAddLog *log=NULL;
  QString err_msg;

  if(rda->user()->createLog()) {
    log=new RDAddLog(&logname,&svcname,RDLogFilter::UserFilter,
		     tr("Add Log"),this);
    if(log->exec()<0) {
      return;
    }
    if(!RDLog::create(logname,svcname,QDate(),rda->ripc()->user(),&err_msg,
		      rda->config())) {
      QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),err_msg);
      return;
    }
    SendNotification(RDNotification::AddAction,logname);
    delete edit_log;
    edit_newlogs->push_back(logname);
    edit_log=new RDLog(logname);
    RDCreateLogTable(RDLog::tableName(logname),rda->config());
    edit_log_event->setLogName(RDLog::tableName(logname));
    for(int i=0;i<edit_service_box->count();i++) {
      if(edit_service_box->text(i)==svcname) {
	edit_service_box->setCurrentItem(i);
      }
    }
    SaveLog();
    edit_logname_label->setText(logname);
    edit_origin_label->
      setText(edit_log->originUser()+QString(" - ")+
	      edit_log->originDatetime().toString("MM/dd/yyyy - hh:mm:ss"));
  }
  delete log;
  RefreshList();
  SetLogModified(false);
  edit_deleted_tracks.clear();
}


void EditLog::renderasData()
{
#ifndef WIN32
  int first_line=-1;
  int last_line=-1;

  QListViewItem *next=edit_log_list->firstChild();
  while(next!=NULL) {
    if(edit_log_list->isSelected(next)) {
      if(next->text(13).toInt()!=END_MARKER_ID) {
	if(first_line<0) {
	  first_line=next->text(14).toInt();
	}
	last_line=next->text(14).toInt();
      }
    }
    next=next->nextSibling();
  }
  if(first_line<0) {
    edit_render_dialog->exec(rda->user(),edit_log_event,0,0);
  }
  else {
    edit_render_dialog->exec(rda->user(),edit_log_event,first_line,last_line+1);
  }
#endif  // WIN32
}


void EditLog::reportsData()
{
  QDate start_date;
  if(edit_startdate_box->isChecked()) {
    start_date=edit_startdate_edit->date();
  }
  QDate end_date;
  if(edit_enddate_box->isChecked()) {
    end_date=edit_enddate_edit->date();
  }
  ListReports *lr=
    new ListReports(edit_log->name(),edit_description_edit->text(),
		    edit_service_box->currentText(),start_date,end_date,
		    edit_autorefresh_box->currentItem()==0,
		    edit_log_event,this);
  lr->exec();
  delete lr;
}


void EditLog::okData()
{
  if(edit_changed) {
    if(!ValidateSvc()) {
      if(QMessageBox::warning(this,tr("Invalid Carts"),
			      tr("The log contains carts that are disabled\nfor the selected service!\n\nDo you still want to save?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
	return;
      }
    }
    SaveLog();
    DeleteTracks();
  }
#ifndef WIN32
  edit_player->stop();
#endif  // WIN32
  for(unsigned i=0;i<edit_clipboard->size();i++) {
    edit_clipboard->at(i).clearExternalData();
  }
  done(true);
}


void EditLog::cancelData()
{
  if(edit_changed) {
    switch(QMessageBox::question(this,
	   tr("RDLogEdit"),
	   tr("The log has been modified.\nDo you want to save your changes?"),
				 QMessageBox::Yes,QMessageBox::No,
				 QMessageBox::Cancel)) {
    case QMessageBox::Yes:
      if(!ValidateSvc()) {
	QMessageBox::warning(this,tr("Invalid Carts"),
			     tr("The log contains carts that are disabled\nfor the selected service!"));
	    return;
      }
      SaveLog();
      break;

    case QMessageBox::Cancel:
    case QMessageBox::NoButton:
      return;
      break;
    }
  }
#ifndef WIN32
  edit_player->stop();
#endif  // WIN32
  for(unsigned i=0;i<edit_clipboard->size();i++) {
    edit_clipboard->at(i).clearExternalData();
  }
  done(false);
}


void EditLog::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditLog::resizeEvent(QResizeEvent *e)
{
  edit_logname_label->setGeometry(155,11,size().width()-500,18);
  edit_logname_label_label->setGeometry(80,11,70,18);
  edit_modified_label->setGeometry(60,14,20,18);
  edit_origin_label->setGeometry(size().width()-300,11,200,18);
  edit_origin_label_label->setGeometry(size().width()-345,11,40,18);
  edit_track_label->setGeometry(size().width()-425,11,40,18);
  edit_track_label_label->setGeometry(size().width()-510,11,80,18);
  edit_description_edit->setGeometry(110,40,size().width()-390,20);
  edit_description_label->setGeometry(10,40,95,20);
  edit_purgedate_box->setGeometry(size().width()-255,42,15,15);
  edit_purgedate_label->setGeometry(size().width()-240,40,60,20);
  edit_purgedate_edit->setGeometry(size().width()-170,40,100,20);
  edit_purgedate_button->setGeometry(size().width()-60,37,50,26);
  edit_service_box->setGeometry(110,68,120,22);
  edit_service_edit->setGeometry(110,68,120,22);
  edit_service_label->setGeometry(10,68,95,22);
  edit_autorefresh_box->setGeometry(180,94,50,22);
  edit_autorefresh_edit->setGeometry(180,94,50,22);
  edit_autorefresh_label->setGeometry(10,94,165,22);
  edit_startdate_edit->setGeometry(315,68,100,22);
  edit_startdate_label->setGeometry(240,68,70,22);
  edit_enddate_edit->setGeometry(490,68,100,22);
  edit_enddate_label->setGeometry(415,68,70,22);
  edit_startdate_box->setGeometry(250,98,15,15);
  edit_enddate_box->setGeometry(430,98,15,15);
  edit_startdate_box_label->setGeometry(270,96,175,20);
  edit_enddate_box_label->setGeometry(450,96,140,20);

  edit_time_label->setGeometry(655,62,75,20);
  edit_stoptime_label->setGeometry(625,82,65,18);
  edit_stoptime_edit->setGeometry(695,82,60,18);
  edit_endtime_label->setGeometry(625,102,65,18);
  edit_endtime_edit->setGeometry(695,102,60,18);

  edit_log_list->setGeometry(10,128,
			    size().width()-20,size().height()-258);
  edit_cart_button->setGeometry(20,size().height()-125,80,50);
  edit_marker_button->setGeometry(110,size().height()-125,80,50);
  edit_edit_button->setGeometry(200,size().height()-125,80,50);
  edit_delete_button->setGeometry(290,size().height()-125,80,50);
  edit_up_button->setGeometry(390,size().height()-125,50,50);
  edit_down_button->setGeometry(450,size().height()-125,50,50);
  edit_cut_button->
    setGeometry(size().width()-280,size().height()-125,80,50);
  edit_copy_button->
    setGeometry(size().width()-190,size().height()-125,80,50);
  edit_paste_button->
    setGeometry(size().width()-100,size().height()-125,80,50);
  edit_save_button->setGeometry(10,size().height()-60,80,50);
  edit_saveas_button->setGeometry(100,size().height()-60,80,50);
  edit_reports_button->setGeometry(300,size().height()-60,80,50);
#ifndef WIN32
  edit_renderas_button->setGeometry(190,size().height()-60,80,50);
  edit_player->playButton()->setGeometry(410,size().height()-60,80,50);
  edit_player->stopButton()->setGeometry(500,size().height()-60,80,50);
#endif  // WIN32
  edit_ok_button->
    setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditLog::paintEvent(QPaintEvent *e)
{
  QColor system_mid_color = colorGroup().mid();
  QPainter *p=new QPainter(this);
  p->fillRect(60,8,size().width()-120,24,QColor(system_mid_color));
  p->fillRect(9,size().height()-130,size().width()-20,60,
	      QColor(system_mid_color));

  p->setPen(black);
  p->setBrush(black);
  p->moveTo(624,70);
  p->lineTo(760,70);
  p->lineTo(760,124);
  p->lineTo(624,124);
  p->lineTo(624,70);

  p->end();
  delete p;
}


void EditLog::DeleteLines(int line,int count)
{
  RDListViewItem *item=NULL;
  RDListViewItem *next=NULL;
  if(count>0) {
    for(int i=line;i<(line+count);i++) {
      if(edit_log_event->logLine(i)->source()==RDLogLine::Tracker) {
	edit_deleted_tracks.
	  push_back(edit_log_event->logLine(i)->cartNumber());
      }
    }
    item=(RDListViewItem *)edit_log_list->
      findItem(QString().sprintf("%d",line),14);
    for(int i=0;i<count;i++) {
      next=(RDListViewItem *)item->nextSibling();
      delete item;
      item=next;
    }
    if(next!=NULL) {
      next->setSelected(true);
    }
    edit_log_event->remove(line,count);
    SetLogModified(true);
    RenumberList(line);
  }
  UpdateTracks();
  UpdateSelection();
}


void EditLog::SaveLog()
{
  edit_log->setDescription(edit_description_edit->text());
  if(edit_purgedate_box->isChecked()) {
    edit_log->setPurgeDate(edit_purgedate_edit->date());
  }
  else {
    edit_log->setPurgeDate(QDate());
  }
  edit_log->setService(edit_service_box->currentText());
  if(edit_startdate_box->isChecked()) {
    edit_log->setStartDate(edit_startdate_edit->date());
  }
  else {
    edit_log->setStartDate(QDate());
  }
  if(edit_enddate_box->isChecked()) {
    edit_log->setEndDate(edit_enddate_edit->date());
  }
  else {
    edit_log->setEndDate(QDate());
  }
  edit_log->setAutoRefresh(edit_autorefresh_box->currentItem()==0);
  edit_log_event->save(rda->config());
  edit_log->
    setModifiedDatetime(QDateTime(QDate::currentDate(),QTime::currentTime()));
  SendNotification(RDNotification::ModifyAction,edit_log->name());
}


void EditLog::RefreshLine(RDListViewItem *item)
{
  int line=item->text(14).toInt();
  if(line<0) {
    return;
  }
  edit_log_event->refresh(line);
  RDLogLine *logline=edit_log_event->logLine(line);
  switch(logline->timeType()) {
  case RDLogLine::Hard:
    item->setText(1,QString("T")+edit_log_event->
		  logLine(line)->startTime(RDLogLine::Logged).
		  toString("hh:mm:ss.zzz").left(10));
    break;

  default:
    if(logline->
       startTime(RDLogLine::Predicted).isNull()) {
      item->setText(1,edit_log_event->
		    blockStartTime(line).
		    toString("hh:mm:ss.zzz").left(10));
    }
    else {
      item->setText(1,edit_log_event->
		    logLine(line)->startTime(RDLogLine::Predicted).
		    toString("hh:mm:ss.zzz").left(10));
    }
    break;
  }
  switch(logline->transType()) {
  case RDLogLine::Play:
    item->setText(2,tr("PLAY"));
    item->setTextColor(2,item->textColor(1),QFont::Normal);
    break;

  case RDLogLine::Stop:
    item->setText(2,tr("STOP"));
    item->setTextColor(2,item->textColor(1),QFont::Normal);
    break;

  case RDLogLine::Segue:
    item->setText(2,tr("SEGUE"));
    if(logline->hasCustomTransition()) {
      item->setTextColor(2,RD_CUSTOM_TRANSITION_COLOR,QFont::Bold);
    }
    else {
      item->setTextColor(2,item->textColor(1),QFont::Normal);
    }
    break;

  default:
    break;
  }
  switch(logline->type()) {
  case RDLogLine::Cart:
    switch(logline->source()) {
    case RDLogLine::Tracker:
      item->setPixmap(0,*edit_track_cart_map);
      break;

    default:
      item->setPixmap(0,*edit_playout_map);
      break;
    }
    item->setText(3,QString().
		  sprintf("%06u",logline->cartNumber()));
    if(logline->title().isEmpty()) {
      item->setText(4,"");
      item->setText(5,tr("[cart not found]"));
    }
    else {
      item->setText(4,logline->groupName());
      item->setTextColor(4,logline->groupColor(),QFont::Bold);
      if((logline->source()!=RDLogLine::Tracker)||
	 logline->originUser().isEmpty()||
	 (!logline->originDateTime().isValid())) {
	item->setText(6,logline->title());
      }
      else {
	item->setText(6,QString().
		      sprintf("%s -- %s %s",
			      (const char *)logline->title(),
			      (const char *)logline->originUser(),
			      (const char *)logline->originDateTime().
			      toString("M/d hh:mm")));
      }
    }
    item->
      setText(5,RDGetTimeLength(logline->forcedLength(),false,false));
    item->setText(7,logline->artist());
    item->setText(8,logline->client());
    item->setText(9,logline->agency());
    item->setText(12,logline->extData());
    break;
	
  case RDLogLine::Macro:
    item->setPixmap(0,*edit_macro_map);
    item->setText(3,QString().
		  sprintf("%06u",logline->cartNumber()));
    if(logline->title().isEmpty()) {
      item->setText(4,"");
      item->setText(6,tr("[cart not found]"));
    }
    else {
      item->setText(4,logline->groupName());
      item->setTextColor(4,logline->groupColor(),QFont::Bold);
      item->setText(6,logline->title());
    }
    item->setText(5,RDGetTimeLength(logline->forcedLength(),false,false));
    item->setText(7,logline->artist());
    item->setText(8,logline->client());
    item->setText(9,logline->agency());
    item->setText(12,logline->extData());
    break;
	
  case RDLogLine::Marker:
    item->setPixmap(0,*edit_notemarker_map);
    item->setText(3,tr("MARKER"));
    item->setText(4,"");
    item->setText(6,RDTruncateAfterWord(edit_log_event->
					logLine(line)->markerComment(),5,true));
    item->setText(10,logline->markerLabel());
    item->setText(12,logline->extData());
    break;

  case RDLogLine::Track:
    item->setPixmap(0,*edit_mic16_map);
    item->setText(3,tr("TRACK"));
    item->setText(4,"");
    item->setText(6,RDTruncateAfterWord(edit_log_event->
					logLine(line)->markerComment(),5,true));
    item->setText(12,logline->extData());
    break;

  case RDLogLine::Chain:
    item->setPixmap(0,*edit_chain_map);
    item->setText(3,tr("LOG CHAIN"));
    item->setText(4,"");
    item->setText(6,logline->markerLabel());
    item->setText(7,RDTruncateAfterWord(edit_log_event->
					logLine(line)->markerComment(),5,true));
    item->setText(12,logline->extData());
    break;

  case RDLogLine::MusicLink:
    item->setPixmap(0,*edit_music_map);
    item->setText(3,tr("LINK"));
    item->setText(4,"");
    item->setText(6,tr("[music import]"));
    item->setText(12,tr("Link Start")+": "+
		  logline->linkStartTime().toString("hh:mm:ss")+", "+
		  tr("Len")+": "+
		  RDGetTimeLength(logline->linkLength(),false,false));
    break;

  case RDLogLine::TrafficLink:
    item->setPixmap(0,*edit_traffic_map);
    item->setText(3,tr("LINK"));
    item->setText(4,"");
    item->setText(6,tr("[traffic import]"));
    item->setText(12,tr("Link Start")+": "+
		  logline->linkStartTime().toString("hh:mm:ss")+", "+
		  tr("Len")+": "+
		  RDGetTimeLength(logline->linkLength(),false,false));
    break;

  default:
    break;
  }
  switch(logline->source()) {
  case RDLogLine::Manual:
    item->setText(11,tr("Manual"));
    break;

  case RDLogLine::Traffic:
    item->setText(11,tr("Traffic"));
    break;

  case RDLogLine::Music:
    item->setText(11,tr("Music"));
    break;

  case RDLogLine::Template:
    item->setText(11,tr("RDLogManager"));
    break;

  case RDLogLine::Tracker:
    item->setText(11,tr("Voice Tracker"));
    break;
  }
  item->setText(13,QString().sprintf("%d",logline->id()));
  UpdateColor(item,logline);
}


void EditLog::RefreshList()
{
  RDListViewItem *l;

  edit_log_list->clear();
  l=new RDListViewItem(edit_log_list);
  l->setText(6,tr("--- end of log ---"));
  l->setText(13,QString().sprintf("%d",END_MARKER_ID));
  l->setText(14,QString().sprintf("%d",edit_log_event->size()));
  for(int i=edit_log_event->size()-1;i>=0;i--) {
    l=new RDListViewItem(edit_log_list);
    l->setText(14,QString().sprintf("%d",i));
    RefreshLine(l);
  }
}


void EditLog::UpdateSelection()
{
  RDListViewItem *rditem=SingleSelection();
  if(rditem==NULL) {  // Multiple items selected?
    edit_endtime_edit->setText("");
    edit_stoptime_label->setText(tr("Selected:"));
    QListViewItem *next=edit_log_list->firstChild();
    int start_line=-1;
    int end_line=-1;
    while(next!=NULL) {
      if(edit_log_list->isSelected(next)) {
	if((start_line<0)&&(next->text(13).toInt()!=END_MARKER_ID)) {
	  start_line=next->text(14).toInt();
	}
	if(next->text(13).toInt()!=END_MARKER_ID) {
	  end_line=next->text(14).toInt();
	}
      }
      next=next->nextSibling();
    }
    if(start_line>=0) {
      edit_stoptime_edit->setText(RDGetTimeLength(edit_log_event->
		      length(start_line,end_line+1),true,false));
    }
    return;
  }
  if(rditem->text(13).toInt()>0) {
    edit_endtime_edit->setText(RDGetTimeLength(edit_log_event->
      length(rditem->text(14).toInt(),edit_log_event->size()),true,false));
    edit_stoptime_label->setText(tr("Next Stop:"));
    int stoplen=edit_log_event->lengthToStop(rditem->text(14).toInt());
    if(stoplen>=0) {
      edit_stoptime_edit->setText(RDGetTimeLength(stoplen,true,false));
    }
    else {
      edit_stoptime_edit->setText("");
    }
  }
  else {
    edit_endtime_edit->setText("");
    edit_stoptime_edit->setText("");
  }
}


bool EditLog::UpdateColor(RDListViewItem *item,RDLogLine *logline)
{
  bool ret=true;
  QDateTime now=QDateTime(QDate::currentDate(),QTime::currentTime());

  switch(logline->type()) {
  case RDLogLine::Cart:
    switch(logline->validity(now)) {
    case RDCart::AlwaysValid:
      if(edit_group_list.isGroupValid(item->text(4))||
	 item->text(4).isEmpty()) {
	item->setBackgroundColor(palette().color(QPalette::Active,
						 QColorGroup::Base));
      }
      else {
	item->setBackgroundColor(RD_CART_INVALID_SERVICE_COLOR);
	ret=false;
      }
      break;
	      
    case RDCart::ConditionallyValid:
      item->setBackgroundColor(RD_CART_CONDITIONAL_COLOR);
      break;
	      
    case RDCart::FutureValid:
      item->setBackgroundColor(RD_CART_FUTURE_COLOR);
      break;
	      
    case RDCart::EvergreenValid:
      item->setBackgroundColor(RD_CART_EVERGREEN_COLOR);
      break;
	      
    case RDCart::NeverValid:
      item->setBackgroundColor(RD_CART_ERROR_COLOR);
      item->setText(6,tr("[INVALID CART]"));
      break;
    }
    break;

  default:
    if(edit_group_list.isGroupValid(item->text(4))||
       item->text(4).isEmpty()) {
      item->setBackgroundColor(palette().color(QPalette::Active,
					       QColorGroup::Base));
    }
    else {
      item->setBackgroundColor(RD_CART_INVALID_SERVICE_COLOR);
      ret=false;
    }
    break;
  }
  return ret;
}


void EditLog::RenumberList(int line)
{
  QListViewItem *prev=NULL;
  QListViewItem *item=edit_log_list->firstChild();
  if(item==NULL) {
    return;
  }
  for(int i=0;i<line;i++) {
    item=item->nextSibling();
  }
  while(item!=NULL) {
    item->setText(14,QString().sprintf("%d",line++));
    prev=item;
    item=item->nextSibling();
  }
  prev->setText(13,QString().sprintf("%d",END_MARKER_ID));
}


void EditLog::SelectRecord(int id)
{
  QListViewItem *item=edit_log_list->firstChild();

  while(item!=NULL) {
    if(item->text(13).toInt()==id) {
      edit_log_list->setSelected(item,true);
      edit_log_list->ensureItemVisible(item);
      return;
    }
    item=item->nextSibling();
  }
}


RDListViewItem *EditLog::SingleSelection()
{
  RDListViewItem *item=NULL;
  RDListViewItem *next=(RDListViewItem *)edit_log_list->firstChild();

  while(next!=NULL) {
    if(edit_log_list->isSelected(next)) {
      if(item==NULL) {
	item=next;
      }
      else {
	return NULL;
      }
    }
    next=(RDListViewItem *)next->nextSibling();
  }
  return item;
}


bool EditLog::ValidateSvc()
{
  RDLogLine *logline=NULL;
  bool valid=true;
  RDListViewItem *item=(RDListViewItem *)edit_log_list->firstChild();
  while(item!=NULL) {
    if((logline=edit_log_event->logLine(item->text(14).toInt()))!=NULL) {
      valid&=UpdateColor(item,logline);
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  return valid;
}


void EditLog::UpdateTracks()
{
  unsigned markers=0;
  unsigned tracks=0;
  RDLogLine *logline;

  for(int i=0;i<edit_log_event->size();i++) {
    logline=edit_log_event->logLine(i);
    if(logline->type()==RDLogLine::Track) {
      markers++;
    }
    if(logline->source()==RDLogLine::Tracker) {
      tracks++;
    }
  }
  edit_track_label->
    setText(QString().sprintf("%u / %u",tracks,markers+tracks));
}


bool EditLog::DeleteTracks()
{
  RDCart *cart;
  for(unsigned i=0;i<edit_deleted_tracks.size();i++) {
    cart=new RDCart(edit_deleted_tracks[i]);
    if(!cart->remove(rda->station(),rda->user(),rda->config())) {
      delete cart;
      return false;
    }
    delete cart;
  }
  return true;
}


void EditLog::LoadClipboard(bool clear_ext)
{
  QListViewItem *next=edit_log_list->firstChild();

  edit_clipboard->clear();
  while(next!=NULL) {
    if((edit_log_list->isSelected(next))&&
       (next->text(13).toInt()!=END_MARKER_ID)) {
      edit_clipboard->
	push_back(*edit_log_event->logLine(next->text(14).toInt()));
      if(clear_ext) {
	edit_clipboard->back().clearExternalData();
      }
    }
    next=next->nextSibling();
  }
}


void EditLog::SetLogModified(bool state)
{
  if(state!=edit_changed) {
    if(state) {
      edit_modified_label->setText("*");
    }
    else {
      edit_modified_label->setText("");
    }
    edit_save_button->setEnabled(state);
    edit_changed=state;
  }
}


void EditLog::SendNotification(RDNotification::Action action,
			       const QString &log_name)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,
					    action,QVariant(log_name));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}
