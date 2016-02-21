//   rdmarkertransport.h
//
//   Audio transport controls for the RDEditAudio dialog.
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

#ifndef RDMARKERTRANSPORT_H
#define RDMARKERTRANSPORT_H

#include <qlabel.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qwidget.h>

#include <rdcae.h>
#include <rdcut.h>
#include <rdstereometer.h>
#include <rdtransportbutton.h>

#define RDMARKERTRANSPORT_HIGHLIGHT_COLOR colorGroup().mid()

class RDMarkerTransport : public QWidget
{
  Q_OBJECT
 public:
  enum PlayMode {FromStart=1,FromCursor=2,Region=3};
  RDMarkerTransport(RDCut *cut,RDCae *cae,int card,int port,QWidget *parent=0);
  ~RDMarkerTransport();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setStartPosition(int msecs);
  void setEndPosition(int msecs);
  void setGain(int gain);

 signals:
  void positionChanged(int msecs);

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private slots:
  void playStartData();
  void playCursorData();
  void pauseData();
  void stopData();
  void loopData();
  void meterData();
  void playedData(int handle);
  void stoppedData(int handle);
  void positionData(int handle,unsigned msecs);

 private:
  RDTransportButton *trans_play_cursor_button;
  RDTransportButton *trans_play_start_button;
  RDTransportButton *trans_pause_button;
  RDTransportButton *trans_stop_button;
  RDTransportButton *trans_loop_button;
  QLabel *trans_position_label;
  QLineEdit *trans_overall_edit;
  QLabel *trans_region_trans_label;
  QLineEdit *trans_region_edit;
  QLabel *trans_size_label;
  QLineEdit *trans_size_edit;
  RDStereoMeter *trans_meter;
  QTimer *trans_meter_timer;
  int trans_handle;
  int trans_card;
  int trans_stream;
  int trans_port;
  bool trans_is_playing;
  bool trans_pause_mode;
  bool trans_is_paused;
  bool trans_is_stopped;
  bool trans_is_looping;
  RDMarkerTransport::PlayMode trans_play_mode;
  RDCut *trans_cut;
  RDCae *trans_cae;
  int trans_start_position;
  int trans_end_position;
  int trans_gain;
};


#endif  // RDMARKERTRANSPORT_H
