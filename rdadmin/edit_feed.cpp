// edit_feed.cpp
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

#include <math.h>

#include <qapplication.h>
#include <qclipboard.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qurl.h>

#include <rdapplication.h>
#include <rddelete.h>
#include <rdexport_settings_dialog.h>
#include <rdupload.h>

#include "edit_feed.h"
#include "edit_superfeed.h"
#include "globals.h"

EditFeed::EditFeed(const QString &feed,QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  feed_feed=new RDFeed(feed,rda->config(),this);
  feed_image_model=new RDImagePickerModel("FEED_IMAGES","FEED_ID","ID",this);
  feed_image_model->setCategoryId(feed_feed->id());

  setWindowTitle("RDAdmin - "+tr("Feed: ")+feed);

  //
  // Dialogs
  //
  feed_images_dialog=new ListImages(feed_image_model,this);

  //
  // Superfeed Settings
  //
  feed_is_superfeed_box=new QComboBox(this);
  feed_is_superfeed_box->insertItem(0,tr("No"));
  feed_is_superfeed_box->insertItem(1,tr("Yes"));
  connect(feed_is_superfeed_box,SIGNAL(activated(int)),
	  this,SLOT(superfeedActivatedData(int)));
  feed_is_superfeed_label=new QLabel(tr("Is Superfeed")+":",this);
  feed_is_superfeed_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_is_superfeed_label->setFont(labelFont());
  feed_is_superfeed_button=new QPushButton(tr("Select Member\nFeeds"),this);
  feed_is_superfeed_button->setFont(buttonFont());
  connect(feed_is_superfeed_button,SIGNAL(clicked()),
	  this,SLOT(selectSubfeedsData()));

  //
  // Image Management Button
  //
  feed_list_images_button=new QPushButton(tr("Manage")+"\n"+tr("Images"),this);
  feed_list_images_button->setFont(buttonFont());
  connect(feed_list_images_button,SIGNAL(clicked()),
	  this,SLOT(listImagesData()));

  //
  // Channel Section
  //
  feed_channel_section_groupbox=new QGroupBox(tr("Channel Values"),this);
  feed_channel_section_groupbox->setFont(labelFont());

  //
  // Channel Title
  //
  feed_channel_title_edit=new QLineEdit(this);
  feed_channel_title_edit->setMaxLength(191);
  feed_channel_title_label=
    new QLabel(feed_channel_title_edit,tr("Title:"),this);
  feed_channel_title_label->setFont(labelFont());
  feed_channel_title_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Category
  //
  feed_channel_category_box=new RDRssCategoryBox(this);
  feed_channel_category_label=new QLabel(tr("Category:"),this);
  feed_channel_category_label->setFont(labelFont());
  feed_channel_category_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Link
  //
  feed_channel_link_edit=new QLineEdit(this);
  feed_channel_link_edit->setMaxLength(191);
  feed_channel_link_label=
    new QLabel(feed_channel_link_edit,tr("Link:"),this);
  feed_channel_link_label->setFont(labelFont());
  feed_channel_link_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Copyright
  //
  feed_channel_copyright_edit=new QLineEdit(this);
  feed_channel_copyright_edit->setMaxLength(64);
  feed_channel_copyright_label=
    new QLabel(feed_channel_copyright_edit,tr("Copyright:"),this);
  feed_channel_copyright_label->setFont(labelFont());
  feed_channel_copyright_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Editor
  //
  feed_channel_editor_edit=new QLineEdit(this);
  feed_channel_editor_edit->setMaxLength(64);
  feed_channel_editor_label=
    new QLabel(feed_channel_editor_edit,tr("Editor:"),this);
  feed_channel_editor_label->setFont(labelFont());
  feed_channel_editor_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Author
  //
  feed_channel_author_edit=new QLineEdit(this);
  feed_channel_author_edit->setMaxLength(64);
  feed_channel_author_label=
    new QLabel(feed_channel_author_edit,tr("Author:"),this);
  feed_channel_author_label->setFont(labelFont());
  feed_channel_author_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  feed_channel_author_is_default_check=new QCheckBox(this);
  feed_channel_author_is_default_label=
    new QLabel(tr("Use as default Item Author"),this);
  feed_channel_author_is_default_label->setFont(labelFont());
  feed_channel_author_is_default_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Channel Owner Name
  //
  feed_channel_owner_name_edit=new QLineEdit(this);
  feed_channel_owner_name_edit->setMaxLength(64);
  feed_channel_owner_name_label=
    new QLabel(feed_channel_owner_name_edit,tr("Owner Name:"),this);
  feed_channel_owner_name_label->setFont(labelFont());
  feed_channel_owner_name_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Owner Email
  //
  feed_channel_owner_email_edit=new QLineEdit(this);
  feed_channel_owner_email_edit->setMaxLength(64);
  feed_channel_owner_email_label=
    new QLabel(feed_channel_owner_email_edit,tr("Owner E-Mail:"),this);
  feed_channel_owner_email_label->setFont(labelFont());
  feed_channel_owner_email_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Webmaster
  //
  feed_channel_webmaster_edit=new QLineEdit(this);
  feed_channel_webmaster_edit->setMaxLength(64);
  feed_channel_webmaster_label=
    new QLabel(feed_channel_webmaster_edit,tr("Webmaster:"),this);
  feed_channel_webmaster_label->setFont(labelFont());
  feed_channel_webmaster_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Language
  //
  feed_channel_language_edit=new QLineEdit(this);
  feed_channel_language_edit->setMaxLength(8);
  feed_channel_language_label=
    new QLabel(feed_channel_language_edit,tr("Language:"),this);
  feed_channel_language_label->setFont(labelFont());
  feed_channel_language_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel is Explicit
  //
  feed_channel_explicit_check=new QCheckBox(this);
  feed_channel_explicit_label=new QLabel(feed_channel_explicit_check,
			       tr("Channel contains explicit content"),this);
  feed_channel_explicit_label->setFont(labelFont());
  feed_channel_explicit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Channel Image
  //
  feed_channel_image_box=
    new RDImagePickerBox("FEED_IMAGES","FEED_ID","ID",this);
  feed_channel_image_box->setCategoryId(feed_feed->id());
  feed_channel_image_label=
    new QLabel(feed_channel_image_box,tr("Image")+":",this);
  feed_channel_image_label->setFont(labelFont());
  feed_channel_image_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channel Description
  //
  feed_channel_description_edit=new QTextEdit(this);
  feed_channel_description_edit->setAcceptRichText(false);
  feed_channel_description_label=
    new QLabel(feed_channel_description_edit,tr("Description:"),this);
  feed_channel_description_label->setFont(labelFont());
  feed_channel_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Purge Audio URL
  //
  feed_purge_url_edit=new QLineEdit(this);
  feed_purge_url_edit->setMaxLength(191);
  connect(feed_purge_url_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(purgeUrlChangedData(const QString &)));
  feed_purge_url_label=
    new QLabel(feed_purge_url_edit,tr("Upload URL")+":",this);
  feed_purge_url_label->setFont(labelFont());
  feed_purge_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Purge Username
  //
  feed_purge_username_edit=new QLineEdit(this);
  feed_purge_username_edit->setMaxLength(64);
  connect(feed_purge_username_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(lineeditChangedData(const QString &)));
  feed_purge_username_label=
    new QLabel(feed_purge_username_edit,tr("Username:"),this);
  feed_purge_username_label->setFont(labelFont());
  feed_purge_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Purge Password
  //
  feed_purge_password_edit=new QLineEdit(this);
  feed_purge_password_edit->setMaxLength(64);
  feed_purge_password_edit->setEchoMode(QLineEdit::Password);
  feed_purge_password_label=
    new QLabel(feed_purge_password_edit,tr("Password:"),this);
  feed_purge_password_label->setFont(labelFont());
  feed_purge_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Purge Use Id File
  //
  feed_purge_use_id_file_check=new QCheckBox(this);
  feed_purge_use_id_file_label=
    new QLabel(feed_purge_use_id_file_check,
	       tr("Authenticate with local identity file"),this);
  feed_purge_use_id_file_label->setFont(labelFont());
  feed_purge_use_id_file_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Audio Format
  //
  feed_format_edit=new QLineEdit(this);
  feed_format_edit->setReadOnly(true);
  feed_format_label=new QLabel(feed_format_edit,tr("Audio Format:"),this);
  feed_format_label->setFont(labelFont());
  feed_format_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_format_button=new QPushButton(this);
  feed_format_button->setFont(subButtonFont());
  feed_format_button->setText(tr("S&et"));
  connect(feed_format_button,SIGNAL(clicked()),this,SLOT(setFormatData()));

  //
  // Normalize Check Box
  //
  feed_normalize_check=new QCheckBox(this);
  feed_normalize_check->setChecked(true);
  feed_normalize_check_label=
    new QLabel(feed_normalize_check,tr("Normalize"),this);
  feed_normalize_check_label->setFont(labelFont());
  feed_normalize_check_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(feed_normalize_check,SIGNAL(toggled(bool)),
	  this,SLOT(checkboxToggledData(bool)));

  //
  // Normalize Level
  //
  feed_normalize_spin=new QSpinBox(this);
  feed_normalize_spin->setRange(-30,-1);
  feed_normalize_label=new QLabel(feed_normalize_spin,tr("Level:"),this);
  feed_normalize_label->setFont(labelFont());
  feed_normalize_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_normalize_unit_label=new QLabel(tr("dBFS"),this);
  feed_normalize_unit_label->setFont(labelFont());
  feed_normalize_unit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Base Audio URL
  //
  feed_base_url_edit=new QLineEdit(this);
  feed_base_url_edit->setMaxLength(191);
  feed_base_url_label=
    new QLabel(feed_base_url_edit,tr("Download URL")+":",this);
  feed_base_url_label->setFont(labelFont());
  feed_base_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // AutoPost
  //
  feed_autopost_box=new QComboBox(this);
  feed_autopost_box->insertItem(feed_autopost_box->count(),tr("No"));
  feed_autopost_box->insertItem(feed_autopost_box->count(),tr("Yes"));
  feed_autopost_label=
    new QLabel(feed_autopost_box,tr("Enable AutoPost")+":",this);
  feed_autopost_label->setFont(labelFont());
  feed_autopost_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
 
  //
  // Enclosure Preamble
  //
  feed_base_preamble_edit=new QLineEdit(this);
  feed_base_preamble_edit->setMaxLength(191);
  feed_base_preamble_label=
    new QLabel(feed_base_preamble_edit,tr("Enclosure Preamble:"),this);
  feed_base_preamble_label->setFont(labelFont());
  feed_base_preamble_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Audio File Extension
  //
  feed_extension_edit=new QLineEdit(this);
  feed_extension_edit->setMaxLength(16);
  feed_extension_label=
    new QLabel(feed_extension_edit,tr("Audio Extension:"),this);
  feed_extension_label->setFont(labelFont());
  feed_extension_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Maximum Shelf Life
  //
  feed_max_shelf_life_spin=new QSpinBox(this);
  feed_max_shelf_life_spin->setRange(0,365);
  feed_max_shelf_life_spin->setSpecialValueText(tr("Unlimited"));
  feed_max_shelf_life_label=
    new QLabel(feed_max_shelf_life_spin,tr("Default Shelf Life")+":",this);
  feed_max_shelf_life_label->setFont(labelFont());
  feed_max_shelf_life_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_max_shelf_life_unit_label=
    new QLabel(feed_max_shelf_life_spin,tr("days"),this);
  feed_max_shelf_life_unit_label->setFont(labelFont());
  feed_max_shelf_life_unit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Episode Order
  //
  feed_castorder_box=new QComboBox(this);
  feed_castorder_box->insertItem(tr("Descending"));
  feed_castorder_box->insertItem(tr("Ascending"));
  feed_castorder_label=
    new QLabel(feed_castorder_box,tr("Episode Sort Order:"),this);
  feed_castorder_label->setFont(labelFont());
  feed_castorder_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Default Item Image
  //
  feed_item_image_box=new RDImagePickerBox("FEED_IMAGES","FEED_ID","ID",this);
  feed_item_image_box->setCategoryId(feed_feed->id());
  feed_item_image_label=
    new QLabel(feed_item_image_box,tr("Default Item Image")+":",this);
  feed_item_image_label->setFont(labelFont());
  feed_item_image_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // RSS Schema
  //
  feed_rss_schema_box=new QComboBox(this);
  for(int i=0;i<RDRssSchemas::LastSchema;i++) {
    feed_rss_schema_box->
      insertItem(feed_rss_schema_box->count(),
		 rda->rssSchemas()->name((RDRssSchemas::RssSchema)i),i);
  }
  connect(feed_rss_schema_box,SIGNAL(activated(int)),
	  this,SLOT(schemaActivatedData(int)));
  feed_rss_schema_label=new QLabel(tr("RSS Schema")+":",this);
  feed_rss_schema_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_rss_schema_label->setFont(labelFont());

  //
  // Header XML
  //
  feed_header_xml_edit=new QTextEdit(this);
  feed_header_xml_edit->setAcceptRichText(false);
  feed_header_xml_label=new QLabel(feed_header_xml_edit,tr("Header XML:"),this);
  feed_header_xml_label->setFont(labelFont());
  feed_header_xml_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_header_xml_button=new QPushButton(tr("Copy to\nClipboard"),this);
  feed_header_xml_button->setFont(subButtonFont());
  connect(feed_header_xml_button,SIGNAL(clicked()),
	  this,SLOT(copyHeaderXmlData()));

  //
  // Channel XML
  //
  feed_channel_xml_edit=new QTextEdit(this);
  feed_channel_xml_edit->setAcceptRichText(false);
  feed_channel_xml_label=
    new QLabel(feed_channel_xml_edit,tr("Channel XML:"),this);
  feed_channel_xml_label->setFont(labelFont());
  feed_channel_xml_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_channel_xml_button=new QPushButton(tr("Copy to\nClipboard"),this);
  feed_channel_xml_button->setFont(subButtonFont());
  connect(feed_channel_xml_button,SIGNAL(clicked()),
	  this,SLOT(copyChannelXmlData()));

  //
  // Item XML
  //
  feed_item_xml_edit=new QTextEdit(this);
  feed_item_xml_edit->setAcceptRichText(false);
  feed_item_xml_label=new QLabel(feed_item_xml_edit,tr("Item XML:"),this);
  feed_item_xml_label->setFont(labelFont());
  feed_item_xml_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_item_xml_button=new QPushButton(tr("Copy to\nClipboard"),this);
  feed_item_xml_button->setFont(subButtonFont());
  connect(feed_item_xml_button,SIGNAL(clicked()),
	  this,SLOT(copyItemXmlData()));

  //
  //  Ok Button
  //
  feed_ok_button=new QPushButton(this);
  feed_ok_button->setDefault(true);
  feed_ok_button->setFont(buttonFont());
  feed_ok_button->setText(tr("&OK"));
  connect(feed_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  feed_cancel_button=new QPushButton(this);
  feed_cancel_button->setFont(buttonFont());
  feed_cancel_button->setText(tr("&Cancel"));
  connect(feed_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Values
  //
  feed_is_superfeed_box->setCurrentIndex(feed_feed->isSuperfeed());
  feed_channel_title_edit->setText(feed_feed->channelTitle());
  feed_channel_category_box->setSchema(feed_feed->rssSchema());
  feed_channel_category_box->
    setCategory(feed_feed->channelCategory(),feed_feed->channelSubCategory());
  feed_channel_link_edit->setText(feed_feed->channelLink());
  feed_channel_copyright_edit->setText(feed_feed->channelCopyright());
  feed_channel_editor_edit->setText(feed_feed->channelEditor());
  feed_channel_author_edit->setText(feed_feed->channelAuthor());
  feed_channel_author_is_default_check->
    setChecked(feed_feed->channelAuthorIsDefault());
  feed_channel_owner_name_edit->setText(feed_feed->channelOwnerName());
  feed_channel_owner_email_edit->setText(feed_feed->channelOwnerEmail());
  feed_channel_webmaster_edit->setText(feed_feed->channelWebmaster());
  feed_channel_description_edit->setPlainText(feed_feed->channelDescription());
  feed_channel_image_box->setCurrentImageId(feed_feed->channelImageId());
  feed_channel_language_edit->setText(feed_feed->channelLanguage());
  feed_channel_explicit_check->setChecked(feed_feed->channelExplicit());
  feed_base_url_edit->setText(feed_feed->baseUrl(""));
  feed_base_preamble_edit->setText(feed_feed->basePreamble());
  feed_purge_url_edit->setText(feed_feed->purgeUrl());
  feed_purge_username_edit->setText(feed_feed->purgeUsername());
  feed_purge_password_edit->setText(feed_feed->purgePassword());
  feed_purge_use_id_file_check->setChecked(feed_feed->purgeUseIdFile());
  purgeUrlChangedData(feed_purge_url_edit->text());
  RDRssSchemas::RssSchema schema=feed_feed->rssSchema();
  for(int i=0;i<feed_rss_schema_box->count();i++) {
    if(feed_rss_schema_box->itemData(i).toInt()==schema) {
      feed_rss_schema_box->setCurrentItem(i);
      continue;
    }
  }
  feed_header_xml_edit->setPlainText(feed_feed->headerXml());
  feed_channel_xml_edit->setPlainText(feed_feed->channelXml());
  feed_item_xml_edit->setPlainText(feed_feed->itemXml());
  feed_max_shelf_life_spin->setValue(feed_feed->maxShelfLife());
  feed_autopost_box->setCurrentIndex(feed_feed->enableAutopost());
  feed_settings.setFormat(feed_feed->uploadFormat());
  feed_settings.setChannels(feed_feed->uploadChannels());
  feed_settings.setSampleRate(feed_feed->uploadSampleRate());
  feed_settings.setBitRate(feed_feed->uploadBitRate());
  feed_settings.setQuality(feed_feed->uploadQuality());
  feed_extension_edit->setText(feed_feed->uploadExtension());
  feed_format_edit->setText(feed_settings.description());
  if(feed_feed->normalizeLevel()>0) {
    feed_normalize_check->setChecked(false);
  }
  else {
    feed_normalize_check->setChecked(true);
    feed_normalize_spin->setValue(feed_feed->normalizeLevel()/1000);
  }
  feed_castorder_box->setCurrentItem(feed_feed->castOrder());
  feed_item_image_box->setCurrentImageId(feed_feed->defaultItemImageId());

  UpdateControlState();
}


EditFeed::~EditFeed()
{
  delete feed_image_model;
  delete feed_feed;
}


QSize EditFeed::sizeHint() const
{
  return QSize(1000,710);
} 


QSizePolicy EditFeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}



void EditFeed::superfeedActivatedData(int n)
{
  UpdateControlState();
}


void EditFeed::schemaActivatedData(int n)
{
  feed_channel_category_box->setSchema((RDRssSchemas::RssSchema)n);
  
  UpdateControlState();
}


void EditFeed::checkboxToggledData(bool state)
{
  UpdateControlState();
}


void EditFeed::purgeUrlChangedData(const QString &str)
{
  QUrl url(str);

  if((url.scheme().toLower()=="sftp")&&
     (!rda->station()->sshIdentityFile().isEmpty())) {
    feed_purge_use_id_file_check->setEnabled(true);
    feed_purge_use_id_file_label->setEnabled(true);
  }
  else {
    feed_purge_use_id_file_check->setDisabled(true);
    feed_purge_use_id_file_label->setDisabled(true);
  }
}


void EditFeed::lineeditChangedData(const QString &str)
{
  UpdateControlState();
}


void EditFeed::selectSubfeedsData()
{
  EditSuperfeed *d=new EditSuperfeed(feed_feed,this);
  d->exec();
  delete d;
}


void EditFeed::setFormatData()
{
  RDExportSettingsDialog *dialog=new RDExportSettingsDialog("RDAdmin",this);
  dialog->exec(&feed_settings);
  delete dialog;
  feed_format_edit->setText(feed_settings.description());
}


void EditFeed::listImagesData()
{
  feed_images_dialog->exec(feed_feed);
  feed_channel_image_box->refresh();
  feed_item_image_box->refresh();
}


void EditFeed::copyHeaderXmlData()
{
  RDRssSchemas::RssSchema schema=
    (RDRssSchemas::RssSchema)feed_rss_schema_box->currentIndex();

  if(schema==RDRssSchemas::CustomSchema) {
    QApplication::clipboard()->setText(feed_header_xml_edit->text());
  }
  else {
    QApplication::clipboard()->
      setText(rda->rssSchemas()->headerTemplate(schema));
  }
}


void EditFeed::copyChannelXmlData()
{
  RDRssSchemas::RssSchema schema=
    (RDRssSchemas::RssSchema)feed_rss_schema_box->currentIndex();

  if(schema==RDRssSchemas::CustomSchema) {
    QApplication::clipboard()->setText(feed_channel_xml_edit->text());
  }
  else {
    QApplication::clipboard()->
      setText(rda->rssSchemas()->channelTemplate(schema));
  }
}


void EditFeed::copyItemXmlData()
{
  RDRssSchemas::RssSchema schema=
    (RDRssSchemas::RssSchema)feed_rss_schema_box->currentIndex();

  if(schema==RDRssSchemas::CustomSchema) {
    QApplication::clipboard()->setText(feed_item_xml_edit->text());
  }
  else {
    QApplication::clipboard()->
      setText(rda->rssSchemas()->itemTemplate(schema));
  }
}


void EditFeed::okData()
{
  if(feed_is_superfeed_box->currentItem()&&
     feed_feed->subfeedNames().size()==0) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
		      tr("Superfeed must have at least one subfeed assigned!"));
    return;
  }
  RDDelete *d=new RDDelete(rda->config(),this);
  RDUpload *u=new RDUpload(rda->config(),this);
  if((!d->urlIsSupported(feed_purge_url_edit->text()))||
     (!u->urlIsSupported(feed_purge_url_edit->text()))) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Audio Upload URL has unsupported scheme!"));
    delete d;
    delete u;
    return;
  }
  delete d;
  delete u;
  feed_feed->setIsSuperfeed(feed_is_superfeed_box->currentItem());
  feed_feed->setChannelTitle(feed_channel_title_edit->text());
  feed_feed->setChannelCategory(feed_channel_category_box->category());
  feed_feed->setChannelSubCategory(feed_channel_category_box->subCategory());
  feed_feed->setChannelLink(feed_channel_link_edit->text());
  feed_feed->setChannelCopyright(feed_channel_copyright_edit->text());
  feed_feed->setChannelEditor(feed_channel_editor_edit->text());
  feed_feed->setChannelAuthor(feed_channel_author_edit->text());
  feed_feed->setChannelAuthorIsDefault(feed_channel_author_is_default_check->
				       isChecked());
  feed_feed->setChannelOwnerName(feed_channel_owner_name_edit->text());
  feed_feed->setChannelOwnerEmail(feed_channel_owner_email_edit->text());
  feed_feed->setChannelWebmaster(feed_channel_webmaster_edit->text());
  feed_feed->setChannelDescription(feed_channel_description_edit->text());
  feed_feed->setChannelLanguage(feed_channel_language_edit->text());
  feed_feed->setChannelExplicit(feed_channel_explicit_check->isChecked());
  feed_feed->setChannelImageId(feed_channel_image_box->currentImageId());
  feed_feed->setDefaultItemImageId(feed_item_image_box->currentImageId());
  feed_feed->setBaseUrl(feed_base_url_edit->text());
  feed_feed->setBasePreamble(feed_base_preamble_edit->text());
  feed_feed->setPurgeUrl(feed_purge_url_edit->text());
  feed_feed->setPurgeUsername(feed_purge_username_edit->text());
  feed_feed->setPurgePassword(feed_purge_password_edit->text());
  feed_feed->setPurgeUseIdFile(feed_purge_use_id_file_check->isChecked()&&
			       feed_purge_use_id_file_check->isEnabled());
  feed_feed->
    setRssSchema((RDRssSchemas::RssSchema)feed_rss_schema_box->
		 itemData(feed_rss_schema_box->currentIndex()).toUInt());
  feed_feed->setHeaderXml(feed_header_xml_edit->text());
  feed_feed->setChannelXml(feed_channel_xml_edit->text());
  feed_feed->setItemXml(feed_item_xml_edit->text());
  feed_feed->setMaxShelfLife(feed_max_shelf_life_spin->value());
  feed_feed->setEnableAutopost(feed_autopost_box->currentIndex());
  feed_feed->setUploadFormat(feed_settings.format());
  feed_feed->setUploadChannels(feed_settings.channels());
  feed_feed->setUploadSampleRate(feed_settings.sampleRate());
  feed_feed->setUploadBitRate(feed_settings.bitRate());
  feed_feed->setUploadQuality(feed_settings.quality());
  feed_feed->setUploadExtension(feed_extension_edit->text());
  if(feed_normalize_check->isChecked()) {
    feed_feed->setNormalizeLevel(feed_normalize_spin->value()*1000);
  }
  else {
    feed_feed->setNormalizeLevel(1);
  }
  feed_feed->setCastOrder(feed_castorder_box->currentItem());

  if(!feed_feed->postXmlConditional("RDAdmin",this)) {
    return;
  }

  done(0);
}


void EditFeed::cancelData()
{
  done(-1);
}


void EditFeed::resizeEvent(QResizeEvent *e)
{
  feed_image_model->rescaleImages(QSize(36,36));

  //
  // Left-hand Side
  //
  feed_is_superfeed_box->setGeometry(115,2,60,19);
  feed_is_superfeed_label->setGeometry(10,2,100,19);
  feed_is_superfeed_button->setGeometry(185,2,140,38);

  feed_list_images_button->setGeometry(345,2,100,38);

  feed_channel_section_groupbox->setGeometry(10,41,sizeHint().width()/2-10,377);

  feed_channel_title_edit->setGeometry(115,56,375,19);
  feed_channel_title_label->setGeometry(20,56,90,19);

  feed_channel_category_box->setGeometry(115,78,375,19);
  feed_channel_category_label->setGeometry(20,78,90,19);

  feed_channel_link_edit->setGeometry(115,100,375,19);
  feed_channel_link_label->setGeometry(20,100,90,19);

  feed_channel_copyright_edit->setGeometry(115,122,375,19);
  feed_channel_copyright_label->setGeometry(20,122,90,19);

  feed_channel_editor_edit->setGeometry(115,144,375,19);
  feed_channel_editor_label->setGeometry(20,144,90,19);

  feed_channel_author_edit->setGeometry(115,166,375,19);
  feed_channel_author_label->setGeometry(20,166,90,19);

  feed_channel_author_is_default_check->setGeometry(120,186,15,15);
  feed_channel_author_is_default_label->setGeometry(140,185,260,19);

  feed_channel_explicit_check->setGeometry(205,187,15,15);
  feed_channel_explicit_label->setGeometry(225,186,260,19);

  feed_channel_owner_name_edit->setGeometry(115,209,375,19);
  feed_channel_owner_name_label->setGeometry(20,209,90,19);

  feed_channel_owner_email_edit->setGeometry(115,231,375,19);
  feed_channel_owner_email_label->setGeometry(20,231,90,19);

  feed_channel_webmaster_edit->setGeometry(115,253,375,19);
  feed_channel_webmaster_label->setGeometry(20,253,90,19);

  feed_channel_language_edit->setGeometry(115,275,60,19);
  feed_channel_language_label->setGeometry(20,275,90,19);

  feed_channel_explicit_check->setGeometry(205,277,15,15);
  feed_channel_explicit_label->setGeometry(225,276,260,19);

  feed_channel_description_edit->setGeometry(115,297,375,76);
  feed_channel_description_label->setGeometry(20,297,90,19);

  feed_channel_image_box->setGeometry(115,375,375,38);
  feed_channel_image_box->setIconSize(QSize(36,36));
  feed_channel_image_label->setGeometry(20,375,90,19);

  feed_purge_url_edit->setGeometry(155,425,335,19);
  feed_purge_url_label->setGeometry(20,425,130,19);
  feed_purge_username_edit->setGeometry(225,445,95,19);
  feed_purge_username_label->setGeometry(40,445,180,19);
  feed_purge_password_edit->setGeometry(395,445,95,19);
  feed_purge_password_label->setGeometry(320,445,70,19);
  feed_purge_use_id_file_check->setGeometry(160,466,15,15);
  feed_purge_use_id_file_label->setGeometry(180,464,300,19);

  feed_format_edit->setGeometry(155,485,285,20);
  feed_format_label->setGeometry(5,485,145,20);
  feed_format_button->setGeometry(450,483,40,24);

  feed_normalize_check->setGeometry(160,510,15,15);
  feed_normalize_check_label->setGeometry(180,507,83,20);
  feed_normalize_spin->setGeometry(295,507,40,20);
  feed_normalize_label->setGeometry(245,507,45,20);
  feed_normalize_unit_label->setGeometry(340,507,40,20);

  feed_extension_edit->setGeometry(155,529,70,19);
  feed_extension_label->setGeometry(20,529,130,19);

  feed_base_url_edit->setGeometry(155,551,335,19);
  feed_base_url_label->setGeometry(5,551,145,19);

  feed_autopost_box->setGeometry(155,573,60,19);
  feed_autopost_label->setGeometry(5,573,145,19);

  feed_max_shelf_life_spin->setGeometry(155,595,90,19);
  feed_max_shelf_life_label->setGeometry(20,595,130,19);
  feed_max_shelf_life_unit_label->setGeometry(250,595,50,19);

  feed_item_image_box->setGeometry(155,617,335,38);
  feed_item_image_box->setIconSize(QSize(36,36));
  feed_item_image_label->setGeometry(20,617,130,19);

  feed_base_preamble_edit->setGeometry(155,658,335,19);
  feed_base_preamble_label->setGeometry(20,658,130,19);

  feed_castorder_box->setGeometry(155,682,100,19);
  feed_castorder_label->setGeometry(20,682,130,19);

  //
  // Right-hand Side
  //
  feed_rss_schema_label->setGeometry(520,10,90,19);
  feed_rss_schema_box->setGeometry(615,10,200,19);

  feed_header_xml_label->setGeometry(520,32,90,19);
  feed_header_xml_edit->setGeometry(615,32,size().width()-625,76);
  feed_header_xml_button->setGeometry(540,54,70,30);

  feed_channel_xml_label->setGeometry(520,110,90,19);
  feed_channel_xml_edit->setGeometry(615,110,size().width()-625,256);
  feed_channel_xml_button->setGeometry(540,132,70,30);

  feed_item_xml_label->setGeometry(520,368,90,19);
  feed_item_xml_edit->setGeometry(615,368,size().width()-625,240);
  feed_item_xml_button->setGeometry(540,390,70,30);

  feed_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  feed_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditFeed::UpdateControlState()
{
  bool superfeed=feed_is_superfeed_box->currentIndex();
  bool custom_schema=
    feed_rss_schema_box->itemData(feed_rss_schema_box->currentIndex()).toInt()==
    RDRssSchemas::CustomSchema;
  bool item_image=rda->rssSchemas()->
    supportsItemImages((RDRssSchemas::RssSchema)feed_rss_schema_box->
		       itemData(feed_rss_schema_box->currentIndex()).toInt());

  feed_is_superfeed_button->setEnabled(superfeed);

  feed_max_shelf_life_spin->setDisabled(superfeed);
  feed_autopost_box->setDisabled(superfeed);
  feed_autopost_label->setDisabled(superfeed);
  feed_format_edit->setDisabled(superfeed);
  feed_normalize_check->setDisabled(superfeed);
  feed_extension_edit->setDisabled(superfeed);
  feed_format_button->setDisabled(superfeed);
  feed_max_shelf_life_label->setDisabled(superfeed);
  feed_max_shelf_life_unit_label->setDisabled(superfeed);
  feed_format_label->setDisabled(superfeed);
  feed_normalize_check_label->setDisabled(superfeed);
  feed_normalize_unit_label->setDisabled(superfeed);
  feed_extension_label->setDisabled(superfeed);

  feed_normalize_label->
    setDisabled(superfeed||(!feed_normalize_check->isChecked()));
  feed_normalize_spin->
    setDisabled(superfeed||(!feed_normalize_check->isChecked()));
  feed_normalize_unit_label->
    setDisabled(superfeed||(!feed_normalize_check->isChecked()));

  feed_item_image_label->setDisabled(item_image&&(superfeed));
  feed_item_image_box->setDisabled(item_image&&(superfeed));

  feed_header_xml_edit->setEnabled(custom_schema);
  feed_channel_xml_edit->setEnabled(custom_schema);
  feed_item_xml_edit->setDisabled(superfeed||(!custom_schema));
  feed_item_xml_button->setDisabled(superfeed);

  feed_purge_password_label->
    setDisabled(feed_purge_username_edit->text().isEmpty()||
		feed_purge_url_edit->text().isEmpty());
  feed_purge_password_edit->
    setDisabled(feed_purge_username_edit->text().isEmpty()||
		feed_purge_url_edit->text().isEmpty());
}
