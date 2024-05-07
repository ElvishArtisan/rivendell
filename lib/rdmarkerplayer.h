// rdmarkerplayer.h
//
// Audio player for RDMarkerDialog
//
//   (C) Copyright 2021-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDMARKERPLAYER_H
#define RDMARKERPLAYER_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTimer>

#include <rdmarkerreadout.h>
#include <rdmarkerview.h>
#include <rdstereometer.h>
#include <rdtransportbutton.h>
#include <rdwidget.h>

#define RDMARKERPLAYER_READOUT_QUAN 7

class RDMarkerPlayer : public RDWidget
{
  Q_OBJECT;
 public:
  RDMarkerPlayer(int card,int port,QWidget *parent=0);
  ~RDMarkerPlayer();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool isReadOnly() const;
  void setReadOnly(bool state);
  bool setCut(unsigned cartnum,int cutnum);
  void clearCut();
  int cursorPosition() const;

 public slots:
  void setCursorPosition(int msec);
  void setPointerValue(RDMarkerHandle::PointerRole role,int ptr);
  void setSelectedMarkers(RDMarkerHandle::PointerRole start_role,
			  RDMarkerHandle::PointerRole end_role);

 signals:
  void cursorPositionChanged(unsigned msec);
  void selectedMarkersChanged(RDMarkerHandle::PointerRole start_role,
			      RDMarkerHandle::PointerRole end_role);
  void gotoStartClicked();
  void gotoCursorClicked();
  void gotoEndClicked();
  void noFadeOnSegueChanged(bool state);
  void playGainSet(int db);
  void startTrimClicked(int dbfs);
  void endTrimClicked(int dbfs);

 private slots:
  void buttonGotoStartData();
  void buttonGotoCursorData();
  void buttonGotoEndData();
  void buttonPlayData();
  void buttonPlayFromData();
  void buttonPlayToData();
  void buttonStopData();
  void buttonLoopData();
  void playGainData(int db);
  void noFadeOnSegueData(bool state);
  void buttonTrimStartData();
  void buttonTrimEndData();
  void readoutClickedData(int role);
  void meterData();
  void caePlayedData(unsigned serial);
  void caePausedData(unsigned serial);
  void caePositionData(unsigned serial,unsigned pos);
  void trimThresholdChanged(int dbfs);

 protected:
  void resizeEvent(QResizeEvent *);
  void paintEvent(QPaintEvent *e);

 private:
  QLabel *d_readout_labels[RDMARKERPLAYER_READOUT_QUAN];
  QSignalMapper *d_readout_mapper;
  RDMarkerReadout *d_cut_readout;
  RDMarkerReadout *d_fadeup_readout;
  RDMarkerReadout *d_fadedown_readout;
  RDMarkerReadout *d_talk_readout;
  RDMarkerReadout *d_segue_readout;
  RDMarkerReadout *d_hook_readout;
  QLabel *d_position_label;
  QLabel *d_position_edit;
  QPushButton *d_goto_start_button;
  QPushButton *d_goto_cursor_button;
  QPushButton *d_goto_end_button;
  RDTransportButton *d_play_button;
  RDTransportButton *d_play_from_button;
  RDTransportButton *d_play_to_button;
  RDTransportButton *d_active_play_button;
  RDTransportButton *d_stop_button;
  RDTransportButton *d_loop_button;
  RDStereoMeter *d_meter;
  QTimer *d_meter_timer;
  QCheckBox *d_no_segue_fade_check;
  QLabel *d_no_segue_fade_label;
  QLabel *d_play_gain_label;
  QSpinBox *d_play_gain_spin;
  QLabel *d_play_gain_unit_label;
  QPushButton *d_trim_start_button;
  QPushButton *d_trim_end_button;
  QLabel *d_trim_label;
  QSpinBox *d_trim_spin;
  QList<int> d_cards;
  int d_port;
  unsigned d_cae_serial;
  bool d_is_playing;
  RDMarkerHandle::PointerRole d_selected_markers[2];
  int d_pointers[RDMarkerHandle::LastRole];
  bool d_looping;
  bool d_stopping;
  int d_loop_start_msec;
  int d_loop_start_length;
  int d_cursor_position;
  bool d_read_only;
};


#endif  // RDMARKERPLAYER_H
