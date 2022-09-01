// disk_ripper.cpp
//
// CD Disk Ripper Dialog for Rivendell.
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

#include <QApplication>
#include <QList>
#include <QMessageBox>

#include <rdaudioimport.h>
#include <rdcdripper.h>
#include <rdconf.h>
#include <rddisclookup_factory.h>
#include <rdgroup.h>
#include <rdlist_groups.h>
#include <rdtempdirectory.h>
#include <rdtextfile.h>

#include "disk_ripper.h"
#include "globals.h"

DiskRipper::DiskRipper(QString *filter,QString *group,QString *schedcode,
		       bool profile_rip,QWidget *parent) 
  : RDDialog(parent)
{
  rip_filter_text=filter;
  rip_group_text=group;
  rip_schedcode_text=schedcode;
  rip_profile_rip=profile_rip;
  rip_aborting=false;

  setWindowTitle("RDLibrary - "+tr("Rip Disk"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Create Dialogs
  //
  rip_wavedata_dialog=new RDWaveDataDialog("RDLibrary",this);
  rip_cut_dialog=new RDCutDialog(rip_filter_text,rip_group_text,
				 rip_schedcode_text,true,true,true,"RDLibrary",
				 false,this);

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
  rip_cdrom->setDevice(rda->libraryConf()->ripperDevice());
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
  connect(rip_disc_lookup,
	  SIGNAL(lookupDone(RDDiscLookup::Result,const QString &)),
	  this,SLOT(lookupDoneData(RDDiscLookup::Result,const QString &)));

  //
  // Artist Label
  //
  QLabel *label=new QLabel(tr("Artist:"),this);
  label->setGeometry(10,10,50,18);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_artist_edit=new QLineEdit(this);

  //
  // Album Edit
  //
  label=new QLabel(tr("Album:"),this);
  label->setGeometry(10,32,50,18);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  rip_album_edit=new QLineEdit(this);

  //
  // Other Edit
  //
  label=new QLabel(tr("Other:"),this);
  label->setGeometry(10,54,50,16);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight);
  rip_other_edit=new QTextEdit(this);
  rip_other_edit->setReadOnly(true);

  //
  // Apply Lookup Data Check Box
  //
  rip_apply_box=new QCheckBox(this);
  rip_apply_box->setChecked(true);
  rip_apply_box->setDisabled(true);
  rip_apply_label=new QLabel(tr("Apply Metadata Values to Carts"),this);
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
  // Web Browser Button/Label
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
  rip_track_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  rip_track_model=new RDDiscModel(this);
  rip_track_model->setFont(defaultFont());
  rip_track_model->setPalette(palette());
  rip_track_view->setModel(rip_track_model);
  rip_track_label=new QLabel(tr("Tracks"),this);
  rip_track_label->setFont(bigLabelFont());
  connect(rip_track_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(rip_track_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
	  this,
	  SLOT(selectionChangedData(const QItemSelection &,const QItemSelection &)));
  connect(rip_track_model,SIGNAL(modelReset()),
	  rip_track_view,SLOT(resizeColumnsToContents()));
  rip_track_view->resizeColumnsToContents();

  //
  // Progress Bars
  //
  rip_disk_bar=new QProgressBar(this);
  rip_diskbar_label=new QLabel(tr("Disk Progress"),this);
  rip_diskbar_label->setFont(labelFont());
  rip_diskbar_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  rip_diskbar_label->setDisabled(true);
  rip_track_bar=new QProgressBar(this);
  rip_trackbar_label=new QLabel(tr("Track Progress"),this);
  rip_trackbar_label->setFont(labelFont());
  rip_trackbar_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  rip_trackbar_label->setDisabled(true);

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
  // Set Cut Button
  //
  rip_setcut_button=new QPushButton(tr("Set\nCart/Cut"),this);
  rip_setcut_button->setFont(buttonFont());
  rip_setcut_button->setDisabled(true);
  connect(rip_setcut_button,SIGNAL(clicked()),this,SLOT(setCutButtonData()));

  //
  // Set Multi Tracks Button
  //
  rip_setall_button=new QPushButton(tr("Add Cart\nPer Track"),this);
  rip_setall_button->setFont(buttonFont());
  rip_setall_button->setDisabled(true);
  connect(rip_setall_button,SIGNAL(clicked()),this,SLOT(setMultiButtonData()));

  //
  // Set Single Button
  //
  rip_setsingle_button=new QPushButton(tr("Add Single\nCart"),this);
  rip_setsingle_button->setFont(buttonFont());
  rip_setsingle_button->setDisabled(true);
  connect(rip_setsingle_button,SIGNAL(clicked()),
	  this,SLOT(setSingleButtonData()));

  //
  // Set Cart Label Button
  //
  rip_cartlabel_button=new QPushButton(tr("Modify\nCart Label"),this);
  rip_cartlabel_button->setFont(buttonFont());
  rip_cartlabel_button->setDisabled(true);
  connect(rip_cartlabel_button,SIGNAL(clicked()),
	  this,SLOT(modifyCartLabelData()));

  //
  // Clear Selection Button
  //
  rip_clear_button=new QPushButton(tr("Clear\nSelection"),this);
  rip_clear_button->setFont(buttonFont());
  rip_clear_button->setDisabled(true);
  connect(rip_clear_button,SIGNAL(clicked()),this,SLOT(clearSelectionData()));

  //
  // Normalize Check Box
  //
  rip_normalize_box=new QCheckBox(this);
  rip_normalize_box->setChecked(true);
  rip_normalizebox_label=new QLabel(tr("Normalize"),this);
  rip_normalizebox_label->setFont(labelFont());
  rip_normalizebox_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
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
  rip_autotrimbox_label=new QLabel(tr("Autotrim"),this);
  rip_autotrimbox_label->setFont(labelFont());
  rip_autotrimbox_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
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
  // Rip Disc Button
  //
  rip_rip_button=new QPushButton(tr("Rip\nDisc"),this);
  rip_rip_button->setFont(buttonFont());
  connect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripDiskButtonData()));
  rip_rip_button->setDisabled(true);

  //
  // Close Button
  //
  rip_close_button=new QPushButton(tr("Close"),this);
  rip_close_button->setFont(buttonFont());
  connect(rip_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  rip_normalize_spin->setValue(rda->libraryConf()->ripperLevel()/100);
  rip_autotrim_spin->setValue(rda->libraryConf()->trimThreshold()/100);
  rip_channels_box->insertItem(0,"1");
  rip_channels_box->insertItem(1,"2");
  rip_channels_box->setCurrentIndex(rda->libraryConf()->defaultChannels()-1);
  rip_done=false;
}


DiskRipper::~DiskRipper()
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
  delete rip_track_bar;
  delete rip_wavedata_dialog;
  delete rip_cut_dialog;
}


QSize DiskRipper::sizeHint() const
{
  return QSize(730,716+12);
}


QSizePolicy DiskRipper::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void DiskRipper::ejectButtonData()
{
  rip_cdrom->eject();
}


void DiskRipper::playButtonData()
{
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  rip_cdrom->play(1+rows.first().row());
  rip_play_button->on();
  rip_stop_button->off();
}


void DiskRipper::stopButtonData()
{
  rip_cdrom->stop();
  rip_play_button->off();
  rip_stop_button->on();
}


void DiskRipper::ripDiskButtonData()
{
  rip_aborting=false;

  //
  // Calculate number of tracks to rip
  //
  int tracks=0;
  for(int i=0;i<rip_cutnames.size();i++) {
    if(!rip_cutnames[i].isEmpty()) {
      tracks++;
    }
  }
  rip_disk_bar->setMaximum(tracks);

  //
  // Set Artist and Album
  //
  SetArtistAlbum();

  //
  // Rip
  //
  tracks=0;
  for(int i=0;i<rip_track_model->rowCount();i++) {
    QString cutname=rip_track_model->cutName(rip_track_model->index(i,0));
     if(!cutname.isEmpty()) {
      rip_eject_button->setDisabled(true);
      rip_play_button->setDisabled(true);
      rip_stop_button->setDisabled(true);
      rip_rip_button->setText(tr("Abort\nRip"));
      disconnect(rip_rip_button,SIGNAL(clicked()),
		 this,SLOT(ripDiskButtonData()));
      rip_setcut_button->setDisabled(true);
      rip_setall_button->setDisabled(true);
      rip_cartlabel_button->setDisabled(true);
      rip_clear_button->setDisabled(true);
      rip_close_button->setDisabled(true);
      rip_normalize_box->setDisabled(true);
      rip_normalize_spin->setDisabled(true);
      rip_channels_box->setDisabled(true);
      rip_autotrim_box->setDisabled(true);
      rip_autotrim_spin->setDisabled(true);
      rip_disk_bar->setValue(tracks++);
      int start_track=i+1;
      int end_track=rip_end_track[i];
      RipTrack(start_track,end_track,rip_cutnames[i],
	       BuildTrackName(start_track,end_track));
    }
  }
  rip_eject_button->setEnabled(true);
  rip_play_button->setEnabled(true);
  rip_stop_button->setEnabled(true);
  rip_setcut_button->setEnabled(false);
  rip_setall_button->setEnabled(false);
  rip_setsingle_button->setEnabled(false);
  rip_cartlabel_button->setEnabled(false);
  rip_clear_button->setEnabled(false);
  rip_close_button->setEnabled(true);
  rip_rip_button->setText(tr("Rip\nDisk"));
  rip_rip_button->setEnabled(false);
  connect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripDiskButtonData()));
  rip_normalize_box->setEnabled(true);
  rip_normalize_spin->setEnabled(true);
  rip_channels_box->setEnabled(true);
  rip_autotrim_box->setEnabled(true);
  rip_autotrim_spin->setEnabled(true);
  rip_disk_bar->reset();
  rip_diskbar_label->setDisabled(true);
  rip_trackbar_label->setDisabled(true);
  rip_diskbar_label->setText(tr("Total Progress"));
  rip_trackbar_label->setText(tr("Track Progress"));
  rip_track_model->clearCutNames();
  if(rip_aborting) {
    QMessageBox::information(this,"RDLibrary",tr("Rip aborted!"));
  }
  else {
    QMessageBox::information(this,"RDLibrary",tr("Rip complete!"));
  }
}


void DiskRipper::ejectedData()
{
  rip_track_model->clear();
  rip_cdtext_label->hide();
  rip_cddb_label->hide();
  rip_cdtext_label->hide();
  rip_cddb_label->hide();
  rip_track=-1;
  rip_artist_edit->clear();
  rip_album_edit->clear();
  rip_other_edit->clear();
  rip_apply_box->setChecked(false);
  rip_apply_box->setDisabled(true);
  rip_apply_label->setDisabled(true);
  rip_browser_button->setDisabled(true);
  rip_browser_label->setDisabled(true);
}


void DiskRipper::setCutButtonData()
{
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();
  
  if(rows.size()!=1) {
    return;
  }
  QString cutname=rip_cutnames[rows.first().row()];
  if(rip_cut_dialog->exec(&cutname)) {
    if(cutname.isEmpty()) {
      rip_cutnames[rows.first().row()]="";
      rip_track_model->setCutName(rows.first(),"");
    }
    else {
      for(int i=0;i<rip_cutnames.size();i++) {
	if(cutname==rip_cutnames[i]) {
	  QMessageBox::warning(this,tr("Cut Conflict"),
			 tr("That cut has already been assigned a track!"));
	  return;
	}
      }
      if(QFile::exists(RDCut::pathName(cutname))){
	switch(QMessageBox::warning(this,tr("Cut Exists"),
				    tr("A recording already exists.\nDo you want to overwrite it?"),
				    QMessageBox::Yes,
				    QMessageBox::No)) {
	case QMessageBox::No:
	case QMessageBox::NoButton:
	  return;
	  
	default:
	  break;
	}
      }
      if(cut_clipboard!=NULL) {
	if(cutname==cut_clipboard->cutName()) {
	  switch(QMessageBox::warning(this,tr("Empty Clipboard"),
				      tr("Ripping this cut will also empty the clipboard.\nDo you still want to proceed?"),
				      QMessageBox::Yes,
				      QMessageBox::No)) {
	  case QMessageBox::No:
	  case QMessageBox::NoButton:
	    return;
	    
	  default:
	    break;
	  }
	  delete cut_clipboard;
	  cut_clipboard=NULL;
	}
      }
      rip_cutnames[rows.first().row()]=cutname;
      rip_end_track[rows.first().row()]=1+rows.first().row();
      rip_track_model->setCutName(rows.first(),cutname);
    }
  }
  else {
    return;
  }

  bool track_selected=false;
  for(int i=0;i<rip_track_model->rowCount();i++) {
    if(!rip_track_model->cutName(rip_track_model->index(i,0)).isEmpty()) {
      track_selected=true;
    }
  }
  rip_setcut_button->setEnabled(false);
  rip_setall_button->setEnabled(false);
  rip_setsingle_button->setEnabled(false);
  rip_cartlabel_button->setEnabled(true);
  rip_clear_button->setEnabled(true);
  rip_rip_button->setEnabled(track_selected);
}


void DiskRipper::setMultiButtonData()
{
  //
  // Get Destination Group
  //
  RDListGroups *list_groups=new RDListGroups(rip_group_text,"RDLibrary",this);
  if(!list_groups->exec()) {
    delete list_groups;
    return;
  }
  delete list_groups;
  RDGroup *group=new RDGroup(*rip_group_text);

  //
  // Reserve Carts
  //
  QList<unsigned> cart_nums;
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();
  if(!group->reserveCarts(&cart_nums,rda->station()->name(),RDCart::Audio,
			  rows.size())) {
    QMessageBox::warning(this,"RDLibrary - "+tr("Error"),
			 tr("Unable to allocate carts in group")+" \""+
			 group->name()+"\".");
    return;
  }

  //
  // Schedule Rips
  //
  for(int i=0;i<rows.size();i++) {
    rip_cutnames[rows.at(i).row()]=RDCut::cutName(cart_nums[i],1);
    rip_end_track[rows.at(i).row()]=1+rows.at(i).row();
    rip_track_model->setCutName(rows.at(i),rip_cutnames[rows.at(i).row()]);
  }
  rip_setcut_button->setEnabled(false);
  rip_setall_button->setEnabled(false);
  rip_setsingle_button->setEnabled(false);
  rip_cartlabel_button->setEnabled(true);
  rip_clear_button->setEnabled(true);
  rip_rip_button->setEnabled(true);
}


void DiskRipper::setSingleButtonData()
{
  //
  // Get Destination Group
  //
  RDListGroups *list_groups=new RDListGroups(rip_group_text,rda->user()->name(),
					     this);
  if(list_groups->exec()<0) {
    delete list_groups;
    return;
  }
  delete list_groups;
  RDGroup *group=new RDGroup(*rip_group_text);
  QList<unsigned> cart_nums;
  unsigned new_cart=0;
  int first_track=-1;

  //
  // Reserve Cart
  //
  if(!group->reserveCarts(&cart_nums,rda->station()->name(),RDCart::Audio,1)) {
    QMessageBox::warning(this,"RDLibrary - "+tr("Error"),
			 tr("Unable to allocate cart in group")+" \""+
			 group->name()+"\".");
    return;
  }

  //
  // Schedule Rips
  //
  QModelIndexList rows=
    SortRows(rip_track_view->selectionModel()->selectedRows());
  for(int i=0;i<rows.size();i++) {
    if(new_cart==0) {
      first_track=rows.at(i).row();
      new_cart=group->nextFreeCart(cart_nums[0]);
      rip_cutnames[rows.at(i).row()]=RDCut::cutName(cart_nums[0],1);
      rip_track_model->setCutName(rows.at(i),RDCut::cutName(cart_nums[0],1));
    }
    else {
      rip_end_track[first_track]=1+rows.at(i).row();
      rip_track_model->setLeadTrack(rows.at(i),first_track);
    }
  }
  rip_setcut_button->setEnabled(false);
  rip_setall_button->setEnabled(false);
  rip_setsingle_button->setEnabled(false);
  rip_cartlabel_button->setEnabled(true);
  rip_clear_button->setEnabled(true);
  rip_rip_button->setEnabled(true);
}


void DiskRipper::modifyCartLabelData()
{
  SetArtistAlbum();
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(rip_wavedata_dialog->exec(rip_wave_datas[rows.first().row()])==0) {
    rip_track_model->
      setTrackTitle(rows.first(),rip_wave_datas[rows.first().row()]->title());
  }
}


void DiskRipper::clearSelectionData()
{
  rip_track_view->clearSelection();
  rip_track_model->clearCutNames();
  for(int i=0;i<rip_cutnames.size();i++) {
    rip_cutnames[i]="";
  }
  for(int i=0;i<rip_end_track.size();i++) {
    rip_end_track[i]=-1;
  }
  rip_setcut_button->setDisabled(true);
  rip_setall_button->setDisabled(true);
  rip_setsingle_button->setDisabled(true);
  rip_cartlabel_button->setDisabled(true);
  rip_clear_button->setDisabled(true);
  UpdateRipButton();
}


void DiskRipper::mediaChangedData()
{
  rip_cutnames.clear();
  rip_end_track.clear();
  for(int i=0;i<rip_wave_datas.size();i++) {
    delete rip_wave_datas[i];
  }
  rip_wave_datas.clear();
  rip_track_model->clear();
  rip_track=-1;
  rip_setcut_button->setDisabled(true);
  rip_setall_button->setDisabled(true);
  rip_setsingle_button->setDisabled(true);
  rip_cartlabel_button->setDisabled(true);
  rip_clear_button->setDisabled(true);
  rip_cdtext_label->hide();
  rip_cddb_label->hide();
  rip_cdtext_label->hide();
  rip_cddb_label->hide();
  for(int i=rip_cdrom->tracks();i>0;i--) {
    rip_cutnames.push_back(QString());
    rip_end_track.push_back(-1);
    rip_wave_datas.push_back(new RDWaveData());
    rip_wave_datas.back()->setTitle(tr("Track")+QString().sprintf(" %d",i));
  }
  rip_disc_record.clear();
  rip_cdrom->setCddbRecord(&rip_disc_record);
  rip_disc_lookup->setCddbRecord(&rip_disc_record);
  rip_track_model->setDisc(rip_cdrom);
  rip_disc_lookup->lookup();
}


void DiskRipper::playedData(int track)
{
  rip_play_button->on();
  rip_stop_button->off();
}


void DiskRipper::stoppedData()
{
  rip_play_button->off();
  rip_stop_button->on();
}


void DiskRipper::lookupDoneData(RDDiscLookup::Result result,
				const QString &err_msg)
{
  RDDiscRecord::DataSource src=RDDiscRecord::LastSource;
  if(rip_disc_record.hasData(RDDiscRecord::LocalSource)) {
    src=RDDiscRecord::LocalSource;
  }
  else {
    if(rip_disc_record.hasData(RDDiscRecord::RemoteSource)) {
      src=RDDiscRecord::RemoteSource;
    }
    else {
      rip_apply_box->hide();
      rip_apply_label->hide();
      rip_track=-1;
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
      rip_track=-1;
      rip_cdtext_label->hide();
      rip_cddb_label->hide();
      return;
    }
    rip_artist_edit->setText(rip_disc_record.discArtist(src));
    rip_album_edit->setText(rip_disc_record.discAlbum(src));
    rip_other_edit->setText(rip_disc_record.discExtended());
    rip_track_model->refresh(&rip_disc_record,src);
    for(int i=0;i<rip_disc_record.tracks();i++) {
      rip_wave_datas[rip_wave_datas.size()-i-1]->
	setTitle(rip_disc_record.trackTitle(src,i));
      rip_wave_datas[rip_wave_datas.size()-i-1]->
	setArtist(rip_disc_record.discArtist(src));
      rip_wave_datas[rip_wave_datas.size()-i-1]->
	setAlbum(rip_disc_record.discAlbum(src));
      rip_wave_datas[rip_wave_datas.size()-i-1]->
	setLabel(rip_disc_record.discLabel());
    }
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
    rip_apply_box->setChecked(true);
    rip_apply_box->setEnabled(true);
    rip_apply_label->setEnabled(true);
    rip_browser_button->setDisabled(rip_disc_lookup->sourceUrl().isNull());
    rip_browser_label->setDisabled(rip_disc_lookup->sourceUrl().isNull());
    rip_apply_box->show();
    rip_apply_label->show();
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
    break;

  case RDDiscLookup::NoMatch:
    rip_apply_box->hide();
    rip_apply_label->hide();
    rip_track=-1;
    rip_cdtext_label->hide();
    rip_cddb_label->hide();
    break;

  case RDDiscLookup::LookupError:
    QMessageBox::warning(this,"RDLibrary - "+rip_disc_lookup->sourceName()+
			 " "+tr("Lookup Error"),err_msg);
    rip_apply_box->hide();
    rip_apply_label->hide();
    rip_track=-1;
    rip_cdtext_label->hide();
    rip_cddb_label->hide();
    break;
  }
}


void DiskRipper::normalizeCheckData(bool state)
{
  rip_normalize_spin->setEnabled(state);
  rip_normalize_label->setEnabled(state);
  rip_normalize_unit->setEnabled(state);
}


void DiskRipper::autotrimCheckData(bool state)
{
  rip_autotrim_spin->setEnabled(state);
  rip_autotrim_label->setEnabled(state);
  rip_autotrim_unit->setEnabled(state);
}


void DiskRipper::selectionChangedData(const QItemSelection &before,
				      const QItemSelection &after)
{
  QModelIndexList rows=rip_track_view->selectionModel()->selectedRows();
  int count=rows.size();
  bool contiguous=IsContiguous(rows);
  
  for(int i=0;i<rows.size();i++) {
    if(rip_track_model->trackContainsData(rows.at(i))) {
      rip_setcut_button->setEnabled(false);
      rip_setall_button->setEnabled(false);
      rip_setsingle_button->setEnabled(false);
      rip_cartlabel_button->setEnabled(false);
      rip_clear_button->setEnabled(true);
      return;
    }
  }
  rip_setcut_button->setEnabled(count==1);
  rip_setall_button->setEnabled(count>0);
  rip_setsingle_button->setEnabled((count>1)&&contiguous);
  rip_cartlabel_button->setEnabled(false);
  rip_clear_button->setEnabled(count>0);
}


void DiskRipper::openBrowserData()
{
  RDWebBrowser(rip_disc_lookup->sourceUrl());
}


void DiskRipper::doubleClickedData(const QModelIndex &index)
{
  setCutButtonData();
}


void DiskRipper::closeData()
{
  RDCart::removePending(rda->station(),rda->user(),rda->config());
  if(rip_done&&rip_apply_box->isChecked()) {
    done(0);
  }
  else {
    done(-1);
  }
}


void DiskRipper::resizeEvent(QResizeEvent *e)
{
  rip_artist_edit->setGeometry(65,9,size().width()-125,18);
  rip_album_edit->setGeometry(65,31,size().width()-125,18);
  rip_other_edit->setGeometry(65,53,size().width()-125,60);
  rip_apply_box->setGeometry(65,118,15,15);
  rip_apply_label->setGeometry(85,118,250,20);
  if(rip_disc_lookup->sourceName()=="MusicBrainz") {
    rip_cdtext_label->setGeometry(size().width()-270-rip_cdtext_label->sizeHint().width(),117,rip_cdtext_label->sizeHint().width(),rip_cdtext_label->sizeHint().height());
    rip_cddb_label->setGeometry(size().width()-270-rip_cddb_label->sizeHint().width(),117,rip_cddb_label->sizeHint().width(),rip_cddb_label->sizeHint().height());
  }
  else {
    rip_cdtext_label->setGeometry(size().width()-270-rip_cdtext_label->sizeHint().width()+210,
				  117,
				  rip_cdtext_label->sizeHint().width(),
				  rip_cdtext_label->sizeHint().height());
    rip_cddb_label->setGeometry(size().width()-270-rip_cddb_label->sizeHint().width()+210,
				117,
				rip_cddb_label->sizeHint().width(),
				rip_cddb_label->sizeHint().height());
  }
  rip_browser_button->setGeometry(size().width()-260,117,200,35);
  rip_browser_button->setIconSize(QSize(198,33));
  rip_browser_label->setGeometry(size().width()-260,117,200,35);
  rip_track_label->setGeometry(100,140+12,100,14);
  rip_track_view->setGeometry(100,156+12,size().width()-202,size().height()-342);
  rip_diskbar_label->setGeometry(10,size().height()-174+0,size().width()-110,20);
  rip_disk_bar->setGeometry(10,size().height()-154,size().width()-110,20); 
  rip_trackbar_label->setGeometry(10,size().height()-126,size().width()-110,20);
  rip_track_bar->setGeometry(10,size().height()-106,size().width()-110,20);
  rip_eject_button->setGeometry(10,156+12,80,50);
  rip_play_button->setGeometry(10,216+12,80,50);
  rip_stop_button->setGeometry(10,276+12,80,50);
  rip_setcut_button->setGeometry(size().width()-90,156+12,80,50);
  rip_setall_button->setGeometry(size().width()-90,216+12,80,50);
  rip_setsingle_button->setGeometry(size().width()-90,276+12,80,50);
  rip_cartlabel_button->setGeometry(size().width()-90,420+12,80,50);
  rip_clear_button->setGeometry(size().width()-90,480+12,80,50);
  rip_normalizebox_label->setGeometry(30,size().height()-78,85,20);
  rip_normalize_box->setGeometry(10,size().height()-78,20,20);
  rip_normalize_label->setGeometry(120,size().height()-78,45,20);
  rip_normalize_spin->setGeometry(170,size().height()-79,50,20);
  rip_normalize_unit->setGeometry(225,size().height()-78,40,20);
  rip_autotrimbox_label->setGeometry(30,size().height()-54,85,20);
  rip_autotrim_box->setGeometry(10,size().height()-54,20,20);
  rip_autotrim_label->setGeometry(120,size().height()-54,45,20);
  rip_autotrim_spin->setGeometry(170,size().height()-54,50,20);
  rip_autotrim_unit->setGeometry(225,size().height()-54,40,20);
  rip_channels_label->setGeometry(10,size().height()-30,75,20);
  rip_channels_box->setGeometry(90,size().height()-30,50,20);
  rip_rip_button->setGeometry(size().width()-200,size().height()-60,80,50);
  rip_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void DiskRipper::closeEvent(QCloseEvent *e)
{
  if(!ripper_running) {
    closeData();
  }
}


void DiskRipper::RipTrack(int track,int end_track,QString cutname,QString title)
{
  RDCdRipper *ripper=NULL;
  RDCut *cut=new RDCut(cutname);
  RDCart *cart=new RDCart(cut->cartNumber());

  //
  // Create Cut
  //
  RDCut::create(cutname);

  rip_done=false;
  rip_rip_aborted=false;
  rip_track_number=track;
  rip_title=title;
  rip_cutname=cutname;
  if(title.isEmpty()) {
    rip_trackbar_label->setText(tr("Track Progress")+" - "+tr("Track")+
				QString::asprintf(" %d",track));
  }
  else {
    rip_trackbar_label->setText(tr("Track Progress")+" - "+title);
  }
  rip_diskbar_label->setEnabled(true);
  rip_trackbar_label->setEnabled(true);

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
  rip_track_bar->setMaximum(ripper->totalSteps()+1);
  connect(ripper,SIGNAL(progressChanged(int)),
	  rip_track_bar,SLOT(setValue(int)));
  connect(rip_rip_button,SIGNAL(clicked()),ripper,SLOT(abort()));
  RDAudioImport *conv=NULL;
  RDSettings *settings=NULL;
  ripper->setDevice(rda->libraryConf()->ripperDevice());
  ripper->setDestinationFile(tmpfile);
  switch((ripper_err=ripper->rip(rip_track_number-1,end_track-1))) {
  case RDCdRipper::ErrorOk:
    conv=new RDAudioImport(this);
    conv->setSourceFile(tmpfile);
    conv->setCartNumber(cut->cartNumber());
    conv->setCutNumber(cut->cutNumber());
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
	    runImport(rda->user()->name(),rda->user()->password(),
		      &audio_conv_err))) {
    case RDAudioImport::ErrorOk:
      cart->setMetadata(rip_wave_datas[rip_wave_datas.size()-track]);
      cut->setDescription(rip_wave_datas[track-1]->title());
      cut->setIsrc(rip_disc_record.isrc(rip_track_number-1));
      cut->setRecordingMbId(rip_disc_record.trackRecordingMbId(rip_track_number-1));
      cut->setReleaseMbId(rip_disc_record.discReleaseMbId());
      cart->clearPending();
      SendNotification(RDNotification::AddAction,cart->number());
      break;

    default:
      cart->remove(rda->station(),rda->user(),rda->config());
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
    cart->remove(rda->station(),rda->user(),rda->config());
    QMessageBox::warning(this,tr("RDLibrary - Ripper Error"),
			 RDCdRipper::errorText(ripper_err));
    break;

  case RDCdRipper::ErrorAborted:
    rip_aborting=true;
    cart->remove(rda->station(),rda->user(),rda->config());
    break;
  }
  delete ripper;
  unlink(tmpfile.toUtf8());
  rmdir(tmpdir.toUtf8());
  rip_track_bar->setValue(0);

  delete cart;
  delete cut;
}


void DiskRipper::UpdateRipButton()
{
  for(int i=0;i<rip_track_model->rowCount();i++) {
    if(!rip_track_model->cutName(rip_track_model->index(i,0)).isEmpty()) {
      rip_rip_button->setEnabled(true);
      return;
    }
  }
  rip_rip_button->setDisabled(true);
}


QString DiskRipper::BuildTrackName(int start_track,int end_track) const
{
  QString ret;

  for(int i=0;i<rip_track_model->rowCount();i++) {
    int track=i+1;
    QModelIndex row=rip_track_model->index(i,0);
    if(track==start_track) {
      ret=rip_track_model->data(row.sibling(row.row(),2)).toString();
      if(ret.isEmpty()) {
	ret=tr("Track")+QString::asprintf(" %d",start_track);
      }
    }
    else {
      if((track>start_track)&&
	 (track<=end_track)) {
	if(rip_track_model->data(row.sibling(row.row(),2)).toString().
	   isEmpty()) {
	  ret+=" / "+tr("Track")+
	    QString::asprintf(" %d",track);
	}
	else {
	  ret+=" / "+rip_track_model->data(row.sibling(row.row(),2)).toString();
	}
      }
    }
  }

  return ret;
}


void DiskRipper::SetArtistAlbum()
{
  for(int i=0;i<rip_track_model->rowCount();i++) {
    if(rip_wave_datas[i]->artist().isEmpty()) {
      rip_wave_datas[i]->setArtist(rip_artist_edit->text());
    }
    if(rip_wave_datas[i]->album().isEmpty()) {
      rip_wave_datas[i]->setAlbum(rip_album_edit->text());
    }
  }
}


void DiskRipper::SendNotification(RDNotification::Action action,
				  unsigned cartnum)
{
  RDNotification *notify=
    new RDNotification(RDNotification::CartType,action,QVariant(cartnum));
  rda->ripc()->sendNotification(*notify);
  qApp->processEvents();
  delete notify;
}


bool DiskRipper::IsContiguous(const QModelIndexList &rows) const
{
  QModelIndexList sorted_rows=SortRows(rows);

  for(int i=1;i<sorted_rows.size();i++) {
    if(sorted_rows.at(i-1).row()!=(sorted_rows.at(i).row()-1)) {
      return false;
    }
  }
  return true;
}


QModelIndexList DiskRipper::SortRows(const QModelIndexList &rows) const
{
  QModelIndexList ret;
  QList<int> index;
  bool modified;

  for(int i=0;i<rows.size();i++) {
    index.push_back(i);
  }
  do {
    modified=false;
    for(int i=1;i<rows.size();i++) {
      if(rows.at(index.at(i-1)).row()>rows.at(index.at(i)).row()) {
	index.swap(i-1,i);
	modified=true;
      }
    }
  } while(modified);

  for(int i=0;i<rows.size();i++) {
    ret.push_back(rows.at(index.at(i)));
  }

  return ret;
}

