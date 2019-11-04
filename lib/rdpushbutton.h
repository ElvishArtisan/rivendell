//   rdpushbutton.h
//
//   A flashing button widget.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPUSHBUTTON_H
#define RDPUSHBUTTON_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <QMouseEvent>

#include <rdfontengine.h>

/*
 * Widget Defaults
 */
#define RDPUSHBUTTON_DEFAULT_FLASH_PERIOD 300
#define RDPUSHBUTTON_DEFAULT_FLASH_COLOR Qt::blue

class RDPushButton : public QPushButton, public RDFontEngine
{
  Q_OBJECT
 public:
  enum ClockSource {InternalClock=0,ExternalClock=1};
  RDPushButton(QWidget *parent,RDConfig *c=NULL);
  RDPushButton(const QString &text,QWidget *parent,RDConfig *c=NULL);
  RDPushButton(const QIcon &icon,const QString &text,QWidget *parent,
	       RDConfig *c=NULL);
  QString text() const;
  void setText(const QString &str);
  bool wordWrap() const;
  void setWordWrap(bool state);
  QColor flashColor() const;
  void setFlashColor(QColor color);
  int flashPeriod() const;
  void setFlashPeriod(int period);
  ClockSource clockSource() const;
  void setClockSource(ClockSource src);
  int id() const;
  void setId(int id);
  bool flashingEnabled() const;

 public slots:
  void setFlashingEnabled(bool state);
  void setPalette(const QPalette &);
  void tickClock();
  void tickClock(bool state);

 signals:
  void centerClicked();
  void centerClicked(int id,const QPoint &pt);
  void centerPressed();
  void centerReleased();
  void rightClicked();
  void rightClicked(int id,const QPoint &pt);
  void rightPressed();
  void rightReleased();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

 private:
  void flashOn();
  void flashOff();
  void Init();
  void ComposeText();
  QString plain_text;
  bool word_wrap_enabled;
  bool flash_state;
  int flash_period;
  bool flashing_enabled;
  QColor flash_color;
  QPalette flash_palette;
  QPalette off_palette;
  QTimer *flash_timer;
  int button_id;
  ClockSource flash_clock_source;
};


#endif  // RDPUSHBUTTON_H
