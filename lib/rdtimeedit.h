// rdtimeedit.h
//
// A QTimeEdit with tenth-second precision.
//
//   (C) Copyright 2003-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTIMEEDIT_H
#define RDTIMEEDIT_H

#include <QTimeEdit>
#include <QValidator>

class RDTimeEdit : public QTimeEdit
{
  Q_OBJECT
 public:
  enum Mode {TimeMode=0,LengthMode=1};
  RDTimeEdit(QWidget *parent=0);
  Mode mode() const;
  void setMode(Mode mode);
  bool showHours() const;
  void setShowHours(bool state);
  bool showTenths() const;
  void setShowTenths(bool state);
  bool isReadOnly() const;
  int length() const;

 public slots:
  void setLength(int msec);
  void setReadOnly(bool state);

 protected:
  QValidator::State validate(QString &input,int &pos) const;
  void fixup(QString &input) const;
  void stepBy(int steps);
  QAbstractSpinBox::StepEnabled stepEnabled() const;

 private:
  void SetFormat();
  bool d_show_hours;
  bool d_show_tenths;
  int d_width_variance;
  QAbstractSpinBox::StepEnabled d_step_enabled;
  bool d_read_only;
  Mode d_mode;
};


#endif  // RDTIMEEDIT_H
