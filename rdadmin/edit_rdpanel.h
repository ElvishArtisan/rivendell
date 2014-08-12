// edit_rdpanel.h
//
// Edit an RDPanel Configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_rdpanel.h,v 1.6.8.2 2013/12/23 18:35:15 cvs Exp $
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

#ifndef EDIT_RDPANEL_H
#define EDIT_RDPANEL_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include <rd.h>
#include <rdairplay_conf.h>
#include <rdcardselector.h>
#include <rdstation.h>

/*
 * Application Settings
 */
#define LOG_PLAY_PORTS 2
#define MAX_MANUAL_SEGUE 10

class EditRDPanel : public QDialog
{
 Q_OBJECT
 public:
  EditRDPanel(RDStation *station,RDStation *cae_station,
	      QWidget *parent=0,const char *name=0);
  ~EditRDPanel();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void selectSkinData();
  void okData();
  void cancelData();

 private:
  RDAirPlayConf *air_conf;
  RDCardSelector *air_card_sel[6];
  QLineEdit *air_start_rml_edit[6];
  QLineEdit *air_stop_rml_edit[6];
  QLabel *air_station_label;
  QSpinBox *air_station_box;
  QLabel *air_user_label;
  QSpinBox *air_user_box;
  QCheckBox *air_clearfilter_box;
  QCheckBox *air_flash_box;
  QCheckBox *air_panel_pause_box;
  QLineEdit *air_label_template_edit;
  QComboBox *air_defaultsvc_box;
  QLineEdit *air_skin_edit;
};


#endif  // EDIT_RDPANEL_H

