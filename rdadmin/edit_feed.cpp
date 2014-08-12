// edit_feed.cpp
//
// Edit a Rivendell Feed
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_feed.cpp,v 1.9 2012/01/16 11:16:36 cvs Exp $
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

#include <qpushbutton.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qdatetime.h>
#include <qurl.h>

#include <rdexport_settings_dialog.h>

#include <edit_feed.h>
#include <list_aux_fields.h>
#include <globals.h>



EditFeed::EditFeed(const QString &feed,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  feed_feed=new RDFeed(feed,this,"feed_feed");

  setCaption(tr("Feed: ")+feed);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",12,QFont::Normal);
  small_font.setPixelSize(12);

  //
  // Feed Name
  //
  feed_keyname_edit=new QLineEdit(this,"feed_keyname_edit");
  feed_keyname_edit->setGeometry(115,11,100,19);
  feed_keyname_edit->setMaxLength(8);
  feed_keyname_edit->setReadOnly(true);
  QLabel *feed_keyname_label=new QLabel(feed_keyname_edit,tr("Key Name:"),this,
				       "feed_keyname_label");
  feed_keyname_label->setGeometry(10,11,100,19);
  feed_keyname_label->setFont(font);
  feed_keyname_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Section
  //
  feed_channel_section_label=new QLabel(tr("CHANNEL VALUES"),this);
  feed_channel_section_label->setGeometry(30,41,130,20);
  feed_channel_section_label->setAlignment(AlignCenter);
  feed_channel_section_label->setFont(font);

  //
  // Channel Title
  //
  feed_channel_title_edit=new QLineEdit(this,"feed_channel_title_edit");
  feed_channel_title_edit->setGeometry(115,60,375,19);
  feed_channel_title_edit->setMaxLength(255);
  feed_channel_title_label=
    new QLabel(feed_channel_title_edit,tr("Title:"),this,
	       "feed_channel_title_label");
  feed_channel_title_label->setGeometry(20,60,90,19);
  feed_channel_title_label->setFont(font);
  feed_channel_title_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Category
  //
  feed_channel_category_edit=new QLineEdit(this,"feed_channel_category_edit");
  feed_channel_category_edit->setGeometry(115,82,375,19);
  feed_channel_category_edit->setMaxLength(64);
  feed_channel_category_label=
    new QLabel(feed_channel_category_edit,tr("Category:"),this,
	       "feed_channel_category_label");
  feed_channel_category_label->setGeometry(20,82,90,19);
  feed_channel_category_label->setFont(font);
  feed_channel_category_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Link
  //
  feed_channel_link_edit=new QLineEdit(this,"feed_channel_link_edit");
  feed_channel_link_edit->setGeometry(115,104,375,19);
  feed_channel_link_edit->setMaxLength(255);
  feed_channel_link_label=
    new QLabel(feed_channel_link_edit,tr("Link:"),this,
	       "feed_channel_link_label");
  feed_channel_link_label->setGeometry(20,104,90,19);
  feed_channel_link_label->setFont(font);
  feed_channel_link_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Copyright
  //
  feed_channel_copyright_edit=
    new QLineEdit(this,"feed_channel_copyright_edit");
  feed_channel_copyright_edit->setGeometry(115,126,375,19);
  feed_channel_copyright_edit->setMaxLength(64);
  feed_channel_copyright_label=
    new QLabel(feed_channel_copyright_edit,tr("Copyright:"),this,
	       "feed_channel_copyright_label");
  feed_channel_copyright_label->setGeometry(20,126,90,19);
  feed_channel_copyright_label->setFont(font);
  feed_channel_copyright_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Webmaster
  //
  feed_channel_webmaster_edit=
    new QLineEdit(this,"feed_channel_webmaster_edit");
  feed_channel_webmaster_edit->setGeometry(115,148,375,19);
  feed_channel_webmaster_edit->setMaxLength(64);
  feed_channel_webmaster_label=
    new QLabel(feed_channel_webmaster_edit,tr("Webmaster:"),this,
	       "feed_channel_webmaster_label");
  feed_channel_webmaster_label->setGeometry(20,148,90,19);
  feed_channel_webmaster_label->setFont(font);
  feed_channel_webmaster_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Language
  //
  feed_channel_language_edit=
    new QLineEdit(this,"feed_channel_language_edit");
  feed_channel_language_edit->
    setGeometry(115,170,60,19);
  feed_channel_language_edit->setMaxLength(5);
  feed_channel_language_label=
    new QLabel(feed_channel_language_edit,tr("Language:"),this,
	       "feed_channel_language_label");
  feed_channel_language_label->setGeometry(20,170,90,19);
  feed_channel_language_label->setFont(font);
  feed_channel_language_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel Description
  //
  feed_channel_description_edit=
    new QTextEdit(this,"feed_channel_description_edit");
  feed_channel_description_edit->
    setGeometry(115,192,375,76);
  feed_channel_description_label=
    new QLabel(feed_channel_description_edit,tr("Description:"),this,
	       "feed_channel_description_label");
  feed_channel_description_label->setGeometry(20,192,90,19);
  feed_channel_description_label->setFont(font);
  feed_channel_description_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Purge Audio URL
  //
  feed_purge_url_edit=
    new QLineEdit(this,"feed_purge_url_edit");
  feed_purge_url_edit->setGeometry(155,280,335,19);
  feed_purge_url_edit->setMaxLength(255);
  connect(feed_purge_url_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(purgeUrlChangedData(const QString &)));
  feed_purge_url_label=
    new QLabel(feed_purge_url_edit,tr("Audio Upload URL:"),this,
	       "feed_purge_url_label");
  feed_purge_url_label->setGeometry(20,280,130,19);
  feed_purge_url_label->setFont(font);
  feed_purge_url_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Purge Username
  //
  feed_purge_username_edit=
    new QLineEdit(this,"feed_purge_username_edit");
  feed_purge_username_edit->setGeometry(225,302,95,19);
  feed_purge_username_edit->setMaxLength(64);
  connect(feed_purge_username_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(purgeUsernameChangedData(const QString &)));
  feed_purge_username_label=
    new QLabel(feed_purge_username_edit,tr("Username:"),this,
	       "feed_purge_username_label");
  feed_purge_username_label->setGeometry(40,302,180,19);
  feed_purge_username_label->setFont(font);
  feed_purge_username_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Purge Password
  //
  feed_purge_password_edit=
    new QLineEdit(this,"feed_purge_password_edit");
  feed_purge_password_edit->setGeometry(395,302,95,19);
  feed_purge_password_edit->setMaxLength(64);
  feed_purge_password_edit->setEchoMode(QLineEdit::Password);
  feed_purge_password_label=
    new QLabel(feed_purge_password_edit,tr("Password:"),this,
	       "feed_purge_password_label");
  feed_purge_password_label->setGeometry(320,302,70,19);
  feed_purge_password_label->setFont(font);
  feed_purge_password_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Audio Format
  //
  feed_format_edit=new QLineEdit(this,"feed_format_edit");
  feed_format_edit->setGeometry(155,324,285,20);
  feed_format_edit->setReadOnly(true);
  feed_format_label=new QLabel(feed_format_edit,
		   tr("Upload Format:"),this,"feed_format_label");
  feed_format_label->setGeometry(5,324,145,20);
  feed_format_label->setFont(font);
  feed_format_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  feed_format_button=new QPushButton(this,"format_button");
  feed_format_button->setGeometry(450,324,40,24);
  feed_format_button->setFont(small_font);
  feed_format_button->setText(tr("S&et"));
  connect(feed_format_button,SIGNAL(clicked()),this,SLOT(setFormatData()));

  //
  // Normalize Check Box
  //
  feed_normalize_box=new QCheckBox(this,"feed_normalize_box");
  feed_normalize_box->setGeometry(155,348,15,15);
  feed_normalize_box->setChecked(true);
  feed_normalize_check_label=new QLabel(feed_normalize_box,tr("Normalize"),
		   this,"normalize_check_label");
  feed_normalize_check_label->setGeometry(175,346,83,20);
  feed_normalize_check_label->setFont(font);
  feed_normalize_check_label->setAlignment(AlignLeft|AlignVCenter);
  connect(feed_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  feed_normalize_spin=new QSpinBox(this,"feed_normalize_spin");
  feed_normalize_spin->setGeometry(295,346,40,20);
  feed_normalize_spin->setRange(-30,-1);
  feed_normalize_label=new QLabel(feed_normalize_spin,tr("Level:"),
				 this,"normalize_spin_label");
  feed_normalize_label->setGeometry(245,346,45,20);
  feed_normalize_label->setFont(font);
  feed_normalize_label->setAlignment(AlignRight|AlignVCenter);
  feed_normalize_unit_label=new QLabel(tr("dBFS"),this,"normalize_unit_label");
  feed_normalize_unit_label->setGeometry(340,346,40,20);
  feed_normalize_unit_label->setFont(font);
  feed_normalize_unit_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Base Audio URL
  //
  feed_base_url_edit=
    new QLineEdit(this,"feed_base_url_edit");
  feed_base_url_edit->setGeometry(155,368,335,19);
  feed_base_url_edit->setMaxLength(255);
  feed_base_url_label=
    new QLabel(feed_base_url_edit,tr("Audio Download URL:"),this,
	       "feed_base_url_label");
  feed_base_url_label->setGeometry(20,368,130,19);
  feed_base_url_label->setFont(font);
  feed_base_url_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Keep Expired Metadata Checkbox
  //
  feed_keep_metadata_box=new QCheckBox(this,"feed_keep_metadata_box");
  feed_keep_metadata_box->setGeometry(155,390,15,15);
  feed_keep_metadata_label=
    new QLabel(feed_keep_metadata_box,tr("Keep Expired Metadata"),this,
	       "feed_keep_metadata_label");
  feed_keep_metadata_label->setGeometry(175,390,180,19);
  feed_keep_metadata_label->setFont(font);
  feed_keep_metadata_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
 
  //
  // AutoPost Checkbox
  //
  feed_autopost_box=new QCheckBox(this,"feed_autopost_box");
  feed_autopost_box->setGeometry(365,390,15,15);
  feed_autopost_label=
    new QLabel(feed_autopost_box,tr("Enable AutoPost"),this,
	       "feed_autopost_label");
  feed_autopost_label->setGeometry(385,390,200,19);
  feed_autopost_label->setFont(font);
  feed_autopost_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
 
  //
  // Enclosure Preamble
  //
  feed_base_preamble_edit=
    new QLineEdit(this,"feed_base_preamble_edit");
  feed_base_preamble_edit->setGeometry(155,412,335,19);
  feed_base_preamble_edit->setMaxLength(255);
  feed_base_preamble_label=
    new QLabel(feed_base_preamble_edit,tr("Enclosure Preamble:"),this,
	       "feed_base_preamble_label");
  feed_base_preamble_label->setGeometry(20,412,130,19);
  feed_base_preamble_label->setFont(font);
  feed_base_preamble_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Audio File Extension
  //
  feed_extension_edit=new QLineEdit(this,"feed_extension_edit");
  feed_extension_edit->setGeometry(155,434,70,19);
  feed_extension_edit->setMaxLength(16);
  feed_extension_label=
    new QLabel(feed_extension_edit,tr("Audio Extension:"),this,
	       "feed_extension_label");
  feed_extension_label->setGeometry(20,434,130,19);
  feed_extension_label->setFont(font);
  feed_extension_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Maximum Shelf Life
  //
  feed_max_shelf_life_spin=
    new QSpinBox(this,"feed_max_shelf_life_spin");
  feed_max_shelf_life_spin->setGeometry(155,456,60,19);
  feed_max_shelf_life_spin->setRange(0,365);
  feed_max_shelf_life_spin->setSpecialValueText(tr("None"));
  feed_max_shelf_life_label=
    new QLabel(feed_max_shelf_life_spin,tr("Maximum Shelf Life:"),this,
	       "feed_max_shelf_life_label");
  feed_max_shelf_life_label->setGeometry(20,456,130,19);
  feed_max_shelf_life_label->setFont(font);
  feed_max_shelf_life_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  feed_max_shelf_life_unit_label=
    new QLabel(feed_max_shelf_life_spin,tr("days"),this,
	       "feed_max_shelf_life_unit");
  feed_max_shelf_life_unit_label->setGeometry(220,456,50,19);
  feed_max_shelf_life_unit_label->setFont(font);
  feed_max_shelf_life_unit_label->
    setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Episode Order
  //
  feed_castorder_box=new QComboBox(this,"feed_castorder_box");
  feed_castorder_box->setGeometry(155,478,100,19);
  feed_castorder_box->insertItem(tr("Descending"));
  feed_castorder_box->insertItem(tr("Ascending"));
  feed_castorder_label=new QLabel(feed_castorder_box,tr("Episode Sort Order:"),
				  this,"feed_castorder_label");
  feed_castorder_label->setGeometry(20,478,130,19);
  feed_castorder_label->setFont(font);
  feed_castorder_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Media Link Mode
  //
  feed_media_link_mode_box=new QComboBox(this,"feed_media_link_mode_box");
  feed_media_link_mode_box->setGeometry(155,500,100,19);
  feed_media_link_mode_box->insertItem(tr("None"));
  feed_media_link_mode_box->insertItem(tr("Direct"));
  feed_media_link_mode_box->insertItem(tr("Counted"));
  feed_media_link_mode_label=new QLabel(feed_media_link_mode_box,
					tr("Media Link Mode:"),
					this,"feed_media_link_mode_label");
  feed_media_link_mode_label->setGeometry(20,500,130,19);
  feed_media_link_mode_label->setFont(font);
  feed_media_link_mode_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Feed Redirection
  //
  feed_redirect_check=new QCheckBox(this,"feed_redirect_check");
  feed_redirect_check->setGeometry(20,532,15,15);
  QLabel *label=new QLabel(feed_redirect_check,tr("Enable Feed Redirection"),
			     this,"feed_redirect_label");
  label->setGeometry(40,532,200,19);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  feed_redirect_edit=new QLineEdit(this,"feed_redirect_edit");
  feed_redirect_edit->setGeometry(85,552,405,20);
  feed_redirect_label=new QLabel(feed_redirect_edit,tr("URL:"),
			     this,"feed_redirect_label");
  feed_redirect_label->setGeometry(40,552,40,19);
  feed_redirect_label->setFont(font);
  feed_redirect_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Header XML
  //
  feed_header_xml_edit=
    new QTextEdit(this,"feed_header_xml_edit");
  feed_header_xml_edit->
    setGeometry(615,10,365,76);
  feed_header_xml_label=
    new QLabel(feed_header_xml_edit,tr("Header XML:"),this,
	       "feed_header_xml_label");
  feed_header_xml_label->setGeometry(520,10,90,19);
  feed_header_xml_label->setFont(font);
  feed_header_xml_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Channel XML
  //
  feed_channel_xml_edit=
    new QTextEdit(this,"feed_channel_xml_edit");
  feed_channel_xml_edit->
    setGeometry(615,88,365,176);
  feed_channel_xml_label=
    new QLabel(feed_channel_xml_edit,tr("Channel XML:"),this,
	       "feed_channel_xml_label");
  feed_channel_xml_label->setGeometry(520,88,90,19);
  feed_channel_xml_label->setFont(font);
  feed_channel_xml_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Item XML
  //
  feed_item_xml_edit=
    new QTextEdit(this,"feed_item_xml_edit");
  feed_item_xml_edit->
    setGeometry(615,270,365,176);
  feed_item_xml_label=
    new QLabel(feed_item_xml_edit,tr("Item XML:"),this,
	       "feed_item_xml_label");
  feed_item_xml_label->setGeometry(520,270,90,19);
  feed_item_xml_label->setFont(font);
  feed_item_xml_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Metadata Button
  //
  feed_metadata_button=new QPushButton(this,"feed_metadata_button");
  feed_metadata_button->setGeometry(615,460,150,50);
  feed_metadata_button->setDefault(true);
  feed_metadata_button->setFont(font);
  feed_metadata_button->setText(tr("&Define Auxiliary\nMetadata Fields"));
  connect(feed_metadata_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Values
  //
  feed_keyname_edit->setText(feed_feed->keyName());
  feed_channel_title_edit->setText(feed_feed->channelTitle());
  feed_channel_category_edit->setText(feed_feed->channelCategory());
  feed_channel_link_edit->setText(feed_feed->channelLink());
  feed_channel_copyright_edit->setText(feed_feed->channelCopyright());
  feed_channel_webmaster_edit->setText(feed_feed->channelWebmaster());
  feed_channel_description_edit->setText(feed_feed->channelDescription());
  feed_channel_language_edit->setText(feed_feed->channelLanguage());
  feed_base_url_edit->setText(feed_feed->baseUrl());
  feed_base_preamble_edit->setText(feed_feed->basePreamble());
  feed_purge_url_edit->setText(feed_feed->purgeUrl());
  feed_purge_username_edit->setText(feed_feed->purgeUsername());
  feed_purge_password_edit->setText(feed_feed->purgePassword());
  feed_header_xml_edit->setText(feed_feed->headerXml());
  feed_channel_xml_edit->setText(feed_feed->channelXml());
  feed_item_xml_edit->setText(feed_feed->itemXml());
  feed_max_shelf_life_spin->setValue(feed_feed->maxShelfLife());
  feed_autopost_box->setChecked(feed_feed->enableAutopost());
  feed_keep_metadata_box->setChecked(feed_feed->keepMetadata());
  feed_settings.setFormat(feed_feed->uploadFormat());
  feed_settings.setChannels(feed_feed->uploadChannels());
  feed_settings.setSampleRate(feed_feed->uploadSampleRate());
  feed_settings.setBitRate(feed_feed->uploadBitRate());
  feed_settings.setQuality(feed_feed->uploadQuality());
  feed_extension_edit->setText(feed_feed->uploadExtension());
  feed_format_edit->setText(feed_settings.description());
  if(feed_feed->normalizeLevel()>0) {
    feed_normalize_box->setChecked(false);
  }
  else {
    feed_normalize_box->setChecked(true);
    feed_normalize_spin->setValue(feed_feed->normalizeLevel()/1000);
  }
  feed_castorder_box->setCurrentItem(feed_feed->castOrder());
  feed_media_link_mode_box->setCurrentItem((int)feed_feed->mediaLinkMode());
  feed_redirect_edit->setText(feed_feed->redirectPath());
  feed_redirect_check->setChecked(!feed_redirect_edit->text().isEmpty());
  normalizeCheckData(feed_normalize_box->isChecked());

  RedirectChanged(feed_redirect_check->isChecked());
  connect(feed_redirect_check,SIGNAL(toggled(bool)),
	  this,SLOT(redirectToggledData(bool)));
}


QSize EditFeed::sizeHint() const
{
  return QSize(1000,591);
} 


QSizePolicy EditFeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditFeed::purgeUrlChangedData(const QString &str)
{
  QUrl url(str);
  QString protocol=url.protocol();
  if(((protocol=="ftp")||(protocol=="smb"))&&
     (!feed_redirect_check->isChecked())) {
    feed_purge_username_label->setEnabled(true);
    feed_purge_username_edit->setEnabled(true);
  }
  else {
    feed_purge_username_label->setDisabled(true);
    feed_purge_username_edit->setDisabled(true);
  }
  purgeUsernameChangedData(feed_purge_username_edit->text());
}


void EditFeed::purgeUsernameChangedData(const QString &username)
{
  feed_purge_password_label->
    setDisabled(username.isEmpty()||feed_purge_url_edit->text().isEmpty()||
		feed_redirect_check->isChecked());
  feed_purge_password_edit->
    setDisabled(username.isEmpty()||feed_purge_url_edit->text().isEmpty()||
		feed_redirect_check->isChecked());
}


void EditFeed::setFormatData()
{
  RDStation *station=new RDStation(admin_config->stationName());
  RDExportSettingsDialog *dialog=
    new RDExportSettingsDialog(&feed_settings,station,this,"dialog");
  dialog->exec();
  delete dialog;
  delete station;
  feed_format_edit->setText(feed_settings.description());
}


void EditFeed::normalizeCheckData(bool state)
{
  feed_normalize_label->setEnabled(state);
  feed_normalize_spin->setEnabled(state);
  feed_normalize_unit_label->setEnabled(state);
}


void EditFeed::editData()
{
  ListAuxFields *lf=new ListAuxFields(feed_feed->id(),this);
  lf->exec();
  delete lf;
}


void EditFeed::redirectToggledData(bool state)
{
  if(state) {
    switch(QMessageBox::warning(this,tr("Edit Feed - Redirect"),
				tr("Enabling feed redirection will cause clients subscribed to\nthis feed to be PERMANENTLY redirected to the\nspecified URL.\n\nDo you still want to enable redireciton?"),QMessageBox::Yes,QMessageBox::No)) {
      case QMessageBox::Yes:
	break;
	
      default:
	feed_redirect_check->setChecked(false);
	return;
    }
  }
  RedirectChanged(state);
}


void EditFeed::okData()
{
  feed_feed->setChannelTitle(feed_channel_title_edit->text());
  feed_feed->setChannelCategory(feed_channel_category_edit->text());
  feed_feed->setChannelLink(feed_channel_link_edit->text());
  feed_feed->setChannelCopyright(feed_channel_copyright_edit->text());
  feed_feed->setChannelWebmaster(feed_channel_webmaster_edit->text());
  feed_feed->setChannelDescription(feed_channel_description_edit->text());
  feed_feed->setChannelLanguage(feed_channel_language_edit->text());
  feed_feed->setBaseUrl(feed_base_url_edit->text());
  feed_feed->setBasePreamble(feed_base_preamble_edit->text());
  feed_feed->setPurgeUrl(feed_purge_url_edit->text());
  feed_feed->setPurgeUsername(feed_purge_username_edit->text());
  feed_feed->setPurgePassword(feed_purge_password_edit->text());
  feed_feed->setHeaderXml(feed_header_xml_edit->text());
  feed_feed->setChannelXml(feed_channel_xml_edit->text());
  feed_feed->setItemXml(feed_item_xml_edit->text());
  feed_feed->setMaxShelfLife(feed_max_shelf_life_spin->value());
  feed_feed->setLastBuildDateTime(QDateTime(QDate::currentDate(),
					    QTime::currentTime()));
  feed_feed->setEnableAutopost(feed_autopost_box->isChecked());
  feed_feed->setKeepMetadata(feed_keep_metadata_box->isChecked());
  feed_feed->setUploadFormat(feed_settings.format());
  feed_feed->setUploadChannels(feed_settings.channels());
  feed_feed->setUploadSampleRate(feed_settings.sampleRate());
  feed_feed->setUploadBitRate(feed_settings.bitRate());
  feed_feed->setUploadQuality(feed_settings.quality());
  feed_feed->setUploadExtension(feed_extension_edit->text());
  if(feed_normalize_box->isChecked()) {
    feed_feed->setNormalizeLevel(feed_normalize_spin->value()*1000);
  }
  else {
    feed_feed->setNormalizeLevel(1);
  }
  feed_feed->setCastOrder(feed_castorder_box->currentItem());
  feed_feed->setMediaLinkMode((RDFeed::MediaLinkMode)feed_media_link_mode_box->
			      currentItem());
  if(feed_redirect_check->isChecked()) {
    feed_feed->setRedirectPath(feed_redirect_edit->text());
  }
  else {
    feed_feed->setRedirectPath("");
  }

  done(0);
}


void EditFeed::cancelData()
{
  done(-1);
}


void EditFeed::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->moveTo(10,50);
  p->lineTo(sizeHint().width()/2,50);
  p->lineTo(sizeHint().width()/2,272);
  p->lineTo(10,272);
  p->lineTo(10,50);
  delete p;
}


void EditFeed::RedirectChanged(bool state)
{
  feed_redirect_label->setEnabled(state);
  feed_redirect_edit->setEnabled(state);
  feed_channel_title_edit->setDisabled(state);
  feed_channel_description_edit->setDisabled(state);
  feed_channel_category_edit->setDisabled(state);
  feed_channel_link_edit->setDisabled(state);
  feed_channel_copyright_edit->setDisabled(state);
  feed_channel_webmaster_edit->setDisabled(state);
  feed_channel_language_edit->setDisabled(state);
  feed_base_url_edit->setDisabled(state);
  feed_purge_url_edit->setDisabled(state);
  feed_purge_username_label->setDisabled(state);
  feed_purge_username_edit->setDisabled(state);
  feed_purge_password_label->setDisabled(state);
  feed_purge_password_edit->setDisabled(state);
  feed_header_xml_edit->setDisabled(state);
  feed_channel_xml_edit->setDisabled(state);
  feed_item_xml_edit->setDisabled(state);
  feed_max_shelf_life_spin->setDisabled(state);
  feed_autopost_box->setDisabled(state);
  feed_keep_metadata_box->setDisabled(state);
  feed_format_edit->setDisabled(state);
  feed_normalize_box->setDisabled(state);
  feed_extension_edit->setDisabled(state);
  feed_castorder_box->setDisabled(state);
  feed_format_button->setDisabled(state);
  feed_metadata_button->setDisabled(state);
  feed_channel_title_label->setDisabled(state);
  feed_channel_category_label->setDisabled(state);
  feed_channel_link_label->setDisabled(state);
  feed_channel_copyright_label->setDisabled(state);
  feed_channel_webmaster_label->setDisabled(state);
  feed_channel_language_label->setDisabled(state);
  feed_channel_description_label->setDisabled(state);
  feed_base_url_label->setDisabled(state);
  feed_base_preamble_label->setDisabled(state);
  feed_purge_url_label->setDisabled(state);
  feed_max_shelf_life_label->setDisabled(state);
  feed_max_shelf_life_unit_label->setDisabled(state);
  feed_autopost_label->setDisabled(state);
  feed_keep_metadata_label->setDisabled(state);
  feed_format_label->setDisabled(state);
  feed_normalize_check_label->setDisabled(state);
  feed_normalize_unit_label->setDisabled(state);
  feed_castorder_label->setDisabled(state);
  feed_extension_label->setDisabled(state);
  feed_channel_section_label->setDisabled(state);
  feed_header_xml_label->setDisabled(state);
  feed_channel_xml_label->setDisabled(state);
  feed_item_xml_label->setDisabled(state);
  feed_normalize_label->setDisabled(state||(!feed_normalize_box->isChecked()));
  feed_normalize_spin->setDisabled(state||(!feed_normalize_box->isChecked()));
  feed_normalize_unit_label->
    setDisabled(state||(!feed_normalize_box->isChecked()));
  feed_media_link_mode_box->setDisabled(state);
  feed_media_link_mode_label->setDisabled(state);
  purgeUrlChangedData(feed_purge_url_edit->text());
}
