// disk_ripper.cpp
//
// CD Ripper Dialog for Rivendell.
//
//   (C) Copyright 2002-2003,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: disk_ripper.cpp,v 1.30.4.3.2.7 2014/06/02 18:59:24 cvs Exp $
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <linux/cdrom.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcheckbox.h>

#include <rddb.h>
#include <rd.h>
#include <rdconf.h>
#include <rdwavefile.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdcut_dialog.h>
#include <rdlist_groups.h>
#include <rdgroup.h>
#include <rdaudioimport.h>
#include <rdcdripper.h>
#include <rdescape_string.h>

#include <disk_ripper.h>
#include <globals.h>


DiskRipper::DiskRipper(QString *filter,QString *group,QString *schedcode,
		       bool profile_rip,QWidget *parent,const char *name) 
  : QDialog(parent,name)
{
  rip_isrc_read=false;
  rip_filter_text=filter;
  rip_group_text=group;
  rip_schedcode_text=schedcode;
  rip_profile_rip=profile_rip;
  rip_aborting=false;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  setCaption(tr("Rip Disk"));

  //
  // Create Dialogs
  //
  rip_wavedata_dialog=new RDWaveDataDialog("RDLibrary",this);

  //
  // Create Temporary Directory
  //
  char path[PATH_MAX];
  strncpy(path,RIPPER_TEMP_DIR,PATH_MAX);
  strcat(path,"/XXXXXX");
  if(mkdtemp(path)==NULL) {
    QMessageBox::warning(this,"RDLibrary - "+tr("Ripper Error"),
			 tr("Unable to create temporary directory!"));
  }
  else {
    rip_cdda_dir.setPath(path);
  }

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
  rip_cdrom->setDevice(rdlibrary_conf->ripperDevice());
  rip_cdrom->open();

  //
  // CDDB Stuff
  //
  if(rip_profile_rip) {
    rip_cddb_lookup=new RDCddbLookup(stdout,this);
  }
  else {
    rip_cddb_lookup=new RDCddbLookup(NULL,this);
  }
  connect(rip_cddb_lookup,SIGNAL(done(RDCddbLookup::Result)),
	  this,SLOT(cddbDoneData(RDCddbLookup::Result)));

  //
  // Artist Label
  //
  QLabel *label=new QLabel(tr("Artist:"),this);
  label->setGeometry(10,10,50,18);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  rip_artist_edit=new QLineEdit(this);

  //
  // Album Edit
  //
  label=new QLabel(tr("Album:"),this);
  label->setGeometry(10,32,50,18);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter);
  rip_album_edit=new QLineEdit(this);

  //
  // Other Edit
  //
  label=new QLabel(tr("Other:"),this);
  label->setGeometry(10,54,50,16);
  label->setFont(label_font);
  label->setAlignment(AlignRight);
  rip_other_edit=new QTextEdit(this);
  rip_other_edit->setReadOnly(true);

  //
  // Apply FreeDB Check Box
  //
  rip_apply_box=new QCheckBox(this,"rip_apply_box");
  rip_apply_box->setChecked(true);
  rip_apply_box->setDisabled(true);
  rip_apply_label=
    new QLabel(rip_apply_box,tr("Apply FreeDB Values to Carts"),this);
  rip_apply_label->setFont(label_font);
  rip_apply_label->setAlignment(AlignLeft);
  rip_apply_box->setChecked(false);
  rip_apply_label->setDisabled(true);

  //
  // Track List
  //
  rip_track_list=new QListView(this);
  rip_track_list->setAllColumnsShowFocus(true);
  rip_track_list->setItemMargin(5);
  rip_track_list->setSorting(-1);
  rip_track_list->setSelectionMode(QListView::Extended);
  connect(rip_track_list,SIGNAL(selectionChanged()),
	  this,SLOT(selectionChangedData()));
  connect(rip_track_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));
  rip_track_label=new QLabel(rip_track_list,tr("Tracks"),this);
  rip_track_label->setFont(label_font);
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
  rip_track_list->addColumn(tr("CUT"));
  rip_track_list->setColumnAlignment(5,Qt::AlignLeft);

  //
  // Progress Bars
  //
  rip_disk_bar=new QProgressBar(this);
  rip_diskbar_label=new QLabel(tr("Disk Progress"),this);
  rip_diskbar_label->setFont(label_font);
  rip_diskbar_label->setAlignment(AlignLeft|AlignVCenter);
  rip_diskbar_label->setDisabled(true);
  rip_track_bar=new QProgressBar(this);
  rip_trackbar_label=new QLabel(tr("Track Progress"),this);
  rip_trackbar_label->setFont(label_font);
  rip_trackbar_label->setAlignment(AlignLeft|AlignVCenter);
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
  rip_stop_button->setOnColor(red);
  rip_stop_button->on();
  connect(rip_stop_button,SIGNAL(clicked()),this,SLOT(stopButtonData()));
  
  //
  // Set Cut Button
  //
  rip_setcut_button=new QPushButton(tr("Set\n&Cart/Cut"),this);
  rip_setcut_button->setFont(button_font);
  rip_setcut_button->setDisabled(true);
  connect(rip_setcut_button,SIGNAL(clicked()),this,SLOT(setCutButtonData()));

  //
  // Set Multi Tracks Button
  //
  rip_setall_button=new QPushButton(tr("Add Cart\nPer Track"),this);
  rip_setall_button->setFont(button_font);
  rip_setall_button->setDisabled(true);
  connect(rip_setall_button,SIGNAL(clicked()),this,SLOT(setMultiButtonData()));

  //
  // Set Single Button
  //
  rip_setsingle_button=new QPushButton(tr("Add Single\nCart"),this);
  rip_setsingle_button->setFont(button_font);
  rip_setsingle_button->setDisabled(true);
  connect(rip_setsingle_button,SIGNAL(clicked()),
	  this,SLOT(setSingleButtonData()));

  //
  // Set Cart Label Button
  //
  rip_cartlabel_button=new QPushButton(tr("Modify\nCart Label"),this);
  rip_cartlabel_button->setFont(button_font);
  rip_cartlabel_button->setDisabled(true);
  connect(rip_cartlabel_button,SIGNAL(clicked()),
	  this,SLOT(modifyCartLabelData()));

  //
  // Clear Selection Button
  //
  rip_clear_button=new QPushButton(tr("Clear\nSelection"),this);
  rip_clear_button->setFont(button_font);
  rip_clear_button->setDisabled(true);
  connect(rip_clear_button,SIGNAL(clicked()),this,SLOT(clearSelectionData()));

  //
  // Normalize Check Box
  //
  rip_normalize_box=new QCheckBox(this);
  rip_normalize_box->setChecked(true);
  rip_normalizebox_label=new QLabel(rip_normalize_box,tr("Normalize"),this);
  rip_normalizebox_label->setFont(label_font);
  rip_normalizebox_label->setAlignment(AlignLeft|AlignVCenter);
  connect(rip_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  rip_normalize_spin=new QSpinBox(this);
  rip_normalize_spin->setRange(-30,0);
  rip_normalize_label=new QLabel(rip_normalize_spin,tr("Level:"),this);
  rip_normalize_label->setFont(label_font);
  rip_normalize_label->setAlignment(AlignRight|AlignVCenter);
  rip_normalize_unit=new QLabel(tr("dBFS"),this);
  rip_normalize_unit->setFont(label_font);
  rip_normalize_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Autotrim Check Box
  //
  rip_autotrim_box=new QCheckBox(this);
  rip_autotrim_box->setChecked(true);
  rip_autotrimbox_label=new QLabel(rip_autotrim_box,tr("Autotrim"),this);
  rip_autotrimbox_label->setFont(label_font);
  rip_autotrimbox_label->setAlignment(AlignLeft|AlignVCenter);
  connect(rip_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimCheckData(bool)));

  //
  // Autotrim Level
  //
  rip_autotrim_spin=new QSpinBox(this);
  rip_autotrim_spin->setRange(-99,0);
  rip_autotrim_label=new QLabel(rip_autotrim_spin,tr("Level:"),this);
  rip_autotrim_label->setFont(label_font);
  rip_autotrim_label->setAlignment(AlignRight|AlignVCenter);
  rip_autotrim_unit=new QLabel(tr("dBFS"),this);
  rip_autotrim_unit->setFont(label_font);
  rip_autotrim_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Channels
  //
  rip_channels_box=new QComboBox(this);
  rip_channels_label=new QLabel(rip_channels_box,tr("Channels:"),this);
  rip_channels_label->setFont(label_font);
  rip_channels_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Rip Disc Button
  //
  rip_rip_button=new QPushButton(tr("&Rip\nDisc"),this);
  rip_rip_button->setFont(button_font);
  connect(rip_rip_button,SIGNAL(clicked()),this,SLOT(ripDiskButtonData()));
  rip_rip_button->setDisabled(true);

  //
  // Close Button
  //
  rip_close_button=new QPushButton(tr("&Close"),this);
  rip_close_button->setFont(button_font);
  connect(rip_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Populate Data
  //
  rip_normalize_spin->setValue(rdlibrary_conf->ripperLevel()/100);
  rip_autotrim_spin->setValue(rdlibrary_conf->trimThreshold()/100);
  rip_channels_box->insertItem("1");
  rip_channels_box->insertItem("2");
  rip_channels_box->setCurrentItem(rdlibrary_conf->defaultChannels()-1);
  rip_done=false;
}


DiskRipper::~DiskRipper()
{
  QStringList files=rip_cdda_dir.entryList();
  for(unsigned i=0;i<files.size();i++) {
    if((files[i]!=".")&&(files[i]!="..")) {
      rip_cdda_dir.remove(files[i]);
    }
  }
  rmdir(rip_cdda_dir.path());

  rip_cdrom->close();
  delete rip_cdrom;
  delete rip_track_list;
  delete rip_rip_button;
  delete rip_close_button;
  delete rip_eject_button;
  delete rip_play_button;
  delete rip_stop_button;
  delete rip_track_bar;
  delete rip_wavedata_dialog;
}


QSize DiskRipper::sizeHint() const
{
  return QSize(730,716);
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
  if(rip_track_list->currentItem()!=NULL) {
    rip_cdrom->play(rip_track_list->currentItem()->text(0).toInt());
    rip_play_button->on();
    rip_stop_button->off();
  }
}


void DiskRipper::stopButtonData()
{
  rip_cdrom->stop();
  rip_play_button->off();
  rip_stop_button->on();
}


void DiskRipper::ripDiskButtonData()
{
  RDListViewItem *item=(RDListViewItem *)rip_track_list->selectedItem();
  if(item!=NULL) {
    rip_track_list->setSelected(item,false);
  }
  rip_aborting=false;

  //
  // Calculate number of tracks to rip
  //
  int tracks=0;
  for(unsigned i=0;i<rip_cutnames.size();i++) {
    if(!rip_cutnames[i].isEmpty()) {
      tracks++;
    }
  }
  rip_disk_bar->setTotalSteps(tracks);

  //
  // Read ISRCs
  //
  if(!rip_isrc_read) {
    rip_cddb_lookup->
      readIsrc(rip_cdda_dir.path(),rdlibrary_conf->ripperDevice());
    rip_isrc_read=true;
  }

  //
  // Rip
  //
  tracks=0;
  item=(RDListViewItem *)rip_track_list->firstChild();
  while((item!=NULL)&&(!rip_aborting)) {
    if(!rip_cutnames[item->text(0).toInt()-1].isEmpty()) {
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
      rip_disk_bar->setProgress(tracks++);
      rip_disk_bar->setPercentageVisible(true);
      int start_track=item->text(0).toInt();
      int end_track=rip_end_track[item->text(0).toInt()-1];
      RipTrack(start_track,end_track,rip_cutnames[item->text(0).toInt()-1],
	       BuildTrackName(start_track,end_track));
    }
    item=(RDListViewItem *)item->nextSibling();
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
  rip_disk_bar->setPercentageVisible(false);
  rip_disk_bar->reset();
  rip_diskbar_label->setDisabled(true);
  rip_trackbar_label->setDisabled(true);
  rip_diskbar_label->setText(tr("Total Progress"));
  rip_trackbar_label->setText(tr("Track Progress"));
  item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    item->setText(5,"");
    item=(RDListViewItem *)item->nextSibling();
  }
  rip_cdrom->eject();
  if(rip_aborting) {
    QMessageBox::information(this,tr("Rip Complete"),tr("Rip aborted!"));
  }
  else {
    QMessageBox::information(this,tr("Rip Complete"),tr("Rip complete!"));
  }
}


void DiskRipper::ejectedData()
{
  rip_track_list->clear();
  rip_track=-1;
  rip_artist_edit->clear();
  rip_album_edit->clear();
  rip_other_edit->clear();
  rip_apply_box->setChecked(false);
  rip_apply_box->setDisabled(true);
  rip_apply_label->setDisabled(true);
}


void DiskRipper::setCutButtonData()
{
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      break;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  if(item==NULL) {
    return;
  }
  QString cutname=rip_cutnames[item->text(0).toUInt()-1];
  RDCutDialog *dialog=new RDCutDialog(&cutname,rdstation_conf,lib_system,
				      rip_filter_text,
				      rip_group_text,rip_schedcode_text,
				      lib_user->name(),true,
				      true,true,this,"cut_dialog");
  if(dialog->exec()==0) {
    if(cutname.isEmpty()) {
      rip_cutnames[item->text(0).toUInt()-1]="";
      item->setText(5,"");
    }
    else {
      for(unsigned i=0;i<rip_cutnames.size();i++) {
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
	  delete dialog;
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
      rip_cutnames[item->text(0).toUInt()-1]=cutname;
      rip_end_track[item->text(0).toUInt()-1]=item->text(0).toInt();
      RDCart *cart=new RDCart(cutname.left(6).toUInt());
      RDCut *cut=new RDCut(cutname);
      item->setId(cart->number());
      item->setText(5,cart->title()+" -> "+cut->description());
      delete cut;
      delete cart;
    }
  }
  delete dialog;

  bool track_selected=false;
  item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(!item->text(5).isEmpty()) {
      track_selected=true;
    }
    item=(RDListViewItem *)item->nextSibling();
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
  RDListGroups *list_groups=new RDListGroups(rip_group_text,lib_user->name(),
					     this);
  if(list_groups->exec()<0) {
    delete list_groups;
    return;
  }
  delete list_groups;
  RDGroup *group=new RDGroup(*rip_group_text);

  //
  // Reserve Carts
  //
  unsigned count=0;
  std::vector<unsigned> cart_nums;
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      count++;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  if(!group->reserveCarts(&cart_nums,rdstation_conf->name(),RDCart::Audio,
			  count)) {
    QMessageBox::warning(this,"RDLibrary - "+tr("Error"),
			 tr("Unable to allocate carts in group")+" \""+
			 group->name()+"\".");
    return;
  }

  //
  // Schedule Rips
  //
  count=0;
  item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      rip_cutnames[item->text(0).toUInt()-1]=
	QString().sprintf("%06u_001",cart_nums[count]);
      rip_end_track[item->text(0).toUInt()-1]=item->text(0).toInt();
      item->setId(cart_nums[count]);
      item->setText(5,QString().sprintf("[New Cart %06u] -> Cut 001",
					cart_nums[count]));
      count++;
    }
    item=(RDListViewItem *)item->nextSibling();
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
  RDListGroups *list_groups=new RDListGroups(rip_group_text,lib_user->name(),
					     this);
  if(list_groups->exec()<0) {
    delete list_groups;
    return;
  }
  delete list_groups;
  RDGroup *group=new RDGroup(*rip_group_text);
  std::vector<unsigned> cart_nums;
  unsigned new_cart=0;
  int first_track=-1;

  //
  // Reserve Cart
  //
  if(!group->reserveCarts(&cart_nums,rdstation_conf->name(),RDCart::Audio,1)) {
    QMessageBox::warning(this,"RDLibrary - "+tr("Error"),
			 tr("Unable to allocate cart in group")+" \""+
			 group->name()+"\".");
    return;
  }

  //
  // Schedule Rips
  //
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      if(new_cart==0) {
	first_track=item->text(0).toUInt()-1;
	new_cart=group->nextFreeCart(cart_nums[0]);
	rip_cutnames[item->text(0).toUInt()-1]=
	  QString().sprintf("%06u_001",cart_nums[0]);
	item->setId(cart_nums[0]);
	item->
	  setText(5,QString().sprintf("[New Cart %06u] -> Cut 001",
				      cart_nums[0]));
      }
      else {
	rip_end_track[first_track]=item->text(0).toUInt();
	item->setId(cart_nums[0]);
	item->setText(5,tr("[continued]"));
      }
    }
    item=(RDListViewItem *)item->nextSibling();
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
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      int track=item->text(0).toInt()-1;
      if(rip_wavedata_dialog->exec(rip_wave_datas[track])==0) {
	item->setText(2,rip_wave_datas[track]->title());
      }
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
}


void DiskRipper::clearSelectionData()
{
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      rip_cutnames[item->text(0).toInt()-1]="";
      rip_end_track[item->text(0).toInt()-1]=-1;
      item->setId(0);
      item->setText(5,"");
      item->setSelected(false);
    }
    item=(RDListViewItem *)item->nextSibling();
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
  RDListViewItem *l;

  rip_isrc_read=false;
  rip_cutnames.clear();
  rip_end_track.clear();
  for(unsigned i=0;i<rip_wave_datas.size();i++) {
    delete rip_wave_datas[i];
  }
  rip_wave_datas.clear();
  rip_track_list->clear();
  rip_track=-1;
  rip_setcut_button->setDisabled(true);
  rip_setall_button->setDisabled(true);
  rip_setsingle_button->setDisabled(true);
  rip_cartlabel_button->setDisabled(true);
  rip_clear_button->setDisabled(true);
  for(int i=rip_cdrom->tracks();i>0;i--) {
    rip_cutnames.push_back(QString());
    rip_end_track.push_back(-1);
    rip_wave_datas.push_back(new RDWaveData());
    rip_wave_datas.back()->setTitle(tr("Track")+QString().sprintf(" %d",i+1));
    l=new RDListViewItem(rip_track_list);
    l->setText(0,QString().sprintf("%d",i));
    if(rip_cdrom->isAudio(i)) {
      l->setText(4,tr("Audio Track"));
    }
    else {
      l->setText(4,tr("Data Track"));
    }
    l->setText(1,RDGetTimeLength(rip_cdrom->trackLength(i)));
  }
  rip_cddb_record.clear();
  rip_cdrom->setCddbRecord(&rip_cddb_record);
  rip_cddb_lookup->setCddbRecord(&rip_cddb_record);
  rip_cddb_lookup->
    lookupRecord(rip_cdda_dir.path(),rdlibrary_conf->ripperDevice(),
		 rdlibrary_conf->cddbServer(),8880,
		 RIPPER_CDDB_USER,PACKAGE_NAME,VERSION);
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


void DiskRipper::cddbDoneData(RDCddbLookup::Result result)
{
  switch(result) {
      case RDCddbLookup::ExactMatch:
	if(rip_cdrom->status()!=RDCdPlayer::Ok) {
	  return;
	}
	rip_artist_edit->setText(rip_cddb_record.discArtist());
	rip_album_edit->setText(rip_cddb_record.discAlbum());
	rip_other_edit->setText(rip_cddb_record.discExtended());
	for(int i=0;i<rip_cddb_record.tracks();i++) {
	  rip_track_list->findItem(QString().sprintf("%d",i+1),0)->
	    setText(2,rip_cddb_record.trackTitle(i));
	  rip_track_list->findItem(QString().sprintf("%d",i+1),0)->
	    setText(3,rip_cddb_record.trackExtended(i));
	  rip_wave_datas[i]->setTitle(rip_cddb_record.trackTitle(i));
	  rip_wave_datas[i]->setArtist(rip_cddb_record.discArtist());
	  rip_wave_datas[i]->setAlbum(rip_cddb_record.discAlbum());
	}
	rip_apply_box->setChecked(true);
	rip_apply_box->setEnabled(true);
	rip_apply_label->setEnabled(true);
	break;
      case RDCddbLookup::PartialMatch:
	rip_track=-1;
	printf("Partial Match!\n");
	break;
      default:
	rip_track=-1;
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


void DiskRipper::selectionChangedData()
{
  int count=0;
  int last_track=0;
  bool contiguous=true;
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();

  while(item!=NULL) {
    int track=item->text(0).toInt();
    if(item->isSelected()) {
      if(item->id()>0) {
	FocusSelection(item->id());
	rip_setcut_button->setEnabled(false);
	rip_setall_button->setEnabled(false);
	rip_setsingle_button->setEnabled(false);
	rip_cartlabel_button->setEnabled(true);
	rip_clear_button->setEnabled(true);
	return;
      }
      if((last_track!=0)&&(last_track!=(track-1))) {
	contiguous=false;
      }
      last_track=track;
      count++;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  rip_setcut_button->setEnabled(count==1);
  rip_setall_button->setEnabled(count>0);
  rip_setsingle_button->setEnabled((count>1)&&contiguous);
  rip_cartlabel_button->setEnabled(false);
  rip_clear_button->setEnabled(false);
}


void DiskRipper::doubleClickedData(QListViewItem *item,const QPoint &pt,
				   int col)
{
  setCutButtonData();
}


void DiskRipper::closeData()
{
  RDCart::removePending(rdstation_conf,lib_user,lib_config);
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
  rip_track_label->setGeometry(100,140,100,14);
  rip_track_list->setGeometry(100,156,size().width()-202,size().height()-342);
  rip_diskbar_label->setGeometry(10,size().height()-174,size().width()-110,20);
  rip_disk_bar->setGeometry(10,size().height()-154,size().width()-110,20); 
  rip_trackbar_label->setGeometry(10,size().height()-126,size().width()-110,20);
  rip_track_bar->setGeometry(10,size().height()-106,size().width()-110,20);
  rip_eject_button->setGeometry(10,156,80,50);
  rip_play_button->setGeometry(10,216,80,50);
  rip_stop_button->setGeometry(10,276,80,50);
  rip_setcut_button->setGeometry(size().width()-90,156,80,50);
  rip_setall_button->setGeometry(size().width()-90,216,80,50);
  rip_setsingle_button->setGeometry(size().width()-90,276,80,50);
  rip_cartlabel_button->setGeometry(size().width()-90,420,80,50);
  rip_clear_button->setGeometry(size().width()-90,480,80,50);
  rip_normalizebox_label->setGeometry(30,size().height()-78,85,20);
  rip_normalize_box->setGeometry(10,size().height()-78,20,20);
  rip_normalize_spin->setGeometry(170,size().height()-79,40,20);
  rip_normalize_label->setGeometry(120,size().height()-78,45,20);
  rip_normalize_unit->setGeometry(215,size().height()-78,40,20);
  rip_autotrimbox_label->setGeometry(30,size().height()-54,85,20);
  rip_autotrim_box->setGeometry(10,size().height()-54,20,20);
  rip_autotrim_spin->setGeometry(170,size().height()-54,40,20);
  rip_autotrim_label->setGeometry(120,size().height()-54,45,20);
  rip_autotrim_unit->setGeometry(215,size().height()-54,40,20);
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


void DiskRipper::FocusSelection(int cart_num)
{
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    item->setSelected(item->id()==cart_num);
    item=(RDListViewItem *)item->nextSibling();
  }
}


void DiskRipper::RipTrack(int track,int end_track,QString cutname,QString title)
{
  RDCdRipper *ripper=NULL;
  RDCut *cut=new RDCut(cutname);
  RDCart *cart=new RDCart(cut->cartNumber());
  QString sql;
  RDSqlQuery *q;

  //
  // Create Cut
  //
  sql=QString("insert into CUTS set ")+
    "CUT_NAME=\""+RDEscapeString(cutname)+"\","+
    QString().sprintf("CART_NUMBER=%u,",cut->cartNumber())+
    "DESCRIPTION=\""+tr("Cut")+" 001\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  rip_done=false;
  rip_rip_aborted=false;
  rip_track_number=track;
  rip_title=title;
  rip_cutname=cutname;
  if(title.isEmpty()) {
    rip_trackbar_label->setText(tr("Track Progress")+" - "+tr("Track")+
				QString().sprintf(" %d",track));
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
  QString tmpdir=RDTempDir();
  QString tmpfile=tmpdir+"/"+RIPPER_TEMP_WAV;
  if(rip_profile_rip) {
    ripper=new RDCdRipper(stdout,this);
  }
  else {
    ripper=new RDCdRipper(NULL,this);
  }
  rip_track_bar->setTotalSteps(ripper->totalSteps()+1);
  connect(ripper,SIGNAL(progressChanged(int)),
	  rip_track_bar,SLOT(setProgress(int)));
  connect(rip_rip_button,SIGNAL(clicked()),ripper,SLOT(abort()));
  RDAudioImport *conv=NULL;
  RDSettings *settings=NULL;
  ripper->setDevice(rdlibrary_conf->ripperDevice());
  ripper->setDestinationFile(tmpfile);
  switch((ripper_err=ripper->rip(rip_track_number-1,end_track-1))) {
  case RDCdRipper::ErrorOk:
    conv=new RDAudioImport(rdstation_conf,lib_config,this);
    conv->setSourceFile(tmpfile);
    conv->setCartNumber(cut->cartNumber());
    conv->setCutNumber(cut->cutNumber());
    conv->setUseMetadata(false);
    settings=new RDSettings();
    if(rdlibrary_conf->defaultFormat()==1) {
      settings->setFormat(RDSettings::MpegL2Wav);
    }
    else {
      settings->setFormat(RDSettings::Pcm16);
    }
    settings->setChannels(rip_channels_box->currentText().toInt());
    settings->setSampleRate(lib_system->sampleRate());
    settings->setBitRate(rdlibrary_conf->defaultBitrate());
    if(rip_normalize_box->isChecked()) {
      settings->setNormalizationLevel(rip_normalize_spin->value());
    }
    if(rip_autotrim_box->isChecked()) {
      settings->setAutotrimLevel(rip_autotrim_spin->value());
    }
    conv->setDestinationSettings(settings);
    switch((conv_err=conv->
	    runImport(lib_user->name(),lib_user->password(),
		      &audio_conv_err))) {
    case RDAudioImport::ErrorOk:
      cart->setMetadata(rip_wave_datas[track-1]);
      cut->setDescription(rip_wave_datas[track-1]->title());
      cut->setIsrc(rip_cddb_record.isrc(rip_track_number-1));
      cart->clearPending();
      break;

    default:
      cart->remove(rdstation_conf,lib_user,lib_config);
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
    cart->remove(rdstation_conf,lib_user,lib_config);
    QMessageBox::warning(this,tr("RDLibrary - Ripper Error"),
			 RDCdRipper::errorText(ripper_err));
    break;

  case RDCdRipper::ErrorAborted:
    rip_aborting=true;
    cart->remove(rdstation_conf,lib_user,lib_config);
    break;
  }
  delete ripper;
  unlink(tmpfile);
  rmdir(tmpdir);
  rip_track_bar->setProgress(0);
  rip_track_bar->setPercentageVisible(true);

  delete cart;
  delete cut;
}


void DiskRipper::UpdateRipButton()
{
  bool ready=false;
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    ready=ready||(!item->text(5).isEmpty());
    item=(RDListViewItem *)item->nextSibling();
  }
  rip_rip_button->setEnabled(ready);
}


QString DiskRipper::BuildTrackName(int start_track,int end_track) const
{
  QString ret;
  RDListViewItem *item=(RDListViewItem *)rip_track_list->firstChild();
  while(item!=NULL) {
    if(item->text(0).toInt()==start_track) {
      ret=item->text(2);
    }
    else {
      if((item->text(0).toInt()>start_track)&&
	 (item->text(0).toInt()<=end_track)) {
	ret+=" / "+item->text(2);
      }
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  return ret;
}
