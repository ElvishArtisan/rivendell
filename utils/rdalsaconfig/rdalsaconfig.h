// rdalsaconfig.h
//
// A Qt-based application to display info about ALSA cards.
//
//   (C) Copyright 2009-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDALSACONFIG_H
#define RDALSACONFIG_H

#include <qwidget.h>
#include <qlistview.h>
#include <qlabel.h>

#include <rdtransportbutton.h>

#include <rd.h>
#include "rdalsamodel.h"

#define RDALSACONFIG_USAGE "[--asoundrc-file=<filename>] [--autogen] [--manage-daemons]\n\nGenerate an ALSA sound card configuration for Rivendell.\n\nThe following options are available:\n\n --asoundrc-file=<filename>\n     Read and write configuration from <filename> (default value \n     \"/etc/asound.conf\").\n\n --autogen\n     Generate and save a configuration containing all available PCM devices\n     and then exit.\n\n --manage-daemons\n     Restart the Rivendell daemons as necessary to make configuration\n     changes active (requires root permission).\n\n"

void StopDaemons();
void StartDaemons();

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void saveData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void LoadConfig();
  void SaveConfig() const;
  QLabel *alsa_system_label;
  QLabel *alsa_description_label;
  QListView *alsa_system_list;
  RDAlsaModel *alsa_system_model;
  QStringList alsa_other_lines;
  QPushButton *alsa_save_button;
  QPushButton *alsa_cancel_button;
};


#endif  // RDALSACONFIG_H
