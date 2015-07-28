//   rdmarkerwidget.h
//
//   Pointer Control Widget
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDMARKERWIDGET_H
#define RDMARKERWIDGET_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcolor.h>

#include "rdmarker_button.h"
#include "rdmarker_edit.h"

#define RDMARKERWIDGET_BUTTON_FLASH_PERIOD 200

class RDMarkerWidget : public QWidget
{
  Q_OBJECT
 public:
  RDMarkerWidget(const QString &caption,const QColor &color,unsigned samprate,
		 QWidget *parent);
  ~RDMarkerWidget();
  void setDeleteMode(bool state);
  bool isSelected() const;
  void setSelected(bool state);
  int frames() const;
  void setFrames(int frames);
  int value() const;
  void setValue(int msecs);
  void setRange(int lo_limit,int hi_limit);
  void setRange(RDMarkerWidget *lo_limit,RDMarkerWidget *hi_limit);

 signals:
  void deleteClicked();
  void selectionChanged();
  void valueChanged();

 private slots:
  void returnPressedData();
  void escapePressedData();
  void buttonClickedData();

 private:
  bool CheckLimits(int value);
  RDMarkerEdit *mark_edit;
  RDMarkerButton *mark_button;
  bool mark_delete_mode;
  int mark_value;
  unsigned mark_sample_rate;
  int mark_lo_limit;
  int mark_hi_limit;
  RDMarkerWidget *mark_lo_marker;
  RDMarkerWidget *mark_hi_marker;
};



#endif  // RDMARKERWIDGET_H
