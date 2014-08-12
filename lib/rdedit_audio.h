// rdedit_audio.h
//
// Edit Rivendell Audio
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdedit_audio.h,v 1.9.6.2 2013/11/13 23:36:33 cvs Exp $
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

#include <qdialog.h>
#include <qscrollbar.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qpointarray.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qnamespace.h>
#include <qcheckbox.h>

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

//
// Widget Settings
//
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
	      int preroll,int trim_level,
	      QWidget *parent=0,const char *name=0);
  ~RDEditAudio();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void saveData();
  void cancelData();
  void xFullIn();
  void xUp();
  void xDown();
  void xFullOut();
  void yUp();
  void yDown();
  void gotoCursorData();
  void gotoHomeData();
  void gotoEndData();
  void hscrollData(int);
  void playStartData();
  void playCursorData();
  void pauseData();
  void stopData();
  void loopData();
  void playedData(int handle);
  void pausedData(int handle);
  void positionData(int handle,unsigned pos);
  void cuePointData(int);
  void cueEditData(int);
  void cueEscData(int);
  void updateMenuData();
  void deleteSegueData();
  void deleteFadeupData();
  void deleteFadedownData();
  void deleteTalkData();
  void deleteHookData();
  void trimHeadData();
  void trimTailData();
  void gainUpPressedData();
  void gainDownPressedData();
  void gainChangedData();
  void gainReleasedData();
  void gainTimerData();
  void removeButtonData();
  void meterData();
  
 protected:
  void paintEvent(QPaintEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void closeEvent(QCloseEvent *e);
  
 private:
  enum CuePoints {Play=0,Start=1,End=2,SegueStart=3,SegueEnd=4,
		  TalkStart=5,TalkEnd=6,HookStart=7,HookEnd=8,
		  FadeUp=9,FadeDown=10,LastMarker=11};
  enum Arrow {None=0,Left=1,Right=2};
  enum PlayMode {FromStart=1,FromCursor=2,Region=3};
  enum GainChange {GainNone=0,GainUp=1,GainDown=2};
  void DeleteMarkerData(int id);
  void PreRoll(int cursor,RDEditAudio::CuePoints);
  bool PositionCursor(int cursor=0,bool relative=false);
  void ValidateMarkers();
  bool SaveMarkers();
  void LoopRegion(int cursor0,int cursor1);
  void UpdateCounters();
  void DrawMaps();
  void UpdateCursors();
  void DrawCursors(int xpos,int ypos,int xsize,int ysize,int chan);
  int DrawCursor(int xpos,int ypos,int xsize,int ysize,int chan,
		 int samp,int prev,QColor color,Arrow arrow,int apos,
		 RDEditAudio::CuePoints pt,Qt::RasterOp op=Qt::CopyROP);
  void EraseCursor(int xpos,int ypos,int xsize,int ysize,int chan,
		   int samp,int prev,QColor color,Arrow arrow,int apos);
  void DrawWave(int xsize,int ysize,int chan,QString label,QPixmap *pix);
  void DrawPointers();
  void CenterDisplay();
  int GetTime(int samples);
  RDCut *edit_cut;
  RDPeaksExport *edit_peaks;
  unsigned edit_sample_rate;
  unsigned edit_sample_length;
  unsigned edit_channels;
  QPopupMenu *edit_menu;
  QScrollBar *edit_hscroll;
  QImage edit_left_ref_image;
  QImage edit_right_ref_image;
  QImage edit_left_image;
  QImage edit_right_image;
  QLineEdit *edit_overall_edit;
  QLineEdit *edit_region_edit;
  QLabel *edit_region_edit_label;
  QLineEdit *edit_size_edit;
  RDMarkerEdit *edit_cursor_edit[RDEditAudio::LastMarker];
  QCursor *edit_arrow_cursor;
  QCursor *edit_cross_cursor;
  QPointArray *edit_wave_array;
  QSpinBox *edit_trim_box;
  RDMarkerButton *edit_cue_button[RDEditAudio::LastMarker];
  QString edit_cue_string;
  RDPushButton *edit_remove_button;
  QCheckBox *edit_overlap_box;
  RDTransportButton *edit_loop_button;
  RDTransportButton *edit_play_start_button;
  RDTransportButton *edit_play_cursor_button;
  RDTransportButton *edit_pause_button;
  RDTransportButton *edit_stop_button;
  RDStereoMeter *edit_meter;
  QRangeControl *edit_gain_control;
  RDMarkerEdit *edit_gain_edit;
  QTimer *edit_gain_timer;
  QTimer *edit_meter_timer;
  GainChange edit_gain_mode;
  int edit_gain_count;
  CuePoints edit_cue_point;
  PlayMode edit_play_mode;
  unsigned short *edit_energy;
  int edit_energy_size;
  double edit_factor_x;
  double edit_max_factor_x;
  int edit_gain;
  bool is_playing;
  bool is_paused;
  bool is_stopped;
  bool use_looping;
  bool is_looping;
  bool left_button_pressed;
  bool center_button_pressed;
  bool delete_marker;
  bool pause_mode;
  int baseline;
  int played_cursor;
  int edit_cursors[RDEditAudio::LastMarker];
  short *energy_data;
  int energy_size;
  bool ignore_pause;
  int edit_handle;
  int edit_card;
  int edit_stream;
  int edit_port;
  int edit_preroll;
  RDCae *edit_cae;
  RDStation *edit_station;
  RDUser *edit_user;
  RDConfig *edit_config;
};


#endif  // RDEDIT_AUDIO_H
