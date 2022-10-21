// importfields.h
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

#ifndef IMPORTFIELDS_H
#define IMPORTFIELDS_H

#include <QLabel>
#include <QSpinBox>

#include <rdsvc.h>
#include <rdwidget.h>

class ImportFields : public RDWidget
{
 Q_OBJECT
 public:
  ImportFields(RDSvc::ImportSource src,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool changed() const;
  void readFields(RDSvc *svc);
  void setFields(RDSvc *svc);
  bool bypassMode() const;
  void setBypassMode(bool state);

 public slots:
  void setBypassMode(int n);
  void setEnabled(bool state);

 private slots:
  void valueChangedData(int);

 private:
  RDSvc::ImportSource import_source;
  bool bypass_mode;
  bool import_changed;
  QSpinBox *cart_offset_spin;
  QSpinBox *cart_length_spin;
  QSpinBox *title_offset_spin;
  QSpinBox *title_length_spin;
  QSpinBox *hours_offset_spin;
  QSpinBox *hours_length_spin;
  QSpinBox *minutes_offset_spin;
  QSpinBox *minutes_length_spin;
  QSpinBox *seconds_offset_spin;
  QSpinBox *seconds_length_spin;
  QSpinBox *len_hours_offset_spin;
  QSpinBox *len_hours_length_spin;
  QSpinBox *len_minutes_offset_spin;
  QSpinBox *len_minutes_length_spin;
  QSpinBox *len_seconds_offset_spin;
  QSpinBox *len_seconds_length_spin;
  QSpinBox *annctype_offset_spin;
  QSpinBox *annctype_length_spin;
  QSpinBox *data_offset_spin;
  QSpinBox *data_length_spin;
  QSpinBox *event_id_offset_spin;
  QSpinBox *event_id_length_spin;
  QLabel *trans_type_label;
  QLabel *trans_type_offset_label;
  QSpinBox *trans_type_offset_spin;
  QLabel *trans_type_length_label;
  QSpinBox *trans_type_length_spin;
  QLabel *time_type_label;
  QLabel *time_type_offset_label;
  QSpinBox *time_type_offset_spin;
  QLabel *time_type_length_label;
  QSpinBox *time_type_length_spin;
};


#endif  // IMPORTFIELDS_H
