// rdmarkerdialog.h
//
// Rivendell Audio Marker Editor
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

#ifndef RDMARKERDIALOG_H
#define RDMARKERDIALOG_H

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>

#include <rddialog.h>
#include <rdmarkerplayer.h>
#include <rdmarkerreadout.h>
#include <rdmarkerview.h>
#include <rdstereometer.h>
#include <rdtransportbutton.h>

//
// Widget Settings
//
//#define RDMARKERDIALOG_WIDGET_WIDTH 834
//#define RDMARKERDIALOG_WIDGET_HEIGHT 680
#define RDMARKERDIALOG_WAVEFORM_WIDTH 717
#define RDMARKERDIALOG_WAVEFORM_HEIGHT 352
//#define RDMARKERDIALOG_PAN_SIZE 300
//#define RDMARKERDIALOG_TAIL_PREROLL 1500
//#define RDMARKERDIALOG_DEFAULT_GAIN -12
//#define RDMARKERDIALOG_BUTTON_FLASH_PERIOD 200
//#define RDMARKERDIALOG_START_GAP 10

//
// Widget Colors
//
//#define RDMARKERDIALOG_PLAY_COLOR Qt::white
//#define RDMARKERDIALOG_REMOVE_FLASH_COLOR Qt::blue
//#define RDMARKERDIALOG_WAVEFORM_COLOR Qt::black
#define RDMARKERDIALOG_HIGHLIGHT_COLOR palette().mid().color()

class RDMarkerDialog : public RDDialog
{
  Q_OBJECT
 public:
  enum PlayMode {FromStart=1,FromCursor=2,Region=3};
  enum GainChange {GainNone=0,GainUp=1,GainDown=2};
  RDMarkerDialog(const QString &caption,int card,int port,QWidget *parent=0);
  ~RDMarkerDialog();
  QSize sizeHint() const;

 public slots:
  int exec(unsigned cartnum,int cutnum);
   
 private slots:
  void amplitudeUpData();
  void amplitudeDownData();
  void timeFullInData();
  void timeInData();
  void timeOutData();

  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  RDMarkerView *d_marker_view;

  QGroupBox *d_amplitude_box;
  RDTransportButton *d_amplitude_up_button;
  RDTransportButton *d_amplitude_down_button;

  QGroupBox *d_time_box;
  QPushButton *d_time_fullin_button;
  RDTransportButton *d_time_in_button;
  RDTransportButton *d_time_out_button;
  QPushButton *d_time_fullout_button;

  RDMarkerPlayer *d_player;

  RDMarkerReadout *d_cut_readout;
  RDMarkerReadout *d_fadeup_readout;
  RDMarkerReadout *d_fadedown_readout;
  RDMarkerReadout *d_talk_readout;
  RDMarkerReadout *d_segue_readout;
  RDMarkerReadout *d_hook_readout;

  QGroupBox *d_goto_group;
  QPushButton *d_goto_cursor_button;
  QPushButton *d_goto_home_button;
  QPushButton *d_goto_end_button;

  QCheckBox *d_no_segue_fade_check;
  QLabel *d_no_segue_fade_label;

  QLabel *d_play_gain_label;
  QSpinBox *d_play_gain_spin;
  QLabel *d_play_gain_unit_label;

  QPushButton *d_ok_button;
  QPushButton *d_cancel_button;
  unsigned d_cart_number;
  int d_cut_number;
  QString d_caption;
};


#endif  // RDMARKERDIALOG_H
