// edit_user.h
//
// Edit a Rivendell User
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_USER_H
#define EDIT_USER_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rduser.h>

class EditUser : public QDialog
{
  Q_OBJECT
 public:
  EditUser(const QString &user,QWidget *parent=0);
  ~EditUser();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void passwordData();
  void groupsData();
  void feedsData();
  void adminToggledData(bool state);
  void okData();
  void cancelData();

 private:
  QLineEdit *user_name_edit;
  QLineEdit *user_full_name_edit;
  QLineEdit *user_description_edit;
  QLineEdit *user_phone_edit;
  QCheckBox *user_web_box;
  QLabel *user_web_label;
  QGroupBox *user_admin_groupbox;
  QLabel *user_admin_config_label;
  QButtonGroup *user_admin_group;
  QGroupBox *user_prod_groupbox;
  QButtonGroup *user_prod_group;
  QButtonGroup *user_traffic_group;
  QGroupBox *user_traffic_groupbox;
  QButtonGroup *user_onair_group;
  QGroupBox *user_onair_groupbox;
  QButtonGroup *user_podcast_group;
  QGroupBox *user_podcast_groupbox;
  QCheckBox *user_admin_config_button;
  QCheckBox *user_create_carts_button;
  QCheckBox *user_delete_carts_button;
  QCheckBox *user_modify_carts_button;
  QCheckBox *user_edit_audio_button;
  QCheckBox *user_voicetrack_log_button;
  QCheckBox *user_create_log_button;
  QCheckBox *user_delete_log_button;
  QCheckBox *user_delete_rec_button;
  QCheckBox *user_playout_log_button;
  QCheckBox *user_arrange_log_button;
  QCheckBox *user_addto_log_button;
  QCheckBox *user_removefrom_log_button;
  QCheckBox *user_config_panels_button;
  QCheckBox *user_modify_template_button;
  QCheckBox *user_edit_catches_button;
  QCheckBox *user_add_podcast_button;
  QCheckBox *user_edit_podcast_button;
  QCheckBox *user_delete_podcast_button;
  QPushButton *user_assign_perms_button;
  QPushButton *user_assign_feeds_button;
  QLabel *user_create_carts_label;
  QLabel *user_delete_carts_label;
  QLabel *user_modify_carts_label;
  QLabel *user_edit_audio_label;
  QLabel *user_create_log_label;
  QLabel *user_delete_log_label;
  QLabel *user_delete_rec_label;
  QLabel *user_playout_log_label;
  QLabel *user_arrange_log_label;
  QLabel *user_addto_log_label;
  QLabel *user_removefrom_log_label;
  QLabel *user_config_panels_label;
  QLabel *user_modify_template_label;
  QLabel *user_edit_catches_label;
  QLabel *user_add_podcast_label;
  QLabel *user_edit_podcast_label;
  QLabel *user_delete_podcast_label;
  QLabel *user_voicetrack_log_label;
  RDUser *user_user;
};


#endif

