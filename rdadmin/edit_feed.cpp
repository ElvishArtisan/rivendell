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
  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  feed_feed=new RDFeed(feed,rda->config(),this);

  setWindowTitle("RDAdmin - "+tr("Feed: ")+feed);

  //
  // Feed Name
  //
  feed_keyname_edit=new QLineEdit(this);
  feed_keyname_edit->setMaxLength(8);
  feed_keyname_edit->setReadOnly(true);
  feed_keyname_label=new QLabel(feed_keyname_edit,tr("Key Name:"),this);
  feed_keyname_label->setFont(labelFont());
  feed_keyname_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  feed_is_superfeed_box=new QComboBox(this);
  feed_is_superfeed_box->insertItem(0,tr("No"));
  feed_is_superfeed_box->insertItem(1,tr("Yes"));
  connect(feed_is_superfeed_box,SIGNAL(activated(int)),
	  this,SLOT(comboboxActivatedData(int)));
  feed_is_superfeed_label=new QLabel(tr("Is Superfeed")+":",this);
  feed_is_superfeed_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_is_superfeed_label->setFont(labelFont());
  feed_is_superfeed_button=new QPushButton(tr("Select Member\nFeeds"),this);
  feed_is_superfeed_button->setFont(buttonFont());
  connect(feed_is_superfeed_button,SIGNAL(clicked()),
	  this,SLOT(selectSubfeedsData()));

  feed_audience_metrics_check=new QCheckBox(this);
  feed_audience_metrics_label=new QLabel(tr("Collect Audience Metrics"),this);
  feed_audience_metrics_label->setFont(labelFont());
  feed_audience_metrics_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

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
  feed_channel_category_edit=new QLineEdit(this);
  feed_channel_category_edit->setMaxLength(64);
  feed_channel_category_label=
    new QLabel(feed_channel_category_edit,tr("Category:"),this);
  feed_channel_category_label->setFont(labelFont());
  feed_channel_category_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  feed_channel_language_edit->setMaxLength(5);
  feed_channel_language_label=
    new QLabel(feed_channel_language_edit,tr("Language:"),this);
  feed_channel_language_label->setFont(labelFont());
  feed_channel_language_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  // Keep Expired Metadata Checkbox
  //
  feed_keep_metadata_box=new QCheckBox(this);
  feed_keep_metadata_label=
    new QLabel(feed_keep_metadata_box,tr("Keep Expired Metadata"),this);
  feed_keep_metadata_label->setFont(labelFont());
  feed_keep_metadata_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
 
  //
  // AutoPost Checkbox
  //
  feed_autopost_box=new QCheckBox(this);
  feed_autopost_label=
    new QLabel(feed_autopost_box,tr("Enable AutoPost"),this);
  feed_autopost_label->setFont(labelFont());
  feed_autopost_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
 
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
  feed_max_shelf_life_spin->setSpecialValueText(tr("None"));
  feed_max_shelf_life_label=
    new QLabel(feed_max_shelf_life_spin,tr("Maximum Shelf Life:"),this);
  feed_max_shelf_life_label->setFont(labelFont());
  feed_max_shelf_life_label->
    setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  feed_max_shelf_life_unit_label=
    new QLabel(feed_max_shelf_life_spin,tr("days"),this);
  feed_max_shelf_life_unit_label->setFont(labelFont());
  feed_max_shelf_life_unit_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

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
  // Media Link Mode
  //
  feed_media_link_mode_box=new QComboBox(this);
  feed_media_link_mode_box->insertItem(tr("None"));
  feed_media_link_mode_box->insertItem(tr("Direct"));
  feed_media_link_mode_box->insertItem(tr("Counted"));
  feed_media_link_mode_label=
    new QLabel(feed_media_link_mode_box,tr("Media Link Mode:"),this);
  feed_media_link_mode_label->setFont(labelFont());
  feed_media_link_mode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Feed Redirection
  //
  feed_redirect_check=new QCheckBox(this);
  feed_redirect_label=
    new QLabel(feed_redirect_check,tr("Enable Feed Redirection"),this);
  feed_redirect_label->setFont(labelFont());
  feed_redirect_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  feed_redirect_url_edit=new QLineEdit(this);
  feed_redirect_url_label=new QLabel(feed_redirect_url_edit,tr("URL:"),this);
  feed_redirect_url_label->setFont(labelFont());
  feed_redirect_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // RSS Schema
  //
  feed_rss_schema_box=new QComboBox(this);
  sql=QString("select ")+
    "ID,"+    // 00
    "NAME "+  // 01
    "from RSS_SCHEMAS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    feed_rss_schema_box->
      insertItem(feed_rss_schema_box->count(),q->value(1).toString(),
		 q->value(0).toUInt());
  }
  delete q;
  connect(feed_rss_schema_box,SIGNAL(activated(int)),
	  this,SLOT(comboboxActivatedData(int)));
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

  //
  // Channel XML
  //
  feed_channel_xml_edit=new QTextEdit(this);
  feed_channel_xml_edit->setAcceptRichText(false);
  feed_channel_xml_label=
    new QLabel(feed_channel_xml_edit,tr("Channel XML:"),this);
  feed_channel_xml_label->setFont(labelFont());
  feed_channel_xml_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Item XML
  //
  feed_item_xml_edit=new QTextEdit(this);
  feed_item_xml_edit->setAcceptRichText(false);
  feed_item_xml_label=new QLabel(feed_item_xml_edit,tr("Item XML:"),this);
  feed_item_xml_label->setFont(labelFont());
  feed_item_xml_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
  feed_keyname_edit->setText(feed_feed->keyName());
  feed_is_superfeed_box->setCurrentIndex(feed_feed->isSuperfeed());
  feed_audience_metrics_check->setChecked(feed_feed->audienceMetrics());
  feed_channel_title_edit->setText(feed_feed->channelTitle());
  feed_channel_category_edit->setText(feed_feed->channelCategory());
  feed_channel_link_edit->setText(feed_feed->channelLink());
  feed_channel_copyright_edit->setText(feed_feed->channelCopyright());
  feed_channel_editor_edit->setText(feed_feed->channelEditor());
  feed_channel_webmaster_edit->setText(feed_feed->channelWebmaster());
  feed_channel_description_edit->setPlainText(feed_feed->channelDescription());
  feed_channel_language_edit->setText(feed_feed->channelLanguage());
  feed_base_url_edit->setText(feed_feed->baseUrl(""));
  feed_base_preamble_edit->setText(feed_feed->basePreamble());
  feed_purge_url_edit->setText(feed_feed->purgeUrl());
  feed_purge_username_edit->setText(feed_feed->purgeUsername());
  feed_purge_password_edit->setText(feed_feed->purgePassword());
  RDFeed::RssSchema schema=feed_feed->rssSchema();
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
    feed_normalize_check->setChecked(false);
  }
  else {
    feed_normalize_check->setChecked(true);
    feed_normalize_spin->setValue(feed_feed->normalizeLevel()/1000);
  }
  feed_castorder_box->setCurrentItem(feed_feed->castOrder());
  feed_media_link_mode_box->setCurrentItem((int)feed_feed->mediaLinkMode());
  feed_redirect_url_edit->setText(feed_feed->redirectPath());
  feed_redirect_check->setChecked(!feed_redirect_url_edit->text().isEmpty());

  connect(feed_redirect_check,SIGNAL(toggled(bool)),
	  this,SLOT(redirectToggledData(bool)));

  UpdateControlState();
}


QSize EditFeed::sizeHint() const
{
  return QSize(1000,665);
} 


QSizePolicy EditFeed::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}



void EditFeed::comboboxActivatedData(int n)
{
  UpdateControlState();
}


void EditFeed::checkboxToggledData(bool state)
{
  UpdateControlState();
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
  RDExportSettingsDialog *dialog=
    new RDExportSettingsDialog(&feed_settings,this);
  dialog->exec();
  delete dialog;
  feed_format_edit->setText(feed_settings.description());
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
  UpdateControlState();
}


void EditFeed::okData()
{
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
  if(feed_redirect_check->isChecked()) {
    QUrl url(feed_redirect_url_edit->text());
    if(url.isValid()&&(!url.isRelative()&&(!url.host().isEmpty()))) {
      feed_feed->setRedirectPath(url.toString());
    }
    else {
      QMessageBox::information(this,"RDAdmin - "+tr("Error"),
			       tr("The Feed Redirection URL is invalid!"));
      return;
    }
  }
  else {
    feed_feed->setRedirectPath("");
  }
  feed_feed->setIsSuperfeed(feed_is_superfeed_box->currentItem());
  feed_feed->setAudienceMetrics(feed_audience_metrics_check->isChecked());
  feed_feed->setChannelTitle(feed_channel_title_edit->text());
  feed_feed->setChannelCategory(feed_channel_category_edit->text());
  feed_feed->setChannelLink(feed_channel_link_edit->text());
  feed_feed->setChannelCopyright(feed_channel_copyright_edit->text());
  feed_feed->setChannelEditor(feed_channel_editor_edit->text());
  feed_feed->setChannelWebmaster(feed_channel_webmaster_edit->text());
  feed_feed->setChannelDescription(feed_channel_description_edit->text());
  feed_feed->setChannelLanguage(feed_channel_language_edit->text());
  feed_feed->setBaseUrl(feed_base_url_edit->text());
  feed_feed->setBasePreamble(feed_base_preamble_edit->text());
  feed_feed->setPurgeUrl(feed_purge_url_edit->text());
  feed_feed->setPurgeUsername(feed_purge_username_edit->text());
  feed_feed->setPurgePassword(feed_purge_password_edit->text());
  feed_feed->
    setRssSchema((RDFeed::RssSchema)feed_rss_schema_box->
		 itemData(feed_rss_schema_box->currentIndex()).toUInt());
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
  if(feed_normalize_check->isChecked()) {
    feed_feed->setNormalizeLevel(feed_normalize_spin->value()*1000);
  }
  else {
    feed_feed->setNormalizeLevel(1);
  }
  feed_feed->setCastOrder(feed_castorder_box->currentItem());
  feed_feed->setMediaLinkMode((RDFeed::MediaLinkMode)feed_media_link_mode_box->
			      currentItem());

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
  feed_keyname_edit->setGeometry(115,11,100,19);
  feed_keyname_label->setGeometry(10,11,100,19);

  feed_is_superfeed_box->setGeometry(115,33,60,19);
  feed_is_superfeed_label->setGeometry(10,33,100,19);
  feed_is_superfeed_button->setGeometry(185,33,160,38);

  feed_audience_metrics_check->setGeometry(20,74,15,15);
  feed_audience_metrics_label->setGeometry(40,72,375,19);

  feed_channel_section_groupbox->setGeometry(10,97,sizeHint().width()/2-10,249);
  feed_channel_title_edit->setGeometry(115,112,375,19);
  feed_channel_title_label->setGeometry(20,112,90,19);
  feed_channel_category_edit->setGeometry(115,134,375,19);
  feed_channel_category_label->setGeometry(20,134,90,19);
  feed_channel_link_edit->setGeometry(115,156,375,19);
  feed_channel_link_label->setGeometry(20,156,90,19);
  feed_channel_copyright_edit->setGeometry(115,178,375,19);
  feed_channel_copyright_label->setGeometry(20,178,90,19);
  feed_channel_editor_edit->setGeometry(115,200,375,19);
  feed_channel_editor_label->setGeometry(20,200,90,19);
  feed_channel_webmaster_edit->setGeometry(115,222,375,19);
  feed_channel_webmaster_label->setGeometry(20,222,90,19);
  feed_channel_language_edit->setGeometry(115,244,60,19);
  feed_channel_language_label->setGeometry(20,244,90,19);
  feed_channel_description_edit->setGeometry(115,266,375,76);
  feed_channel_description_label->setGeometry(20,266,90,19);

  feed_purge_url_edit->setGeometry(155,354,335,19);
  feed_purge_url_label->setGeometry(20,354,130,19);
  feed_purge_username_edit->setGeometry(225,376,95,19);
  feed_purge_username_label->setGeometry(40,376,180,19);
  feed_purge_password_edit->setGeometry(395,376,95,19);
  feed_purge_password_label->setGeometry(320,376,70,19);

  feed_format_edit->setGeometry(155,398,285,20);
  feed_format_label->setGeometry(5,398,145,20);
  feed_format_button->setGeometry(450,398,40,24);

  feed_normalize_check->setGeometry(155,422,15,15);
  feed_normalize_check_label->setGeometry(175,420,83,20);
  feed_normalize_spin->setGeometry(295,418,40,20);
  feed_normalize_label->setGeometry(245,418,45,20);
  feed_normalize_unit_label->setGeometry(340,418,40,20);

  feed_base_url_edit->setGeometry(155,442,335,19);
  feed_base_url_label->setGeometry(5,442,145,19);
  feed_keep_metadata_box->setGeometry(155,464,15,15);
  feed_keep_metadata_label->setGeometry(175,462,180,19);
  feed_autopost_box->setGeometry(365,464,15,15);
  feed_autopost_label->setGeometry(385,462,200,19);

  feed_base_preamble_edit->setGeometry(155,486,335,19);
  feed_base_preamble_label->setGeometry(20,486,130,19);

  feed_extension_edit->setGeometry(155,508,70,19);
  feed_extension_label->setGeometry(20,508,130,19);

  feed_max_shelf_life_spin->setGeometry(155,530,60,19);
  feed_max_shelf_life_label->setGeometry(20,530,130,19);
  feed_max_shelf_life_unit_label->setGeometry(220,530,50,19);

  feed_castorder_box->setGeometry(155,552,100,19);
  feed_castorder_label->setGeometry(20,552,130,19);

  feed_media_link_mode_box->setGeometry(155,574,100,19);
  feed_media_link_mode_label->setGeometry(20,574,130,19);

  feed_redirect_check->setGeometry(20,606,15,15);
  feed_redirect_label->setGeometry(40,606,200,19);
  feed_redirect_url_edit->setGeometry(85,626,405,20);
  feed_redirect_url_label->setGeometry(40,626,40,19);

  feed_rss_schema_label->setGeometry(520,10,90,19);
  feed_rss_schema_box->setGeometry(615,10,200,19);

  feed_header_xml_label->setGeometry(520,32,90,19);
  feed_header_xml_edit->setGeometry(615,32,size().width()-625,76);

  feed_channel_xml_label->setGeometry(520,110,90,19);
  feed_channel_xml_edit->setGeometry(615,110,size().width()-625,216);

  feed_item_xml_label->setGeometry(520,332,90,19);
  feed_item_xml_edit->setGeometry(615,332,size().width()-625,240);

  feed_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  feed_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditFeed::UpdateControlState()
{
  bool superfeed=feed_is_superfeed_box->currentIndex();
  bool redirected=feed_redirect_check->isChecked();
  bool custom_schema=
    feed_rss_schema_box->itemData(feed_rss_schema_box->currentIndex()).toInt()==
    RDFeed::CustomSchema;

  feed_is_superfeed_label->setDisabled(redirected);
  feed_is_superfeed_box->setDisabled(redirected);
  feed_is_superfeed_button->setDisabled(redirected||(!superfeed));

  feed_audience_metrics_check->setDisabled(redirected);
  feed_audience_metrics_label->setDisabled(redirected);

  feed_channel_title_edit->setDisabled(redirected);
  feed_channel_description_edit->setDisabled(redirected);
  feed_channel_category_edit->setDisabled(redirected);
  feed_channel_link_edit->setDisabled(redirected);
  feed_channel_copyright_edit->setDisabled(redirected);
  feed_channel_editor_label->setDisabled(redirected);
  feed_channel_editor_edit->setDisabled(redirected);
  feed_channel_webmaster_label->setDisabled(redirected);
  feed_channel_webmaster_edit->setDisabled(redirected);
  feed_channel_language_edit->setDisabled(redirected);
  feed_channel_title_label->setDisabled(redirected);
  feed_channel_category_label->setDisabled(redirected);
  feed_channel_link_label->setDisabled(redirected);
  feed_channel_copyright_label->setDisabled(redirected);
  feed_channel_language_label->setDisabled(redirected);
  feed_channel_description_label->setDisabled(redirected);

  feed_redirect_url_label->setEnabled(redirected);
  feed_redirect_url_edit->setEnabled(redirected);

  feed_base_url_edit->setDisabled(redirected);
  feed_purge_url_edit->setDisabled(redirected);
  feed_purge_username_label->setDisabled(redirected);
  feed_purge_username_edit->setDisabled(redirected);
  feed_purge_password_label->setDisabled(redirected);
  feed_purge_password_edit->setDisabled(redirected);

  feed_max_shelf_life_spin->setDisabled(redirected||superfeed);
  feed_autopost_box->setDisabled(redirected||superfeed);
  feed_keep_metadata_box->setDisabled(redirected);
  feed_keep_metadata_label->setDisabled(redirected);
  feed_format_edit->setDisabled(redirected||superfeed);
  feed_normalize_check->setDisabled(redirected||superfeed);
  feed_extension_edit->setDisabled(redirected||superfeed);
  feed_castorder_box->setDisabled(redirected);
  feed_format_button->setDisabled(redirected||superfeed);
  feed_base_url_label->setDisabled(redirected);
  feed_base_preamble_label->setDisabled(redirected);
  feed_purge_url_label->setDisabled(redirected);
  feed_max_shelf_life_label->setDisabled(redirected||superfeed);
  feed_max_shelf_life_unit_label->setDisabled(redirected||superfeed);
  feed_autopost_label->setDisabled(redirected||superfeed);
  feed_format_label->setDisabled(redirected||superfeed);
  feed_normalize_check_label->setDisabled(redirected||superfeed);
  feed_normalize_unit_label->setDisabled(redirected||superfeed);
  feed_castorder_label->setDisabled(redirected);
  feed_extension_label->setDisabled(redirected||superfeed);
  feed_channel_section_groupbox->setDisabled(redirected);

  feed_normalize_label->
    setDisabled(redirected||superfeed||(!feed_normalize_check->isChecked()));
  feed_normalize_spin->
    setDisabled(redirected||superfeed||(!feed_normalize_check->isChecked()));
  feed_normalize_unit_label->
    setDisabled(redirected||superfeed||(!feed_normalize_check->isChecked()));
  feed_media_link_mode_box->setDisabled(redirected||superfeed);
  feed_media_link_mode_label->setDisabled(redirected||superfeed);

  feed_header_xml_label->setDisabled(redirected||(!custom_schema));
  feed_header_xml_edit->setDisabled(redirected||(!custom_schema));

  feed_channel_xml_label->setDisabled(redirected||(!custom_schema));
  feed_channel_xml_edit->setDisabled(redirected||(!custom_schema));

  feed_item_xml_label->setDisabled(redirected||superfeed||(!custom_schema));
  feed_item_xml_edit->setDisabled(redirected||superfeed||(!custom_schema));

  feed_purge_password_label->
    setDisabled(feed_purge_username_edit->text().isEmpty()||
		feed_purge_url_edit->text().isEmpty()||
		feed_redirect_check->isChecked());
  feed_purge_password_edit->
    setDisabled(feed_purge_username_edit->text().isEmpty()||
		feed_purge_url_edit->text().isEmpty()||
		feed_redirect_check->isChecked());
}
