// voice_tracker.h
//
// A Rivendell Voice Tracker
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: voice_tracker.h,v 1.50 2010/09/16 19:52:08 cvs Exp $
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

#ifndef VOICE_TRACKER_H
#define VOICE_TRACKER_H

#include <qdialog.h>
#include <qlistview.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qpopupmenu.h>

#include <rdtransportbutton.h>
#include <rdstereometer.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdgroup.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdplay_deck.h>
#include <log_listview.h>
#include <rdevent_player.h>
#include <rdwavepainter.h>
#include <rdsettings.h>

//
// Widget Settings
//
#define TRACKER_TEXT_COLOR red
#define TRACKER_RUBBERBAND_COLOR "#008000"
#define TRACKER_RECORD_COLOR green
#define TRACKER_RECORD_BUTTON_COLOR red
#define TRACKER_START_BUTTON_COLOR green
#define TRACKER_ABORT_BUTTON_COLOR red
#define TRACKER_DONE_BUTTON_COLOR blue
#define TRACKER_START_WIDTH 19633
#define TRACKER_MSECS_PER_PIXEL 29
//#define TRACKER_MB_PER_PIXEL 141
#define TRACKER_MB_PER_PIXEL 42
#define TRACKER_GAIN_MARGIN 4
#define TRACKER_MAX_GAIN 0
#define TRACKER_MIN_GAIN -3000
#define TRACKER_X_ORIGIN 12
#define TRACKER_X_WIDTH sizeHint().width()-123
#define TRACKER_Y_ORIGIN 12
#define TRACKER_Y_HEIGHT 79
#define TRACKER_SCROLL_SCALE 2
#define TRACKER_SCROLL_SCALE 2
#define TRACKER_FORCED_SEGUE 1000

class VoiceTracker : public QDialog
{
  Q_OBJECT
 public:
  VoiceTracker(const QString &logname,QString *import_path,
	       QWidget *parent=0,const char *name=0);
  ~VoiceTracker();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void updateMenuData();
  void hideMenuData();
  virtual void keyPressEvent(QKeyEvent *e);
  virtual void keyReleaseEvent(QKeyEvent *e);
  virtual void wheelEvent(QWheelEvent *e);
  void playData();
  void stopData();
  void track1Data();
  void recordData();
  void track2Data();
  void finishedData();
  void postData();
  void resetData();
  void insertData();
  void insertData(int line,RDLogLine *logline,bool warn);
  void deleteData();
  void deleteData(int line,bool warn);
  void previousData();
  void nextData();
  void editAudioData();
  void undoChangesData();
  void setStartPointData();
  void setEndPointData();
  void setHookPointData();
  void stateChangedData(int id,RDPlayDeck::State state);
  void positionData(int id,int msecs);
  void segueStartData(int id);
  void logClickedData(QListViewItem *item,const QPoint &pt,int col);
  void transitionChangedData(int line,RDLogLine::TransType trans);
  void meterData();
  void recordLoadedData(int card,int stream);
  void recordingData(int card,int stream);
  void recordStoppedData(int card,int stream);
  void recordUnloadedData(int cart,int stream,unsigned msecs);
  void closeData();

 protected:
  void paintEvent(QPaintEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

 private:
  enum DeckState {DeckIdle=0,DeckTrack1=1,DeckTrack2=2,DeckTrack3=3};
  enum Target {FadeupGain=0,FadeupPoint=1,FadedownGain=2,FadedownPoint=3,
	       TrackFadeupGain=4,TrackFadeupPoint=5,
	       TrackFadedownGain=6,TrackFadedownPoint=7,TargetSize=8};
  void LoadTrack(int line);
  void SaveTrack(int line);
  bool ImportTrack(RDListViewItem *item);
  void RenderTransition(int line);
  void LoadBlockLength(int line);
  void RefreshList();
  void RefreshLine(RDListViewItem *item);
  void StartNext(int finishing_id,int next_id=-1);
  QString GetCutName(int line,RDCut **cut);
  int GetClick(QMouseEvent *e,QPoint *p);
  void DragTrack(int trackno,int xdiff);
  void DragTarget(int trackno,const QPoint &pt);
  void DrawTrackMap(int trackno);
  void DrawCursor(RDWavePainter *wp,int height,int xpos,const QColor &color,
		  int arrow_offset,bool left_arrow);
  void DrawRubberBand(RDWavePainter *wp,int trackno);
  void DrawTarget(RDWavePainter *wp,VoiceTracker::Target target,
		  int trackno,int xpos,int ypos);
  void WriteTrackMap(int trackno);
  bool TransportActive();
  bool PlayoutActive();
  void UpdateControls();
  void UpdateRemaining();
  bool TrackAvailable();
  void LogLine(const QString &line);
  bool InitTrack();
  void FinishTrack();
  double GetCurrentTime();
  bool IsTrack(int line,bool *offset);
  bool CanInsertTrack();
  bool CanDeleteTrack();
  void ClearCursor(QPainter *p);
  RDListViewItem *GetItemByLine(int line);
  void CheckChanges();
  void PushSegues();
  void PopSegues();
  RDStereoMeter *track_meter;
  QTimer *track_meter_timer;
  RDTransportButton *track_play_button;
  RDTransportButton *track_stop_button;
  QPushButton *track_track1_button;
  QPushButton *track_record_button;
  QPushButton *track_track2_button;
  QPushButton *track_finished_button;
  QPushButton *track_post_button;
  QPushButton *track_reset_button;
  QPushButton *track_previous_button;
  QPushButton *track_next_button;
  QPushButton *track_insert_button;
  QPushButton *track_delete_button;
  QPushButton *track_close_button;
  RDLog *track_log;
  RDLogEvent *track_log_event;
  RDEventPlayer *track_event_player;
  QString edit_log_name;
  int track_line;
  unsigned track_tracks;
  int track_time_remaining;
  int track_time_remaining_start;
  int track_time_counter;
  bool track_block_valid;
  QTime track_start_time;
  LogListView *track_log_list;
  QLabel *edit_length_label;
  QLabel *edit_tracks_remaining_label;
  QLabel *edit_time_remaining_label;
  QPalette edit_time_remaining_palette[2];
  QPixmap *edit_wave_map[3];
  QString edit_wave_name[3];
  RDWavePainter *wpg[3];
  //int edit_wave_pos[3];
  int edit_scroll_pos[3];
  int edit_wave_origin[3];
  RDLogLine *edit_logline[3];
  RDLogLine *edit_saved_logline[3];
  int edit_segue_start_point[3];
  int edit_segue_gain[3];
  int edit_segue_start_offset[3];
  RDPlayDeck *edit_deck[3];
  int edit_track_line[3];
  bool track_redraw[3];
  unsigned track_redraw_count;
  RDCart *edit_track_cart;

  //RDCut *edit_track_cut;
  RDCut *edit_track_cuts[3];

  RDGroup *track_group;
  int edit_wave_width;
  int edit_cursor_pos;
  QPixmap *edit_playout_map;
  QPixmap *edit_macro_map;
  QPixmap *edit_marker_map;
  QPixmap *edit_chain_map;
  QPixmap *edit_track_cart_map;
  QPixmap *edit_music_map;
  QPixmap *edit_notemarker_map;
  QPixmap *edit_traffic_map;
  QPixmap *edit_mic16_map;
  int edit_input_card;
  int edit_input_port;
  int edit_output_card;
  int edit_output_port;
  int edit_tail_preroll;
  int edit_threshold_level;
  RDCae::AudioCoding edit_coding;
  unsigned edit_format;
  unsigned edit_samprate;
  unsigned edit_bitrate;
  unsigned edit_chans;
  unsigned play_start_macro;
  unsigned play_end_macro;
  unsigned record_start_macro;
  unsigned record_end_macro;
  DeckState edit_deck_state;
  bool edit_sliding;
  bool edit_scrolling;
  int edit_scroll_threshold;
  QPoint *edit_previous_point;
  int edit_current_track;
  int edit_rightclick_track;
  int edit_rightclick_pos;
  QCursor *track_current_cursor;
  QCursor *track_arrow_cursor;
  QCursor *track_hand_cursor;
  QCursor *track_cross_cursor;
  bool track_loaded;
  bool track_offset;
  bool segue_loaded;
  bool track_recording;
  bool track_changed;
  double track_record_start_time;
  int track_recording_pos;
  bool track_aborting;
  bool track_record_ran;
  RDLogLine *track_dummy0_logline;
  RDLogLine *track_dummy2_logline;
  QString track_import_filter;
  QString track_import_group;
  int track_preroll;
  bool track_size_altered;
  QPalette track_start_palette;
  QPalette track_record_palette;
  QPalette track_done_palette;
  QPalette track_abort_palette;
  QPopupMenu *track_menu;
  int menu_clicked_point;
  QString *edit_import_path;
  RDSettings *edit_settings;
  QRect *track_track_rect;
  QRect *track_trackzones_rect[3];
  QRect *track_target_rect[VoiceTracker::TargetSize];
  int track_target_track[VoiceTracker::TargetSize];
  VoiceTracker::Target track_current_target;
  bool edit_shift_pressed;
};


#endif
