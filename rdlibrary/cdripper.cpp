// cdripper.cpp
//
// CD Track Ripper Dialog for Rivendell.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include <rdaudioimport.h>
#include <rdcdripper.h>
#include <rddisclookup_factory.h>
#include <rdtempdirectory.h>
#include <rdtextfile.h>

#include "cdripper.h"
#include "globals.h"

//
// Global Variables
//
bool ripper_running;

CdRipper::CdRipper(QString cutname,RDDiscRecord *rec,RDLibraryConf *conf,
		   bool profile_rip,QWidget *parent) 
  : RDDialog(parent)
{
  rip_profile_rip=profile_rip;
  rip_conf=conf;
  rip_disc_record=rec;
  rip_track[0]=-1;
  rip_track[1]=-1;
  rip_title=NULL;

  setWindowTitle("RDLibrary - "+tr("Rip CD"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Target Cut
  //
  rip_cut=new RDCut(cutname);

  //
  // The CDROM Drive
  //
  if(rip_profile_rip) {
    rip_cdrom=new RDCdPlayer(stdout,this);
  }
  else {
    rip_cdrom=new RDCdPlayer(NULL,this);
  }
  connect(rip_cdrom,SIGNAL(ejected()),this,SLOT(ejectedData()));
  connect(rip_cdrom,SIGNAL(mediaChanged()),this,SLOT(mediaChangedData()));
  connect(rip_cdrom,SIGNAL(played(int)),this,SLOT(playedData(int)));
  connect(rip_cdrom,SIGNAL(stopped()),this,SLOT(stoppedData()));
  rip_cdrom->setDevice(rip_conf->ripperDevice());
  rip_cdrom->open();

  //
  // Disc Metadata Lookup
  //
  if(rip_profile_rip) {
    rip_disc_lookup=RDDiscLookupFactory(rda->libraryConf()->cdServerType(),
					"RDLibrary",stdout,this);
  }
  else {
    rip_disc_lookup=RDDiscLookupFactory(rda->libraryConf()->cdServerType(),
					"RDLibrary",NULL,this);
  }
  connect(rip_disc_lookup,SIGNAL(lookupDone(RDDiscLookup::Result,const QString &)),
	  this,SLOT(lookupDoneData(RDDiscLookup::Result,const QString &)));

  //
  // Title Selector
  //
  rip_title_label=new QLabel(tr("Title:"),this);
  rip_title_label->setFont(labelFont());
  rip_title_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_title_box=new QComboBox(this);
  rip_title_box->insertItem(0,tr("[none]"));

  //
  // Artist Label
  //
  rip_artist_label=new QLabel(tr("Artist:"),this);
  rip_artist_label->setFont(labelFont());
  rip_artist_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_artist_edit=new QLineEdit(this);

  //
  // Album Edit
  //
  rip_album_label=new QLabel(tr("Album:"),this);
  rip_album_label->setFont(labelFont());
  rip_album_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_album_edit=new QLineEdit(this);

  //
  // Label Edit
  //
  rip_label_label=new QLabel(tr("Label:"),this);
  rip_label_label->setFont(labelFont());
  rip_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_label_edit=new QLineEdit(this);

  //
  // Other Edit
  //
  rip_other_label=new QLabel(tr("Other:"),this);
  rip_other_label->setFont(labelFont());
  rip_other_label->setAlignment(Qt::AlignRight);
  rip_other_edit=new QTextEdit(this);
  rip_other_edit->setReadOnly(true);

  //
  // Apply Metadata Check Box
  //
  rip_apply_box=new QCheckBox(this);
  rip_apply_box->setChecked(true);
  rip_apply_box->setDisabled(true);
  rip_apply_label=new QLabel(tr("Apply Metadata Values to Cart"),this);
  rip_apply_label->setFont(labelFont());
  rip_apply_label->setAlignment(Qt::AlignLeft);
  rip_apply_box->setChecked(false);
  rip_apply_label->setDisabled(true);
  rip_apply_box->setVisible(!rip_disc_lookup->sourceName().isNull());
  rip_apply_label->setVisible(!rip_disc_lookup->sourceName().isNull());

  //
  // Data Source Labels
  //
  rip_cdtext_label=new QLabel(this);
  rip_cdtext_label->setFrameStyle(QFrame::Panel|QFrame::Raised);
  rip_cdtext_label->
    setPixmap(RDLibraryConf::cdServerLogo(RDLibraryConf::DummyType));
  rip_cdtext_label->hide();
  rip_cddb_label=new QLabel(this);
  rip_cddb_label->setFrameStyle(QFrame::Panel|QFrame::Raised);
  rip_cddb_label->
    setPixmap(RDLibraryConf::cdServerLogo(RDLibraryConf::CddbType));
  rip_cddb_label->hide();

  //
  // Web Browser Button
  //
  rip_browser_button=new QPushButton(this);
  rip_browser_button->setIcon(rip_disc_lookup->sourceLogo());
  rip_browser_button->setDisabled(true);
  rip_browser_label=new QLabel(this);
  rip_browser_label->setPixmap(rip_disc_lookup->sourceLogo());
  rip_browser_label->setDisabled(true);
  connect(rip_browser_button,SIGNAL(clicked()),this,SLOT(openBrowserData()));
  if(rip_disc_lookup->sourceLogo().isNull()) {
    rip_browser_button->hide();
    rip_browser_label->hide();
  }
  else {
    if(rda->station()->browserPath().isEmpty()) {
      rip_browser_button->hide();
    }
    else {
      rip_browser_label->hide();
    }
  }

  //
  // Track List
  //
  rip_track_view=new RDTableView(this);
  rip_track_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  rip_track_model=new RDDiscModel(this);
  rip_track_model->setFont(defaultFont());
  rip_track_model->setPalette(palette());
  rip_track_view->setModel(rip_track_model);
  rip_track_view->hideColumn(5);
  rip_track_label=new QLabel(tr("Tracks"),this);
  rip_track_label->setFont(bigLabelFont());
  connect(rip_track_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,
				  const QItemSelection &)),
	  this,
	  SLOT(trackSelectionChangedData(const QItemSelection &,
					 const QItemSelection &)));
  connect(rip_track_model,SIGNAL(modelReset()),
	  rip_track_view,SLOT(resizeColumnsToContents()));
  rip_track_view->resizeColumnsToContents();

  //
  // Progress Bar
  //
  rip_bar=new QProgressBar(this);

  //
  // Eject Button
  //
  rip_eject_button=new RDTransportButton(RDTransportButton::Eject,this);
  connect(rip_eject_button,SIGNAL(clicked()),this,SLOT(ejectButtonData()));
  
  //
  // Play Button
  //
  rip_play_button=new RDTransportButton(RDTransportButton::Play,this);
  connect(rip_play_button,SIGNAL(clicked()),this,SLOT(playButtonData()));
  
  //
  // Stop Button
  //
  rip_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  rip_stop_button->setOnColor(Qt::red);
  rip_stop_button->on();
  connect(rip_stop_button,SIGNAL(clicked()),this,SLOT(stopButtonData()));
  
  //
  // Rip Track Button
  //
  rip_rip_button=new QPushButton(tr("Rip\nTrack"),this);
  rip_rip_button->setFont(buttonFont());
  rip_rip_button->setDisabled(true);
  connect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripTrackButtonData()));

  //
  // Normalize Check Box
  //
  rip_normalize_box=new QCheckBox(this);
  rip_normalize_box->setChecked(true);
  rip_normalize_box_label=new QLabel(tr("Normalize"),this);
  rip_normalize_box_label->setFont(labelFont());
  rip_normalize_box_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(rip_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  rip_normalize_spin=new QSpinBox(this);
  rip_normalize_spin->setRange(-30,0);
  rip_normalize_label=new QLabel(tr("Level:"),this);
  rip_normalize_label->setFont(labelFont());
  rip_normalize_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_normalize_unit=new QLabel(tr("dBFS"),this);
  rip_normalize_unit->setFont(labelFont());
  rip_normalize_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Autotrim Check Box
  //
  rip_autotrim_box=new QCheckBox(this);
  rip_autotrim_box->setChecked(true);
  rip_autotrim_box_label=new QLabel(tr("Autotrim"),this);
  rip_autotrim_box_label->setFont(labelFont());
  rip_autotrim_box_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(rip_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimCheckData(bool)));

  //
  // Autotrim Level
  //
  rip_autotrim_spin=new QSpinBox(this);
  rip_autotrim_spin->setRange(-99,0);
  rip_autotrim_label=new QLabel(tr("Level:"),this);
  rip_autotrim_label->setFont(labelFont());
  rip_autotrim_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_autotrim_unit=new QLabel(tr("dBFS"),this);
  rip_autotrim_unit->setFont(labelFont());
  rip_autotrim_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Channels
  //
  rip_channels_box=new QComboBox(this);
  rip_channels_label=new QLabel(tr("Channels:"),this);
  rip_channels_label->setFont(labelFont());
  rip_channels_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Close Button
  //
  rip_close_button=new QPushButton("Close",this);
  rip_close_button->setFont(buttonFont());
  connect(rip_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  rip_normalize_spin->setValue(rip_conf->ripperLevel()/100);
  rip_autotrim_spin->setValue(rip_conf->trimThreshold()/100);
  rip_channels_box->insertItem(0,"1");
  rip_channels_box->insertItem(1,"2");
  rip_channels_box->setCurrentIndex(rip_conf->defaultChannels()-1);
  rip_done=false;
}


CdRipper::~CdRipper()
{
  rip_cdrom->close();
  delete rip_cdrom;
  delete rip_track_view;
  delete rip_track_model;
  delete rip_rip_button;
  delete rip_close_button;
  delete rip_eject_button;
  delete rip_play_button;
  delete rip_stop_button;
  delete rip_bar;
}


QSize CdRipper::sizeHint() const
{
  return QSize(730,628);
}


QSizePolicy CdRipper::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int CdRipper::exec(QString *title,QString *artist,QString *album,QString *label)
{
  rip_title=title;
  rip_artist=artist;
  rip_album=album;
  rip_label=label;
  return QDialog::exec();
}


void CdRipper::trackSelectionChangedData(const QItemSelection &before,
					 const QItemSelection &after)
{
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();
  QStringList titles;
  for(int i=0;i<rows.size();i++) {
    if(rip_track_model->trackContainsData(rows.at(i))) {
      break;
    }
    titles.push_back(rip_track_model->trackTitle(rows.at(i)));
  }
  
  rip_rip_button->setDisabled((rows.size()==0)||
			      rip_track_model->trackContainsData(rows.first()));
  rip_title_box->clear();
  switch(titles.size()) {
  case 0:
    rip_title_box->insertItem(0,tr("[none]"));
    break;

  case 1:
    rip_title_box->insertItem(0,titles[0]);
    break;

  default:
    rip_title_box->insertItem(0,titles.join(" / "));
    for(int i=0;i<titles.size();i++) {
      rip_title_box->insertItem(rip_title_box->count(),titles[i]);
    }
    break;
  }
  rip_rip_button->setEnabled(titles.size()>0);
}


void CdRipper::ejectButtonData()
{
  rip_cdrom->eject();
}


void CdRipper::playButtonData()
{
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  rip_cdrom->play(rows.first().row()+1);
  rip_play_button->on();
  rip_stop_button->off();
}


void CdRipper::stopButtonData()
{
  rip_cdrom->stop();
  rip_play_button->off();
  rip_stop_button->on();
}


void CdRipper::ripTrackButtonData()
{
  RDCdRipper *ripper=NULL;
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();

  rip_done=false;
  rip_rip_aborted=false;
  if(rip_cut->length()>0) {
    switch(QMessageBox::warning(this,tr("Audio Exists"),
				tr("This will overwrite the existing recording.\nDo you want to proceed?"),
				QMessageBox::Yes,QMessageBox::No)) {
    case QMessageBox::No:
    case Qt::NoButton:
      return;
      
    default:
      break;
    }
  }
  if(cut_clipboard!=NULL) {
    if(rip_cut->cutName()==cut_clipboard->cutName()) {
      switch(QMessageBox::warning(this,tr("Empty Clipboard"),
				  tr("Ripping this cut will also empty the clipboard.\nDo you still want to proceed?"),
				  QMessageBox::Yes,
				  QMessageBox::No)) {
      case QMessageBox::No:
      case Qt::NoButton:
	return;

      default:
	break;
      }
      delete cut_clipboard;
      cut_clipboard=NULL;
    }
  }
  rip_eject_button->setDisabled(true);
  rip_play_button->setDisabled(true);
  rip_stop_button->setDisabled(true);
  rip_rip_button->setText(tr("Abort\nRip"));
  rip_close_button->setDisabled(true);

  //
  // Set Track Title
  //
  if(rip_apply_box->isChecked()) {
    *rip_title=rip_title_box->currentText();
    *rip_artist=rip_artist_edit->text();
    *rip_album=rip_album_edit->text();
    *rip_label=rip_label_edit->text();
  }

  //
  // Rip from disc
  //
  RDAudioImport::ErrorCode conv_err;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDCdRipper::ErrorCode ripper_err;
  QString tmpdir=RDTempDirectory::basePath();
  QString tmpfile=tmpdir+"/"+RIPPER_TEMP_WAV;
  if(rip_profile_rip) {
    ripper=new RDCdRipper(stdout,this);
  }
  else {
    ripper=new RDCdRipper(NULL,this);
  }
  disconnect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripTrackButtonData()));
  connect(rip_rip_button,SIGNAL(clicked()),ripper,SLOT(abort()));
  rip_bar->setMaximum(ripper->totalSteps()+1);
  connect(ripper,SIGNAL(progressChanged(int)),rip_bar,SLOT(setValue(int)));
  RDAudioImport *conv=NULL;
  RDSettings *settings=NULL;
  ripper->setDevice(rip_conf->ripperDevice());
  ripper->setDestinationFile(tmpfile);

  rip_track[0]=-1;
  rip_track[1]=-1;
  for(int i=0;i<rows.size();i++) {
    if(rip_track[0]<0) {
      rip_track[0]=rows.at(i).row();
    }
    rip_track[1]=rows.at(i).row();
  }

  switch((ripper_err=ripper->rip(rip_track[0],rip_track[1]))) {
  case RDCdRipper::ErrorOk:
    conv=new RDAudioImport(this);
    conv->setSourceFile(tmpfile);
    conv->setCartNumber(rip_cut->cartNumber());
    conv->setCutNumber(rip_cut->cutNumber());
    conv->setUseMetadata(false);
    settings=new RDSettings();
    if(rda->libraryConf()->defaultFormat()==1) {
      settings->setFormat(RDSettings::MpegL2Wav);
    }
    else {
      settings->setFormat(RDSettings::Pcm16);
    }
    settings->setChannels(rip_channels_box->currentText().toInt());
    settings->setSampleRate(rda->system()->sampleRate());
    settings->setBitRate(rda->libraryConf()->defaultBitrate());
    if(rip_normalize_box->isChecked()) {
      settings->setNormalizationLevel(rip_normalize_spin->value());
    }
    if(rip_autotrim_box->isChecked()) {
      settings->setAutotrimLevel(rip_autotrim_spin->value());
    }
    conv->setDestinationSettings(settings);
    switch((conv_err=conv->
	  runImport(rda->user()->name(),rda->user()->password(),&audio_conv_err))) {
    case RDAudioImport::ErrorOk:
      QMessageBox::information(this,tr("Rip Complete"),tr("Rip complete!"));
      break;

    default:
      QMessageBox::warning(this,tr("RDLibrary - Importer Error"),
			   RDAudioImport::errorText(conv_err,audio_conv_err));
      break;
    }
    delete settings;
    delete conv;
    break;

  case RDCdRipper::ErrorNoDevice:
  case RDCdRipper::ErrorNoDestination:
  case RDCdRipper::ErrorInternal:
  case RDCdRipper::ErrorNoDisc:
  case RDCdRipper::ErrorNoTrack:
  case RDCdRipper::ErrorAborted:
    QMessageBox::warning(this,tr("RDLibrary - Ripper Error"),
			 RDCdRipper::errorText(ripper_err));
    break;
  }
  delete ripper;
  unlink(tmpfile.toUtf8());
  rmdir(tmpdir.toUtf8());
  rip_bar->reset();
  rip_eject_button->setEnabled(true);
  rip_play_button->setEnabled(true);
  rip_stop_button->setEnabled(true);
  rip_rip_button->setText(tr("Rip\nTrack"));
  connect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripTrackButtonData()));
  rip_close_button->setEnabled(true);
  rip_cdrom->unlock();
  rip_done=true;
  rip_bar->setValue(0);
}


void CdRipper::ejectedData()
{
  rip_track_model->clear();
  rip_browser_button->setDisabled(true);
  rip_cdtext_label->hide();
  rip_cddb_label->hide();
  rip_artist_edit->clear();
  rip_album_edit->clear();
  rip_other_edit->clear();
  rip_apply_box->setChecked(false);
  rip_apply_box->setDisabled(true);
  rip_apply_label->setDisabled(true);
}


void CdRipper::mediaChangedData()
{
  rip_track_model->clear();
  rip_track[0]=-1;
  rip_track[1]=-1;
  rip_disc_record->clear();
  rip_cdrom->setCddbRecord(rip_disc_record);
  rip_disc_lookup->setCddbRecord(rip_disc_record);
  rip_track_model->setDisc(rip_cdrom);
  Profile("starting metadata lookup");
  rip_disc_lookup->lookup();
  Profile("metadata lookup finished");
}


void CdRipper::playedData(int track)
{
  rip_play_button->on();
  rip_stop_button->off();
}


void CdRipper::stoppedData()
{
  rip_play_button->off();
  rip_stop_button->on();
}


void CdRipper::lookupDoneData(RDDiscLookup::Result result,const QString &err_msg)
{
  RDDiscRecord::DataSource src=RDDiscRecord::LastSource;
  if(rip_disc_record->hasData(RDDiscRecord::LocalSource)) {
    src=RDDiscRecord::LocalSource;
  }
  else {
    if(rip_disc_record->hasData(RDDiscRecord::RemoteSource)) {
      src=RDDiscRecord::RemoteSource;
    }
    else {
      rip_apply_box->hide();
      rip_apply_label->hide();
      rip_track[0]=-1;
      rip_track[1]=-1;
      rip_cdtext_label->hide();
      rip_cddb_label->hide();
      return;  // Apply no metadata
    }
  }

  switch(result) {
  case RDDiscLookup::ExactMatch:
    if(rip_cdrom->status()!=RDCdPlayer::Ok) {
      rip_apply_box->hide();
      rip_apply_label->hide();
      rip_track[0]=-1;
      rip_track[1]=-1;
      rip_cdtext_label->hide();
      rip_cddb_label->hide();
      return;
    }
    rip_artist_edit->setText(rip_disc_record->discArtist(src));
    rip_album_edit->setText(rip_disc_record->discAlbum(src));
    rip_label_edit->setText(rip_disc_record->discLabel());
    rip_other_edit->setText(rip_disc_record->discExtended());
    rip_track_model->refresh(rip_disc_record,src);
    rip_apply_box->setChecked(true);
    rip_apply_box->setEnabled(true);
    rip_apply_label->setEnabled(true);
    if(rip_disc_lookup->hasCdText()) {
      rip_cdtext_label->show();
      rip_cddb_label->hide();
    }
    else {
      rip_cdtext_label->hide();
      if(rip_disc_lookup->sourceName()=="CDDB") {
	rip_cddb_label->show();
      }
    }
    rip_browser_button->setDisabled(rip_disc_lookup->sourceUrl().isNull());
    rip_browser_label->setDisabled(rip_disc_lookup->sourceUrl().isNull());
    rip_apply_box->show();
    rip_apply_label->show();
    trackSelectionChangedData(QItemSelection(),QItemSelection());
    break;

  case RDDiscLookup::NoMatch:
    rip_apply_box->hide();
    rip_apply_label->hide();
    rip_track[0]=-1;
    rip_track[1]=-1;
    rip_cdtext_label->hide();
    rip_cddb_label->hide();
    break;

  case RDDiscLookup::LookupError:
    QMessageBox::warning(this,"RDLibrary - "+rip_disc_lookup->sourceName()+
			 " "+tr("Lookup Error"),err_msg);
    rip_apply_box->hide();
    rip_apply_label->hide();
    rip_track[0]=-1;
    rip_track[1]=-1;
    rip_cdtext_label->hide();
    rip_cddb_label->hide();
    break;
  }
}


void CdRipper::normalizeCheckData(bool state)
{
  rip_normalize_spin->setEnabled(state);
  rip_normalize_label->setEnabled(state);
  rip_normalize_unit->setEnabled(state);
}


void CdRipper::autotrimCheckData(bool state)
{
  rip_autotrim_spin->setEnabled(state);
  rip_autotrim_label->setEnabled(state);
  rip_autotrim_unit->setEnabled(state);
}


void CdRipper::openBrowserData()
{
  RDWebBrowser(rip_disc_lookup->sourceUrl());
}


void CdRipper::closeData()
{
  if(rip_done) {
    done(rip_track[0]);
  }
  else {
    done(-1);
  }
}


void CdRipper::resizeEvent(QResizeEvent *e)
{
  rip_title_label->setGeometry(10,10,50,18);
  rip_title_box->setGeometry(65,9,size().width()-125,18);
  rip_artist_label->setGeometry(10,32,50,18);
  rip_artist_edit->setGeometry(65,31,size().width()-125,18);
  rip_album_label->setGeometry(10,54,50,18);
  rip_album_edit->setGeometry(65,53,size().width()-125,18);
  rip_label_label->setGeometry(10,76,50,18);
  rip_label_edit->setGeometry(65,75,size().width()-125,18);
  rip_other_label->setGeometry(10,98,50,16);
  rip_other_edit->setGeometry(65,97,size().width()-125,60);
  rip_apply_box->setGeometry(65,162,15,15);
  rip_apply_label->setGeometry(85,162,250,20);


  if(rip_disc_lookup->sourceName()=="MusicBrainz") {
    rip_cdtext_label->setGeometry(size().width()-270-rip_cdtext_label->sizeHint().width(),161,rip_cdtext_label->sizeHint().width(),rip_cdtext_label->sizeHint().height());
    rip_cddb_label->setGeometry(size().width()-270-rip_cddb_label->sizeHint().width(),161,rip_cddb_label->sizeHint().width(),rip_cddb_label->sizeHint().height());
  }
  else {
    rip_cdtext_label->setGeometry(size().width()-270-rip_cdtext_label->sizeHint().width()+210,
                                 161,
                                 rip_cdtext_label->sizeHint().width(),
                                 rip_cdtext_label->sizeHint().height());
    rip_cddb_label->setGeometry(size().width()-270-rip_cddb_label->sizeHint().width()+210,
                               161,
                               rip_cddb_label->sizeHint().width(),
                               rip_cddb_label->sizeHint().height());
  }



  rip_browser_button->setGeometry(size().width()-260,161,200,35);
  rip_browser_button->setIconSize(QSize(198,33));
  rip_browser_label->setGeometry(size().width()-260,161,200,35);
  rip_track_view->setGeometry(10,200+12,size().width()-110,size().height()-305-12);
  rip_track_label->setGeometry(10,184+12,100,14);
  rip_bar->setGeometry(10,size().height()-100,size().width()-110,20);
  rip_eject_button->setGeometry(size().width()-90,200+12,80,50);
  rip_play_button->setGeometry(size().width()-90,260+12,80,50);
  rip_stop_button->setGeometry(size().width()-90,320+12,80,50);
  rip_rip_button->setGeometry(size().width()-90,424+12,80,50);
  rip_normalize_box->setGeometry(10,size().height()-76,20,20);
  rip_normalize_box_label->setGeometry(30,size().height()-76,85,20);
  rip_normalize_label->setGeometry(120,size().height()-76,45,20);
  rip_normalize_spin->setGeometry(170,size().height()-76,50,20);
  rip_normalize_unit->setGeometry(225,size().height()-76,40,20);
  rip_autotrim_box_label->setGeometry(30,size().height()-52,85,20);
  rip_autotrim_box->setGeometry(10,size().height()-52,20,20);
  rip_autotrim_label->setGeometry(120,size().height()-52,45,20);
  rip_autotrim_spin->setGeometry(170,size().height()-52,50,20);
  rip_autotrim_unit->setGeometry(225,size().height()-52,40,20);
  rip_channels_box->setGeometry(90,size().height()-28,50,20);
  rip_channels_label->setGeometry(10,size().height()-28,75,20);
  rip_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void CdRipper::closeEvent(QCloseEvent *e)
{
  if(!ripper_running) {
    closeData();
  }
}


void CdRipper::Profile(const QString &msg)
{
  if(rip_profile_rip) {
    printf("%s | CdRipper::%s\n",
	   QTime::currentTime().toString("hh:mm:ss.zzz").toUtf8().constData(),
	   msg.toUtf8().constData());
  }
}
