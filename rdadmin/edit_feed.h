// edit_feed.h
//
// Edit a Rivendell Feed
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

#ifndef EDIT_FEED_H
#define EDIT_FEED_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtextedit.h>

#include <rddialog.h>
#include <rdfeed.h>
#include <rdimagepickerbox.h>
#include <rdsettings.h>
#include <rdrsscategorybox.h>
#include <rdstation.h>

#include "list_images.h"

class EditFeed : public RDDialog
{
 Q_OBJECT
 public:
  EditFeed(const QString &feed,QWidget *parent=0);
  ~EditFeed();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void superfeedActivatedData(int n);
  void schemaActivatedData(int n);
  void checkboxToggledData(bool state);
  void purgeUrlChangedData(const QString &str);
  void lineeditChangedData(const QString &str);
  void selectSubfeedsData();
  void setFormatData();
  void listImagesData();
  void copyHeaderXmlData();
  void copyChannelXmlData();
  void copyItemXmlData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  
 private:
  void UpdateControlState();
  RDFeed *feed_feed;
  RDImagePickerModel *feed_image_model;
  QLabel *feed_is_superfeed_label;
  QPushButton *feed_is_superfeed_button;
  QPushButton *feed_list_images_button;
  QComboBox *feed_is_superfeed_box;
  QLineEdit *feed_channel_title_edit;
  QTextEdit *feed_channel_description_edit;
  QLabel *feed_channel_category_label;
  RDRssCategoryBox *feed_channel_category_box;
  QLineEdit *feed_channel_link_edit;
  QLineEdit *feed_channel_copyright_edit;
  QLabel *feed_channel_editor_label;
  QLineEdit *feed_channel_editor_edit;
  QLabel *feed_channel_author_label;
  QLineEdit *feed_channel_author_edit;
  QCheckBox *feed_channel_author_is_default_check;
  QLabel *feed_channel_author_is_default_label;
  QLabel *feed_channel_owner_name_label;
  QLineEdit *feed_channel_owner_name_edit;
  QLabel *feed_channel_owner_email_label;
  QLineEdit *feed_channel_owner_email_edit;
  QLabel *feed_channel_webmaster_label;
  QLineEdit *feed_channel_webmaster_edit;
  QLineEdit *feed_channel_language_edit;
  QCheckBox *feed_channel_explicit_check;
  QLabel *feed_channel_explicit_label;
  QLineEdit *feed_base_url_edit;
  QLineEdit *feed_base_preamble_edit;
  QLineEdit *feed_purge_url_edit;
  QLabel *feed_purge_username_label;
  QLineEdit *feed_purge_username_edit;
  QLabel *feed_purge_password_label;
  QLineEdit *feed_purge_password_edit;
  QCheckBox *feed_purge_use_id_file_check;
  QLabel *feed_purge_use_id_file_label;
  QLabel *feed_rss_schema_label;
  QComboBox *feed_rss_schema_box;
  QLabel *feed_header_xml_label;
  QTextEdit *feed_header_xml_edit;
  QPushButton *feed_header_xml_button;
  QLabel *feed_channel_xml_label;
  QTextEdit *feed_channel_xml_edit;
  QPushButton *feed_channel_xml_button;
  QLabel *feed_item_xml_label;
  QTextEdit *feed_item_xml_edit;
  QPushButton *feed_item_xml_button;
  QSpinBox *feed_max_shelf_life_spin;
  QLabel *feed_autopost_label;
  QComboBox *feed_autopost_box;
  RDSettings feed_settings;
  ListImages *feed_images_dialog;
  QLineEdit *feed_format_edit;
  QCheckBox *feed_normalize_check;
  QLabel *feed_normalize_label;
  QSpinBox *feed_normalize_spin;
  QLineEdit *feed_extension_edit;
  QComboBox *feed_castorder_box;
  QPushButton *feed_format_button;
  QGroupBox *feed_channel_section_groupbox;
  QLabel *feed_channel_title_label;
  QLabel *feed_channel_link_label;
  QLabel *feed_channel_copyright_label;
  QLabel *feed_channel_language_label;
  QLabel *feed_channel_description_label;
  QLabel *feed_channel_image_label;
  RDImagePickerBox *feed_channel_image_box;
  QLabel *feed_base_url_label;
  QLabel *feed_base_preamble_label;
  QLabel *feed_purge_url_label;
  QLabel *feed_max_shelf_life_label;
  QLabel *feed_max_shelf_life_unit_label;
  QLabel *feed_format_label;
  QLabel *feed_normalize_check_label;
  QLabel *feed_normalize_unit_label;
  QLabel *feed_castorder_label;
  QLabel *feed_extension_label;
  QLabel *feed_item_image_label;
  RDImagePickerBox *feed_item_image_box;
  QPushButton *feed_ok_button;
  QPushButton *feed_cancel_button;
};


#endif  // EDIT_FEED_H

