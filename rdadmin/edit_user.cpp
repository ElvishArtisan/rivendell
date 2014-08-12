// edit_user.cpp
//
// Edit a Rivendell User
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_user.cpp,v 1.32 2010/07/29 19:32:34 cvs Exp $
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsqldatabase.h>

#include <edit_user.h>
#include <rduser.h>
#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include <edit_user_perms.h>
#include <edit_feed_perms.h>
#include <globals.h>


EditUser::EditUser(const QString &user,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("User: ")+user);
  user_user=new RDUser(user);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // User Name
  //
  user_name_edit=new QLineEdit(this,"user_name_edit");
  user_name_edit->setGeometry(100,11,sizeHint().width()-110,19);
  user_name_edit->setMaxLength(255);
  user_name_edit->setValidator(validator);
  QLabel *user_name_label=new QLabel(user_name_edit,tr("&User Name:"),this,
				       "user_name_label");
  user_name_label->setGeometry(5,11,90,19);
  user_name_label->setFont(font);
  user_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Full Name
  //
  user_full_name_edit=new QLineEdit(this,"user_full_name_edit");
  user_full_name_edit->setGeometry(100,32,sizeHint().width()-110,19);
  user_full_name_edit->setMaxLength(255);
  user_full_name_edit->setValidator(validator);
  QLabel *user_full_name_label=
    new QLabel(user_full_name_edit,tr("&Full Name:"),this,
	       "user_full_name_label");
  user_full_name_label->setGeometry(10,32,85,19);
  user_full_name_label->setFont(font);
  user_full_name_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // User Description
  //
  user_description_edit=new QLineEdit(this,"user_description_edit");
  user_description_edit->setGeometry(100,53,sizeHint().width()-110,19);
  user_description_edit->setMaxLength(255);
  user_description_edit->setValidator(validator);
  QLabel *user_description_label=
    new QLabel(user_description_edit,tr("&Description:"),this,
	       "user_description_label");
  user_description_label->setGeometry(5,53,90,19);
  user_description_label->setFont(font);
  user_description_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // User Phone
  //
  user_phone_edit=new QLineEdit(this,"user_phone_edit");
  user_phone_edit->setGeometry(100,75,120,19);
  user_phone_edit->setMaxLength(20);
  user_phone_edit->setValidator(validator);
  QLabel *user_phone_label=
    new QLabel(user_phone_edit,tr("&Phone:"),this,"user_phone_label");
  user_phone_label->setGeometry(10,75,85,19);
  user_phone_label->setFont(font);
  user_phone_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Enable Web Login
  //
  user_web_box=new QCheckBox(this,"user_web_box");
  user_web_box->setGeometry(20,96,15,15);
  user_web_label=
    new QLabel(user_web_box,tr("Allow Web Logins"),this,"user_web_label");
  user_web_label->setGeometry(40,96,180,19);
  user_web_label->setFont(font);
  user_web_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Change Password Button
  //
  QPushButton *password_button=new QPushButton(this,"password_button");
  password_button->setGeometry(230,75,80,50);
  password_button->setFont(font);
  password_button->setText(tr("Change\n&Password"));
  connect(password_button,SIGNAL(clicked()),this,SLOT(passwordData()));

  //
  // Administrative Group Priviledges
  //
  user_admin_group=new QButtonGroup(tr("Administrative Rights"),this,
				    "user_admin_group");
  user_admin_group->setGeometry(10,125,355,45);
  user_admin_group->setFont(font);

  user_admin_config_button=new QCheckBox(user_admin_group,
					"user_admin_config_button");
  user_admin_config_button->setGeometry(10,21,15,15);
  connect(user_admin_config_button,SIGNAL(toggled(bool)),
	  this,SLOT(adminToggledData(bool)));
  QLabel *user_admin_config_label=new QLabel(user_admin_config_button,
					    tr("Administer S&ystem"),
					    user_admin_group,
					    "user_admin_config_label");
  user_admin_config_label->setGeometry(192,21,150,19);
  user_admin_config_label->setGeometry(30,21,150,19);
  user_admin_config_label->setFont(small_font);
  user_admin_config_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Production Group Priviledges
  //
  user_prod_group=new QButtonGroup(tr("Production Rights"),this,
				    "user_prod_group");
  user_prod_group->setGeometry(10,180,355,85);
  user_prod_group->setFont(font);

  user_create_carts_button=new QCheckBox(user_prod_group,
					"user_create_carts_button");
  user_create_carts_button->setGeometry(10,21,15,15);
  user_create_carts_label=new QLabel(user_create_carts_button,
					    tr("&Create Carts"),
					    user_prod_group,
					    "user_create_carts_label");
  user_create_carts_label->setGeometry(30,21,150,19);
  user_create_carts_label->setFont(small_font);
  user_create_carts_label->setAlignment(AlignLeft|ShowPrefix);

  user_delete_carts_button=new QCheckBox(user_prod_group,
					"user_delete_carts_button");
  user_delete_carts_button->setGeometry(172,21,15,15);
  user_delete_carts_label=new QLabel(user_delete_carts_button,
					    tr("&Delete Carts"),
					    user_prod_group,
					    "user_delete_carts_label");
  user_delete_carts_label->setGeometry(192,21,150,19);
  user_delete_carts_label->setFont(small_font);
  user_delete_carts_label->setAlignment(AlignLeft|ShowPrefix);

  user_modify_carts_button=new QCheckBox(user_prod_group,
					"user_modify_carts_button");
  user_modify_carts_button->setGeometry(10,42,15,15);
  user_modify_carts_label=new QLabel(user_modify_carts_button,
					    tr("&Modify Carts"),
					    user_prod_group,
					    "user_modify_carts_label");
  user_modify_carts_label->setGeometry(30,41,150,19);
  user_modify_carts_label->setFont(small_font);
  user_modify_carts_label->setAlignment(AlignLeft|ShowPrefix);

  user_edit_audio_button=new QCheckBox(user_prod_group,
					"user_edit_audio_button");
  user_edit_audio_button->setGeometry(10,63,15,15);
  user_edit_audio_label=new QLabel(user_edit_audio_button,
					   tr("&Edit Audio"),
					   user_prod_group,
					   "user_edit_audio_label");
  user_edit_audio_label->setGeometry(30,62,150,19);
  user_edit_audio_label->setFont(small_font);
  user_edit_audio_label->setAlignment(AlignLeft|ShowPrefix);

  user_edit_catches_button=new QCheckBox(user_prod_group,
					"user_edit_catches_button");
  user_edit_catches_button->setGeometry(172,42,15,15);
  user_edit_catches_label=new QLabel(user_edit_catches_button,
					    tr("&Edit Netcatch Schedule"),
					    user_prod_group,
					    "user_edit_catches_label");
  user_edit_catches_label->setGeometry(192,41,150,19);
  user_edit_catches_label->setFont(small_font);
  user_edit_catches_label->setAlignment(AlignLeft|ShowPrefix);

  user_voicetrack_log_button=new QCheckBox(user_prod_group,
					"user_voicetrack_log_button");
  user_voicetrack_log_button->setGeometry(172,63,15,15);
  user_voicetrack_log_label=new QLabel(user_voicetrack_log_button,
					    tr("&Voicetrack Logs"),
					    user_prod_group,
					    "user_voicetrack_log_label");
  user_voicetrack_log_label->setGeometry(192,62,150,19);
  user_voicetrack_log_label->setFont(small_font);
  user_voicetrack_log_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Traffic Group Priviledges
  //
  user_traffic_group=new QButtonGroup(tr("Traffic Rights"),this,
				      "user_traffic_group");
  user_traffic_group->setGeometry(10,275,355,66);
  user_traffic_group->setFont(font);

  user_create_log_button=new QCheckBox(user_traffic_group,
					"user_create_log_button");
  user_create_log_button->setGeometry(10,21,15,15);
  user_create_log_label=new QLabel(user_create_log_button,
					    tr("Create &Log"),
					    user_traffic_group,
					    "user_create_log_label");
  user_create_log_label->setGeometry(30,21,150,19);
  user_create_log_label->setFont(small_font);
  user_create_log_label->setAlignment(AlignLeft|ShowPrefix);

  user_delete_log_button=new QCheckBox(user_traffic_group,
					"user_delete_log_button");
  user_delete_log_button->setGeometry(172,21,15,15);
  user_delete_log_label=new QLabel(user_delete_log_button,
					    tr("De&lete Log"),
					    user_traffic_group,
					    "user_delete_log_label");
  user_delete_log_label->setGeometry(192,21,150,19);
  user_delete_log_label->setFont(small_font);
  user_delete_log_label->setAlignment(AlignLeft|ShowPrefix);

  user_delete_rec_button=new QCheckBox(user_traffic_group,
					"user_delete_rec_button");
  user_delete_rec_button->setGeometry(172,42,15,15);
  user_delete_rec_label=new QLabel(user_delete_rec_button,
					    tr("Delete &Report Data"),
					    user_traffic_group,
					    "user_delete_rec_label");
  user_delete_rec_label->setGeometry(192,42,150,19);
  user_delete_rec_label->setFont(small_font);
  user_delete_rec_label->setAlignment(AlignLeft|ShowPrefix);

  user_modify_template_button=new QCheckBox(user_traffic_group,
					"user_modify_template_button");
  user_modify_template_button->setGeometry(10,42,15,15);
  user_modify_template_label=new QLabel(user_modify_template_button,
					    tr("&Modify Template"),
					    user_traffic_group,
					    "user_modify_template_label");
  user_modify_template_label->setGeometry(30,42,100,19);
  user_modify_template_label->setFont(small_font);
  user_modify_template_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // OnAir Group Priviledges
  //
  user_onair_group=new QButtonGroup(tr("OnAir Rights"),this,
				    "user_onair_group");
  user_onair_group->setGeometry(10,351,355,85);
  user_onair_group->setFont(font);

  user_playout_log_button=new QCheckBox(user_onair_group,
					"user_playout_log_button");
  user_playout_log_button->setGeometry(10,21,15,15);
  user_playout_log_label=new QLabel(user_playout_log_button,
					    tr("&Playout Logs"),
					    user_onair_group,
					    "user_playout_log_label");
  user_playout_log_label->setGeometry(30,21,150,19);
  user_playout_log_label->setFont(small_font);
  user_playout_log_label->setAlignment(AlignLeft|ShowPrefix);

  user_arrange_log_button=new QCheckBox(user_onair_group,
					"user_arrange_log_button");
  user_arrange_log_button->setGeometry(172,21,15,15);
  user_arrange_log_label=new QLabel(user_arrange_log_button,
					    tr("&Rearrange Log Items"),
					    user_onair_group,
					    "user_arrange_log_label");
  user_arrange_log_label->setGeometry(192,21,150,19);
  user_arrange_log_label->setFont(small_font);
  user_arrange_log_label->setAlignment(AlignLeft|ShowPrefix);

  user_addto_log_button=new QCheckBox(user_onair_group,
					"user_addto_log_button");
  user_addto_log_button->setGeometry(10,42,15,15);
  user_addto_log_label=new QLabel(user_addto_log_button,
					    tr("Add Log &Items"),
					    user_onair_group,
					    "user_addto_log_label");
  user_addto_log_label->setGeometry(30,42,150,19);
  user_addto_log_label->setFont(small_font);
  user_addto_log_label->setAlignment(AlignLeft|ShowPrefix);

  user_removefrom_log_button=new QCheckBox(user_onair_group,
					"user_removefrom_log_button");
  user_removefrom_log_button->setGeometry(172,42,15,15);
  user_removefrom_log_label=new QLabel(user_removefrom_log_button,
					    tr("Delete Lo&g Items"),
					    user_onair_group,
					    "user_removefrom_log_label");
  user_removefrom_log_label->setGeometry(192,42,150,19);
  user_removefrom_log_label->setFont(small_font);
  user_removefrom_log_label->setAlignment(AlignLeft|ShowPrefix);

  user_config_panels_button=new QCheckBox(user_onair_group,
					"user_config_panels_button");
  user_config_panels_button->setGeometry(10,63,15,15);
  user_config_panels_label=new QLabel(user_config_panels_button,
					    tr("Configure System Panels"),
					    user_onair_group,
					    "user_config_panels_label");
  user_config_panels_label->setGeometry(30,63,150,19);
  user_config_panels_label->setFont(small_font);
  user_config_panels_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Podcast Group Priviledges
  //
  user_podcast_group=new QButtonGroup(tr("Podcasting Rights"),this,
				      "user_podcast_group");
  user_podcast_group->setGeometry(10,446,355,66);
  user_podcast_group->setFont(font);

  user_add_podcast_button=new QCheckBox(user_podcast_group,
					"user_add_podcast_button");
  user_add_podcast_button->setGeometry(10,21,15,15);
  user_add_podcast_label=new QLabel(user_add_podcast_button,
					    tr("Cre&ate Podcast"),
					    user_podcast_group,
					    "user_add_podcast_label");
  user_add_podcast_label->setGeometry(30,21,150,19);
  user_add_podcast_label->setFont(small_font);
  user_add_podcast_label->setAlignment(AlignLeft|ShowPrefix);

  user_edit_podcast_button=new QCheckBox(user_podcast_group,
					"user_edit_podcast_button");
  user_edit_podcast_button->setGeometry(172,21,15,15);
  user_edit_podcast_label=new QLabel(user_edit_podcast_button,
					    tr("E&dit Podcast"),
					    user_podcast_group,
					    "user_edit_podcast_label");
  user_edit_podcast_label->setGeometry(192,21,150,19);
  user_edit_podcast_label->setFont(small_font);
  user_edit_podcast_label->setAlignment(AlignLeft|ShowPrefix);

  user_delete_podcast_button=new QCheckBox(user_podcast_group,
					"user_delete_podcast_button");
  user_delete_podcast_button->setGeometry(10,42,15,15);
  user_delete_podcast_label=new QLabel(user_delete_podcast_button,
					    tr("Dele&te Podcast"),
					    user_podcast_group,
					    "user_delete_podcast_label");
  user_delete_podcast_label->setGeometry(30,42,150,19);
  user_delete_podcast_label->setFont(small_font);
  user_delete_podcast_label->setAlignment(AlignLeft|ShowPrefix);

  //
  //  Group Permissions Button
  //
  user_assign_perms_button=new QPushButton(this,"user_assign_perms_button");
  user_assign_perms_button->setGeometry(10,516,sizeHint().width()/2-20,50);
  user_assign_perms_button->setFont(font);
  user_assign_perms_button->setText(tr("Assign Group\nPermissions"));
  connect(user_assign_perms_button,SIGNAL(clicked()),this,SLOT(groupsData()));

  //
  //  Feeds Permissions Button
  //
  user_assign_feeds_button=new QPushButton(this,"user_assign_feeds_button");
  user_assign_feeds_button->
    setGeometry(sizeHint().width()/2+10,516,sizeHint().width()/2-20,50);
  user_assign_feeds_button->setFont(font);
  user_assign_feeds_button->setText(tr("Assign Podcast Feed\nPermissions"));
  connect(user_assign_feeds_button,SIGNAL(clicked()),this,SLOT(feedsData()));

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  user_name_edit->setText(user_user->name());
  user_name_edit->setReadOnly(true);
  user_full_name_edit->setText(user_user->fullName());
  user_description_edit->setText(user_user->description());
  user_phone_edit->setText(user_user->phone());
  user_web_box->setChecked(user_user->enableWeb());
  if(user_user->adminConfig()) {
    user_admin_config_button->setChecked(true);
    adminToggledData(true);
  }
  else {
    user_create_carts_button->setChecked(user_user->createCarts());
    user_delete_carts_button->setChecked(user_user->deleteCarts());
    user_modify_carts_button->setChecked(user_user->modifyCarts());
    user_edit_audio_button->setChecked(user_user->editAudio());
    user_edit_catches_button->setChecked(user_user->editCatches());
    user_voicetrack_log_button->setChecked(user_user->voicetrackLog());
    
    user_create_log_button->setChecked(user_user->createLog());
    user_delete_log_button->setChecked(user_user->deleteLog());
    user_delete_rec_button->setChecked(user_user->deleteRec());
    
    user_playout_log_button->setChecked(user_user->playoutLog());
    user_arrange_log_button->setChecked(user_user->arrangeLog());
    user_addto_log_button->setChecked(user_user->addtoLog());
    user_removefrom_log_button->setChecked(user_user->removefromLog());
    user_config_panels_button->setChecked(user_user->configPanels());
    user_modify_template_button->setChecked(user_user->modifyTemplate());

    user_add_podcast_button->setChecked(user_user->addPodcast());
    user_edit_podcast_button->setChecked(user_user->editPodcast());
    user_delete_podcast_button->setChecked(user_user->deletePodcast());
  }

  //
  // Don't Allow an Administrator to Disable Himself!
  //
  if(user_user->name()==admin_user->name()) {
    user_admin_config_label->setDisabled(true);
    user_admin_config_button->setDisabled(true);
  }
}


EditUser::~EditUser()
{
  delete user_name_edit;
  delete user_full_name_edit;
  delete user_description_edit;
  delete user_phone_edit;
  delete user_admin_group;
  delete user_prod_group;
  delete user_traffic_group;
  delete user_onair_group;
}


QSize EditUser::sizeHint() const
{
  return QSize(375,636);
} 


QSizePolicy EditUser::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditUser::passwordData()
{
  QString password;

  RDPasswd *passwd=new RDPasswd(&password,this,"passwd");
  if(passwd->exec()==0) {
    user_user->setPassword(password);
  }
  delete passwd;
}


void EditUser::groupsData()
{
  EditUserPerms *dialog=new EditUserPerms(user_user,this,"dialog");
  dialog->exec();
  delete dialog;
}


void EditUser::feedsData()
{
  EditFeedPerms *dialog=new EditFeedPerms(user_user,this,"dialog");
  dialog->exec();
  delete dialog;
}


void EditUser::adminToggledData(bool state)
{
  user_web_box->setDisabled(state);
  user_web_label->setDisabled(state);
  user_create_carts_button->setDisabled(state);
  user_delete_carts_button->setDisabled(state);
  user_modify_carts_button->setDisabled(state);
  user_edit_audio_button->setDisabled(state);
  user_create_log_button->setDisabled(state);
  user_delete_log_button->setDisabled(state);
  user_delete_rec_button->setDisabled(state);
  user_playout_log_button->setDisabled(state);
  user_arrange_log_button->setDisabled(state);
  user_addto_log_button->setDisabled(state);
  user_removefrom_log_button->setDisabled(state);
  user_config_panels_button->setDisabled(state);
  user_modify_template_button->setDisabled(state);
  user_edit_catches_button->setDisabled(state);
  user_voicetrack_log_button->setDisabled(state);
  user_add_podcast_button->setDisabled(state);
  user_edit_podcast_button->setDisabled(state);
  user_delete_podcast_button->setDisabled(state);
  user_create_carts_label->setDisabled(state);
  user_delete_carts_label->setDisabled(state);
  user_modify_carts_label->setDisabled(state);
  user_edit_audio_label->setDisabled(state);
  user_create_log_label->setDisabled(state);
  user_delete_log_label->setDisabled(state);
  user_delete_rec_label->setDisabled(state);
  user_playout_log_label->setDisabled(state);
  user_arrange_log_label->setDisabled(state);
  user_addto_log_label->setDisabled(state);
  user_removefrom_log_label->setDisabled(state);
  user_config_panels_label->setDisabled(state);
  user_modify_template_label->setDisabled(state);
  user_edit_catches_label->setDisabled(state);
  user_add_podcast_label->setDisabled(state);
  user_edit_podcast_label->setDisabled(state);
  user_delete_podcast_label->setDisabled(state);
  user_voicetrack_log_label->setDisabled(state);
  user_assign_perms_button->setDisabled(state);
  user_assign_feeds_button->setDisabled(state);
}


void EditUser::okData()
{
  user_user->setFullName(user_full_name_edit->text());
  user_user->setDescription(user_description_edit->text());
  user_user->setPhone(user_phone_edit->text());
  user_user->setEnableWeb(user_web_box->isChecked());
  user_user->setAdminConfig(user_admin_config_button->isChecked());
  user_user->setCreateCarts(user_create_carts_button->isChecked());
  user_user->setDeleteCarts(user_delete_carts_button->isChecked());
  user_user->setModifyCarts(user_modify_carts_button->isChecked());
  user_user->setEditAudio(user_edit_audio_button->isChecked());
  user_user->setEditCatches(user_edit_catches_button->isChecked());
  user_user->setVoicetrackLog(user_voicetrack_log_button->isChecked());
  user_user->setCreateLog(user_create_log_button->isChecked());
  user_user->setDeleteLog(user_delete_log_button->isChecked());
  user_user->setDeleteRec(user_delete_rec_button->isChecked());
  user_user->setPlayoutLog(user_playout_log_button->isChecked());
  user_user->setArrangeLog(user_arrange_log_button->isChecked());
  user_user->setAddtoLog(user_addto_log_button->isChecked());
  user_user->setRemovefromLog(user_removefrom_log_button->isChecked());
  user_user->setConfigPanels(user_config_panels_button->isChecked());
  user_user->setModifyTemplate(user_modify_template_button->isChecked());
  user_user->setAddPodcast(user_add_podcast_button->isChecked());
  user_user->setEditPodcast(user_edit_podcast_button->isChecked());
  user_user->setDeletePodcast(user_delete_podcast_button->isChecked());

  done(0);
}


void EditUser::cancelData()
{
  done(-1);
}
