// rdtimeedit.h
//
// A QTimeEdit with tenth-second precision.
//
//   (C) Copyright 2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <q3frame.h>
#include <qdatetime.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>

#include <rdtransportbutton.h>

class RDTimeEdit : public Q3Frame
{
  Q_OBJECT
 public:
  enum Display {Hours=0x01,Minutes=0x02,Seconds=0x04,Tenths=0x08};
  RDTimeEdit(QWidget *parent=0);
  ~RDTimeEdit();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QTime time() const;
  bool isReadOnly() const;
  void setFont(const QFont &f);
  uint display() const;
  void setDisplay(uint disp);

 public slots:
  void setTime(const QTime &time);
  void setReadOnly(bool state);
  void setFocus();
  void setGeometry(int x,int y,int w,int h);
  void setGeometry(const QRect &r);

 signals:
  void valueChanged(const QTime &time);

 private slots:
  void upClickedData();
  void downClickedData();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void focusInEvent(QFocusEvent *e);
  void focusOutEvent(QFocusEvent *e);

 private:
  void GetSizeHint();
  void ProcessKey(int key);
  QLabel *edit_labels[4];
  QLabel *edit_sep_labels[3];
  RDTransportButton *edit_up_button;
  RDTransportButton *edit_down_button;
  int edit_widths[4];
  int edit_section_x[3];
  int edit_sep_widths[3];
  int edit_height;
  int edit_section;
  int edit_digit;
  uint edit_display;
  bool edit_read_only;
};


#endif  // RDTimeEdit
