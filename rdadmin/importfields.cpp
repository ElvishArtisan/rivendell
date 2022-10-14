// importfields.cpp
//
// Import Parser Parameters for RDAdmin.
//
// (C) Copyright 2010-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "importfields.h"

ImportFields::ImportFields(RDSvc::ImportSource src,QWidget *parent)
  : RDWidget(parent)
{
  QLabel *label;
  import_changed=false;
  bypass_mode=false;
  import_source=src;

  //
  // Cartname Parser Data Section
  //
  label=new QLabel(tr("Cart Number:"),this);
  label->setGeometry(0,0,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart Offset
  //
  cart_offset_spin=new QSpinBox(this);
  cart_offset_spin->setGeometry(175,0,50,19);
  cart_offset_spin->setRange(0,1024);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,0,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(cart_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Cart Length
  //
  cart_length_spin=new QSpinBox(this);
  cart_length_spin->setGeometry(285,0,50,19);
  cart_length_spin->setRange(0,6);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,0,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(cart_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Title Parser Data Section
  //
  label=new QLabel(tr("Title:"),this);
  label->setGeometry(0,21,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Title Cart Offset
  //
  title_offset_spin=new QSpinBox(this);
  title_offset_spin->setGeometry(175,21,50,19);
  title_offset_spin->setRange(0,1024);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,21,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(title_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Title Cart Length
  //
  title_length_spin=new QSpinBox(this);
  title_length_spin->setGeometry(285,21,50,19);
  title_length_spin->setRange(0,255);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,21,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(title_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Start Hours Parser Data Section
  //
  label=new QLabel(tr("Start Time - Hours:"),this);
  label->setGeometry(0,42,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Start Hours Offset
  //
  hours_offset_spin=new QSpinBox(this);
  hours_offset_spin->setGeometry(175,42,50,19);
  hours_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,42,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(hours_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Start Hours Length
  //
  hours_length_spin=new QSpinBox(this);
  hours_length_spin->setGeometry(285,42,50,19);
  hours_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,42,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(hours_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Start Minutes Parser Data Section
  //
  label=new QLabel(tr("Start Time - Minutes:"),this);
  label->setGeometry(0,63,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Start Minutes Offset
  //
  minutes_offset_spin=new QSpinBox(this);
  minutes_offset_spin->setGeometry(175,63,50,19);
  minutes_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,63,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(minutes_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Start Minutes Length
  //
  minutes_length_spin=new QSpinBox(this);
  minutes_length_spin->setGeometry(285,63,50,19);
  minutes_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,63,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(minutes_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Start Seconds Parser Data Section
  //
  label=new QLabel(tr("Start Time - Seconds:"),this);
  label->setGeometry(0,84,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Start Seconds Offset
  //
  seconds_offset_spin=new QSpinBox(this);
  seconds_offset_spin->setGeometry(175,84,50,19);
  seconds_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,84,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(seconds_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Start Seconds Length
  //
  seconds_length_spin=new QSpinBox(this);
  seconds_length_spin->setGeometry(285,84,50,19);
  seconds_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,84,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(seconds_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Length Hours Parser Data Section
  //
  label=new QLabel(tr("Length - Hours:"),this);
  label->setGeometry(0,106,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Length Hours Offset
  //
  len_hours_offset_spin=new QSpinBox(this);
  len_hours_offset_spin->setGeometry(175,106,50,19);
  len_hours_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,106,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(len_hours_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Length Hours Length
  //
  len_hours_length_spin=new QSpinBox(this);
  len_hours_length_spin->setGeometry(285,106,50,19);
  len_hours_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,106,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(len_hours_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Length Minutes Parser Data Section
  //
  label=new QLabel(tr("Length - Minutes:"),this);
  label->setGeometry(0,127,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Length Minutes Offset
  //
  len_minutes_offset_spin=new QSpinBox(this);
  len_minutes_offset_spin->setGeometry(175,127,50,19);
  len_minutes_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,127,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(len_minutes_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Length Minutes Length
  //
  len_minutes_length_spin=new QSpinBox(this);
  len_minutes_length_spin->setGeometry(285,127,50,19);
  len_minutes_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,127,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(len_minutes_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Length Seconds Parser Data Section
  //
  label=new QLabel(tr("Length - Seconds:"),this);
  label->setGeometry(0,148,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Length Seconds Offset
  //
  len_seconds_offset_spin=new QSpinBox(this);
  len_seconds_offset_spin->setGeometry(175,148,50,19);
  len_seconds_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,148,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(len_seconds_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Length Seconds Length
  //
  len_seconds_length_spin=new QSpinBox(this);
  len_seconds_length_spin->setGeometry(285,148,50,19);
  len_seconds_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,148,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(len_seconds_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // GUID Parser Data Section
  //
  label=new QLabel(tr("Globally Unique ID:"),this);
  label->setGeometry(0,169,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Contract # Offset
  //
  data_offset_spin=new QSpinBox(this);
  data_offset_spin->setGeometry(175,169,50,19);
  data_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,169,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(data_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Contract # Length
  //
  data_length_spin=new QSpinBox(this);
  data_length_spin->setGeometry(285,169,50,19);
  data_length_spin->setRange(0,32);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,169,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(data_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Event ID Parser Data Section
  //
  label=new QLabel(tr("Event ID:"),this);
  label->setGeometry(0,190,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Event ID Offset
  //
  event_id_offset_spin=new QSpinBox(this);
  event_id_offset_spin->setGeometry(175,190,50,19);
  event_id_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,190,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(event_id_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Event ID Length
  //
  event_id_length_spin=new QSpinBox(this);
  event_id_length_spin->setGeometry(285,190,50,19);
  event_id_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,190,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(event_id_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Announcement Type Parser Data Section
  //
  label=new QLabel(tr("Annc. Type:"),this);
  label->setGeometry(0,211,125,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Announcement Type Offset
  //
  annctype_offset_spin=new QSpinBox(this);
  annctype_offset_spin->setGeometry(175,211,50,19);
  annctype_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  label=new QLabel(tr("Offset:"),this);
  label->setGeometry(125,211,45,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(annctype_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Announcement Type Length
  //
  annctype_length_spin=new QSpinBox(this);
  annctype_length_spin->setGeometry(285,211,50,19);
  annctype_length_spin->setRange(0,8);
  label=new QLabel(tr("Length:"),this);
  label->setGeometry(230,211,50,19);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(annctype_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Transition Type Parser Data Section
  //
  trans_type_label=new QLabel(tr("Transition Type")+":",this);
  trans_type_label->setGeometry(0,232,125,19);
  trans_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  trans_type_label->setVisible(import_source==RDSvc::Music);
  trans_type_label->setEnabled(bypass_mode);

  //
  // Transition Type Offset
  //
  trans_type_offset_spin=new QSpinBox(this);
  trans_type_offset_spin->setGeometry(175,232,50,19);
  trans_type_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  trans_type_offset_spin->setVisible(import_source==RDSvc::Music);
  trans_type_offset_spin->setEnabled(bypass_mode);
  trans_type_offset_label=new QLabel(tr("Offset:"),this);
  trans_type_offset_label->setGeometry(125,232,45,19);
  trans_type_offset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  trans_type_offset_label->setVisible(import_source==RDSvc::Music);
  trans_type_offset_label->setEnabled(bypass_mode);
  connect(trans_type_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Transition Type Length
  //
  trans_type_length_spin=new QSpinBox(this);
  trans_type_length_spin->setGeometry(285,232,50,19);
  trans_type_length_spin->setRange(0,8);
  trans_type_length_spin->setVisible(import_source==RDSvc::Music);
  trans_type_length_spin->setEnabled(bypass_mode);
  trans_type_length_label=new QLabel(tr("Length:"),this);
  trans_type_length_label->setGeometry(230,232,50,19);
  trans_type_length_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  trans_type_length_label->setVisible(import_source==RDSvc::Music);
  trans_type_length_label->setEnabled(bypass_mode);
  connect(trans_type_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Time Type Parser Data Section
  //
  time_type_label=new QLabel(tr("Time Type")+":",this);
  time_type_label->setGeometry(0,253,125,19);
  time_type_label->setVisible(import_source==RDSvc::Music);
  time_type_label->setEnabled(bypass_mode);
  time_type_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Time Type Offset
  //
  time_type_offset_spin=new QSpinBox(this);
  time_type_offset_spin->setGeometry(175,253,50,19);
  time_type_offset_spin->setRange(0,RD_MAX_IMPORT_LINE_LENGTH);
  time_type_offset_spin->setVisible(import_source==RDSvc::Music);
  time_type_offset_spin->setEnabled(bypass_mode);
  time_type_offset_label=new QLabel(tr("Offset:"),this);
  time_type_offset_label->setGeometry(125,253,45,19);
  time_type_offset_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  time_type_offset_label->setVisible(import_source==RDSvc::Music);
  time_type_offset_label->setEnabled(bypass_mode);
  connect(time_type_offset_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));

  //
  // Time Type Length
  //
  time_type_length_spin=new QSpinBox(this);
  time_type_length_spin->setGeometry(285,253,50,19);
  time_type_length_spin->setRange(0,8);
  time_type_length_spin->setVisible(import_source==RDSvc::Music);
  time_type_length_spin->setEnabled(bypass_mode);
  time_type_length_label=new QLabel(tr("Length:"),this);
  time_type_length_label->setGeometry(230,253,50,19);
  time_type_length_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  time_type_length_label->setVisible(import_source==RDSvc::Music);
  time_type_length_label->setEnabled(bypass_mode);
  connect(time_type_length_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(valueChangedData(int)));
}


QSize ImportFields::sizeHint() const
{
  if(import_source==RDSvc::Music) {
  return QSize(335,230+42);
  }
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


void ImportFields::readFields(RDSvc *svc)
{
  svc->setImportOffset(import_source,RDSvc::CartNumber,
		       cart_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::CartNumber,
		       cart_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::Title,title_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::Title,title_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::StartHours,
		       hours_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::StartHours,
		       hours_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::StartMinutes,
		       minutes_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::StartMinutes,
		       minutes_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::StartSeconds,
		       seconds_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::StartSeconds,
		       seconds_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::LengthHours,
		       len_hours_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::LengthHours,
		       len_hours_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::LengthMinutes,
		       len_minutes_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::LengthMinutes,
		       len_minutes_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::LengthSeconds,
		       len_seconds_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::LengthSeconds,
		       len_seconds_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::ExtData,
		       data_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::ExtData,data_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::ExtEventId,
		       event_id_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::ExtEventId,
		       event_id_length_spin->value());
  svc->setImportOffset(import_source,RDSvc::ExtAnncType,
		       annctype_offset_spin->value());
  svc->setImportLength(import_source,RDSvc::ExtAnncType,
		       annctype_length_spin->value());
  if(import_source==RDSvc::Music) {
    svc->setImportOffset(import_source,RDSvc::TransType,
		       trans_type_offset_spin->value());
    svc->setImportLength(import_source,RDSvc::TransType,
		       trans_type_length_spin->value());
    svc->setImportOffset(import_source,RDSvc::TimeType,
		       time_type_offset_spin->value());
    svc->setImportLength(import_source,RDSvc::TimeType,
		       time_type_length_spin->value());
  }
  import_changed=false;
}


void ImportFields::setFields(RDSvc *svc)
{
  setBypassMode(svc->bypassMode());
  cart_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::CartNumber));
  cart_length_spin->
    setValue(svc->importLength(import_source,RDSvc::CartNumber));
  title_offset_spin->setValue(svc->importOffset(import_source,RDSvc::Title));
  title_length_spin->setValue(svc->importLength(import_source,RDSvc::Title));
  hours_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::StartHours));
  hours_length_spin->
    setValue(svc->importLength(import_source,RDSvc::StartHours));
  minutes_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::StartMinutes));
  minutes_length_spin->
    setValue(svc->importLength(import_source,RDSvc::StartMinutes));
  seconds_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::StartSeconds));
  seconds_length_spin->
    setValue(svc->importLength(import_source,RDSvc::StartSeconds));
  len_hours_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::LengthHours));
  len_hours_length_spin->
    setValue(svc->importLength(import_source,RDSvc::LengthHours));
  len_minutes_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::LengthMinutes));
  len_minutes_length_spin->
    setValue(svc->importLength(import_source,RDSvc::LengthMinutes));
  len_seconds_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::LengthSeconds));
  len_seconds_length_spin->
    setValue(svc->importLength(import_source,RDSvc::LengthSeconds));
  data_offset_spin->setValue(svc->importOffset(import_source,RDSvc::ExtData));
  data_length_spin->setValue(svc->importLength(import_source,RDSvc::ExtData));
  event_id_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::ExtEventId));
  event_id_length_spin->
    setValue(svc->importLength(import_source,RDSvc::ExtEventId));
  annctype_offset_spin->
    setValue(svc->importOffset(import_source,RDSvc::ExtAnncType));
  annctype_length_spin->
    setValue(svc->importLength(import_source,RDSvc::ExtAnncType));
  if(import_source==RDSvc::Music) {
    trans_type_offset_spin->
      setValue(svc->importOffset(import_source,RDSvc::TransType));
    trans_type_length_spin->
      setValue(svc->importLength(import_source,RDSvc::TransType));
    time_type_offset_spin->
      setValue(svc->importOffset(import_source,RDSvc::TimeType));
    time_type_length_spin->
      setValue(svc->importLength(import_source,RDSvc::TimeType));
  }
  import_changed=false;
}


bool ImportFields::bypassMode() const
{
  return bypass_mode;
}


void ImportFields::setBypassMode(bool state)
{
  if(state!=bypass_mode) {
    trans_type_label->setEnabled(state);
    trans_type_offset_label->setEnabled(state);
    trans_type_offset_spin->setEnabled(state);
    trans_type_length_label->setEnabled(state);
    trans_type_length_spin->setEnabled(state);
    time_type_label->setEnabled(state);
    time_type_offset_label->setEnabled(state);
    time_type_offset_spin->setEnabled(state);
    time_type_length_label->setEnabled(state);
    time_type_length_spin->setEnabled(state);

    bypass_mode=state;
    import_changed=true;
  }
}


void ImportFields::setBypassMode(int n)
{
  setBypassMode((bool)n);
}


void ImportFields::setEnabled(bool state)
{
  QWidget::setEnabled(state);
  trans_type_label->setEnabled(state&&bypass_mode);
  trans_type_offset_label->setEnabled(state&&bypass_mode);
  trans_type_offset_spin->setEnabled(state&&bypass_mode);
  trans_type_length_label->setEnabled(state&&bypass_mode);
  trans_type_length_spin->setEnabled(state&&bypass_mode);
  time_type_label->setEnabled(state&&bypass_mode);
  time_type_offset_label->setEnabled(state&&bypass_mode);
  time_type_offset_spin->setEnabled(state&&bypass_mode);
  time_type_length_label->setEnabled(state&&bypass_mode);
  time_type_length_spin->setEnabled(state&&bypass_mode);
}


void ImportFields::valueChangedData(int)
{
  import_changed=true;
}
