// edit_rdlibrary.h
//
// Edit an RDLibrry Configuration
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_RDLIBRARY_H
#define EDIT_RDLIBRARY_H

#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <q3datetimeedit.h>

#include <rdcardselector.h>
#include <rddialog.h>
#include <rdlibrary_conf.h>
#include <rdstation.h>

class EditRDLibrary : public RDDialog
{
  Q_OBJECT
 public:
  EditRDLibrary(RDStation *station,RDStation *cae_station,QWidget *parent=0);
  ~EditRDLibrary();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void formatData(int n);
  void cdServerTypeData(int n);
  void okData();
  void cancelData();

 private:
  void ShowBitRates(int layer,int rate);
  RDLibraryConf *lib_lib;
  RDCardSelector *lib_input_card;
  RDCardSelector *lib_output_card;
  QComboBox *lib_format_box;
  QComboBox *lib_channels_box;
  QComboBox *lib_bitrate_box;
  QComboBox *lib_recmode_box;
  QComboBox *lib_trimstate_box;
  QSpinBox *lib_vox_spin;
  QSpinBox *lib_trim_spin;
  QSpinBox *lib_preroll_spin;
  Q3TimeEdit *lib_maxlength_time;
  QLineEdit *lib_ripdev_edit;
  QComboBox *lib_paranoia_box;
  QComboBox *lib_isrc_box;
  QSpinBox *lib_riplevel_spin;
  QComboBox *lib_cd_server_type_box;
  QLabel *lib_cd_server_label;
  QLineEdit *lib_cddb_server_edit;
  QLineEdit *lib_mb_server_edit;
  QComboBox *lib_editor_box;
  QComboBox *lib_converter_box;
  QComboBox *lib_limit_search_box;
};


#endif  // EDIT_RDLIBRARY_H

