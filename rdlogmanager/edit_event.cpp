// edit_event.cpp
//
// Edit a Rivendell Log Event
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

#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>

#include <rdcartfilter.h>
#include <rdcart_search_text.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include "add_event.h"
#include "edit_event.h"
#include "edit_perms.h"

EditEvent::EditEvent(QString eventname,bool new_event,
		     std::vector<QString> *new_events,QWidget *parent)
  : RDDialog(parent)
{
  event_saved=false;
  event_name=eventname;
  event_new_event=new_event;
  event_new_events=new_events;
  event_event=new RDEvent(eventname);

  setWindowTitle("RDLogManager - "+tr("Editing Event")+" - "+
		 event_event->name());

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  // *******************************
  // Library Section
  // *******************************
  //
  // Text Filter
  //
  event_lib_filter_edit=new QLineEdit(this);
  if(rda->station()->filterMode()==RDStation::FilterAsynchronous) {
    event_lib_filter_edit->setGeometry(55,2,CENTER_LINE-135,20);
    connect(event_lib_filter_edit,SIGNAL(returnPressed()),
	    this,SLOT(searchData()));
  }
  else {
    event_lib_filter_edit->setGeometry(55,2,CENTER_LINE-70,20);
  }
  connect(event_lib_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  QLabel *label=new QLabel(tr("Filter:"),this);
  label->setFont(labelFont());
  label->setGeometry(5,2,45,20);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  event_search_button=new QPushButton(tr("Search"),this);
  event_search_button->setFont(subButtonFont());
  event_search_button->setGeometry(CENTER_LINE-70,2,60,20);
  event_search_button->setDisabled(true);
  connect(event_search_button,SIGNAL(clicked()),this,SLOT(searchData()));
  event_search_button->
    setVisible(rda->station()->filterMode()==RDStation::FilterAsynchronous);

  //
  // Group Filter
  //
  event_group_box=new QComboBox(this);
  event_group_box->setGeometry(55,25,CENTER_LINE-70,20);
  event_group_model=new RDGroupListModel(true,false,this);
  event_group_model->changeUser();
  event_group_box->setModel(event_group_model);
  connect(event_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(filterActivatedData(const QString &)));
  label=new QLabel(tr("Group:"),this);
  label->setFont(labelFont());
  label->setGeometry(5,25,45,20);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  //
  // Type Filter
  //
  event_lib_type_group=new QButtonGroup(this);
  connect(event_lib_type_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(filterClickedData(int)));
  QRadioButton *rbutton=new QRadioButton(this);
  rbutton->setGeometry(55,55,15,15);
  event_lib_type_group->addButton(rbutton,0);
  label=new QLabel(tr("All"),this);
  label->setFont(labelFont());
  label->setGeometry(75,55,30,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  rbutton->setChecked(true);

  rbutton=new QRadioButton(this);
  rbutton->setGeometry(125,55,15,15);
  event_lib_type_group->addButton(rbutton,1);
  label=new QLabel(tr("Audio Only"),this);
  label->setFont(labelFont());
  label->setGeometry(145,55,80,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  rbutton=new QRadioButton(this);
  rbutton->setGeometry(235,55,15,15);
  event_lib_type_group->addButton(rbutton,2);
  label=new QLabel(tr("Macros Only"),this);
  label->setFont(labelFont());
  label->setGeometry(255,55,80,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Cart List
  //
  event_lib_view=new LibraryTableView(this);
  event_lib_view->setGeometry(10,80,CENTER_LINE-20,sizeHint().height()-300);
  event_lib_view->setDragEnabled(true);
  event_lib_model=new RDLibraryModel(this);
  event_lib_model->setFont(font());
  event_lib_model->setPalette(palette());
  event_lib_view->setModel(event_lib_model);
  event_lib_view->hideColumn(3);
  connect(event_lib_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));
  connect(event_lib_model,SIGNAL(modelReset()),
	  event_lib_view,SLOT(resizeColumnsToContents()));

  //
  // Empty Cart Source
  //
  event_empty_cart=new RDEmptyCart(this);
  event_empty_cart->setGeometry(CENTER_LINE-227,sizeHint().height()-202,32,32);

  //
  // Cart Player
  //
  QString sql;
  RDSqlQuery *q;
  event_player = NULL;
  sql=QString("select ")+
    "`OUTPUT_CARD`,"+  // 00
    "`OUTPUT_PORT`,"+  // 01
    "`START_CART`,"+   // 02
    "`END_CART` "+     // 03
    "from `RDLOGEDIT` where "+
    "`STATION`='"+RDEscapeString(rda->station()->name())+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    event_player=
      new RDSimplePlayer(rda->cae(),rda->ripc(),q->value(0).toInt(),
			 q->value(1).toInt(),q->value(2).toUInt(),
			 q->value(3).toUInt(),this);
    event_player->playButton()->
      setGeometry(CENTER_LINE-180,sizeHint().height()-210,80,50);
    event_player->
      stopButton()->setGeometry(CENTER_LINE-90,sizeHint().height()-210,80,50);
    event_player->stopButton()->setOnColor(Qt::red);
  }
  delete q;

  //
  // Remarks
  //
  event_remarks_edit=new QTextEdit(this);
  event_remarks_edit->
    setGeometry(10,sizeHint().height()-150,CENTER_LINE-20,140);
  event_remarks_edit->setAcceptRichText(false);
  label=new QLabel(tr("USER NOTES"),this);
  label->setFont(labelFont());
  label->setGeometry(15,sizeHint().height()-165,100,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Load Group List
  //
  RefreshLibrary();

  // *******************************
  // Pre-Position Log Section
  // *******************************
  event_position_group=new QGroupBox(tr("PRE-POSITION LOG"),this);
  event_position_group->setFont(labelFont());
  event_position_group->
    setGeometry(CENTER_LINE+10,1,sizeHint().width()-CENTER_LINE-15,43);

  event_position_box=new QCheckBox(this);
  event_position_box->setGeometry(CENTER_LINE+15,21,15,22);
  //  event_position_box->setGeometry(CENTER_LINE+15,19,15,22);
  connect(event_position_box,SIGNAL(toggled(bool)),
	  this,SLOT(prepositionToggledData(bool)));
  event_position_label=new QLabel(tr("Cue to this event"),this);
  event_position_label->setFont(labelFont());
  event_position_label->setGeometry(CENTER_LINE+35,20,150,22);
  event_position_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_position_edit=new QTimeEdit(this);
  event_position_edit->setGeometry(CENTER_LINE+144,20,60,22);
  event_position_edit->setDisplayFormat("mm:ss");
  event_position_unit=new QLabel(tr("before scheduled start.  (First cart will have a STOP transition.)"),
				 this);
  event_position_unit->setFont(labelFont());
  event_position_unit->setGeometry(CENTER_LINE+212,21,
				   sizeHint().width()-CENTER_LINE-227,22);
  event_position_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  // *******************************
  // Timed Start Section
  // *******************************
  event_timetype_group=new QGroupBox(tr("TIMED START"),this);
  event_timetype_group->setFont(labelFont());
  event_timetype_group->
    setGeometry(CENTER_LINE+10,49,sizeHint().width()-CENTER_LINE-15,66);

  //
  // Time Type
  //
  event_timetype_check=new QCheckBox(this);
  event_timetype_check->setGeometry(CENTER_LINE+15,70,15,15);
  event_timetype_label=new QLabel(tr("Use hard start time"),this);
  event_timetype_label->setGeometry(CENTER_LINE+35,69,120,16);
  event_timetype_label->setFont(labelFont());
  event_timetype_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Grace Time
  //
  event_grace_groupbox=
    new QGroupBox(tr("Action If Previous Event Still Playing"),this);
  event_grace_groupbox->setFont(labelFont());
  event_grace_groupbox->
    setGeometry(CENTER_LINE+170,69,sizeHint().width()-CENTER_LINE-200,42);
  event_grace_group=new QButtonGroup(this);
  event_immediate_button=new QRadioButton(tr("Start immediately"),this);
  event_immediate_button->setGeometry(CENTER_LINE+180,92,160,15);
  event_immediate_button->setFont(subLabelFont());
  event_grace_group->addButton(event_immediate_button,0);
  event_next_button=new QRadioButton(tr("Make next"),this);
  event_next_button->setGeometry(CENTER_LINE+320,92,160,15);
  event_next_button->setFont(subLabelFont());
  event_grace_group->addButton(event_next_button,1);
  event_wait_button=new QRadioButton(tr("Wait up to"),this);
  event_wait_button->setGeometry(CENTER_LINE+430,92,160,15);
  event_wait_button->setFont(subLabelFont());
  event_grace_group->addButton(event_wait_button,2);

  event_grace_edit=new QTimeEdit(this);
  event_grace_edit->setGeometry(CENTER_LINE+510,89,60,20);
  event_grace_edit->setDisplayFormat("mm:ss");
  connect(event_timetype_check,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(event_grace_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(graceClickedData(int)));

  // *******************************
  // Transitions Section
  // *******************************
  event_transitions_group=new QGroupBox(tr("TRANSITIONS"),this);
  event_transitions_group->setFont(labelFont());
  event_transitions_group->
    setGeometry(CENTER_LINE+10,120,sizeHint().width()-CENTER_LINE-15,63);

  //
  // First Cart Transition Type
  //
  event_firsttrans_label=
    new QLabel(tr("First cart has a"),this);
  event_firsttrans_label->setFont(labelFont());
  event_firsttrans_label->
    setGeometry(CENTER_LINE+15,140,
		labelFontMetrics()->width(event_firsttrans_label->text()+" "),
		20);
  event_firsttrans_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_firsttrans_box=new QComboBox(this);
  event_firsttrans_box->setGeometry(event_firsttrans_label->geometry().x()+
				    event_firsttrans_label->geometry().width(),
				    140,90,20);
  event_firsttrans_box->insertItem(0,tr("Play"));
  event_firsttrans_box->insertItem(1,tr("Segue"));
  event_firsttrans_box->insertItem(2,tr("Stop"));
  event_firsttrans_unit=new QLabel("transition.",this);
  event_firsttrans_unit->setFont(labelFont());
  event_firsttrans_unit->
    setGeometry(event_firsttrans_box->geometry().x()+
		event_firsttrans_box->geometry().width()+5,140,
		labelFontMetrics()->width(tr("transition.")),20);
  event_firsttrans_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  connect(event_firsttrans_box,SIGNAL(activated(int)),
	  this,SLOT(timeTransitionData(int)));
  
  //
  // Default Transition Type
  //
  event_defaulttrans_label=new QLabel(tr("Imported carts have a"),this);
  event_defaulttrans_label->setFont(labelFont());
  event_defaulttrans_label->
    setGeometry(CENTER_LINE+15,161,
		labelFontMetrics()->width(event_defaulttrans_label->text()+" "),
		20);
  event_defaulttrans_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_defaulttrans_box=new QComboBox(this);
  event_defaulttrans_box->
    setGeometry(event_defaulttrans_label->geometry().x()+
		event_defaulttrans_label->geometry().width(),161,90,20);
  event_defaulttrans_box->insertItem(0,tr("Play"));
  event_defaulttrans_box->insertItem(1,tr("Segue"));
  event_defaulttrans_box->insertItem(2,tr("Stop"));
  event_defaulttrans_unit=new QLabel(tr("transition."),this);
  event_defaulttrans_unit->setFont(labelFont());
  event_defaulttrans_unit->
    setGeometry(event_defaulttrans_box->geometry().x()+
		event_defaulttrans_box->geometry().width()+5,161,
		labelFontMetrics()->width(tr("transition.")),20);
  event_defaulttrans_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  // *******************************
  // Enforcing Length Section
  // *******************************
  event_autofill_group=new QGroupBox(tr("ENFORCING LENGTH"),this);
  event_autofill_group->setFont(labelFont());
  event_autofill_group->
    setGeometry(CENTER_LINE+10,188,sizeHint().width()-CENTER_LINE-15,43);

  event_autofill_box=new QCheckBox(this);
  event_autofill_box->setGeometry(CENTER_LINE+15,210,15,15);
  connect(event_autofill_box,SIGNAL(toggled(bool)),
	  this,SLOT(autofillToggledData(bool)));
  label=new QLabel(tr("Use AutoFill"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+35,210,150,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  event_autofill_slop_box=new QCheckBox(this);
  event_autofill_slop_box->setGeometry(CENTER_LINE+210,210,15,15);
  connect(event_autofill_slop_box,SIGNAL(toggled(bool)),
	  this,SLOT(autofillWarnToggledData(bool)));
  event_autofill_slop_label1=
    new QLabel(tr("Warn if fill is over or under"),this);
  event_autofill_slop_label1->setFont(labelFont());
  event_autofill_slop_label1->
    setGeometry(CENTER_LINE+230,210,
		labelFontMetrics()->width(event_autofill_slop_label1->text()),
		15);
  event_autofill_slop_label1->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  event_autofill_slop_edit=new QTimeEdit(this);
  event_autofill_slop_edit->setDisplayFormat("mm:ss");
  event_autofill_slop_label=new QLabel(tr("by at least")+" ",this);
  event_autofill_slop_label->
    setGeometry(event_autofill_slop_label1->geometry().x()+
	   event_autofill_slop_label1->geometry().width(),210,
	   labelFontMetrics()->width(event_autofill_slop_label->text()+" "),15);
  event_autofill_slop_edit->
    setGeometry(event_autofill_slop_label->geometry().x()+
		event_autofill_slop_label->geometry().width(),207,60,22);
  event_autofill_slop_label->setFont(labelFont());
  event_autofill_slop_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  event_timescale_box=new QCheckBox(this);
  event_timescale_box->setGeometry(CENTER_LINE+250,210,15,15);
  label=new QLabel(tr("Use Timescaling"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+270,214,150,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_timescale_box->hide();
  label->hide();

  // *******************************
  // Cart Stack Section
  // *******************************
  event_stack_group=new QGroupBox(tr("CART STACK"),this);
  event_stack_group->setFont(labelFont());
  event_stack_group->
    setGeometry(CENTER_LINE+10,235,sizeHint().width()-CENTER_LINE-15,408);
  //
  // Pre-Import Carts Section
  //
  label=new QLabel(tr("PRE-IMPORT CARTS"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+15,255,200,16);

  //
  // Pre-Import Carts List
  //
  event_preimport_length_edit=new QLineEdit(this);
  event_preimport_length_edit->setGeometry(sizeHint().width()-140,253,80,20);
  event_preimport_length_edit->setReadOnly(true);
  label=new QLabel(tr("Len:"),this);
  label->setFont(labelFont());
  label->setGeometry(sizeHint().width()-330,255,185,16);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  event_preimport_view=new ImportCartsView(this);
  event_preimport_view->setGeometry(CENTER_LINE+15,272,
				    sizeHint().width()-CENTER_LINE-75,115);
  event_preimport_view->setDragEnabled(true);
  event_preimport_model=
    new ImportCartsModel(event_name,ImportCartsModel::PreImport,true,this);
  event_preimport_model->setFont(font());
  event_preimport_model->setPalette(palette());
  event_preimport_view->setModel(event_preimport_model);
  connect(event_preimport_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));
  connect(event_preimport_model,SIGNAL(modelReset()),
	  event_preimport_view,SLOT(resizeColumnsToContents()));
  connect(event_preimport_view,SIGNAL(cartDropped(int,RDLogLine *)),
	  event_preimport_model,SLOT(processCartDrop(int,RDLogLine *)));
  connect(event_preimport_model,SIGNAL(totalLengthChanged(int)),
	  this,SLOT(preimportLengthChangedData(int)));
  event_preimport_view->resizeColumnsToContents();
  preimportLengthChangedData(event_preimport_model->totalLength());

  event_preimport_up_button=new RDTransportButton(RDTransportButton::Up,this);
  event_preimport_up_button->setGeometry(sizeHint().width()-50,277,40,40);
  connect(event_preimport_up_button,SIGNAL(clicked()),
	  this,SLOT(preimportUpData()));
  event_preimport_down_button=
    new RDTransportButton(RDTransportButton::Down,this);
  event_preimport_down_button->setGeometry(sizeHint().width()-50,342,40,40);
  connect(event_preimport_down_button,SIGNAL(clicked()),
	  this,SLOT(preimportDownData()));

  //
  // Imports
  //
  label=new QLabel(tr("IMPORT"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+15,392,200,16);
  
  event_source_group=new QButtonGroup(this);
  connect(event_source_group,SIGNAL(buttonClicked(int)),
	  this,SLOT(importClickedData(int)));
  rbutton=new QRadioButton(this);
  event_source_group->addButton(rbutton,RDEventLine::None);
  rbutton->setGeometry(CENTER_LINE+80,392,15,15);
  label=new QLabel(tr("None"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+100,392,150,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  rbutton=new QRadioButton(this);
  event_source_group->addButton(rbutton,RDEventLine::Traffic);
  rbutton->setGeometry(CENTER_LINE+160,392,15,15);
  label=new QLabel(tr("From Traffic"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+180,392,150,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  rbutton=new QRadioButton(this);
  rbutton->setGeometry(CENTER_LINE+280,392,15,15);
  event_source_group->addButton(rbutton,RDEventLine::Music);
  label=new QLabel(tr("From Music"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+300,392,150,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  rbutton=new QRadioButton(this);
  rbutton->setGeometry(CENTER_LINE+400,392,15,15);
  event_source_group->addButton(rbutton,RDEventLine::Scheduler);
  label=new QLabel(tr("Select from:"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+420,392,150,15);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  //
  // Scheduler Group
  //
  event_sched_group_box=new RDComboBox(this);
  event_sched_group_box->setGeometry(CENTER_LINE+510,389,100,20);
  QString sql2="select `NAME` from `GROUPS` order by `NAME`";
  RDSqlQuery *q2=new RDSqlQuery(sql2);
  while(q2->next()) {
    event_sched_group_box->insertItem(q2->value(0).toString());
  }
  delete q2;

  // 
  // Artist Separation SpinBox
  //
  event_artist_sep_label=new QLabel(tr("Artist Separation"),this);
  event_artist_sep_label->setFont(defaultFont());
  event_artist_sep_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  event_artist_sep_label->setGeometry(CENTER_LINE+410,412,100,20);
  
  event_artist_sep_spinbox=new QSpinBox(this);
  event_artist_sep_spinbox->setGeometry(CENTER_LINE+515,413,53,20);
  event_artist_sep_spinbox->setMinimum( -1 );
  event_artist_sep_spinbox->setMaximum( 50000 );
  event_artist_sep_spinbox->setSpecialValueText("None");

  event_artist_none_button=new QPushButton(this);
  event_artist_none_button->setGeometry(CENTER_LINE+575,413,40,20);
  event_artist_none_button->setFont(subButtonFont());
  event_artist_none_button->setText(tr("None"));
  connect(event_artist_none_button,SIGNAL(clicked()),this,SLOT(artistData()));

  //
  // Title Separation SpinBox
  //
  event_title_sep_label=new QLabel(tr("Title Separation"),this);
  event_title_sep_label->setFont(defaultFont());
  event_title_sep_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  event_title_sep_label->setGeometry(CENTER_LINE+410,434,100,20);
  
  event_title_sep_spinbox=new QSpinBox(this);
  event_title_sep_spinbox->setGeometry(CENTER_LINE+515,434,53,20);
  event_title_sep_spinbox->setMinimum( -1 );
  event_title_sep_spinbox->setMaximum( 50000 );
  event_title_sep_spinbox->setSpecialValueText("None");

  event_title_none_button=new QPushButton(this);
  event_title_none_button->setGeometry(CENTER_LINE+575,434,40,20);
  event_title_none_button->setFont(subButtonFont());
  event_title_none_button->setText(tr("None"));
  connect(event_title_none_button,SIGNAL(clicked()),this,SLOT(titleData()));

  //
  // Must have code..
  //
  event_have_code_label=new QLabel(tr("Must have code"),this);
  event_have_code_label->setFont(defaultFont());
  event_have_code_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  event_have_code_label->setGeometry(CENTER_LINE+410,457,100,20);
  
  event_have_code_box=new RDComboBox(this);
  event_have_code_box->setGeometry(CENTER_LINE+515,457,100,20);

  //
  // And code
  //
  event_have_code2_label=new QLabel(tr("and code"),this);
  event_have_code2_label->setFont(defaultFont());
  event_have_code2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  event_have_code2_label->setGeometry(CENTER_LINE+410,478,100,20);

  event_have_code2_box=new RDComboBox(this);
  event_have_code2_box->setGeometry(CENTER_LINE+515,478,100,20);

  //
  // Fill scheduler codes
  //
  event_have_code_box->insertItem(tr("[None]"));
  event_have_code2_box->insertItem(tr("[None]"));

  sql2="select `CODE` from `SCHED_CODES` order by `CODE`";
  q2=new RDSqlQuery(sql2);
  while(q2->next()) {
    event_have_code_box->insertItem(q2->value(0).toString());
    event_have_code2_box->insertItem(q2->value(0).toString());
  }
  delete q2;

  //
  // Start Slop Time
  //
  event_startslop_label=new QLabel(tr("Import carts scheduled"),this);
  event_startslop_label->setFont(defaultFont());
  event_startslop_label->setGeometry(CENTER_LINE+30,413,140,22);
  event_startslop_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_startslop_edit=new QTimeEdit(this);
  event_startslop_edit->setGeometry(CENTER_LINE+166,413,60,22);
  event_startslop_edit->setDisplayFormat("mm:ss");
  event_startslop_unit=new QLabel(tr("prior to the start of this event."),this);
  event_startslop_unit->setFont(defaultFont());
  event_startslop_unit->setGeometry(CENTER_LINE+230,413,
		     sizeHint().width()-CENTER_LINE-450,22);
  event_startslop_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  //
  // End Slop Time
  //
  event_endslop_label=new QLabel(tr("Import carts scheduled"),this);
  event_endslop_label->setFont(defaultFont());
  event_endslop_label->setGeometry(CENTER_LINE+30,434,140,22);
  event_endslop_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_endslop_edit=new QTimeEdit(this);
  event_endslop_edit->setGeometry(CENTER_LINE+166,434,60,22);
  event_endslop_edit->setDisplayFormat("mm:ss");
  event_endslop_unit=new QLabel(tr("after the end of this event."),this);
  event_endslop_unit->setFont(defaultFont());
  event_endslop_unit->setGeometry(CENTER_LINE+230,434,
				  sizeHint().width()-CENTER_LINE-460,22);
  event_endslop_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  
  //
  // Nested Event
  //
  event_nestevent_label=new QLabel(tr("Import inline traffic event"),this);
  event_nestevent_label->setFont(defaultFont());
  event_nestevent_label->setGeometry(CENTER_LINE+35,459,190,20);
  event_nestevent_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  event_nestevent_box=new QComboBox(this);
  event_nestevent_box->setGeometry(CENTER_LINE+30,478,365,20);
  event_nestevent_box->insertItem(0,tr("[none]"));

  //
  // Post-Import Carts Section
  //
  label=new QLabel(tr("POST-IMPORT CARTS"),this);
  label->setFont(labelFont());
  label->setGeometry(CENTER_LINE+15,510,200,16);
  
  //
  // Post-Import Carts List
  //
  event_postimport_length_edit=new QLineEdit(this);
  event_postimport_length_edit->setGeometry(sizeHint().width()-140,506,80,20);
  event_postimport_length_edit->setReadOnly(true);
  label=new QLabel(tr("Len:"),this);
  label->setFont(labelFont());
  label->setGeometry(sizeHint().width()-330,508,185,16);
  label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  event_postimport_view=new ImportCartsView(this);
  event_postimport_view->setGeometry(CENTER_LINE+15,525,
				     sizeHint().width()-CENTER_LINE-75,125-10);
  event_postimport_view->setDragEnabled(true);
  event_postimport_model=
    new ImportCartsModel(event_name,ImportCartsModel::PostImport,false,this);
  event_postimport_model->setFont(font());
  event_postimport_model->setPalette(palette());
  event_postimport_view->setModel(event_postimport_model);
  connect(event_postimport_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));
  connect(event_postimport_model,SIGNAL(modelReset()),
	  event_postimport_view,SLOT(resizeColumnsToContents()));
  connect(event_postimport_view,SIGNAL(cartDropped(int,RDLogLine *)),
	  event_postimport_model,SLOT(processCartDrop(int,RDLogLine *)));
  connect(event_postimport_model,SIGNAL(totalLengthChanged(int)),
	  this,SLOT(postimportLengthChangedData(int)));
  event_postimport_view->resizeColumnsToContents();
  postimportLengthChangedData(event_postimport_model->totalLength());

  /*
  event_postimport_up_button=new RDTransportButton(RDTransportButton::Up,this);
  event_postimport_up_button->setGeometry(sizeHint().width()-50,275,40,40);
  connect(event_postimport_up_button,SIGNAL(clicked()),
	  this,SLOT(postimportUpData()));
  event_postimport_down_button=
    new RDTransportButton(RDTransportButton::Down,this);
  event_postimport_down_button->setGeometry(sizeHint().width()-50,340,40,40);
  connect(event_postimport_down_button,SIGNAL(clicked()),
	  this,SLOT(postimportDownData()));
  */
  event_postimport_up_button=
    new RDTransportButton(RDTransportButton::Up,this);
  event_postimport_up_button->setGeometry(sizeHint().width()-50,532,40,40);
  connect(event_postimport_up_button,SIGNAL(clicked()),
	  this,SLOT(postimportUpData()));
  event_postimport_down_button=
    new RDTransportButton(RDTransportButton::Down,this);
  event_postimport_down_button->setGeometry(sizeHint().width()-50,592,40,40);
  connect(event_postimport_down_button,SIGNAL(clicked()),
	  this,SLOT(postimportDownData()));
  
  //
  //  Save Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+10,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Save"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+100,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Save As"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveAsData()));
  button->setDisabled(new_event);

  //
  //  Service Association Button
  //
  button=new QPushButton(this);
  button->setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2-85,
		      sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Services\nList"));
  connect(button,SIGNAL(clicked()),this,SLOT(svcData()));

  //
  //  Color Button
  //
  event_color_button=new QPushButton(this);
  event_color_button->
    setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2+5,
		sizeHint().height()-60,80,50);
  event_color_button->setFont(buttonFont());
  event_color_button->setText(tr("Color"));
  connect(event_color_button,SIGNAL(clicked()),this,SLOT(colorData()));
  event_color=palette().color(QPalette::Background);
  
  //
  //  OK Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    button->setDefault(true);
  }
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Event
  //
  event_remarks_edit->setText(event_event->remarks());
  int pos=event_event->preposition();
  if(pos<0) {
    event_position_box->setChecked(false);
    prepositionToggledData(false);
  }
  else {
    event_position_box->setChecked(true);
    event_position_edit->setTime(QTime(0,0,0).addMSecs(pos));
    prepositionToggledData(true);
  }
  
  int grace=0;
  switch(event_event->timeType()) {
  case RDLogLine::Relative:
    event_timetype_check->setChecked(false);
    event_grace_group->button(0)->setChecked(true);
    timeToggledData(false);
    break;
	
  case RDLogLine::Hard:
    event_timetype_check->setChecked(true);
    switch((grace=event_event->graceTime())) {
    case 0:
      event_grace_group->button(0)->setChecked(true);
      event_grace_edit->setTime(QTime());
      break;
	      
    case -1:
      event_grace_group->button(1)->setChecked(true);
      event_grace_edit->setTime(QTime());
      break;
	      
    default:
      event_grace_group->button(2)->setChecked(true);
      event_grace_edit->setTime(QTime(0,0,0).addMSecs(grace));
      break;
    }

  case RDLogLine::NoTime:
    break;
  }
  
  event_autofill_box->setChecked(event_event->useAutofill());
  int autofill_slop=event_event->autofillSlop();
  if(autofill_slop>=0) {
    event_autofill_slop_box->setChecked(true);
    event_autofill_slop_edit->setTime(QTime(0,0,0).addMSecs(autofill_slop));
  }
  autofillToggledData(event_autofill_box->isChecked());
  event_timescale_box->setChecked(event_event->useTimescale());
  event_source_group->button(event_event->importSource())->setChecked(true);
  event_startslop_edit->setTime(QTime(0,0,0).addMSecs(event_event->startSlop()));
  event_endslop_edit->setTime(QTime(0,0,0).addMSecs(event_event->endSlop()));
  event_firsttrans_box->setCurrentIndex(event_event->firstTransType());
  event_defaulttrans_box->setCurrentIndex(event_event->defaultTransType());
  if(!event_event->schedGroup().isEmpty()) {
    event_sched_group_box->setCurrentText(event_event->schedGroup());
  }
  event_artist_sep_spinbox->setValue(event_event->artistSep());
  event_title_sep_spinbox->setValue(event_event->titleSep());
  if(event_have_code_box->findText(event_event->HaveCode())!=-1) {
    event_have_code_box->setCurrentText(event_event->HaveCode());
  }
  if(event_have_code2_box->findText(event_event->HaveCode2())!=-1) {
    event_have_code2_box->setCurrentText(event_event->HaveCode2());
  }
  event_color=event_event->color();
  if(event_color.isValid()) {
    event_color_button->setPalette(QPalette(event_color,palette().color(QPalette::Background)));
  }
  QString str=event_event->nestedEvent();
  sql=QString("select `NAME` from `EVENTS` where ")+
    "`NAME`!='"+RDEscapeString(eventname)+"'"+
    "order by `NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_nestevent_box->insertItem(event_nestevent_box->count(),
				    q->value(0).toString());
    if(q->value(0).toString()==str) {
      event_nestevent_box->setCurrentIndex(event_nestevent_box->count()-1);
    }
  }
  delete q;
  prepositionToggledData(event_position_box->isChecked());
  timeToggledData(event_timetype_check->isChecked());
  importClickedData(event_source_group->checkedId());
}


EditEvent::~EditEvent()
{
  delete event_lib_view;
  delete event_preimport_view;
  delete event_preimport_model;
  delete event_postimport_view;
  delete event_postimport_model;
}


QSize EditEvent::sizeHint() const
{
  return QSize(1024,715);
} 


QSizePolicy EditEvent::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditEvent::filterChangedData(const QString &str)
{
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    RefreshLibrary();
  }
  else {
    event_search_button->setEnabled(true);
  }
}


void EditEvent::filterActivatedData(const QString &str)
{
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    RefreshLibrary();
  }
  else {
    event_search_button->setEnabled(true);
  }
}


void EditEvent::filterClickedData(int id)
{
  if(rda->station()->filterMode()==RDStation::FilterSynchronous) {
    RefreshLibrary();
  }
  else {
    event_search_button->setEnabled(true);
  }
}


void EditEvent::searchData()
{
  RefreshLibrary();
  event_search_button->setDisabled(true);
}


void EditEvent::selectionChangedData(const QItemSelection &before,
				     const QItemSelection &after)
{
  QModelIndexList rows=event_lib_view->selectionModel()->selectedRows();

  if(event_player==NULL) {
    return;
  }
  if(rows.size()!=1) {
    event_player->setCart(0);
    return;
  }
  event_player->setCart(event_lib_model->cartNumber(rows.first()));
}


void EditEvent::prepositionToggledData(bool state)
{
  event_position_edit->setEnabled(state);

  //
  // TIMED START Section
  //
  event_timetype_check->setDisabled(state);
  event_timetype_group->setDisabled(state);
  event_timetype_label->setDisabled(state);
  if(event_timetype_check->isChecked()) {
    event_grace_groupbox->setDisabled(state);
    event_immediate_button->setDisabled(state);
    event_next_button->setDisabled(state);
    event_wait_button->setDisabled(state);
  }

  //
  // TRANSITIONS Section
  //
  if(state) {
    event_firsttrans_box->setCurrentIndex(2);
  }
  event_firsttrans_box->setDisabled(state);
  event_firsttrans_label->setDisabled(state);
  event_firsttrans_unit->setDisabled(state);
}


void EditEvent::timeToggledData(bool state)
{
  event_grace_groupbox->setEnabled(state);
  event_immediate_button->setEnabled(state);
  event_next_button->setEnabled(state);
  event_wait_button->setEnabled(state);
  event_grace_edit->setEnabled(state);
  if(state) {
    graceClickedData(event_grace_group->checkedId());
    timeTransitionData(2);
    event_position_box->setDisabled(true);
    event_position_edit->setDisabled(true);
    event_position_group->setDisabled(true);
    event_position_label->setDisabled(true);
    event_position_unit->setDisabled(true);
  }
  else {
    event_grace_edit->setDisabled(true);
    if(event_position_box->isChecked()) {
      event_position_edit->setEnabled(true);
    }
    event_position_box->setEnabled(true);
    event_position_group->setEnabled(true);
    event_position_label->setEnabled(true);
    event_position_unit->setEnabled(true);
  }
}


void EditEvent::graceClickedData(int id)
{
  switch(id) {
  case 0:
    timeTransitionData(RDLogLine::Stop);
    event_grace_edit->setDisabled(true);
    break;

  case 1:
    timeTransitionData(RDLogLine::Segue);
    event_grace_edit->setDisabled(true);
    break;

  case 2:
    timeTransitionData(RDLogLine::Segue);
    event_grace_edit->setEnabled(true);
    break;
  }
}


void EditEvent::timeTransitionData(int id)
{
  //event_preimport_list->refreshList();
}


void EditEvent::autofillToggledData(bool state)
{
  event_autofill_slop_box->setEnabled(state);
  event_autofill_slop_label1->setEnabled(state);
  if(state) {
    autofillWarnToggledData(event_autofill_slop_box->isChecked());
  }
  else {
    autofillWarnToggledData(false);
  }
}


void EditEvent::autofillWarnToggledData(bool state)
{
  event_autofill_slop_edit->setEnabled(state);
  event_autofill_slop_label->setEnabled(state);
}


void EditEvent::importClickedData(int id)
{
  bool state=true;
  bool statesched=true;
  bool stateschedinv=false;
  if(id==0) {
    state=false;
    statesched=false;
  }
  if(id==3) {
    state=false;
    statesched=false;
    stateschedinv=true;
  }
  event_startslop_edit->setEnabled(statesched);
  event_startslop_label->setEnabled(statesched);
  event_startslop_unit->setEnabled(statesched);
  event_endslop_edit->setEnabled(statesched);
  event_endslop_label->setEnabled(statesched);
  event_endslop_unit->setEnabled(statesched);
  event_defaulttrans_box->setEnabled(state);
  event_defaulttrans_label->setEnabled(state);
  event_defaulttrans_unit->setEnabled(state);
  state=(id==2)&&state;
  event_nestevent_label->setEnabled(state);
  event_nestevent_box->setEnabled(state);
  event_sched_group_box->setEnabled(stateschedinv);
  event_artist_sep_label->setEnabled(stateschedinv);
  event_artist_sep_spinbox->setEnabled(stateschedinv);
  event_artist_none_button->setEnabled(stateschedinv);
  event_title_sep_label->setEnabled(stateschedinv);
  event_title_sep_spinbox->setEnabled(stateschedinv);
  event_title_none_button->setEnabled(stateschedinv);
  event_have_code_box->setEnabled(stateschedinv);
  event_have_code_label->setEnabled(stateschedinv);
  event_have_code2_box->setEnabled(stateschedinv);
  event_have_code2_label->setEnabled(stateschedinv);
}


void EditEvent::preimportLengthChangedData(int msecs)
{
  event_preimport_length_edit->setText(RDGetTimeLength(msecs,true,false));
}


void EditEvent::preimportUpData()
{
  QModelIndexList rows=event_preimport_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(event_preimport_model->moveUp(rows.first())) {
    event_preimport_view->selectRow(rows.first().row()-1);
  }
}


void EditEvent::preimportDownData()
{
  QModelIndexList rows=event_preimport_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(event_preimport_model->moveDown(rows.first())) {
    event_preimport_view->selectRow(rows.first().row()+1);
  }
}


void EditEvent::postimportUpData()
{
  QModelIndexList rows=event_postimport_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(event_postimport_model->moveUp(rows.first())) {
    event_postimport_view->selectRow(rows.first().row()-1);
  }
}


void EditEvent::postimportDownData()
{
  QModelIndexList rows=event_postimport_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(event_postimport_model->moveDown(rows.first())) {
    event_postimport_view->selectRow(rows.first().row()+1);
  }
}


void EditEvent::postimportLengthChangedData(int msecs)
{
  event_postimport_length_edit->setText(RDGetTimeLength(msecs,true,false));
}


void EditEvent::artistData()
{
  event_artist_sep_spinbox->setValue(-1);
}


void EditEvent::titleData()
{
  event_title_sep_spinbox->setValue(-1);
}


void EditEvent::saveData()
{
  Save();
  event_new_event=false;
}


void EditEvent::saveAsData()
{
  QString old_name;
  QString str;

  old_name=event_name;
  AddEvent *add_dialog=new AddEvent(&event_name,this);
  if(add_dialog->exec()<0) {
    delete add_dialog;
    return;
  }
  delete add_dialog;
  QString sql=QString("select `NAME` from `EVENTS` where ")+
    "`NAME`='"+RDEscapeString(event_name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete event_event;
    event_event=new RDEvent(event_name,true);
    Save();
    event_new_events->push_back(event_name);
    CopyEventPerms(old_name,event_name);
    if(event_new_event) {
      AbandonEvent(old_name);
    }
    setWindowTitle("RDLogManager - "+tr("Editing Event")+" - "+
		   event_event->name());
  }
  else {
    if(QMessageBox::question(this,tr("RDLogManager"),
		   tr("Event already exists!\nDo you want to overwrite it?"),
		   QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    delete event_event;
    event_event=new RDEvent(event_name,true);
    Save();
    event_new_events->push_back(event_name);
    sql=QString("delete from `EVENT_PERMS` where ")+
      "`EVENT_NAME`='"+RDEscapeString(event_name)+"'";
    q=new RDSqlQuery(sql);
    delete q;
    CopyEventPerms(old_name,event_name);
    if(event_new_event) {
      AbandonEvent(old_name);
    }
    str=QString(tr("Edit Event"));
    setWindowTitle("RDLogManager - "+tr("Edit Event")+" - "+
		   event_event->name());
  }
}


void EditEvent::svcData()
{
  EditPerms *dialog=new EditPerms(event_name,EditPerms::ObjectEvent,this);
  dialog->exec();
  delete dialog;
}


void EditEvent::colorData()
{
  QColor color=
    QColorDialog::getColor(event_color_button->palette().color(QPalette::Background),this);
  if(color.isValid()) {
    event_color=color;
    event_color_button->setPalette(QPalette(color,palette().color(QPalette::Background)));
  }
}


void EditEvent::okData()
{
  Save();
  if (event_player){
    event_player->stop();
  }

  done(0);
}


void EditEvent::cancelData()
{
  if (event_player){
    event_player->stop();
  }
  if(event_saved) {
    done(-1);
  }
  else {
    done(-2);
  }
}


void EditEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditEvent::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->drawLine(CENTER_LINE,10,CENTER_LINE,sizeHint().height()-10);
  p->drawLine(CENTER_LINE+408,408,CENTER_LINE+408,493);
  p->end();
}


void EditEvent::RefreshLibrary()
{
  QString sql=QString("where ")+
    RDCartFilter::typeFilter(event_lib_type_group->button(0)->isChecked()||
			     event_lib_type_group->button(1)->isChecked(),
			     event_lib_type_group->button(0)->isChecked()||
			     event_lib_type_group->button(2)->isChecked(),
			     RDCart::All)+
    RDCartFilter::phraseFilter(event_lib_filter_edit->text(),false)+
    RDCartFilter::groupFilter(event_group_box->currentText(),
			      event_group_model->allGroupNames());
  sql=sql.left(sql.length()-3);
  event_lib_model->setFilterSql(sql,RD_MAX_CART_NUMBER+1);
}


void EditEvent::Save()
{
  QString properties;

  event_event->setRemarks(event_remarks_edit->toPlainText());
  if(event_position_box->isChecked()) {
    event_event->setPreposition(QTime(0,0,0).msecsTo(event_position_edit->time()));
  }
  else {
    event_event->setPreposition(-1);
  }
  if(event_timetype_check->isChecked()) {
    event_event->setTimeType(RDLogLine::Hard);
    event_event->setFirstTransType((RDLogLine::TransType)
				   event_firsttrans_box->currentIndex());
    switch(event_grace_group->checkedId()) {
    case 0:
      event_event->setGraceTime(0);
      break;

    case 1:
      event_event->setGraceTime(-1);
      break;

    default:
      event_event->setGraceTime(QTime(0,0,0).msecsTo(event_grace_edit->time()));
      break;	  
    }
  }
  else {
    event_event->setTimeType(RDLogLine::Relative);
    event_event->setGraceTime(0);
    event_event->setFirstTransType(RDLogLine::Play);
  }

  event_event->setUseAutofill(event_autofill_box->isChecked());
  if(event_autofill_slop_box->isChecked()) {
    event_event->
      setAutofillSlop(QTime(0,0,0).msecsTo(event_autofill_slop_edit->time()));
  }
  else {
    event_event->setAutofillSlop(-1);
  }
  event_event->setUseTimescale(event_timescale_box->isChecked());
  event_event->
    setImportSource((RDEventLine::ImportSource)event_source_group->checkedId());
  event_event->setStartSlop(QTime(0,0,0).msecsTo(event_startslop_edit->time()));
  event_event->setEndSlop(QTime(0,0,0).msecsTo(event_endslop_edit->time()));
  if(!event_timetype_check->isChecked()) {
    event_event->
      setFirstTransType((RDLogLine::TransType)event_firsttrans_box->
			currentIndex());
  }
  event_event->
    setDefaultTransType((RDLogLine::TransType)event_defaulttrans_box->
  			currentIndex());
  event_event->setColor(event_color);
  if(event_nestevent_box->currentIndex()==0) {
    event_event->setNestedEvent("");
  }
  else {
    event_event->setNestedEvent(event_nestevent_box->currentText());
  }
  event_event->setSchedGroup(event_sched_group_box->currentText());  
  event_event->setArtistSep(event_artist_sep_spinbox->value());
  event_event->setTitleSep(event_title_sep_spinbox->value());
  event_event->setHaveCode("");
  event_event->setHaveCode2("");
  if(event_have_code_box->currentIndex()>0) {
    event_event->setHaveCode(event_have_code_box->currentText());
  }
  if(event_have_code2_box->currentIndex()>0) {
    event_event->setHaveCode2(event_have_code2_box->currentText());
  }

  //
  // If both codes are the same, remove second code
  //
  if (event_event->HaveCode()==event_event->HaveCode2()) {
    event_event->setHaveCode2("");
  }

  //
  // Save second code as first code when first code isn't defined
  //
  if (event_event->HaveCode().isEmpty()) {
    event_event->setHaveCode(event_event->HaveCode2());
    event_event->setHaveCode2("");
  }

  event_preimport_model->
    save((RDLogLine::TransType)event_firsttrans_box->currentIndex());
  event_postimport_model->save();

  event_saved=true;
}


QString EditEvent::GetProperties()
{
  int prepos_msec=-1;
  RDLogLine::TimeType time_type=RDLogLine::Relative;
  int grace_msec=-1;

  if(event_position_box->isChecked()) {
    prepos_msec=QTime(0,0,0).msecsTo(event_position_edit->time());
  }
  if(event_timetype_check->isChecked()) {
    time_type=RDLogLine::Hard;
  }
  if(event_timetype_check->isChecked()) {
    switch(event_grace_group->checkedId()) {
    case 0:
      grace_msec=0;
      break;

    case 1:
      grace_msec=-1;
      break;

    default:
      grace_msec=QTime(0,0,0).msecsTo(event_grace_edit->time());
      break;	  
    }
  }

  return RDEventLine::
    propertiesText(prepos_msec,
		   (RDLogLine::TransType)event_firsttrans_box->currentIndex(),
		   time_type,
		   grace_msec,
		   event_autofill_box->isChecked(),
		   (RDEventLine::ImportSource)event_source_group->checkedId(),
		   event_nestevent_box->currentIndex()>0);
}


void EditEvent::CopyEventPerms(QString old_name,QString new_name)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("select `SERVICE_NAME` from `EVENT_PERMS` where ")+
    "`EVENT_NAME`='"+RDEscapeString(old_name)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString("insert into `EVENT_PERMS` set ")+
      "`EVENT_NAME`='"+RDEscapeString(new_name)+"',"+
      "`SERVICE_NAME`='"+RDEscapeString(q->value(0).toString())+"'";
    q1=new RDSqlQuery(sql);
    delete q1;
  }
  delete q;
}


void EditEvent::AbandonEvent(QString name)
{
  if(name==event_name) {
    return;
  }
  QString sql=QString("delete from `EVENTS` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from `EVENT_PERMS` where ")+
    "`EVENT_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from `EVENT_LINES` where ")+
    "`EVENT_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
}
