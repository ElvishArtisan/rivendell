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

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

#include <rddialog.h>
#include <rdmarkerview.h>
#include <rdtransportbutton.h>

//
// Widget Settings
//
/*
#define EDITAUDIO_WIDGET_WIDTH 834
#define EDITAUDIO_WIDGET_HEIGHT 680
#define EDITAUDIO_WAVEFORM_WIDTH 717
#define EDITAUDIO_WAVEFORM_HEIGHT 352
#define EDITAUDIO_PAN_SIZE 300
#define EDITAUDIO_TAIL_PREROLL 1500
#define EDITAUDIO_DEFAULT_GAIN -12
#define EDITAUDIO_BUTTON_FLASH_PERIOD 200
#define EDITAUDIO_START_GAP 10

//
// Widget Colors
//
#define EDITAUDIO_PLAY_COLOR Qt::white
#define EDITAUDIO_REMOVE_FLASH_COLOR Qt::blue
#define EDITAUDIO_WAVEFORM_COLOR Qt::black
#define EDITAUDIO_HIGHLIGHT_COLOR palette().mid().color()
*/
class RDMarkerDialog : public RDDialog
{
  Q_OBJECT
 public:
  RDMarkerDialog(const QString &caption,QWidget *parent=0);
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

  QPushButton *d_ok_button;
  QPushButton *d_cancel_button;
  unsigned d_cart_number;
  int d_cut_number;
  QString d_caption;
};


#endif  // RDMARKERDIALOG_H
