// rdedit_audio.h
//
// Edit Rivendell Audio
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEDIT_AUDIO_H
#define RDEDIT_AUDIO_H

#include <qcheckbox.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qspinbox.h>

#include <rdconfig.h>
#include <rdmarker_edit.h>
#include <rdtransportbutton.h>
#include <rdstereometer.h>
#include <rdwavefile.h>
#include <rdpushbutton.h>
#include <rdmarker_button.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdcae.h>
#include <rduser.h>
#include <rdstation.h>
#include <rdpeaksexport.h>
#include <rdmarkerwaveform.h>
#include <rdmarkerwidget.h>

//
// Widget Settings
//
#define EDITAUDIO_WIDGET_WIDTH 834
#define EDITAUDIO_WIDGET_HEIGHT 680
#define EDITAUDIO_PAN_SIZE 300
#define EDITAUDIO_TAIL_PREROLL 1500
#define EDITAUDIO_DEFAULT_GAIN -12
#define EDITAUDIO_BUTTON_FLASH_PERIOD 200
#define EDITAUDIO_START_GAP 10

//
// Widget Colors
//
#define EDITAUDIO_PLAY_COLOR white
#define EDITAUDIO_REMOVE_FLASH_COLOR blue
#define EDITAUDIO_WAVEFORM_COLOR black
#define EDITAUDIO_HIGHLIGHT_COLOR colorGroup().mid()

class RDEditAudio : public QDialog
{
  Q_OBJECT
 public:
  RDEditAudio(RDCart *cart,QString cut_name,RDCae *cae,RDUser *user,
	      RDStation *station,RDConfig *config,int card,int port,
	      int preroll,int trim_level,QWidget *parent=0);
  ~RDEditAudio();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private slots:
  void viewportWidthChangedData(int msecs);
  void waveformClickedData(int msecs);
  void playStartData();
  void playCursorData();
  void pauseData();
  void stopData();
  void loopData();
  void meterData();
  void markerButtonEnabledData(int id);
  void markerButtonDeleteData(int id);
  void markerValueChangedData(int id);
  void trimHeadData();
  void trimTailData();
  void gainUpPressedData();
  void gainDownPressedData();
  void gainChangedData();
  void gainReleasedData();
  void gainTimerData();
  void removeButtonData();
  void gotoCursorData();
  void gotoHomeData();
  void gotoEndData();
  void saveData();
  void cancelData();

 private:
  RDMarkerWaveform *edit_waveform[2];
  QScrollBar *edit_waveform_scroll;

  RDTransportButton *edit_play_cursor_button;
  RDTransportButton *edit_play_start_button;
  RDTransportButton *edit_pause_button;
  RDTransportButton *edit_stop_button;
  RDTransportButton *edit_loop_button;
  QLabel *edit_position_label;
  QLineEdit *edit_overall_edit;
  QLabel *edit_region_edit_label;
  QLineEdit *edit_region_edit;
  QLabel *edit_size_label;
  QLineEdit *edit_size_edit;
  RDStereoMeter *edit_meter;
  QTimer *edit_meter_timer;
  RDMarkerWidget *edit_marker_widget[RDMarkerWaveform::LastMarker];
  QLabel *edit_trim_label;
  QSpinBox *edit_trim_box;
  QPushButton *edit_trim_start_button;
  QPushButton *edit_trim_end_button;
  RDTransportButton *gain_up_button;
  RDTransportButton *gain_down_button;
  QRangeControl *edit_gain_control;
  RDMarkerEdit*edit_gain_edit;
  QLabel *edit_gain_label;
  QTimer *edit_gain_timer;
  RDPushButton *edit_remove_button;
  QCheckBox *edit_overlap_box;
  QLabel *edit_overlap_label;
  QLabel *edit_goto_label;
  QPushButton *edit_goto_cursor_button;
  QPushButton *edit_goto_home_button;
  QPushButton *edit_goto_end_button;

  QLabel *edit_amp_label;
  RDTransportButton *edit_ampup_button;
  RDTransportButton *edit_ampdown_button;
  QLabel *edit_time_label;
  QPushButton *edit_fullin_button;
  RDTransportButton *edit_zoomin_button;
  RDTransportButton *edit_zoomout_button;
  QPushButton *edit_fullout_button;
  RDCut *edit_cut;
  int edit_card;
  int edit_port;
  QPushButton *edit_save_button;
  QPushButton *edit_cancel_button;
};


#endif  // RDEDIT_AUDIO_H
