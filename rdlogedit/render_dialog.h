// render_dialog.h
//
// Log Rendering Dialog for Rivendell.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RENDER_DIALOG_H
#define RENDER_DIALOG_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressdialog.h>

#include <rdbusybar.h>
#include <rdconfig.h>
#include <rdcut_dialog.h>
#include <rdsettings.h>
#include <rdlog_event.h>
#include <rdsystem.h>
#include <rduser.h>

//
// Widget Settings
//
#define IMPORT_BAR_INTERVAL 500
#define IMPORT_TEMP_BASENAME "rdlib"


class RenderDialog : public QDialog
{
 Q_OBJECT
 public:
  RenderDialog(RDStation *station,RDSystem *system,RDConfig *config,
	      QWidget *parent=0);
  ~RenderDialog();
  QSize sizeHint() const;

 public slots:
   int exec(RDUser *user,RDLogEvent *log,int first_line,int last_line);

 private slots:
  void toChangedData(int item);
  void filenameChangedData(const QString &str);
  void selectData();
  void starttimeSourceData(int item);
  void audiosettingsData();
  void renderData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  RDStation *render_station;
  RDSystem *render_system;
  RDConfig *render_config;
  RDUser *render_user;
  RDLogEvent *render_log;
  int render_first_line;
  int render_last_line;
  RDSettings *render_settings;
  QProgressDialog *render_progress_dialog;
  QLabel *render_to_label;
  QComboBox *render_to_box;
  QLabel *render_filename_label;
  QLineEdit *render_filename_edit;
  QLabel *render_starttime_label;
  QComboBox *render_starttime_box;
  QTimeEdit *render_starttime_edit;
  QLabel *render_audiosettings_label;
  QLineEdit *render_audiosettings_edit;
  QPushButton *render_audiosettings_button;
  QLabel *render_events_label;
  QComboBox *render_events_box;
  QLabel *render_ignorestop_label;
  QComboBox *render_ignorestop_box;
  QPushButton *render_filename_button;
  QPushButton *render_render_button;
  QPushButton *render_cancel_button;
  QString render_save_path;
  unsigned render_to_cartnum;
  int render_to_cutnum;
};


#endif  // RDRENDER_DIALOG_H
