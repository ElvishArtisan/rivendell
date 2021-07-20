// dowselector.h
//
// Day of the week selector
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef DOWSELECTOR_H
#define DOWSELECTOR_H

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>

#include <rdwidget.h>

class DowSelector : public RDWidget
{
  Q_OBJECT
 public:
  DowSelector(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool dayOfWeekEnabled(int dow);
  void toRecording(unsigned record_id) const;
  void fromRecording(unsigned record_id);
  
 public slots:
  void enableDayOfWeek(int dow,bool state);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QGroupBox *d_group_box;
  QCheckBox *d_checks[7];
  QLabel *d_labels[7];
};


#endif  // DOWSELECTOR_H

