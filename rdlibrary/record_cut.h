// record_cut.h
//
// Record a Rivendell cut.
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RECORD_CUT_H
#define RECORD_CUT_H

#define RECORD_CUT_TIMER_INTERVAL 100

#include <qdialog.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <rdtransportbutton.h>
#include <rdstereometer.h>
#include <rdwavefile.h>
#include <rdcae.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdtimeedit.h>

#include <globals.h>

class RecordCut : public QDialog
{
  Q_OBJECT
  public:
   RecordCut(RDCart *cart,QString cut,bool use_weight,QWidget *parent=0);
   ~RecordCut();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void airDateButtonData(int);
   void daypartButtonData(int);
   void setAllData();
   void clearAllData();
   void channelsData(int);
   void recordData();
   void playData();
   void stopData();
   void recordLoadedData(int,int);
   void recordedData(int,int);
   void recordStoppedData(int,int);
   void recordUnloadedData(int,int,unsigned);
   void playedData(int);
   void playStoppedData(int);
   void closeData();
   void initData(bool);
   void recTimerData();
   void aesAlarmData(int,int,bool);
   void meterData();
   void evergreenToggledData(bool);

  protected:
   void resizeEvent(QResizeEvent *e);
   void paintEvent(QPaintEvent *e);
   void closeEvent(QCloseEvent *e);

  private:
   void AutoTrim(RDWaveFile *name);
   RDCut *rec_cut;
   unsigned rec_length;
   QString cut_origin_name;
   QDateTime cut_origin_datetime;
   QLabel *cut_description_label;
   QLineEdit *cut_description_edit;
   QLabel *cut_outcue_label;
   QLineEdit *cut_outcue_edit;
   QLabel *cut_isrc_label;
   QLineEdit *cut_isrc_edit;
   QLabel *cut_isci_label;
   QLineEdit *cut_isci_edit;
   QLabel *cut_origin_label;
   QLineEdit *cut_origin_edit;
   QLabel *cut_source_label;
   QLineEdit *cut_source_edit;
   QLabel *cut_weight_label;
   QSpinBox *cut_weight_box;
   QLabel *cut_playdate_label;
   QLineEdit *cut_playdate_edit;
   QLabel *cut_playcounter_label;
   QLineEdit *cut_playcounter_edit;
   QLabel *cut_killdatetime_label;
   QRadioButton *cut_startdatetime_enable_button;
   QRadioButton *cut_startdatetime_disable_button;
   QLabel *cut_startdatetime_label;
   QDateTimeEdit *cut_startdatetime_edit;
   QLabel *cut_enddatetime_label;
   QDateTimeEdit *cut_enddatetime_edit;
   QLabel *cut_daypart_label;
   QRadioButton *cut_starttime_enable_button;
   QRadioButton *cut_starttime_disable_button;
   RDTimeEdit *cut_starttime_edit;
   QLabel *cut_starttime_label;
   RDTimeEdit *cut_endtime_edit;
   QLabel *cut_endtime_label;
   QTimer *rec_timer;
   QLabel *rec_timer_label;
   QLabel *rec_aes_alarm_label;
   int rec_timer_value;
   RDTransportButton *rec_record_button;
   RDTransportButton *rec_play_button;
   RDTransportButton *rec_stop_button;
   RDStereoMeter *rec_meter;
   QString rec_name;
   QLabel *rec_channels_box_label;
   QComboBox *rec_channels_box;
   QLineEdit *rec_channels_edit;
   QLabel *rec_mode_box_label;
   QComboBox *rec_mode_box;
   QLabel *rec_trim_box_label;
   QComboBox *rec_trim_box;
   QLabel *rec_dayofweek_label;
   QPushButton *rec_set_button;
   QPushButton *rec_clear_button;
   QCheckBox *rec_weekpart_button[7];
   QLabel *rec_weekpart_label[7];
   QPushButton *close_button;
   int rec_card_no[2];
   int rec_stream_no[2];
   int rec_port_no[2];
   int rec_play_handle;
   RDCae::AudioCoding rec_format;
   unsigned rec_channels;
   unsigned rec_samprate;
   unsigned rec_bitrate;
   bool is_playing;
   bool is_ready;
   bool is_recording;
   bool is_closing;
   QCheckBox *rec_evergreen_box;
   QLabel *rec_evergreen_label;
   bool rec_use_weighting;
};


#endif

