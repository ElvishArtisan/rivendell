// rdwavedata_dialog.cpp
//
// A dialog to edit the contents of an RDWaveData.
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdwavedata_dialog.cpp,v 1.1.2.1 2014/05/28 21:21:41 cvs Exp $
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

#include <qvalidator.h>

#include "rdwavedata_dialog.h"

RDWaveDataDialog::RDWaveDataDialog(const QString &caption,QWidget *parent)
  : QDialog(parent,"",true)
{
  wave_caption=caption;
  setCaption(caption+" - "+tr("Edit Cart Label"));

  //
  // Fonts
  //
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // Dialogs
  //
  wave_schedcodes_dialog=new RDSchedCodesDialog(this);

  //
  // Title
  //
  wave_title_label=new QLabel(tr("Title")+":",this);
  wave_title_label->setFont(label_font);
  wave_title_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_title_edit=new QLineEdit(this);
  wave_title_edit->setMaxLength(255);

  //
  // Artist
  //
  wave_artist_label=new QLabel(tr("Artist")+":",this);
  wave_artist_label->setFont(label_font);
  wave_artist_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_artist_edit=new QLineEdit(this);
  wave_artist_edit->setMaxLength(255);

  //
  // Year
  //
  wave_year_label=new QLabel(tr("Year")+":",this);
  wave_year_label->setFont(label_font);
  wave_year_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_year_edit=new QLineEdit(this);
  wave_year_edit->setMaxLength(4);
  wave_year_edit->setValidator(new QIntValidator(1980,8000,this));

  //
  // Usage
  //
  wave_usage_label=new QLabel(tr("Usage")+":",this);
  wave_usage_label->setFont(label_font);
  wave_usage_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_usage_box=new QComboBox(this);
  for(int i=0;i<(int)RDCart::UsageLast;i++) {
    wave_usage_box->insertItem(RDCart::usageText((RDCart::UsageCode)i));
  }

  //
  // Scheduler Codes
  //
  wave_sched_button=new QPushButton(tr("Scheduler Codes"),this);
  wave_sched_button->setFont(label_font);
  connect(wave_sched_button,SIGNAL(clicked()),this,SLOT(schedClickedData()));

  //
  // Song ID
  //
  wave_songid_label=new QLabel(tr("Song ID")+":",this);
  wave_songid_label->setFont(label_font);
  wave_songid_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_songid_edit=new QLineEdit(this);
  wave_songid_edit->setMaxLength(32);

  //
  // Beats Per Minute
  //
  wave_bpm_label=new QLabel(tr("Beats per Minute")+":",this);
  wave_bpm_label->setFont(label_font);
  wave_bpm_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_bpm_spin=new QSpinBox(this);
  wave_bpm_spin->setRange(0,300);
  wave_bpm_spin->setSpecialValueText(tr("Unknown"));

  //
  // Album
  //
  wave_album_label=new QLabel(tr("Album")+":",this);
  wave_album_label->setFont(label_font);
  wave_album_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_album_edit=new QLineEdit(this);
  wave_album_edit->setMaxLength(255);

  //
  // Label
  //
  wave_label_label=new QLabel(tr("Label")+":",this);
  wave_label_label->setFont(label_font);
  wave_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_label_edit=new QLineEdit(this);
  wave_label_edit->setMaxLength(64);

  //
  // Client
  //
  wave_client_label=new QLabel(tr("Client")+":",this);
  wave_client_label->setFont(label_font);
  wave_client_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_client_edit=new QLineEdit(this);
  wave_client_edit->setMaxLength(64);

  //
  // Agency
  //
  wave_agency_label=new QLabel(tr("Agency")+":",this);
  wave_agency_label->setFont(label_font);
  wave_agency_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_agency_edit=new QLineEdit(this);
  wave_agency_edit->setMaxLength(64);

  //
  // Publisher
  //
  wave_publisher_label=new QLabel(tr("Publisher")+":",this);
  wave_publisher_label->setFont(label_font);
  wave_publisher_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_publisher_edit=new QLineEdit(this);
  wave_publisher_edit->setMaxLength(64);

  //
  // Composer
  //
  wave_composer_label=new QLabel(tr("Composer")+":",this);
  wave_composer_label->setFont(label_font);
  wave_composer_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_composer_edit=new QLineEdit(this);
  wave_composer_edit->setMaxLength(64);

  //
  // Conductor
  //
  wave_conductor_label=new QLabel(tr("Conductor")+":",this);
  wave_conductor_label->setFont(label_font);
  wave_conductor_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_conductor_edit=new QLineEdit(this);
  wave_conductor_edit->setMaxLength(64);

  //
  // User Defined
  //
  wave_userdef_label=new QLabel(tr("User Defined")+":",this);
  wave_userdef_label->setFont(label_font);
  wave_userdef_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  wave_userdef_edit=new QLineEdit(this);
  wave_userdef_edit->setMaxLength(255);

  //
  // OK Button
  //
  wave_ok_button=new QPushButton(tr("OK"),this);
  wave_ok_button->setFont(label_font);
  connect(wave_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  wave_cancel_button=new QPushButton(tr("Cancel"),this);
  wave_cancel_button->setFont(label_font);
  connect(wave_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDWaveDataDialog::~RDWaveDataDialog()
{
  delete wave_schedcodes_dialog;
}


QSize RDWaveDataDialog::sizeHint() const
{
  return QSize(600,344);
}


QSizePolicy RDWaveDataDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDWaveDataDialog::exec(RDWaveData *data)
{
  wave_data=data;

  wave_title_edit->setText(wave_data->title());
  wave_artist_edit->setText(wave_data->artist());
  if(wave_data->releaseYear()>0) {
    wave_year_edit->setText(QString().sprintf("%d",wave_data->releaseYear()));
  }
  wave_usage_box->setCurrentItem(wave_data->usageCode());
  wave_songid_edit->setText(wave_data->tmciSongId());
  wave_bpm_spin->setValue(wave_data->beatsPerMinute());
  wave_album_edit->setText(wave_data->album());
  wave_label_edit->setText(wave_data->label());
  wave_client_edit->setText(wave_data->client());
  wave_agency_edit->setText(wave_data->agency());
  wave_publisher_edit->setText(wave_data->publisher());
  wave_composer_edit->setText(wave_data->composer());
  wave_conductor_edit->setText(wave_data->conductor());
  wave_userdef_edit->setText(wave_data->userDefined());

  return QDialog::exec();
}


void RDWaveDataDialog::schedClickedData()
{
  QStringList codes=wave_data->schedCodes();
  if(wave_schedcodes_dialog->exec(&codes,NULL)==0) {
    wave_data->setSchedCodes(codes);
  }
}


void RDWaveDataDialog::okData()
{
  wave_data->setTitle(wave_title_edit->text());
  wave_data->setArtist(wave_artist_edit->text());
  if(wave_year_edit->text().isEmpty()) {
    wave_data->setReleaseYear(0);
  }
  else {
    wave_data->setReleaseYear(wave_year_edit->text().toInt());
  }
  wave_data->setUsageCode(wave_usage_box->currentItem());
  wave_data->setTmciSongId(wave_songid_edit->text());
  wave_data->setBeatsPerMinute(wave_bpm_spin->value());
  wave_data->setAlbum(wave_album_edit->text());
  wave_data->setLabel(wave_label_edit->text());
  wave_data->setClient(wave_client_edit->text());
  wave_data->setAgency(wave_agency_edit->text());
  wave_data->setPublisher(wave_publisher_edit->text());
  wave_data->setComposer(wave_composer_edit->text());
  wave_data->setConductor(wave_conductor_edit->text());
  wave_data->setUserDefined(wave_userdef_edit->text());

  done(0);
}


void RDWaveDataDialog::cancelData()
{
  done(-1);
}


void RDWaveDataDialog::resizeEvent(QResizeEvent *e)
{
  wave_title_label->setGeometry(10,10,100,20);
  wave_title_edit->setGeometry(115,10,size().width()-125,20);
  wave_artist_label->setGeometry(10,32,100,20);
  wave_artist_edit->setGeometry(115,32,size().width()-125,20);
  wave_year_label->setGeometry(10,54,100,20);
  wave_year_edit->setGeometry(115,54,50,20);
  wave_usage_label->setGeometry(178,54,60,20);
  wave_usage_box->setGeometry(245,54,170,20);
  wave_sched_button->setGeometry(440,53,150,22);
  wave_songid_label->setGeometry(10,76,100,20);
  wave_songid_edit->setGeometry(115,76,240,20);
  wave_bpm_label->setGeometry(375,76,120,20);
  wave_bpm_spin->setGeometry(500,76,90,20);
  wave_album_label->setGeometry(10,98,100,20);
  wave_album_edit->setGeometry(115,98,size().width()-125,20);
  wave_label_label->setGeometry(10,120,100,20);
  wave_label_edit->setGeometry(115,120,size().width()-125,20);
  wave_client_label->setGeometry(10,142,100,20);
  wave_client_edit->setGeometry(115,142,size().width()-125,20);
  wave_agency_label->setGeometry(10,164,100,20);
  wave_agency_edit->setGeometry(115,164,size().width()-125,20);
  wave_publisher_label->setGeometry(10,186,100,20);
  wave_publisher_edit->setGeometry(115,186,size().width()-125,20);
  wave_composer_label->setGeometry(10,208,100,20);
  wave_composer_edit->setGeometry(115,208,size().width()-125,20);
  wave_conductor_label->setGeometry(10,230,100,20);
  wave_conductor_edit->setGeometry(115,230,size().width()-125,20);
  wave_userdef_label->setGeometry(10,252,100,20);
  wave_userdef_edit->setGeometry(115,252,size().width()-125,20);
  wave_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  wave_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void RDWaveDataDialog::closeEvent(QCloseEvent *e)
{
  cancelData();
}
