// rdcueedit.h
//
// Cueing Editor for RDLogLine-based Events
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCUEEDIT_H
#define RDCUEEDIT_H

#include <rdcae.h>
#include <rdevent_player.h>
#include <rdplay_deck.h>
#include <rdpushbutton.h>
#include <rdslider.h>
#include <rdtransportbutton.h>
#include <rdmarker_bar.h>
#include <rdtimeedit.h>
#include <rdwidget.h>

class RDCueEdit : public RDWidget
{
  Q_OBJECT
 public:
  RDCueEdit(QWidget *parent=0);
  ~RDCueEdit();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setRml(RDEventPlayer *player,const QString &start_rml,
	      const QString &stop_rml);
  bool initialize(RDLogLine *logline);
  unsigned playPosition(RDMarkerBar::Marker marker) const;
  void stop();

 public slots:
  void recue();

 private slots:
  void sliderPressedData();
  void sliderReleasedData();
  void sliderChangedData(int pos);
  void auditionButtonData();
  void pauseButtonData();
  void stopButtonData();
  void stateChangedData(int id,RDPlayDeck::State state);
  void positionData(int id,int msecs);
  void startClickedData();
  void endClickedData();
  void auditionTimerData();
  virtual void wheelEvent(QWheelEvent *e);
  virtual void mousePressEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

 private:
  void SetStartMode(bool state);
  void SetEndMode(bool state);
  void Playing(int id);
  void Paused(int id);
  void Stopped(int id);
  void UpdateCounters();
  void ClearChannel();
  RDLogLine *edit_logline;
  RDPlayDeck *edit_play_deck;
  RDEventPlayer *edit_event_player;
  QString edit_start_rml;
  QString edit_stop_rml;
  RDSlider *edit_slider;
  QLabel *edit_up_label;
  QLabel *edit_down_label;
  QFont normal_font;
  RDTransportButton *edit_audition_button;
  RDTransportButton *edit_pause_button;
  RDTransportButton *edit_stop_button;
  int edit_height;
  bool edit_slider_pressed;
  QPalette edit_play_color;
  QPalette edit_start_color;
  int edit_start_pos;
  QLabel *edit_position_label;
  RDMarkerBar *edit_position_bar;
  RDPushButton *edit_start_button;
  RDPushButton *edit_end_button;
  RDPushButton *edit_recue_button;
  bool edit_shift_pressed;
  bool edit_right_click_stop;
  QTimer *edit_audition_timer;
};


#endif  // RDCUEEDIT_H

