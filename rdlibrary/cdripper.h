// cdripper.h
//
// CD Ripper Dialog for Rivendell
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cdripper.h,v 1.14.8.5 2014/01/14 18:02:26 cvs Exp $
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

#ifndef CDRIPPER_H
#define CDRIPPER_H

#include <sys/types.h>
#include <unistd.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qtimer.h>
#include <qdir.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtextedit.h>

#include <rdcut.h>
#include <rdtransportbutton.h>
#include <rdcdplayer.h>
#include <rdcddbrecord.h>
#include <rdcddblookup.h>
#include <rdlistview.h>

#include <rdlibrary_conf.h>

class CdRipper : public QDialog
{
  Q_OBJECT
 public:
  CdRipper(QString cutname,RDCddbRecord *rec,RDLibraryConf *conf,
	   bool profile_rip,QWidget *parent=0,const char *name=0);
  ~CdRipper();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *title,QString *artist,QString *album);

 private slots:
  void trackSelectionChangedData();
  void ejectButtonData();
  void playButtonData();
  void stopButtonData();
  void ripTrackButtonData();
  void ejectedData();
  void mediaChangedData();
  void playedData(int);
  void stoppedData();
  void cddbDoneData(RDCddbLookup::Result);
  void normalizeCheckData(bool);
  void autotrimCheckData(bool);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void Profile(const QString &msg);
  RDLibraryConf *rip_conf;
  RDCdPlayer *rip_cdrom;
  RDCddbRecord *rip_cddb_record;
  RDCddbLookup *rip_cddb_lookup;
  RDCut *rip_cut;
  QLabel *rip_track_label;
  RDListView *rip_track_list;
  QPushButton *rip_rip_button;
  bool rip_rip_aborted;
  QPushButton *rip_close_button;
  QString *rip_title;
  QString *rip_artist;
  QString *rip_album;
  QLabel *rip_title_label;
  QComboBox *rip_title_box;
  QLabel *rip_album_label;
  QLineEdit *rip_album_edit;
  QLabel *rip_artist_label;
  QLineEdit *rip_artist_edit;
  QLabel *rip_other_label;
  QTextEdit *rip_other_edit;
  QCheckBox *rip_apply_box;
  QLabel *rip_apply_label;
  RDTransportButton *rip_eject_button;
  RDTransportButton *rip_play_button;
  RDTransportButton *rip_stop_button;
  QProgressBar *rip_bar;
  QLabel *rip_normalize_label;
  QCheckBox *rip_normalize_box;
  QLabel *rip_normalize_box_label;
  QSpinBox *rip_normalize_spin;
  QLabel *rip_normalize_unit;
  QLabel *rip_autotrim_box_label;
  QLabel *rip_channels_label;
  QComboBox *rip_channels_box;
  int rip_track[2];
  QCheckBox *rip_autotrim_box;
  QSpinBox *rip_autotrim_spin;
  QLabel *rip_autotrim_label;
  QLabel *rip_autotrim_unit;   
  bool rip_done;
  bool rip_profile_rip;
  QDir rip_cdda_dir;
  bool rip_isrc_read;
};


#endif  // CDRIPPER_H
