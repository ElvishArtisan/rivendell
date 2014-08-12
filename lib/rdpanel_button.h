// rdpanel_button.h
//
// The SoundPanel Button for RDAirPlay.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpanel_button.h,v 1.18.6.5 2014/02/06 20:43:47 cvs Exp $
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

#ifndef RDPANEL_BUTTON_H
#define RDPANEL_BUTTON_H

#include <qdatetime.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qpalette.h>
#include <qtimer.h>

#include <rdstation.h>
#include <rdairplay_conf.h>
#include <rdplay_deck.h>
#include <rdlog_line.h>
#include <rdcartdrag.h>

#define RDPANEL_BUTTON_MARGIN 5

class RDPanelButton : public QPushButton
{
 Q_OBJECT
 public:
  RDPanelButton(int row,int col,RDStation *station,bool flash,QWidget *parent=0,
		const char *name=0);
  void clear();
  QString text() const;
  void setText(const QString &text);
  QString outputText() const;
  void setOutputText(const QString &text);
  bool state() const;
  void setState(bool state);
  QColor defaultColor() const;
  void setDefaultColor(QColor color);
  RDLogLine::StartSource startSource() const;
  void setStartSource(RDLogLine::StartSource src);
  void setColor(QColor color);
  unsigned cart() const;
  void setCart(unsigned cart);
  QString cutName() const;
  void setCutName(const QString &name);
  int deck() const;
  void setDeck(int deck);
  QTime startTime() const;
  void setStartTime(QTime time);
  bool hookMode() const;
  void setHookMode(bool state);
  RDPlayDeck *playDeck() const;
  void setPlayDeck(RDPlayDeck *deck);
  int output() const;
  void setOutput(int outnum);
  int length(bool hookmode) const;
  void setLength(bool hookmode, int msecs);
  void setActiveLength(int msecs);
  void reset();
  bool pauseWhenFinished() const;
  void setPauseWhenFinished(bool pause_when_finished);
  int duckVolume() const;
  void setDuckVolume(int lvel);
  void setAllowDrags(bool state);
  void resetCounter();

 signals:
  void cartDropped(int row,int col,unsigned cartnum,const QColor &color,
		   const QString &title);

 public slots:
  void tickClock();
  void flashButton(bool state);

 private slots:
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

 protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void dragEnterEvent(QDragEnterEvent *e);
  void dropEvent(QDropEvent *e);

 private:
  void WriteKeycap(int secs);
  QString WrapText(QString text,int *lines);
  QString GetNextLine(QString *str,const QFontMetrics &m,int len);
  QColor button_default_color;
  QWidget *button_parent;
  QString button_text;
  QString button_formatted_text;
  QString button_output_text;
  QColor button_color;
  unsigned button_cart;
  QString button_cutname;
  int button_deck;
  QTime button_start_time;
  bool button_hook_mode;
  QTime button_end_time;
  int button_length[2];
  int button_active_length;
  int button_secs;
  RDStation *button_station;
  RDPlayDeck *button_play_deck;
  int button_output;
  bool button_flash;
  bool button_flashing;
  bool button_flash_state;
  bool button_state;
  bool button_pause_when_finished;
  int button_duck_volume;
  RDLogLine::StartSource button_start_source;
  int button_row;
  int button_col;
  int button_move_count;
  bool button_allow_drags;
};
#endif
