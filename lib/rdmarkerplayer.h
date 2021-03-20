// rdmarkerplayer.h
//
// Audio player for RDMarkerDialog
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

#ifndef RDMARKERPLAYER_H
#define RDMARKERPLAYER_H

#include <QLabel>
#include <QLineEdit>
#include <QTimer>

#include <rdmarkerview.h>
#include <rdstereometer.h>
#include <rdtransportbutton.h>
#include <rdwidget.h>

#define RDMARKERPLAYER_HIGHLIGHT_COLOR palette().mid().color()

class RDMarkerPlayer : public RDWidget
{
  Q_OBJECT;
 public:
  RDMarkerPlayer(int card,int port,QWidget *parent=0);
  ~RDMarkerPlayer();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool setCut(unsigned cartnum,int cutnum);
  void clearCut();

 public slots:
  void setPlayPosition(int msec);
  void setPointerValue(RDMarkerHandle::PointerRole role,int ptr);
  void setSelectedMarker(RDMarkerHandle::PointerRole role);

 signals:
  void cursorPositionChanged(unsigned msec);

 private slots:
  void playCursorData();
  void playStartData();
  void pauseData();
  void stopData();
  void loopData();
  void meterData();
  void caePlayedData(int handle);
  void caePausedData(int handle);
  void caePositionData(int handle,unsigned pos);

 protected:
  void resizeEvent(QResizeEvent *);
  void paintEvent(QPaintEvent *e);

 private:
  void UpdateReadouts();
  QLabel *d_position_label;
  //  QLineEdit *d_position_edit;
  QLabel *d_position_edit;
  QLabel *d_region_edit_label;
  //  QLineEdit *d_region_edit;
  QLabel *d_region_edit;
  QLabel *d_length_label;
  //  QLineEdit *d_length_edit;
  QLabel *d_length_edit;
  RDTransportButton *d_play_cursor_button;
  RDTransportButton *d_play_start_button;
  RDTransportButton *d_active_play_button;
  RDTransportButton *d_pause_button;
  RDTransportButton *d_stop_button;
  RDTransportButton *d_loop_button;
  RDStereoMeter *d_meter;
  QTimer *d_meter_timer;
  QList<int> d_cards;
  int d_port;
  int d_cae_stream;
  int d_cae_handle;
  bool d_is_playing;
  RDMarkerHandle::PointerRole d_selected_marker;

  int d_pointers[RDMarkerHandle::LastRole];
};


#endif  // RDMARKERPLAYER_H
