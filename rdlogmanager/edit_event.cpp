// edit_event.cpp
//
// Edit a Rivendell Log Event
//
//   (C) Copyright 2002-2004,2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_event.cpp,v 1.53.2.4.2.1 2014/06/24 18:27:06 cvs Exp $
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
#include <qcombobox.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qcolordialog.h>

#include <rddb.h>
#include <rd.h>
#include <rdconf.h>
#include <rdcart.h>
#include <rdcart_search_text.h>
#include <rdescape_string.h>

#include <globals.h>
#include <add_event.h>
#include <edit_event.h>
#include <edit_perms.h>

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"


EditEvent::EditEvent(QString eventname,bool new_event,
		     std::vector<QString> *new_events,
		     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString str;

  event_saved=false;
  event_name=eventname;
  event_new_event=new_event;
  event_new_events=new_events;
  event_event=new RDEvent(eventname);
  str=QString(tr("Editing Event"));
  setCaption(QString().
	     sprintf("%s - %s",(const char *)str,
		     (const char *)event_event->name()));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",10,QFont::Bold);
  bold_font.setPixelSize(10);
  QFont font=QFont("Helvetica",10,QFont::Normal);
  font.setPixelSize(10);

  //
  // Create Icons
  //
  event_playout_map=new QPixmap(play_xpm);
  event_macro_map=new QPixmap(rml5_xpm);

  //
  // Library Section
  //
  // Text Filter
  //
  event_lib_filter_edit=new QLineEdit(this,"event_lib_filter_edit");
  event_lib_filter_edit->setGeometry(55,10,CENTER_LINE-70,14);
  connect(event_lib_filter_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));
  QLabel *label=new QLabel(event_lib_filter_edit,tr("Filter:"),
			   this,"event_lib_filter_label");
  label->setFont(bold_font);
  label->setGeometry(10,10,40,14);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Group Filter
  //
  event_group_box=new QComboBox(this,"event_group_box");
  event_group_box->setGeometry(55,30,CENTER_LINE-70,18);
  connect(event_group_box,SIGNAL(activated(const QString &)),
	  this,SLOT(filterActivatedData(const QString &)));
  label=new QLabel(event_group_box,tr("Group:"),
		   this,"event_lib_filter_label");
  label->setFont(bold_font);
  label->setGeometry(10,30,40,18);
  label->setAlignment(AlignVCenter|AlignRight);

  //
  // Type Filter
  //
  event_lib_type_group=new QButtonGroup(this,"cart_type_filter");
  event_lib_type_group->setExclusive(true);
  event_lib_type_group->hide();
  connect(event_lib_type_group,SIGNAL(clicked(int)),this,SLOT(filterClickedData(int)));

  QRadioButton *rbutton=new QRadioButton(this,"all_button");
  rbutton->setGeometry(55,55,15,15);
  event_lib_type_group->insert(rbutton);
  label=new QLabel(rbutton,tr("All"),this,"all_button");
  label->setFont(bold_font);
  label->setGeometry(75,55,30,15);
  label->setAlignment(AlignVCenter|AlignLeft);
  rbutton->setChecked(true);

  rbutton=new QRadioButton(this,"audio_button");
  rbutton->setGeometry(125,55,15,15);
  event_lib_type_group->insert(rbutton);
  label=new QLabel(rbutton,tr("Audio Only"),this,"audio_button_label");
  label->setFont(bold_font);
  label->setGeometry(145,55,80,15);
  label->setAlignment(AlignVCenter|AlignLeft);

  rbutton=new QRadioButton(this,"macro_button");
  rbutton->setGeometry(235,55,15,15);
  event_lib_type_group->insert(rbutton);
  label=new QLabel(rbutton,tr("Macros Only"),this,"macro_button_label");
  label->setFont(bold_font);
  label->setGeometry(255,55,80,15);
  label->setAlignment(AlignVCenter|AlignLeft);

  //
  // Cart List
  //
  event_lib_list=new LibListView(this,"event_lib_list");
  event_lib_list->setGeometry(10,80,CENTER_LINE-20,sizeHint().height()-300);
  event_lib_list->setAllColumnsShowFocus(true);
  event_lib_list->setItemMargin(5);
  event_lib_list->addColumn("");
  event_lib_list->addColumn(tr("CART"));
  event_lib_list->addColumn(tr("GROUP"));
  event_lib_list->addColumn(tr("LENGTH"));
  event_lib_list->setColumnAlignment(3,AlignRight);
  event_lib_list->addColumn(tr("TITLE"));
  event_lib_list->addColumn(tr("ARTIST"));
  event_lib_list->addColumn(tr("START"));
  event_lib_list->addColumn(tr("END"));
  event_lib_list->addColumn(tr("TYPE"));
  connect(event_lib_list,SIGNAL(clicked(QListViewItem *)),
	  this,SLOT(cartClickedData(QListViewItem *)));
  
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
#ifndef WIN32
  sql=QString().sprintf("select OUTPUT_CARD,OUTPUT_PORT,START_CART,END_CART \
                       from RDLOGEDIT where STATION=\"%s\"",
			(const char *)rdstation_conf->name());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    event_player=
      new RDSimplePlayer(rdcae,rdripc,q->value(0).toInt(),q->value(1).toInt(),
			 q->value(2).toUInt(),q->value(3).toUInt(),
			 this,"event_player");
    event_player->playButton()->
      setGeometry(CENTER_LINE-180,sizeHint().height()-210,80,50);
    event_player->stopButton()->
      setPalette(QPalette(backgroundColor(),QColor(lightGray)));
    event_player->stopButton()->setGeometry(CENTER_LINE-90,sizeHint().height()-210,80,50);
    event_player->stopButton()->setOnColor(red);
  }
  delete q;
#endif  // WIN32  

  //
  // Remarks
  //
  event_remarks_edit=new QTextEdit(this,"event_remarks_edit");
  event_remarks_edit->
    setGeometry(10,sizeHint().height()-150,CENTER_LINE-20,140);
  event_remarks_edit->setTextFormat(QTextEdit::PlainText);
  label=new QLabel(event_remarks_edit,tr("REMARKS"),this,"event_remarks_label");
  label->setFont(bold_font);
  label->setGeometry(15,sizeHint().height()-165,100,15);
  label->setAlignment(AlignVCenter|AlignLeft);

  //
  // Load Group List
  //
  sql="select NAME from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  event_group_box->insertItem(tr("ALL"));
  while(q->next()) {
    event_group_box->insertItem(q->value(0).toString());
  }
  delete q;

  RefreshLibrary();

  //
  // Pre-Position Log Section
  //
  event_position_header=new QLabel(tr("PRE-POSITION LOG"),
				   this,"event_position_header");
  event_position_header->setFont(bold_font);
  event_position_header->setGeometry(CENTER_LINE+15,10,200,16);

  event_position_box=new QCheckBox(this,"event_position_box");
  event_position_box->setGeometry(CENTER_LINE+15,27,15,22);
  connect(event_position_box,SIGNAL(toggled(bool)),
	  this,SLOT(prepositionToggledData(bool)));
  event_position_label=new QLabel(event_position_box,tr("Cue to this event"),
		   this,"event_position_label");
  event_position_label->setFont(bold_font);
  event_position_label->setGeometry(CENTER_LINE+35,27,150,22);
  event_position_label->setAlignment(AlignVCenter|AlignLeft);
  event_position_edit=new QTimeEdit(this,"event_position_edit");
  event_position_edit->setGeometry(CENTER_LINE+144,26,60,22);
  event_position_edit->setDisplay(QTimeEdit::Minutes|QTimeEdit::Seconds);
  event_position_unit=new QLabel(event_position_box,
     tr("before scheduled start.  (First cart will have a STOP transition.)"),
		   this,"event_position_unit");
  event_position_unit->setFont(bold_font);
  event_position_unit->setGeometry(CENTER_LINE+212,27,
				   sizeHint().width()-CENTER_LINE-232,22);
  event_position_unit->setAlignment(AlignVCenter|AlignLeft);

  //
  // Timed Start Section
  //
  event_timetype_header=new QLabel(tr("TIMED START"),
				   this,"event_timetype_header");
  event_timetype_header->setFont(bold_font);
  event_timetype_header->setGeometry(CENTER_LINE+15,62,200,16);

  //
  // Time Type
  //
  event_timetype_box=new QCheckBox(this,"event_timetype_box");
  event_timetype_box->setGeometry(CENTER_LINE+15,85,15,15);
  event_timetype_label=new QLabel(event_timetype_box,tr("Use hard start time"),
			   this,"event_timetype_label");
  event_timetype_label->setGeometry(CENTER_LINE+35,84,120,16);
  event_timetype_label->setFont(bold_font);
  event_timetype_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Post Point
  //
  event_post_box=new QCheckBox(this,"event_post_box");
  event_post_box->setGeometry(CENTER_LINE+35,108,15,15);
  event_post_label=new QLabel(event_post_box,tr("Make Post Point"),
			   this,"event_post_label");
  event_post_label->setGeometry(CENTER_LINE+55,107,95,16);
  event_post_label->setFont(bold_font);
  event_post_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Grace Time
  //
  event_grace_group=
    new QButtonGroup(1,Qt::Vertical,
		     tr("Action If Previous Event Still Playing"),
		     this,"event_grace_group");
  event_grace_group->hide();

  event_grace_group->setFont(bold_font);
  event_grace_group->setRadioButtonExclusive(true);
  event_grace_label=new QLabel(tr("Action If Previous Event Still Playing"),
			       this,"event_grace_label");
  event_grace_label->setGeometry(CENTER_LINE+180,75,225,16);
  event_grace_label->setFont(bold_font);
  event_grace_label->setAlignment(AlignCenter);
  event_immediate_button=new QRadioButton(tr("Start immediately"),this);
  event_immediate_button->setGeometry(CENTER_LINE+170,100,160,15);
  event_immediate_button->setFont(font);
  event_grace_group->insert(event_immediate_button);
  event_next_button=new QRadioButton(tr("Make next"),this);
  event_next_button->setGeometry(CENTER_LINE+310,100,160,15);
  event_next_button->setFont(font);
  event_grace_group->insert(event_next_button);
  event_wait_button=new QRadioButton(tr("Wait up to"),this);
  event_wait_button->setGeometry(CENTER_LINE+420,100,160,15);
  event_wait_button->setFont(font);
  event_grace_group->insert(event_wait_button);

  event_grace_edit=new QTimeEdit(this,"event_grace_edit");
  event_grace_edit->setGeometry(CENTER_LINE+500,95,60,20);
  event_grace_edit->setDisplay(QTimeEdit::Minutes|QTimeEdit::Seconds);
  connect(event_timetype_box,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(event_grace_group,SIGNAL(clicked(int)),
	  this,SLOT(graceClickedData(int)));

  //
  // Transition Type
  //
  event_transtype_box=new QComboBox(this,"event_transtype_box");
  event_transtype_box->setGeometry(CENTER_LINE+390,131,110,26);
  event_transtype_box->insertItem(tr("Play"));
  event_transtype_box->insertItem(tr("Segue"));
  event_transtype_box->insertItem(tr("Stop"));
//  event_transtype_box->setCurrentItem(2);
  connect(event_transtype_box,SIGNAL(activated(int)),
	  this,SLOT(timeTransitionData(int)));
  event_time_label=
    new QLabel(event_transtype_box,
	       tr("Transition if previous event ends before start time:"),
	       this,"event_transtype_label");
  event_time_label->setGeometry(CENTER_LINE+15,131,370,26);
  event_time_label->setFont(bold_font);
  event_time_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Enforcing Length Section
  //
  label=new QLabel(tr("ENFORCING LENGTH"),this,"enforcing_length_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+15,162,200,16);

  event_autofill_box=new QCheckBox(this,"event_autofill_box");
  event_autofill_box->setGeometry(CENTER_LINE+100,182,15,15);
  label=new QLabel(event_autofill_box,tr("Use AutoFill"),
		   this,"event_autofill_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+120,184,150,15);
  label->setAlignment(AlignVCenter|AlignLeft);

  event_autofill_slop_box=new QCheckBox(this,"event_autofill_slop_box");
  event_autofill_slop_box->setGeometry(CENTER_LINE+210,182,15,15);
  connect(event_autofill_slop_box,SIGNAL(toggled(bool)),
	  this,SLOT(autofillWarnToggledData(bool)));
  event_autofill_slop_label1=new QLabel(event_autofill_slop_box,
					tr("Warn if fill is over or under"),
					this,"event_autofill_slop_label1");
  event_autofill_slop_label1->setFont(font);
  event_autofill_slop_label1->setGeometry(CENTER_LINE+230,184,140,15);
  event_autofill_slop_label1->setAlignment(AlignLeft|AlignVCenter);

  event_autofill_slop_edit=new QTimeEdit(this,"event_autofill_slop_edit");
  event_autofill_slop_edit->setGeometry(CENTER_LINE+440,179,60,22);
  event_autofill_slop_edit->setDisplay(QTimeEdit::Minutes|QTimeEdit::Seconds);
  event_autofill_slop_label=
    new QLabel(event_autofill_slop_edit,
	       tr("by at least"),this,"event_autofill_slop_edit");
  event_autofill_slop_label->setGeometry(CENTER_LINE+370,184,65,15);
  event_autofill_slop_label->setFont(font);
  event_autofill_slop_label->setAlignment(AlignRight|AlignVCenter);

  event_timescale_box=new QCheckBox(this,"event_timescale_box");
  event_timescale_box->setGeometry(CENTER_LINE+250,182,15,15);
  label=new QLabel(event_timescale_box,tr("Use Timescaling"),
		   this,"event_timescale_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+270,182,150,15);
  label->setAlignment(AlignVCenter|AlignLeft);
  event_timescale_box->hide();
  label->hide();

  //
  // Pre-Import Carts Section
  //
  label=new QLabel(tr("PRE-IMPORT CARTS"),this,"preimport_carts_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+15,210,200,16);

  //
  // Pre-Import Carts List
  //
  event_preimport_length_edit=new QLineEdit(this,"event_preimport_length_edit");
  event_preimport_length_edit->setGeometry(sizeHint().width()-140,208,80,20);
  event_preimport_length_edit->setReadOnly(true);
  label=new QLabel(event_preimport_length_edit,tr("Len:"),
		   this,"event_preimport_length_label");
  label->setFont(bold_font);
  label->setGeometry(sizeHint().width()-330,210,185,16);
  label->setAlignment(AlignVCenter|AlignRight);


  event_preimport_list=new ImportListView(this,"event_preimport_list");
  event_preimport_list->setGeometry(CENTER_LINE+15,227,
				    sizeHint().width()-CENTER_LINE-75,125);
  event_preimport_list->setAllColumnsShowFocus(true);
  event_preimport_list->setItemMargin(5);
  event_preimport_list->logEvent()->
    setLogName(QString().sprintf("%s_PRE",(const char *)event_name).
	       replace(' ',"_"));
  event_preimport_list->setSortColumn(-1);
  connect(event_preimport_list,SIGNAL(sizeChanged(int)),
	  this,SLOT(preimportChangedData(int)));
  event_preimport_list->addColumn("");
  event_preimport_list->addColumn(tr("CART"));
  event_preimport_list->addColumn(tr("GROUP"));
  event_preimport_list->addColumn(tr("LENGTH"));
  event_preimport_list->setColumnAlignment(3,AlignRight);
  event_preimport_list->addColumn(tr("TITLE"));
  event_preimport_list->addColumn(tr("TRANSITION"));
  event_preimport_list->addColumn(tr("COUNT"));
  connect(event_preimport_list,SIGNAL(clicked(QListViewItem *)),
	  this,SLOT(cartClickedData(QListViewItem *)));
  connect(event_preimport_list,SIGNAL(lengthChanged(int)),
	  this,SLOT(preimportLengthChangedData(int)));
  event_preimport_up_button=
    new RDTransportButton(RDTransportButton::Up,this,
			 "event_preimport_up_button");
  event_preimport_up_button->setGeometry(sizeHint().width()-50,237,40,40);
  connect(event_preimport_up_button,SIGNAL(clicked()),
	  this,SLOT(preimportUpData()));
  event_preimport_down_button=
    new RDTransportButton(RDTransportButton::Down,this,
			 "event_preimport_down_button");
  event_preimport_down_button->setGeometry(sizeHint().width()-50,302,40,40);
  connect(event_preimport_down_button,SIGNAL(clicked()),
	  this,SLOT(preimportDownData()));

  //
  // Import Section
  //
  label=new QLabel(tr("IMPORT"),this,"preimport_carts_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+15,362,200,16);
  
  event_source_group=new QButtonGroup(this,"event_source_group");
  event_source_group->hide();
  connect(event_source_group,SIGNAL(clicked(int)),
	  this,SLOT(importClickedData(int)));
  rbutton=new QRadioButton(this,"event_noimport_button");
  event_source_group->insert(rbutton);
  rbutton->setGeometry(CENTER_LINE+100,362,15,15);
  label=new QLabel(rbutton,tr("None"),
		   this,"event_noimport_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+120,362,150,15);
  label->setAlignment(AlignVCenter|AlignLeft);
  
  rbutton=new QRadioButton(this,"event_traffic_button");
  event_source_group->insert(rbutton);
  rbutton->setGeometry(CENTER_LINE+200,362,15,15);
  label=new QLabel(rbutton,tr("From Traffic"),
		   this,"event_traffic_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+220,362,150,15);
  label->setAlignment(AlignVCenter|AlignLeft);
  
  rbutton=new QRadioButton(this,"event_music_button");
  rbutton->setGeometry(CENTER_LINE+300,362,15,15);
  event_source_group->insert(rbutton);
  label=new QLabel(rbutton,tr("From Music"),
		   this,"event_music_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+320,362,150,15);
  label->setAlignment(AlignVCenter|AlignLeft);
  
  rbutton=new QRadioButton(this,"event_scheduler_button");
  rbutton->setGeometry(CENTER_LINE+400,362,15,15);
  event_source_group->insert(rbutton);
  label=new QLabel(rbutton,tr("Select from:"),
		   this,"event_scheduler_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+420,362,150,15);
  label->setAlignment(AlignVCenter|AlignLeft);
  

// Scheduler Group

  event_sched_group_box=new QComboBox(this,"event_sched_group_box");
  event_sched_group_box->setGeometry(CENTER_LINE+510,359,100,20);
  QString sql2="select NAME from GROUPS order by NAME";
  RDSqlQuery *q2=new RDSqlQuery(sql2);
  while(q2->next()) {
    event_sched_group_box->insertItem(q2->value(0).toString());
  }
  delete q2;
 
// Title Separation SpinBox

  event_title_sep_label=
    new QLabel(tr("Title Separation"),this,"event_title_sep_label");
  event_title_sep_label->setFont(bold_font);
  event_title_sep_label->setGeometry(CENTER_LINE+420,383,100,20);
  

  event_title_sep_spinbox = new QSpinBox( this, "event_title_sep_spinbox" );
  event_title_sep_spinbox->setGeometry(CENTER_LINE+510,383,50,20);
  event_title_sep_spinbox->setMinValue( 0 );
  event_title_sep_spinbox->setMaxValue( 50000 );


// Must have code..

  event_have_code_label=
    new QLabel(tr("Must have code"),this,"event_have_code_label");
  event_have_code_label->setFont(bold_font);
  event_have_code_label->setGeometry(CENTER_LINE+420,404,100,20);
  
  event_have_code_box=new QComboBox(this,"event_have_code_box");
  event_have_code_box->setGeometry(CENTER_LINE+510,404,100,20);
  event_have_code_box->insertItem("");
  sql2="select CODE from SCHED_CODES order by CODE";
  q2=new RDSqlQuery(sql2);
  while(q2->next()) {
    event_have_code_box->insertItem(q2->value(0).toString());
  }
  delete q2;


  //
  // Start Slop Time
  //
  event_startslop_label=
    new QLabel(tr("Import carts scheduled"),this,"start_slop_label");
  event_startslop_label->setFont(bold_font);
  event_startslop_label->setGeometry(CENTER_LINE+30,383,140,22);
  event_startslop_label->setAlignment(AlignVCenter|AlignLeft);
  event_startslop_edit=new QTimeEdit(this,"event_startslop_edit");
  event_startslop_edit->setGeometry(CENTER_LINE+171,383,60,22);
  event_startslop_edit->setDisplay(QTimeEdit::Minutes|QTimeEdit::Seconds);
  event_startslop_unit=new QLabel(tr("prior to the start of this event."),
		   this,"event_startslop_label");
  event_startslop_unit->setFont(bold_font);
  event_startslop_unit->setGeometry(CENTER_LINE+235,383,
		     sizeHint().width()-CENTER_LINE-460,22);
  event_startslop_unit->setAlignment(AlignVCenter|AlignLeft);
  
  //
  // End Slop Time
  //
  event_endslop_label=
    new QLabel(tr("Import carts scheduled"),this,"end_slop_label");
  event_endslop_label->setFont(bold_font);
  event_endslop_label->setGeometry(CENTER_LINE+30,404,140,22);
  event_endslop_label->setAlignment(AlignVCenter|AlignLeft);
  event_endslop_edit=new QTimeEdit(this,"event_endslop_edit");
  event_endslop_edit->setGeometry(CENTER_LINE+171,404,60,22);
  event_endslop_edit->setDisplay(QTimeEdit::Minutes|QTimeEdit::Seconds);
  event_endslop_unit=new QLabel(tr("after the end of this event."),
		   this,"event_endslop_label");
  event_endslop_unit->setFont(bold_font);
  event_endslop_unit->setGeometry(CENTER_LINE+235,404,
		     sizeHint().width()-CENTER_LINE-460,22);
  event_endslop_unit->setAlignment(AlignVCenter|AlignLeft);
  
  //
  // First Cart Transition Type
  //
  event_firsttrans_label=
    new QLabel(tr("First cart has a"),this,"first_cart_label");
  event_firsttrans_label->setFont(bold_font);
  event_firsttrans_label->setGeometry(CENTER_LINE+30,428,140,22);
  event_firsttrans_label->setAlignment(AlignVCenter|AlignLeft);
  event_firsttrans_box=new QComboBox(this,"event_firsttrans_box");
  event_firsttrans_box->setGeometry(CENTER_LINE+123,428,90,22);
  event_firsttrans_box->insertItem(tr("Play"));
  event_firsttrans_box->insertItem(tr("Segue"));
  event_firsttrans_box->insertItem(tr("Stop"));
  event_firsttrans_unit=new QLabel("transition.",this,"first_cart_label");
  event_firsttrans_unit->setFont(bold_font);
  event_firsttrans_unit->setGeometry(CENTER_LINE+215,428,
		     sizeHint().width()-CENTER_LINE-450,22);
  event_firsttrans_unit->setAlignment(AlignVCenter|AlignLeft);
  
  //
  // Default Transition Type
  //
  event_defaulttrans_label=new QLabel(tr("Imported carts have a"),
				      this,"default_cart_label");
  event_defaulttrans_label->setFont(bold_font);
  event_defaulttrans_label->setGeometry(CENTER_LINE+30,451,180,22);
  event_defaulttrans_label->setAlignment(AlignVCenter|AlignLeft);
  event_defaulttrans_box=new QComboBox(this,"event_endslop_edit");
  event_defaulttrans_box->setGeometry(CENTER_LINE+163,451,90,22);
  event_defaulttrans_box->insertItem(tr("Play"));
  event_defaulttrans_box->insertItem(tr("Segue"));
  event_defaulttrans_box->insertItem(tr("Stop"));
  event_defaulttrans_unit=new QLabel(tr("transition."),
				     this,"default_cart_unit");
  event_defaulttrans_unit->setFont(bold_font);
  event_defaulttrans_unit->setGeometry(CENTER_LINE+255,451,
		     sizeHint().width()-CENTER_LINE-420,22);
  event_defaulttrans_unit->setAlignment(AlignVCenter|AlignLeft);
  
  //
  // Nested Event
  //
  event_nestevent_label=new QLabel(tr("Import inline traffic with the"),
				      this,"default_cart_label");
  event_nestevent_label->setFont(bold_font);
  event_nestevent_label->setGeometry(CENTER_LINE+30,474,190,22);
  event_nestevent_label->setAlignment(AlignVCenter|AlignLeft);
  event_nestevent_box=new QComboBox(this,"event_endslop_edit");
  event_nestevent_box->setGeometry(CENTER_LINE+183,474,365,22);
  event_nestevent_box->insertItem(tr("[none]"));
  event_nestevent_unit=new QLabel(tr("event."),
				     this,"default_cart_unit");
  event_nestevent_unit->setFont(bold_font);
  event_nestevent_unit->setGeometry(CENTER_LINE+553,474,40,22);
  event_nestevent_unit->setAlignment(AlignVCenter|AlignLeft);
  
  //
  // Post-Import Carts Section
  //
  label=new QLabel(tr("POST-IMPORT CARTS"),this,"postimport_carts_label");
  label->setFont(bold_font);
  label->setGeometry(CENTER_LINE+15,505,200,16);
  
  //
  // Post-Import Carts List
  //
  event_postimport_length_edit=new QLineEdit(this,"event_postimport_length_edit");
  event_postimport_length_edit->setGeometry(sizeHint().width()-140,503,80,20);
  event_postimport_length_edit->setReadOnly(true);
  label=new QLabel(event_postimport_length_edit,tr("Len:"),
		   this,"event_postimport_length_label");
  label->setFont(bold_font);
  label->setGeometry(sizeHint().width()-330,505,185,16);
  label->setAlignment(AlignVCenter|AlignRight);

  event_postimport_list=new ImportListView(this,"event_postimport_list");
  event_postimport_list->setGeometry(CENTER_LINE+15,522,
				     sizeHint().width()-CENTER_LINE-75,125);
  event_postimport_list->setAllColumnsShowFocus(true);
  event_postimport_list->setItemMargin(5);
  event_postimport_list->setSortColumn(-1);
  event_postimport_list->setAllowStop(false);
  event_postimport_list->logEvent()->
    setLogName(QString().sprintf("%s_POST",(const char *)event_name).
	       replace(' ',"_"));
  event_postimport_list->addColumn("");
  event_postimport_list->addColumn(tr("CART"));
  event_postimport_list->addColumn(tr("GROUP"));
  event_postimport_list->addColumn(tr("LENGTH"));
  event_postimport_list->setColumnAlignment(3,AlignRight);
  event_postimport_list->addColumn(tr("TITLE"));
  event_postimport_list->addColumn(tr("TRANSITION"));
  event_postimport_list->addColumn(tr("COUNT"));
  connect(event_postimport_list,SIGNAL(clicked(QListViewItem *)),
	  this,SLOT(cartClickedData(QListViewItem *)));
  connect(event_postimport_list,SIGNAL(lengthChanged(int)),
	  this,SLOT(postimportLengthChangedData(int)));
  event_postimport_up_button=
    new RDTransportButton(RDTransportButton::Up,this,
			 "event_postimport_up_button");
  event_postimport_up_button->setGeometry(sizeHint().width()-50,532,40,40);
  connect(event_postimport_up_button,SIGNAL(clicked()),
	  this,SLOT(postimportUpData()));
  event_postimport_down_button=
    new RDTransportButton(RDTransportButton::Down,this,
			 "event_postimport_down_button");
  event_postimport_down_button->setGeometry(sizeHint().width()-50,597,40,40);
  connect(event_postimport_down_button,SIGNAL(clicked()),
	  this,SLOT(postimportDownData()));
  
  //
  //  Save Button
  //
  QPushButton *button=new QPushButton(this,"save_button");
  button->setGeometry(CENTER_LINE+10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Save"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  //  Save As Button
  //
  button=new QPushButton(this,"save_as_button");
  button->setGeometry(CENTER_LINE+100,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("Save &As"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveAsData()));

  //
  //  Service Association Button
  //
  button=new QPushButton(this,"svc_button");
  button->setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2-85,
		      sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Services\nList"));
  connect(button,SIGNAL(clicked()),this,SLOT(svcData()));

  //
  //  Color Button
  //
  event_color_button=new QPushButton(this,"event_color_button");
  event_color_button->
    setGeometry(CENTER_LINE+(sizeHint().width()-CENTER_LINE)/2+5,
		sizeHint().height()-60,80,50);
  event_color_button->setFont(bold_font);
  event_color_button->setText(tr("C&olor"));
  connect(event_color_button,SIGNAL(clicked()),this,SLOT(colorData()));

  //
  //  OK Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
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
    event_position_edit->setTime(QTime().addMSecs(pos));
    prepositionToggledData(true);
  }
  
  int grace=0;
  switch(event_event->timeType()) {
      case RDLogLine::Relative:
	event_timetype_box->setChecked(false);
	event_grace_group->setButton(0);
	timeToggledData(false);
	break;
	
      case RDLogLine::Hard:
	event_timetype_box->setChecked(true);
	event_post_box->setChecked(event_event->postPoint());
	event_transtype_box->setCurrentItem(event_event->firstTransType());
	switch((grace=event_event->graceTime())) {
	    case 0:
	      event_grace_group->setButton(0);
	      event_grace_edit->setTime(QTime());
	      break;
	      
	    case -1:
	      event_grace_group->setButton(1);
	      event_grace_edit->setTime(QTime());
	      break;
	      
	    default:
	      event_grace_group->setButton(2);
	      event_grace_edit->setTime(QTime().addMSecs(grace));
	      break;
	}
  }
  
  event_autofill_box->setChecked(event_event->useAutofill());
  int autofill_slop=event_event->autofillSlop();
  if(autofill_slop>=0) {
    event_autofill_slop_box->setChecked(true);
    event_autofill_slop_edit->setTime(QTime().addMSecs(autofill_slop));
  }
  autofillWarnToggledData(event_autofill_slop_box->isChecked());
  event_timescale_box->setChecked(event_event->useTimescale());

  event_source_group->setButton(event_event->importSource());
  event_startslop_edit->setTime(QTime().addMSecs(event_event->startSlop()));
  event_endslop_edit->setTime(QTime().addMSecs(event_event->endSlop()));
  event_firsttrans_box->setCurrentItem(event_event->firstTransType());
  event_defaulttrans_box->setCurrentItem(event_event->defaultTransType());
  if (event_event->SchedGroup()!=NULL) {
    event_sched_group_box->setCurrentText(event_event->SchedGroup());
  }
  event_title_sep_spinbox->setValue(event_event->titleSep());
  event_have_code_box->setCurrentText(event_event->HaveCode());
  QColor color=event_event->color();
  if(color.isValid()) {
    event_color_button->setPalette(QPalette(color,backgroundColor()));
  }
  str=event_event->nestedEvent();
  sql=QString().sprintf("select NAME from EVENTS where NAME!=\"%s\"\
                         order by NAME",
			(const char *)eventname);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_nestevent_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==str) {
      event_nestevent_box->setCurrentItem(event_nestevent_box->count()-1);
    }
  }
  delete q;

  if(!new_event) {
    event_preimport_list->logEvent()->load();
    event_preimport_list->refreshList();
    event_postimport_list->logEvent()->load();
    event_postimport_list->refreshList();
  }

  prepositionToggledData(event_position_box->isChecked());
  timeToggledData(event_timetype_box->isChecked());
  importClickedData(event_source_group->selectedId());
  preimportChangedData(event_preimport_list->childCount());
  SetPostTransition();
}


EditEvent::~EditEvent()
{
  delete event_lib_list;
  delete event_preimport_list;
  delete event_postimport_list;
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
  RefreshLibrary();
}


void EditEvent::filterActivatedData(const QString &str)
{
  RefreshLibrary();
}


void EditEvent::filterClickedData(int id)
{
  RefreshLibrary();
}


void EditEvent::cartClickedData(QListViewItem *item)
{
#ifndef WIN32
  if (!event_player) return;
  if(item==NULL) {
    event_player->setCart(0);
    return;
  }
  event_player->setCart(item->text(1).toUInt());
#endif  // WIN32
}


void EditEvent::prepositionToggledData(bool state)
{
  event_position_edit->setEnabled(state);
  event_timetype_box->setDisabled(state);
  event_timetype_header->setDisabled(state);
  event_timetype_label->setDisabled(state);
  event_preimport_list->setAllowFirstTrans(!state);
  if(event_timetype_box->isChecked()) {
    event_grace_label->setDisabled(state);
    event_immediate_button->setDisabled(state);
    event_next_button->setDisabled(state);
    event_wait_button->setDisabled(state);
  }
  if((RDEventLine::ImportSource)event_source_group->selectedId()!=
     RDEventLine::None) {
    if(event_preimport_list->childCount()==0) {
      event_firsttrans_box->setDisabled(state);
      event_firsttrans_label->setDisabled(state);
      event_firsttrans_unit->setDisabled(state);
    }
  }
  if(state) {
    event_preimport_list->setForceTrans(RDLogLine::Stop);
  }
  else {
    if(event_timetype_box->isChecked()) {
      event_preimport_list->
	setForceTrans((RDLogLine::TransType)event_transtype_box->
		      currentItem());
    }
    else {
      event_preimport_list->setForceTrans(RDLogLine::NoTrans);
    }
  }
  event_preimport_list->refreshList();
  SetPostTransition();
}


void EditEvent::timeToggledData(bool state)
{
  event_grace_group->setEnabled(state);
  event_grace_label->setEnabled(state);
  event_immediate_button->setEnabled(state);
  event_next_button->setEnabled(state);
  event_wait_button->setEnabled(state);
  event_grace_edit->setEnabled(state);
  event_post_label->setEnabled(state&&(event_grace_group->selectedId()==0));
  event_post_box->setEnabled(state&&(event_grace_group->selectedId()==0));
  event_preimport_list->setAllowFirstTrans(!state);
  if(state) {
    event_preimport_list->
      setForceTrans((RDLogLine::TransType)event_transtype_box->currentItem());
  }
  else {
    if(event_position_box->isChecked()) {
      event_preimport_list->setForceTrans(RDLogLine::Stop);
    }
    else {
      event_preimport_list->setForceTrans(RDLogLine::NoTrans);
    }
  }
  event_preimport_list->refreshList();
  if(state) {
    graceClickedData(event_grace_group->selectedId());
    event_time_label->setEnabled(true);
    event_transtype_box->setEnabled(true);
    timeTransitionData(2);
    event_position_box->setDisabled(true);
    event_position_edit->setDisabled(true);
    event_position_header->setDisabled(true);
    event_position_label->setDisabled(true);
    event_position_unit->setDisabled(true);
    event_firsttrans_box->setDisabled(true);
    event_firsttrans_label->setDisabled(true);
    event_firsttrans_unit->setDisabled(true);
  }
  else {
    event_post_box->setChecked(false);
    event_grace_edit->setDisabled(true);
    event_time_label->setDisabled(true);
    event_transtype_box->setDisabled(true);
    if(event_position_box->isChecked()) {
      event_position_edit->setEnabled(true);
    }
    event_position_box->setEnabled(true);
    event_position_header->setEnabled(true);
    event_position_label->setEnabled(true);
    event_position_unit->setEnabled(true);
    if(((RDEventLine::ImportSource)event_source_group->selectedId()!=
       RDEventLine::None)&&(!event_position_box->isChecked())&&
       (event_preimport_list->childCount()==0)) {
      event_firsttrans_box->setEnabled(true);
      event_firsttrans_label->setEnabled(true);
      event_firsttrans_unit->setEnabled(true);
    }
  }
  SetPostTransition();
}


void EditEvent::graceClickedData(int id)
{
  switch(id) {
      case 0:
	event_post_label->setEnabled(event_timetype_box->isChecked());
	event_post_box->setEnabled(event_timetype_box->isChecked());
	timeTransitionData(RDLogLine::Stop);
	event_grace_edit->setDisabled(true);
	break;

      case 1:
	event_post_label->setDisabled(true);
	event_post_box->setDisabled(true);
	timeTransitionData(RDLogLine::Segue);
	event_grace_edit->setDisabled(true);
	break;

      case 2:
	event_post_label->setDisabled(true);
	event_post_box->setDisabled(true);
	timeTransitionData(RDLogLine::Segue);
	event_grace_edit->setEnabled(true);
	break;
  }
  SetPostTransition();
}


void EditEvent::timeTransitionData(int id)
{
  if(event_timetype_box->isChecked()) {
    event_preimport_list->
      setForceTrans((RDLogLine::TransType)event_transtype_box->currentItem());
  }
  else {
    event_preimport_list->setForceTrans(RDLogLine::NoTrans);
  }
  event_preimport_list->refreshList();
  SetPostTransition();
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
    statesched=false;
    stateschedinv=true;
  }
  event_startslop_edit->setEnabled(statesched);
  event_startslop_label->setEnabled(statesched);
  event_startslop_unit->setEnabled(statesched);
  event_endslop_edit->setEnabled(statesched);
  event_endslop_label->setEnabled(statesched);
  event_endslop_unit->setEnabled(statesched);
  if((!event_timetype_box->isChecked())&&(!event_position_box->isChecked())) {
    if((state&&(event_preimport_list->childCount()==0))||(!state)) {
      event_firsttrans_box->setEnabled(state);
      event_firsttrans_label->setEnabled(state);
      event_firsttrans_unit->setEnabled(state);
    }
  }
  event_defaulttrans_box->setEnabled(state);
  event_defaulttrans_label->setEnabled(state);
  event_defaulttrans_unit->setEnabled(state);
  state=(id==2)&&state;
  event_nestevent_label->setEnabled(state);
  event_nestevent_box->setEnabled(state);
  event_nestevent_unit->setEnabled(state);
  SetPostTransition();
  event_sched_group_box->setEnabled(stateschedinv);
  event_title_sep_label->setEnabled(stateschedinv);
  event_title_sep_spinbox->setEnabled(stateschedinv);
  event_have_code_box->setEnabled(stateschedinv);
  event_have_code_label->setEnabled(stateschedinv);
}


void EditEvent::preimportChangedData(int size)
{
  if((size==0)&&(event_source_group->selectedId()!=0)&&
     (!event_position_box->isChecked())&&(!event_timetype_box->isChecked())) {
    event_firsttrans_box->setEnabled(true);
    event_firsttrans_label->setEnabled(true);
    event_firsttrans_unit->setEnabled(true);
  }
  else {
    event_firsttrans_box->setDisabled(true);
    event_firsttrans_label->setDisabled(true);
    event_firsttrans_unit->setDisabled(true);
  }
  SetPostTransition();
}


void EditEvent::preimportLengthChangedData(int msecs)
{
  event_preimport_length_edit->setText(RDGetTimeLength(msecs,true,false));
}


void EditEvent::preimportUpData()
{
  int line;
  QListViewItem *item=event_preimport_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if((line=item->text(6).toInt())<1) {
    event_preimport_list->setSelected(item,true);
    event_preimport_list->ensureItemVisible(item);
    return;
  }
  event_preimport_list->logEvent()->move(line,line-1);
  event_preimport_list->validateTransitions();
  event_preimport_list->refreshList(line-1);
}


void EditEvent::preimportDownData()
{
  int line;
  QListViewItem *item=event_preimport_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if((line=item->text(6).toInt())>=(event_preimport_list->childCount()-1)) {
    event_preimport_list->setSelected(item,true);
    event_preimport_list->ensureItemVisible(item);
    return;
  }
  event_preimport_list->logEvent()->move(line,line+1);
  event_preimport_list->validateTransitions();
  event_preimport_list->refreshList(line+1);
}


void EditEvent::postimportUpData()
{
  int line;
  QListViewItem *item=event_postimport_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if((line=item->text(6).toInt())<1) {
    event_postimport_list->setSelected(item,true);
    event_postimport_list->ensureItemVisible(item);
    return;
  }
  event_postimport_list->logEvent()->move(line,line-1);
  event_postimport_list->validateTransitions();
  event_postimport_list->refreshList(line-1);
}


void EditEvent::postimportDownData()
{
  int line;
  QListViewItem *item=event_postimport_list->selectedItem();
  if(item==NULL) {
    return;
  }
  if((line=item->text(6).toInt())>=(event_postimport_list->childCount()-1)) {
    event_postimport_list->setSelected(item,true);
    event_postimport_list->ensureItemVisible(item);
    return;
  }
  event_postimport_list->logEvent()->move(line,line+1);
  event_postimport_list->validateTransitions();
  event_postimport_list->refreshList(line+1);
}


void EditEvent::postimportLengthChangedData(int msecs)
{
  event_postimport_length_edit->setText(RDGetTimeLength(msecs,true,false));
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
  AddEvent *add_dialog=new AddEvent(&event_name,this,"add_dialog");
  if(add_dialog->exec()<0) {
    delete add_dialog;
    return;
  }
  delete add_dialog;
  QString sql=QString().sprintf("select NAME from EVENTS where NAME=\"%s\"",
				(const char *)RDEscapeString(event_name));
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
    setCaption(QString().
	      sprintf("Editing Event - %s",(const char *)event_event->name()));
  }
  else {
    if(QMessageBox::question(this,tr("RDLogManager"),tr("Event already exists!\nDo you want to overwrite it?"),QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    delete event_event;
    event_event=new RDEvent(event_name,true);
    Save();
    event_new_events->push_back(event_name);
    sql=QString().sprintf("delete from EVENT_PERMS where EVENT_NAME=\"%s\"",
			  (const char *)RDEscapeString(event_name));
    q=new RDSqlQuery(sql);
    delete q;
    CopyEventPerms(old_name,event_name);
    if(event_new_event) {
      AbandonEvent(old_name);
    }
    str=QString(tr("Edit Event"));
    setCaption(QString().
	      sprintf("%s - %s",(const char *)str,
		      (const char *)event_event->name()));
  }
}


void EditEvent::svcData()
{
  EditPerms *dialog=new EditPerms(event_name,EditPerms::ObjectEvent,
				  this,"dialog");
  dialog->exec();
  delete dialog;
}


void EditEvent::colorData()
{
  QColor color=QColorDialog::getColor(event_color_button->backgroundColor(),
				      this,"color_dialog");
  if(color.isValid()) {
    event_color_button->setPalette(QPalette(color,backgroundColor()));
  }
}


void EditEvent::okData()
{
  Save();
#ifndef WIN32
  if (event_player){
    event_player->stop();
  }
#endif  // WIN32
  done(0);
}


void EditEvent::cancelData()
{
#ifndef WIN32
  if (event_player){
    event_player->stop();
  }
#endif  // WIN32
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
  p->setPen(QColor(black));
  p->moveTo(CENTER_LINE,10);
  p->lineTo(CENTER_LINE,sizeHint().height()-10);
  p->drawRect(CENTER_LINE+160,82,sizeHint().width()-CENTER_LINE-200,45);
  p->moveTo(CENTER_LINE+408,383);
  p->lineTo(CENTER_LINE+408,450);
  p->end();
}


void EditEvent::RefreshLibrary()
{
  QString type_filter;

  switch(event_lib_type_group->selectedId()) {
      case 0:
	type_filter="((TYPE=1)||(TYPE=2)||(TYPE=3))";
	break;

      case 1:
	type_filter="((TYPE=1)||(TYPE=3))";
	break;

      case 2:
	type_filter="(TYPE=2)";
	break;
  }
  QString sql="select TYPE,NUMBER,GROUP_NAME,FORCED_LENGTH,TITLE,ARTIST,\
               START_DATETIME,END_DATETIME from CART";
  QString group=event_group_box->currentText();
  if(group==QString(tr("ALL"))) {
    group="";
  }
  sql+=QString().
    sprintf(" where %s && %s",
	    (const char *)
	    RDCartSearchText(event_lib_filter_edit->text(),group,"",false).
	    utf8(),(const char *)type_filter);
  RDSqlQuery *q=new RDSqlQuery(sql);
  QListViewItem *item;
  event_lib_list->clear();
  while(q->next()) {
    item=new QListViewItem(event_lib_list);
    switch((RDCart::Type)q->value(0).toInt()) {
	case RDCart::Audio:
	  item->setPixmap(0,*event_playout_map);
	  item->setText(8,tr("Audio"));
	  break;

	case RDCart::Macro:
	  item->setPixmap(0,*event_macro_map);
	  item->setText(8,tr("Macro"));
	  break;

    case RDCart::All:
	  break;
    }
    item->setText(1,QString().sprintf("%06u",q->value(1).toInt()));
    item->setText(2,q->value(2).toString());
    item->setText(3,RDGetTimeLength(q->value(3).toInt(),false,false));
    item->setText(4,q->value(4).toString());
    item->setText(5,q->value(5).toString());
    if(!q->value(6).toDateTime().isNull()) {
      item->setText(6,q->value(7).toDateTime().toString("MM/dd/yyyy"));
    }
    if(!q->value(7).toDateTime().isNull()) {
      item->setText(7,q->value(7).toDateTime().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(7,"TFN");
    }
  }
  delete q;
}


void EditEvent::SetPostTransition()
{
  if((event_position_box->isChecked())||
     (event_timetype_box->isChecked())||
     (event_preimport_list->childCount()!=0)||
     (event_source_group->selectedId()!=0)) {
    event_postimport_list->setAllowStop(false);
  }
  else {
    event_postimport_list->setAllowStop(true);
  }
  if(event_preimport_list->childCount()==0) {
    if(event_position_box->isChecked()) {
      event_postimport_list->setAllowFirstTrans(false);
      event_postimport_list->setForceTrans(RDLogLine::Stop);
    }
    else {
      if(event_timetype_box->isChecked()) {
	event_postimport_list->
	  setForceTrans((RDLogLine::TransType)event_transtype_box->
			currentItem());
	  event_postimport_list->setAllowFirstTrans(false);
      }
      else {
	event_postimport_list->setAllowFirstTrans(true);
	event_postimport_list->setForceTrans(RDLogLine::NoTrans);
      }
    }
  }
  else { 
    event_postimport_list->setAllowFirstTrans(true);
    event_postimport_list->setForceTrans(RDLogLine::NoTrans);
  }
  event_postimport_list->refreshList();
}


void EditEvent::Save()
{
  QString properties;
  QString listname;

  event_event->setRemarks(event_remarks_edit->text());
  if(event_position_box->isChecked()) {
    event_event->setPreposition(QTime().msecsTo(event_position_edit->time()));
  }
  else {
    event_event->setPreposition(-1);
  }
  if(event_timetype_box->isChecked()) {
    event_event->setTimeType(RDLogLine::Hard);
    event_event->setPostPoint(event_post_box->isChecked());
    event_event->setFirstTransType((RDLogLine::TransType)
				   event_transtype_box->currentItem());
    switch((RDLogLine::TransType)event_transtype_box->currentItem()) {
	case RDLogLine::Play:
	  break;

	case RDLogLine::Segue:
	  break;

	case RDLogLine::Stop:
	  break;

	default:
	  break;
    }
    switch(event_grace_group->selectedId()) {
	case 0:
	  event_event->setGraceTime(0);
	  break;

	case 1:
	  event_event->setGraceTime(-1);
	  break;

	default:
	  event_event->setGraceTime(QTime().msecsTo(event_grace_edit->time()));
	  break;	  
    }
  }
  else {
    event_event->setTimeType(RDLogLine::Relative);
    event_event->setGraceTime(0);
    event_event->setPostPoint(false);
    event_event->setFirstTransType(RDLogLine::Play);
  }

  event_event->setUseAutofill(event_autofill_box->isChecked());
  if(event_autofill_slop_box->isChecked()) {
    event_event->
      setAutofillSlop(QTime().msecsTo(event_autofill_slop_edit->time()));
  }
  else {
    event_event->setAutofillSlop(-1);
  }
  event_event->setUseTimescale(event_timescale_box->isChecked());

  switch((RDEventLine::ImportSource)event_source_group->selectedId()) {
      case RDEventLine::Traffic:
	break;

      case RDEventLine::Music:
	break;

      case RDEventLine::Scheduler:
	break;

      default:
	break;
  }
  event_event->
    setImportSource((RDEventLine::ImportSource)event_source_group->selectedId());
  event_event->setStartSlop(QTime().msecsTo(event_startslop_edit->time()));
  event_event->setEndSlop(QTime().msecsTo(event_endslop_edit->time()));
  if(!event_timetype_box->isChecked()) {
    event_event->
      setFirstTransType((RDLogLine::TransType)event_firsttrans_box->
			currentItem());
    switch((RDLogLine::TransType)event_firsttrans_box->currentItem()) {
	case RDLogLine::Play:
	  break;

	case RDLogLine::Segue:
	  break;

	case RDLogLine::Stop:
	  break;

	default:
	  break;
    }
  }
  event_event->
    setDefaultTransType((RDLogLine::TransType)event_defaulttrans_box->
			currentItem());
  event_event->setColor(event_color_button->backgroundColor());
  if(event_nestevent_box->currentItem()==0) {
    event_event->setNestedEvent("");
  }
  else {
    event_event->setNestedEvent(event_nestevent_box->currentText());
  }
  event_event->setProperties(GetProperties());
  event_event->setSchedGroup(event_sched_group_box->currentText());  
  event_event->setTitleSep(event_title_sep_spinbox->value());
  event_event->setHaveCode(event_have_code_box->currentText()); 
  listname=event_name;
  listname.replace(" ","_");
  event_preimport_list->logEvent()->
    setLogName(QString().sprintf("%s_PRE",(const char *)listname));
  event_preimport_list->logEvent()->save(false);
  event_postimport_list->logEvent()->
    setLogName(QString().sprintf("%s_POST",(const char *)listname));
  event_postimport_list->logEvent()->save(false);
  event_saved=true;
}


QString EditEvent::GetProperties()
{
  QString properties;
  RDLogLine::TransType trans_type;
  QString str;

  if(event_position_box->isChecked()) {
    str=QString(tr("Cue"));
    properties+=
      QString().sprintf("%s(-%s), ",(const char *)str,
	       (const char *)event_position_edit->time().toString("mm:ss"));
  }
  if(event_timetype_box->isChecked()) {
    trans_type=(RDLogLine::TransType)event_transtype_box->currentItem();
  }
  else {
    if(event_preimport_list->childCount()>0) {
      str=event_preimport_list->firstChild()->text(5);
      trans_type=RDLogLine::Play;
      if(str==tr("SEGUE")) {
	trans_type=RDLogLine::Segue;
      }
      if(str==tr("STOP")) {
	trans_type=RDLogLine::Stop;
      }
    }
    else {
      if(event_position_box->isChecked()) {
	trans_type=RDLogLine::Stop;
      }
      else {
	trans_type=(RDLogLine::TransType)event_firsttrans_box->currentItem();
      }
    }
  }
  switch(trans_type) {
      case RDLogLine::Play:
	properties+=tr("PLAY");
	break;

      case RDLogLine::Segue:
	properties+=tr("SEGUE");
	break;

      case RDLogLine::Stop:
	properties+=tr("STOP");
	break;

      default:
	break;
  }

  if(event_timetype_box->isChecked()) {
    switch(event_grace_group->selectedId()) {
	case 0:
	  properties+=tr(", Timed(Start)");
	  break;

	case 1:
	  properties+=tr(", Timed(MakeNext)");
	  break;

	default:
	  str=QString(tr("Timed(Wait"));
	  properties+=
	    QString().sprintf(", %s %s)",(const char *)str,(const char *)
			      event_grace_edit->time().toString("mm:ss"));
	  break;
    }
    if(event_post_box->isChecked()) {
      properties+=tr(", Post");
    }
  }
  if(event_autofill_box->isChecked()) {
    properties+=tr(", Fill");
  }
  if(event_timescale_box->isChecked()) {
    properties+=tr(", Scale");
  }
  switch((RDEventLine::ImportSource)event_source_group->selectedId()) {
      case RDEventLine::Traffic:
	properties+=tr(", Traffic");
	break;

      case RDEventLine::Music:
	properties+=tr(", Music");
	break;

      case RDEventLine::Scheduler:
	properties+=tr(", Scheduler");
	break;


      default:
	break;
  }
  if(event_nestevent_box->currentItem()>0) {
    properties+=tr(", Inline Traffic");
  }

  return properties;
}


void EditEvent::CopyEventPerms(QString old_name,QString new_name)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString().sprintf("select SERVICE_NAME from EVENT_PERMS where\
                         EVENT_NAME=\"%s\"",
			(const char *)RDEscapeString(old_name));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("insert into EVENT_PERMS set\
                          EVENT_NAME=\"%s\",SERVICE_NAME=\"%s\"",
			  (const char *)RDEscapeString(new_name),
			  (const char *)
			  RDEscapeString(q->value(0).toString()));
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
  QString sql=QString().sprintf("delete from EVENTS where NAME=\"%s\"",
				(const char *)RDEscapeString(name));
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from EVENT_PERMS where EVENT_NAME=\"%s\"",
			(const char *)RDEscapeString(name));
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("drop table `")+RDEvent::preimportTableName(name)+"`";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("drop table `")+RDEvent::postimportTableName(name)+"`";
  q=new RDSqlQuery(sql);
  delete q;
}
