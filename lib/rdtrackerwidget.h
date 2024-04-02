// rdtrackerwidget.h
//
// Rivendell Voice Tracker
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTRACKERWIDGET_H
#define RDTRACKERWIDGET_H

#include <rdevent_player.h>
#include <rdgroup.h>
#include <rdlog.h>
#include <rdloglock.h>
#include <rdmarkerdialog.h>
#include <rdplay_deck.h>
#include <rdsvc.h>
#include <rdstereometer.h>
#include <rdtransportbutton.h>
#include <rdtrackermodel.h>
#include <rdtrackertableview.h>
#include <rdwavepainter.h>
#include <rdwidget.h>

//
// Widget Settings
//
#define TRACKER_TEXT_COLOR Qt::red
#define TRACKER_RUBBERBAND_COLOR QColor("#008000")
#define TRACKER_RECORD_COLOR Qt::green
#define TRACKER_RECORD_BUTTON_COLOR Qt::red
#define TRACKER_START_BUTTON_COLOR Qt::green
#define TRACKER_ABORT_BUTTON_COLOR Qt::red
#define TRACKER_DONE_BUTTON_COLOR Qt::blue
#define TRACKER_START_WIDTH 19633
#define TRACKER_MSECS_PER_PIXEL 29
#define TRACKER_MB_PER_PIXEL 42
#define TRACKER_GAIN_MARGIN 4
#define TRACKER_MAX_GAIN 0
#define TRACKER_MIN_GAIN -3000
#define TRACKER_X_ORIGIN 2
#define TRACKER_X_WIDTH size().width()-89
#define TRACKER_Y_ORIGIN 2
#define TRACKER_Y_HEIGHT 79
#define TRACKER_SCROLL_SCALE 2
#define TRACKER_SCROLL_SCALE 2
#define TRACKER_FORCED_SEGUE 1000

#define TRACKER_MAX_LINENO 2147483647

class RDTrackerWidget : public RDWidget
{
  Q_OBJECT
 public:
  RDTrackerWidget(QString *import_path,QWidget *parent=0);
  ~RDTrackerWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  bool isActive() const;
  void setFocusPolicy(Qt::FocusPolicy pol);

 signals:
  void activeChanged(bool state);

 public slots:
  bool load(const QString &logname);
  void unload();

 private slots:
  void updateMenuData();
  void hideMenuData();
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
  void selectionChangedData(const QItemSelection &selected,
			    const QItemSelection &deselected);
  void meterData();
  void recordLoadedData(int card,int stream);
  void recordingData(int card,int stream);
  void recordStoppedData(int card,int stream);
  void recordUnloadedData(int cart,int stream,unsigned msecs);

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  void wheelEvent(QWheelEvent *e);

 private:
  enum DeckState {DeckIdle=0,DeckTrack1=1,DeckTrack2=2,DeckTrack3=3};
  enum Target {FadeupGain=0,FadeupPoint=1,FadedownGain=2,FadedownPoint=3,
	       TrackFadeupGain=4,TrackFadeupPoint=5,
	       TrackFadedownGain=6,TrackFadedownPoint=7,TargetSize=8};
  void LoadTrack(int line);
  void SaveTrack(int line);
  bool ImportTrack(int line);
  void RenderTransition(int line);
  void LoadBlockLength(int line);
  void RefreshLine(int line);
  void StartNext(int finishing_id,int next_id=-1);
  QString GetCutName(int line,RDCut **cut);
  int GetClick(QMouseEvent *e,QPoint *p);
  void DragTrack(int trackno,int xdiff);
  void DragTarget(int trackno,const QPoint &pt);
  void DrawTrackMap(int trackno);
  void DrawCursor(RDWavePainter *wp,int height,int xpos,const QColor &color,
		  int arrow_offset,bool left_arrow);
  void DrawRubberBand(RDWavePainter *wp,int trackno);
  void DrawTarget(RDWavePainter *wp,RDTrackerWidget::Target target,
		  int trackno,int xpos,int ypos);
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
  void CheckChanges();
  void PushSegues();
  void PopSegues();
  int SingleSelectionLine(bool incl_end_handle=false);
  void SendNotification(RDNotification::Action action,const QString &log_name);
  void SendNotification(RDNotification::Action action,unsigned cartnum);
  bool d_segue_loaded;
  unsigned d_play_start_macro;
  unsigned d_play_end_macro;
  unsigned d_record_start_macro;
  unsigned d_record_end_macro;
  RDWavePainter *d_wpg[3];
  int d_menu_clicked_point;
  RDGroup *d_group;
  int d_track_line;
  RDLogLine *d_loglines[3];
  RDLogLine *d_saved_loglines[3];
  RDCart *d_track_cart;
  RDCut *d_track_cuts[3];
  QString *d_import_path;
  RDSettings *d_settings;
  QPalette d_time_remaining_palette[2];
  QPixmap *d_wave_map[3];
  QString d_wave_name[3];
  int d_scroll_pos[3];
  int d_wave_origin[3];
  int d_segue_start_point[3];
  int d_segue_start_offset[3];
  DeckState d_deck_state;
  RDPlayDeck *d_deck[3];
  int d_time_width;
  int d_input_card;
  int d_input_port;
  int d_output_card;
  int d_output_port;
  int d_tail_preroll;
  int d_threshold_level;
  RDCae::AudioCoding d_coding;
  unsigned d_format;
  unsigned d_samprate;
  unsigned d_bitrate;
  unsigned d_chans;
  bool d_sliding;
  bool d_scrolling;
  int d_scroll_threshold;
  QPoint *d_previous_point;
  int d_current_track;
  int d_rightclick_track;
  int d_rightclick_pos;
  bool d_shift_pressed;
  RDStereoMeter *d_meter;
  QTimer *d_meter_timer;
  RDTransportButton *d_play_button;
  RDTransportButton *d_stop_button;
  QPushButton *d_track1_button;
  QPushButton *d_record_button;
  QPushButton *d_track2_button;
  QPushButton *d_finished_button;
  QPushButton *d_post_button;
  QPushButton *d_reset_button;
  QPushButton *d_previous_button;
  QPushButton *d_next_button;
  QPushButton *d_insert_button;
  QPushButton *d_delete_button;
  RDEventPlayer *d_event_player;
  unsigned d_tracks;
  int d_time_remaining;
  int d_time_remaining_start;
  int d_time_counter;
  bool d_block_valid;
  QTime d_start_time;
  QLabel *d_length_label;
  QLabel *d_tracks_remaining_label;
  QLabel *d_time_remaining_label;
  int d_track_lines[3];
  bool d_redraw[3];
  unsigned d_redraw_count;
  QCursor *d_current_cursor;
  QCursor *d_arrow_cursor;
  QCursor *d_hand_cursor;
  QCursor *d_cross_cursor;
  bool d_loaded;
  bool d_offset;
  bool d_recording;
  bool d_changed;
  double d_record_start_time;
  int d_recording_pos;
  bool d_aborting;
  bool d_record_ran;
  RDLogLine *d_dummy0_logline;
  RDLogLine *d_dummy2_logline;
  int d_preroll;
  bool d_size_altered;
  QPalette d_start_palette;
  QPalette d_record_palette;
  QPalette d_done_palette;
  QPalette d_abort_palette;
  QRect *d_track_rect;
  QRect *d_target_rect[RDTrackerWidget::TargetSize];
  int d_target_track[RDTrackerWidget::TargetSize];
  RDTrackerWidget::Target d_current_target;
  RDLogLock *d_log_lock;
  RDTrackerTableView *d_log_view;
  RDTrackerModel *d_log_model;
  RDLog *d_log;
  RDSvc *d_svc;
  QMenu *d_mouse_menu;
  QAction *d_edit_cue_action;
  QAction *d_undo_segue_action;
  QAction *d_set_start_action;
  QAction *d_set_end_action;
  QAction *d_set_hook_action;
  RDMarkerDialog *d_marker_dialog;
  int d_cursor_xpos;
  QLabel *d_tracks_remaining_label_label;
  QLabel *d_time_remaining_label_label;
  QLabel *d_time_label;
  Qt::FocusPolicy d_focus_policy;
  bool d_active;
};


#endif  // RDTRACKERWIDGET_H
