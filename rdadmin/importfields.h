// importfields.h
//
// Parser Parameters for RDAdmin.
//
// (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: importfields.h,v 1.2 2010/07/29 19:32:34 cvs Exp $
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

#ifndef IMPORTFIELDS_H
#define IMPORTFIELDS_H

#include <qwidget.h>
#include <qlabel.h>
#include <qspinbox.h>

#include <rdsvc.h>

class ImportFields : public QWidget
{
 Q_OBJECT
 public:
  ImportFields(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool changed() const;
  void readFields(RDSvc *svc,RDSvc::ImportSource type);
  void setFields(RDSvc *svc,RDSvc::ImportSource type);

 private slots:
  void valueChangedData(int);

 private:
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
};


#endif  // IMPORTFIELDS_H
