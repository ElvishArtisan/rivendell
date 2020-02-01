// cdripper.cpp
//
// CD Track Ripper Dialog for Rivendell.
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

#include <qmessagebox.h>

#include <rdaudioimport.h>
#include <rdcdripper.h>
#include <rdconf.h>
#include <rddisclookup_factory.h>
#include <rdtempdirectory.h>
#include <rdtextfile.h>

#include "cdripper.h"
#include "globals.h"
#include "rdconfig.h"

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
  rip_title_box->insertItem(tr("[none]"));

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
  // Other Edit
  //
  rip_other_label=new QLabel(tr("Other:"),this);
  rip_other_label->setFont(labelFont());
  rip_other_label->setAlignment(Qt::AlignRight);
  rip_other_edit=new Q3TextEdit(this);
  rip_other_edit->setReadOnly(true);

  //
  // Apply Metadata Check Box
  //
  rip_apply_box=new QCheckBox(this);
  rip_apply_box->setChecked(true);
  rip_apply_box->setDisabled(true);
  rip_apply_label=new QLabel(rip_apply_box,tr("Apply")+" "+
			     rip_disc_lookup->sourceName()+" "+
			     tr("Values to Cart"),this);
  rip_apply_label->setFont(labelFont());
  rip_apply_label->setAlignment(Qt::AlignLeft);
  rip_apply_box->setChecked(false);
  rip_apply_label->setDisabled(true);
  rip_apply_box->setVisible(!rip_disc_lookup->sourceName().isNull());
  rip_apply_label->setVisible(!rip_disc_lookup->sourceName().isNull());

  //
  // Web Browser Button
  //
  rip_browser_button=new QPushButton(this);
  rip_browser_button->setPixmap(rip_disc_lookup->sourceLogo());
  rip_browser_button->setDisabled(true);
  connect(rip_browser_button,SIGNAL(clicked()),this,SLOT(openBrowserData()));
  if(rip_disc_lookup->sourceLogo().isNull()) {
    rip_browser_button->hide();
  }

  //
  // Track List
  //
  rip_track_list=new RDListView(this);
  rip_track_list->setAllColumnsShowFocus(true);
  rip_track_list->setSelectionMode(Q3ListView::Extended);
  rip_track_list->setItemMargin(5);
  rip_track_list->setSorting(-1);
  connect(rip_track_list,SIGNAL(selectionChanged()),
	  this,SLOT(trackSelectionChangedData()));
  rip_track_label=new QLabel(rip_track_list,tr("Tracks"),this);
  rip_track_label->setFont(labelFont());
  rip_track_list->addColumn(tr("TRACK"));
  rip_track_list->setColumnAlignment(0,Qt::AlignHCenter);
  rip_track_list->addColumn(tr("LENGTH"));
  rip_track_list->setColumnAlignment(1,Qt::AlignRight);
  rip_track_list->addColumn(tr("TITLE"));
  rip_track_list->setColumnAlignment(2,Qt::AlignLeft);
  rip_track_list->addColumn(tr("OTHER"));
  rip_track_list->setColumnAlignment(3,Qt::AlignLeft);
  rip_track_list->addColumn(tr("TYPE"));
  rip_track_list->setColumnAlignment(4,Qt::AlignLeft);

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
  rip_rip_button=new QPushButton(tr("&Rip\nTrack"),this);
  rip_rip_button->setFont(buttonFont());
  rip_rip_button->setDisabled(true);
  connect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripTrackButtonData()));

  //
  // Normalize Check Box
  //
  rip_normalize_box=new QCheckBox(this);
  rip_normalize_box->setChecked(true);
  rip_normalize_box_label=new QLabel(rip_normalize_box,tr("Normalize"),this);
  rip_normalize_box_label->setFont(labelFont());
  rip_normalize_box_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(rip_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  rip_normalize_spin=new QSpinBox(this);
  rip_normalize_spin->setRange(-30,0);
  rip_normalize_label=new QLabel(rip_normalize_spin,tr("Level:"),this);
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
  rip_autotrim_box_label=new QLabel(rip_autotrim_box,tr("Autotrim"),this);
  rip_autotrim_box_label->setFont(labelFont());
  rip_autotrim_box_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(rip_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimCheckData(bool)));

  //
  // Autotrim Level
  //
  rip_autotrim_spin=new QSpinBox(this);
  rip_autotrim_spin->setRange(-99,0);
  rip_autotrim_label=new QLabel(rip_autotrim_spin,tr("Level:"),this);
  rip_autotrim_label->setFont(labelFont());
  rip_autotrim_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_autotrim_unit=new QLabel(tr("dBFS"),this);
  rip_autotrim_unit->setFont(labelFont());
  rip_autotrim_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Channels
  //
  rip_channels_box=new QComboBox(this);
  rip_channels_label=new QLabel(rip_channels_box,tr("Channels:"),this);
  rip_channels_label->setFont(labelFont());
  rip_channels_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Close Button
  //
  rip_close_button=new QPushButton("&Close",this);
  rip_close_button->setFont(buttonFont());
  connect(rip_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  rip_normalize_spin->setValue(rip_conf->ripperLevel()/100);
  rip_autotrim_spin->setValue(rip_conf->trimThreshold()/100);
  rip_channels_box->insertItem("1");
  rip_channels_box->insertItem("2");
  rip_channels_box->setCurrentItem(rip_conf->defaultChannels()-1);
  rip_done=false;
}


CdRipper::~CdRipper()
{
  rip_cdrom->close();
  delete rip_cdrom;
  delete rip_track_list;
  delete rip_rip_button;
  delete rip_close_button;
  delete rip_eject_button;
  delete rip_play_button;
  delete rip_stop_button;
  delete rip_bar;
}


QSize CdRipper::sizeHint() const
{
  return QSize(730,606);
}


QSizePolicy CdRipper::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int CdRipper::exec(QString *title,QString *artist,QString *album)
{
  rip_title=title;
  rip_artist=artist;
  rip_album=album;
  return QDialog::exec();
}


void CdRipper::trackSelectionChangedData()
{
  Q3ListViewItem *item=rip_track_list->firstChild();
  QStringList titles;

  while(item!=NULL) {
    if(item->isSelected()) {
      if(item->text(4)==tr("Data Track")) {
	rip_rip_button->setDisabled(true);
	return;
      }
      titles.push_back(item->text(2));
    }
    item=item->nextSibling();
  }
  rip_title_box->clear();
  switch(titles.size()) {
  case 0:
    rip_title_box->insertItem(tr("[none]"));
    break;

  case 1:
    rip_title_box->insertItem(titles[0]);
    break;

  default:
    rip_title_box->insertItem(titles.join(" / "));
    for(int i=0;i<titles.size();i++) {
      rip_title_box->insertItem(titles[i]);
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
  if(rip_track_list->currentItem()!=NULL) {
    rip_cdrom->play(rip_track_list->currentItem()->text(0).toInt());
    rip_play_button->on();
    rip_stop_button->off();
  }
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
  Q3ListViewItem *item=rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      if(rip_track[0]<0) {
	rip_track[0]=item->text(0).toInt()-1;
      }
      rip_track[1]=item->text(0).toInt()-1;
    }
    item=item->nextSibling();
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
  unlink(tmpfile);
  rmdir(tmpdir);
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
  rip_track_list->clear();
  rip_artist_edit->clear();
  rip_album_edit->clear();
  rip_other_edit->clear();
  rip_apply_box->setChecked(false);
  rip_apply_box->setDisabled(true);
  rip_apply_label->setDisabled(true);
}


void CdRipper::mediaChangedData()
{
  Q3ListViewItem *l;

  rip_track_list->clear();
  rip_track[0]=-1;
  rip_track[1]=-1;
  for(int i=rip_cdrom->tracks();i>0;i--) {
    l=new Q3ListViewItem(rip_track_list);
    l->setText(0,QString().sprintf("%d",i));
    if(rip_cdrom->isAudio(i)) {
      l->setText(4,tr("Audio Track"));
    }
    else {
      l->setText(4,tr("Data Track"));
    }
    l->setText(1,RDGetTimeLength(rip_cdrom->trackLength(i)));
  }
  rip_disc_record->clear();
  rip_cdrom->setCddbRecord(rip_disc_record);
  rip_disc_lookup->setCddbRecord(rip_disc_record);
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
  switch(result) {
  case RDDiscLookup::ExactMatch:
    if(rip_cdrom->status()!=RDCdPlayer::Ok) {
      return;
    }
    rip_artist_edit->setText(rip_disc_record->discArtist());
    rip_album_edit->setText(rip_disc_record->discAlbum());
    rip_other_edit->setText(rip_disc_record->discExtended());
    for(int i=0;i<rip_disc_record->tracks();i++) {
      rip_track_list->findItem(QString().sprintf("%d",i+1),0)->
	setText(2,rip_disc_record->trackTitle(i));
      rip_track_list->findItem(QString().sprintf("%d",i+1),0)->
	setText(3,rip_disc_record->trackExtended(i));
    }
    rip_apply_box->setChecked(true);
    rip_apply_box->setEnabled(true);
    rip_apply_label->setEnabled(true);
    rip_browser_button->setDisabled(rip_disc_lookup->sourceUrl().isNull());
    trackSelectionChangedData();
    break;

  case RDDiscLookup::NoMatch:
    rip_track[0]=-1;
    rip_track[1]=-1;
    break;

  case RDDiscLookup::LookupError:
    QMessageBox::warning(this,"RDLibrary - "+rip_disc_lookup->sourceName()+
			 " "+tr("Lookup Error"),err_msg);
    rip_track[0]=-1;
    rip_track[1]=-1;
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
  rip_other_label->setGeometry(10,76,50,16);
  rip_other_edit->setGeometry(65,75,size().width()-125,60);
  rip_apply_box->setGeometry(65,140,15,15);
  rip_apply_label->setGeometry(85,140,250,20);
  rip_browser_button->setGeometry(size().width()-260,139,200,35);
  rip_track_list->setGeometry(10,178,size().width()-110,size().height()-290);
  rip_track_label->setGeometry(10,162,100,14);
  rip_bar->setGeometry(10,size().height()-100,size().width()-112,20);
  rip_eject_button->setGeometry(size().width()-90,178,80,50);
  rip_play_button->setGeometry(size().width()-90,238,80,50);
  rip_stop_button->setGeometry(size().width()-90,298,80,50);
  rip_rip_button->setGeometry(size().width()-90,402,80,50);
  rip_normalize_box->setGeometry(10,size().height()-76,20,20);
  rip_normalize_box_label->setGeometry(30,size().height()-76,85,20);
  rip_normalize_spin->setGeometry(170,size().height()-76,40,20);
  rip_normalize_label->setGeometry(120,size().height()-76,45,20);
  rip_normalize_unit->setGeometry(215,size().height()-76,40,20);
  rip_autotrim_box->setGeometry(10,size().height()-52,20,20);
  rip_autotrim_box_label->setGeometry(30,size().height()-52,85,20);
  rip_autotrim_spin->setGeometry(170,size().height()-52,40,20);
  rip_autotrim_label->setGeometry(120,size().height()-52,45,20);
  rip_autotrim_unit->setGeometry(215,size().height()-52,40,20);
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
	    (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	   (const char *)msg.utf8());
  }
}
