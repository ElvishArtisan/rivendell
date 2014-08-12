// importfields.cpp
//
// Parser Parameters for RDAdmin.
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: importfields.cpp,v 1.2 2010/07/29 19:32:34 cvs Exp $
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
//

#include <rd.h>

#include <importfields.h>

ImportFields::ImportFields(QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  QLabel *label;
  import_changed=false;

  //
  // Traffic Cartname Parser Data Section
  //
  label=new QLabel(tr("Cart Number:"),this,
		   "_cartname_label");
  label->setGeometry(0,0,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Cart Offset
  //
  cart_offset_spin=new QSpinBox(this,"cart_offset_spin");
  cart_offset_spin->setGeometry(175,0,50,19);
  cart_offset_spin->setRange(0,1024);
  label=new QLabel(cart_offset_spin,
		   tr("Offset:"),this,
		   "cart_win_path_label");
  label->setGeometry(125,0,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(cart_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Cart Length
  //
  cart_length_spin=new QSpinBox(this,"cart_length_spin");
  cart_length_spin->setGeometry(285,0,50,19);
  cart_length_spin->setRange(0,6);
  label=new QLabel(cart_length_spin,
		   tr("Length:"),this,
		   "cart_win_path_label");
  label->setGeometry(230,0,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(cart_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Title Parser Data Section
  //
  label=new QLabel(tr("Title:"),this,"_cartname_label");
  label->setGeometry(0,21,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Title Cart Offset
  //
  title_offset_spin=new QSpinBox(this,"title_offset_spin");
  title_offset_spin->setGeometry(175,21,50,19);
  title_offset_spin->setRange(0,1024);
  label=new QLabel(title_offset_spin,
		   tr("Offset:"),this,
		   "title_win_path_label");
  label->setGeometry(125,21,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(title_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Title Cart Length
  //
  title_length_spin=new QSpinBox(this,"title_length_spin");
  title_length_spin->setGeometry(285,21,50,19);
  title_length_spin->setRange(0,255);
  label=new QLabel(title_length_spin,
		   tr("Length:"),this,
		   "title_win_path_label");
  label->setGeometry(230,21,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(title_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Start Hours Parser Data Section
  //
  label=new QLabel(tr("Start Time - Hours:"),this,
		   "_startname_label");
  label->setGeometry(0,42,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Start Hours Offset
  //
  hours_offset_spin=new QSpinBox(this,"hours_offset_spin");
  hours_offset_spin->setGeometry(175,42,50,19);
  hours_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(hours_offset_spin,
		   tr("Offset:"),this,
		   "hours_win_path_label");
  label->setGeometry(125,42,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(hours_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Start Hours Length
  //
  hours_length_spin=new QSpinBox(this,"hours_length_spin");
  hours_length_spin->setGeometry(285,42,50,19);
  hours_length_spin->setRange(0,8);
  label=new QLabel(hours_length_spin,
		   tr("Length:"),this,
		   "hours_win_path_label");
  label->setGeometry(230,42,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(hours_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Start Minutes Parser Data Section
  //
  label=new QLabel(tr("Start Time - Minutes:"),this,
		   "_startname_label");
  label->setGeometry(0,63,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Start Minutes Offset
  //
  minutes_offset_spin=new QSpinBox(this,"minutes_offset_spin");
  minutes_offset_spin->setGeometry(175,63,50,19);
  minutes_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(minutes_offset_spin,
		   tr("Offset:"),this,
		   "minutes_win_path_label");
  label->setGeometry(125,63,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(minutes_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Start Minutes Length
  //
  minutes_length_spin=new QSpinBox(this,"minutes_length_spin");
  minutes_length_spin->setGeometry(285,63,50,19);
  minutes_length_spin->setRange(0,8);
  label=new QLabel(minutes_length_spin,
		   tr("Length:"),this,
		   "minutes_win_path_label");
  label->setGeometry(230,63,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(minutes_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Start Seconds Parser Data Section
  //
  label=new QLabel(tr("Start Time - Seconds:"),this,
		   "_startname_label");
  label->setGeometry(0,84,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Start Seconds Offset
  //
  seconds_offset_spin=new QSpinBox(this,"seconds_offset_spin");
  seconds_offset_spin->setGeometry(175,84,50,19);
  seconds_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(seconds_offset_spin,
		   tr("Offset:"),this,
		   "seconds_win_path_label");
  label->setGeometry(125,84,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(seconds_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Start Seconds Length
  //
  seconds_length_spin=new QSpinBox(this,"seconds_length_spin");
  seconds_length_spin->setGeometry(285,84,50,19);
  seconds_length_spin->setRange(0,8);
  label=new QLabel(seconds_length_spin,
		   tr("Length:"),this,
		   "seconds_win_path_label");
  label->setGeometry(230,84,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(seconds_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Length Hours Parser Data Section
  //
  label=new QLabel(tr("Length - Hours:"),this,
		   "_lengthname_label");
  label->setGeometry(0,106,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Length Hours Offset
  //
  len_hours_offset_spin=new QSpinBox(this,"len_hours_offset_spin");
  len_hours_offset_spin->setGeometry(175,106,50,19);
  len_hours_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(len_hours_offset_spin,
		   tr("Offset:"),this,
		   "len_hours_win_path_label");
  label->setGeometry(125,106,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(len_hours_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Length Hours Length
  //
  len_hours_length_spin=new QSpinBox(this,"len_hours_length_spin");
  len_hours_length_spin->setGeometry(285,106,50,19);
  len_hours_length_spin->setRange(0,8);
  label=new QLabel(len_hours_length_spin,
		   tr("Length:"),this,
		   "len_hours_win_path_label");
  label->setGeometry(230,106,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(len_hours_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Length Minutes Parser Data Section
  //
  label=new QLabel(tr("Length - Minutes:"),this,
		   "_lengthname_label");
  label->setGeometry(0,127,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Length Minutes Offset
  //
  len_minutes_offset_spin=new QSpinBox(this,"len_minutes_offset_spin");
  len_minutes_offset_spin->setGeometry(175,127,50,19);
  len_minutes_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(len_minutes_offset_spin,
		   tr("Offset:"),this,
		   "len_minutes_win_path_label");
  label->setGeometry(125,127,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(len_minutes_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Length Minutes Length
  //
  len_minutes_length_spin=new QSpinBox(this,"len_minutes_length_spin");
  len_minutes_length_spin->setGeometry(285,127,50,19);
  len_minutes_length_spin->setRange(0,8);
  label=new QLabel(len_minutes_length_spin,
		   tr("Length:"),this,
		   "len_minutes_win_path_label");
  label->setGeometry(230,127,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(len_minutes_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Length Seconds Parser Data Section
  //
  label=new QLabel(tr("Length - Seconds:"),this,
		   "_lengthname_label");
  label->setGeometry(0,148,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Length Seconds Offset
  //
  len_seconds_offset_spin=new QSpinBox(this,"len_seconds_offset_spin");
  len_seconds_offset_spin->setGeometry(175,148,50,19);
  len_seconds_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(len_seconds_offset_spin,
		   tr("Offset:"),this,
		   "len_seconds_win_path_label");
  label->setGeometry(125,148,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(len_seconds_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Length Seconds Length
  //
  len_seconds_length_spin=new QSpinBox(this,"len_seconds_length_spin");
  len_seconds_length_spin->setGeometry(285,148,50,19);
  len_seconds_length_spin->setRange(0,8);
  label=new QLabel(len_seconds_length_spin,
		   tr("Length:"),this,
		   "len_seconds_win_path_label");
  label->setGeometry(230,148,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(len_seconds_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Contract # Parser Data Section
  //
  label=new QLabel(tr("Contract #:"),this,
		   "_dataname_label");
  label->setGeometry(0,169,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Contract # Offset
  //
  data_offset_spin=new QSpinBox(this,"data_offset_spin");
  data_offset_spin->setGeometry(175,169,50,19);
  data_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(data_offset_spin,
		   tr("Offset:"),this,
		   "data_win_path_label");
  label->setGeometry(125,169,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(data_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Contract # Length
  //
  data_length_spin=new QSpinBox(this,"data_length_spin");
  data_length_spin->setGeometry(285,169,50,19);
  data_length_spin->setRange(0,32);
  label=new QLabel(data_length_spin,
		   tr("Length:"),this,
		   "data_win_path_label");
  label->setGeometry(230,169,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(data_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Event ID Parser Data Section
  //
  label=new QLabel(tr("Event ID:"),this,
		   "_event_idname_label");
  label->setGeometry(0,190,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Event ID Offset
  //
  event_id_offset_spin=new QSpinBox(this,"event_id_offset_spin");
  event_id_offset_spin->setGeometry(175,190,50,19);
  event_id_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(event_id_offset_spin,
		   tr("Offset:"),this,
		   "event_id_win_path_label");
  label->setGeometry(125,190,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(event_id_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Event ID Length
  //
  event_id_length_spin=new QSpinBox(this,"event_id_length_spin");
  event_id_length_spin->setGeometry(285,190,50,19);
  event_id_length_spin->setRange(0,8);
  label=new QLabel(event_id_length_spin,
		   tr("Length:"),this,
		   "event_id_win_path_label");
  label->setGeometry(230,190,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(event_id_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Announcement Type Parser Data Section
  //
  label=new QLabel(tr("Annc. Type:"),this,
		   "_annctypename_label");
  label->setGeometry(0,211,120,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Traffic Announcement Type Offset
  //
  annctype_offset_spin=new QSpinBox(this,"annctype_offset_spin");
  annctype_offset_spin->setGeometry(175,211,50,19);
  annctype_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(annctype_offset_spin,
		   tr("Offset:"),this,
		   "annctype_win_path_label");
  label->setGeometry(125,211,45,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(annctype_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Traffic Announcement Type Length
  //
  annctype_length_spin=new QSpinBox(this,"annctype_length_spin");
  annctype_length_spin->setGeometry(285,211,50,19);
  annctype_length_spin->setRange(0,8);
  label=new QLabel(annctype_length_spin,
		   tr("Length:"),this,
		   "annctype_win_path_label");
  label->setGeometry(230,211,50,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  connect(annctype_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

}


QSize ImportFields::sizeHint() const
{
  return QSize(335,230);
}


QSizePolicy ImportFields::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


bool ImportFields::changed() const
{
  return import_changed;
}


void ImportFields::readFields(RDSvc *svc,RDSvc::ImportSource type)
{
  svc->setImportOffset(type,RDSvc::CartNumber,cart_offset_spin->value());
  svc->setImportLength(type,RDSvc::CartNumber,cart_length_spin->value());
  svc->setImportOffset(type,RDSvc::Title,title_offset_spin->value());
  svc->setImportLength(type,RDSvc::Title,title_length_spin->value());
  svc->setImportOffset(type,RDSvc::StartHours,hours_offset_spin->value());
  svc->setImportLength(type,RDSvc::StartHours,hours_length_spin->value());
  svc->setImportOffset(type,RDSvc::StartMinutes,minutes_offset_spin->value());
  svc->setImportLength(type,RDSvc::StartMinutes,minutes_length_spin->value());
  svc->setImportOffset(type,RDSvc::StartSeconds,seconds_offset_spin->value());
  svc->setImportLength(type,RDSvc::StartSeconds,seconds_length_spin->value());
  svc->setImportOffset(type,RDSvc::LengthHours,len_hours_offset_spin->value());
  svc->setImportLength(type,RDSvc::LengthHours,len_hours_length_spin->value());
  svc->setImportOffset(type,RDSvc::LengthMinutes,
		       len_minutes_offset_spin->value());
  svc->setImportLength(type,RDSvc::LengthMinutes,
		       len_minutes_length_spin->value());
  svc->setImportOffset(type,RDSvc::LengthSeconds,
		       len_seconds_offset_spin->value());
  svc->setImportLength(type,RDSvc::LengthSeconds,
		       len_seconds_length_spin->value());
  svc->setImportOffset(type,RDSvc::ExtData,data_offset_spin->value());
  svc->setImportLength(type,RDSvc::ExtData,data_length_spin->value());
  svc->setImportOffset(type,RDSvc::ExtEventId,event_id_offset_spin->value());
  svc->setImportLength(type,RDSvc::ExtEventId,event_id_length_spin->value());
  svc->setImportOffset(type,RDSvc::ExtAnncType,annctype_offset_spin->value());
  svc->setImportLength(type,RDSvc::ExtAnncType,annctype_length_spin->value());
  import_changed=false;
}


void ImportFields::setFields(RDSvc *svc,RDSvc::ImportSource type)
{
  cart_offset_spin->setValue(svc->importOffset(type,RDSvc::CartNumber));
  cart_length_spin->setValue(svc->importLength(type,RDSvc::CartNumber));
  title_offset_spin->setValue(svc->importOffset(type,RDSvc::Title));
  title_length_spin->setValue(svc->importLength(type,RDSvc::Title));
  hours_offset_spin->setValue(svc->importOffset(type,RDSvc::StartHours));
  hours_length_spin->setValue(svc->importLength(type,RDSvc::StartHours));
  minutes_offset_spin->setValue(svc->importOffset(type,RDSvc::StartMinutes));
  minutes_length_spin->setValue(svc->importLength(type,RDSvc::StartMinutes));
  seconds_offset_spin->setValue(svc->importOffset(type,RDSvc::StartSeconds));
  seconds_length_spin->setValue(svc->importLength(type,RDSvc::StartSeconds));
  len_hours_offset_spin->setValue(svc->importOffset(type,RDSvc::LengthHours));
  len_hours_length_spin->setValue(svc->importLength(type,RDSvc::LengthHours));
  len_minutes_offset_spin->
    setValue(svc->importOffset(type,RDSvc::LengthMinutes));
  len_minutes_length_spin->
    setValue(svc->importLength(type,RDSvc::LengthMinutes));
  len_seconds_offset_spin->
    setValue(svc->importOffset(type,RDSvc::LengthSeconds));
  len_seconds_length_spin->
    setValue(svc->importLength(type,RDSvc::LengthSeconds));
  data_offset_spin->setValue(svc->importOffset(type,RDSvc::ExtData));
  data_length_spin->setValue(svc->importLength(type,RDSvc::ExtData));
  event_id_offset_spin->setValue(svc->importOffset(type,RDSvc::ExtEventId));
  event_id_length_spin->setValue(svc->importLength(type,RDSvc::ExtEventId));
  annctype_offset_spin->setValue(svc->importOffset(type,RDSvc::ExtAnncType));
  annctype_length_spin->setValue(svc->importLength(type,RDSvc::ExtAnncType));
  import_changed=false;
}


void ImportFields::valueChangedData(int)
{
  import_changed=true;
}
