// audio_cart.cpp
//
// The audio cart editor for RDLibrary.
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
#include <QMessageBox>

#include <rdaudio_exists.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdimport_audio.h>

#include "audio_cart.h"
#include "cdripper.h"
#include "globals.h"
#include "record_cut.h"

bool import_active=false;

AudioCart::AudioCart(AudioControls *controls,RDCart *cart,QString *path,
		     bool new_cart,bool profile_rip,QWidget *parent)
  : RDWidget(parent)
{
  rdcart_import_metadata=true;
  rdcart_controls=controls;
  rdcart_cart=cart;
  rdcart_import_path=path;
  rdcart_new_cart=new_cart;
  rdcart_profile_rip=profile_rip;
  rdcart_modification_allowed=rda->user()->editAudio()&&cart->owner().isEmpty();

  rdcart_use_weighting=true;

  //
  // Dialogs
  //
  rdcart_marker_dialog=
    new RDMarkerDialog("RDLibrary",rda->libraryConf()->outputCard(),
		       rda->libraryConf()->outputPort(),this);

  //
  // Add Cut Button
  //
  add_cut_button=new QPushButton(this);
  add_cut_button->setGeometry(10,0,80,50);
  add_cut_button->setFont(buttonFont());
  add_cut_button->setText(tr("Add"));
  connect(add_cut_button,SIGNAL(clicked()),this,SLOT(addCutData()));

  //
  // Delete Cut Button
  //
  delete_cut_button=new QPushButton(this);
  delete_cut_button->setGeometry(10,60,80,50);
  delete_cut_button->setFont(buttonFont());
  delete_cut_button->setText(tr("Delete"));
  connect(delete_cut_button,SIGNAL(clicked()),this,SLOT(deleteCutData()));

  //
  // Copy Cut Button
  //
  copy_cut_button=new QPushButton(this);
  copy_cut_button->setGeometry(10,120,80,50);
  copy_cut_button->setFont(buttonFont());
  copy_cut_button->setText(tr("Copy"));
  connect(copy_cut_button,SIGNAL(clicked()),this,SLOT(copyCutData()));

  //
  // Paste Cut Button
  //
  paste_cut_button=new QPushButton(this);
  paste_cut_button->setGeometry(10,180,80,50);
  paste_cut_button->setFont(buttonFont());
  paste_cut_button->setText(tr("Paste"));
  connect(paste_cut_button,SIGNAL(clicked()),this,SLOT(pasteCutData()));

  //
  // Cart Cut List
  //
  rdcart_cut_view=new RDTableView(this);
  rdcart_cut_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  rdcart_cut_view->setGeometry(100,0,430,sizeHint().height());
  rdcart_cut_model=NULL;
  connect(rdcart_cut_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  // Record Cut Button
  //
  record_cut_button=
    new RDBiPushButton(tr("Cut Info"),tr("Record"),this,rda->config());
  record_cut_button->setGeometry(550,0,80,50);
  record_cut_button->setFont(buttonFont());
  connect(record_cut_button,SIGNAL(clicked()),this,SLOT(recordCutData()));

  //
  // Edit Cut Button
  //
  edit_cut_button=new QPushButton(this);
  edit_cut_button->setGeometry(550,60,80,50);
  edit_cut_button->setFont(buttonFont());
  edit_cut_button->setText(tr("Edit\nMarkers"));
  connect(edit_cut_button,SIGNAL(clicked()),this,SLOT(editCutData()));

  //
  // Import Cut Button
  //
  import_cut_button=
    new RDBiPushButton(tr("Import"),tr("Export"),this,rda->config());
  import_cut_button->setGeometry(550,120,80,50);
  import_cut_button->setFont(buttonFont());
  connect(import_cut_button,SIGNAL(clicked()),this,SLOT(importCutData()));

  //
  // Rip Cut Button
  //
  rip_cut_button=new QPushButton(this);
  rip_cut_button->setGeometry(550,180,80,50);
  rip_cut_button->setFont(buttonFont());
  rip_cut_button->setText(tr("Rip CD"));
  connect(rip_cut_button,SIGNAL(clicked()),this,SLOT(ripCutData()));

  //
  // Set Control Perms
  //
  add_cut_button->setEnabled(rdcart_modification_allowed);
  delete_cut_button->setEnabled(rdcart_modification_allowed);
  if((cut_clipboard==NULL)||(!rdcart_modification_allowed)) {
    paste_cut_button->setDisabled(true);
  }
  rip_cut_button->setEnabled(rdcart_modification_allowed);
  import_cut_button->setEnabled(rdcart_modification_allowed);
}


AudioCart::~AudioCart()
{
  delete rdcart_marker_dialog;
}


QSize AudioCart::sizeHint() const
{
  return QSize(640,290);
} 


QSizePolicy AudioCart::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


RDCutListModel *AudioCart::cutListModel()
{
  return rdcart_cut_model;
}


void AudioCart::changeCutScheduling(int sched)
{
  RDCutListModel *old_model=rdcart_cut_model;

  rdcart_cut_model=new RDCutListModel(sched,this);
  rdcart_cut_model->setFont(defaultFont());
  rdcart_cut_model->setPalette(palette());
  rdcart_cut_model->setCartNumber(rdcart_cart->number());
  rdcart_cut_view->setModel(rdcart_cut_model);
  rdcart_cut_view->resizeColumnsToContents();
  connect(rdcart_cut_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,
				  const QItemSelection &)),
	  this,
	  SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));

  if(old_model==NULL) {  // Set default cut selection
    if(rdcart_cut_model->rowCount()>0) {
      RDCut *cut=
	new RDCut(rdcart_cut_model->cutName(rdcart_cut_model->index(0,0)));
      if((cut->validity()==RDCut::AlwaysValid)||rdcart_new_cart) {
	rdcart_cut_view->selectRow(0);
      }
      delete cut;
    }
  }
  else {
    delete old_model;
  }

  if(sched) {
    rdcart_cut_view->sortByColumn(12,Qt::AscendingOrder);
  }
  else {
    rdcart_cut_view->sortByColumn(0,Qt::AscendingOrder);
  }

  rdcart_use_weighting=sched!=0;

  UpdateButtons();
}


void AudioCart::addCutData()
{
  QString next_name=RDCut::cutName(rdcart_cart->number(),
		 rdcart_cart->addCut(rda->libraryConf()->defaultFormat(),
				     rda->libraryConf()->defaultBitrate(),
				     rda->libraryConf()->defaultChannels()));
  if(next_name.isEmpty()) {
    QMessageBox::warning(this,tr("RDLibrary - Edit Cart"),
			 tr("This cart cannot contain any additional cuts!"));
    return;
  }
  rdcart_cut_view->clearSelection();

  QModelIndex row=rdcart_cut_model->addCut(next_name);
  rdcart_cut_view->selectRow(row.row());
  rdcart_cut_view->scrollTo(row);
  disk_gauge->update();

  emit cartDataChanged();
}


void AudioCart::deleteCutData()
{
  QModelIndexList rows=rdcart_cut_view->selectionModel()->selectedRows();
  if(rows.size()==0) {
    return;
  }

  QStringList cutnames;
  for(int i=0;i<rows.size();i++) {
    cutnames.push_back(rdcart_cut_model->cutName(rows.at(i)));
  }

  //
  // Prompt for Deletion
  //
  if(cutnames.size()==1) {
    switch(QMessageBox::question(this,"RDLibrary - "+tr("Delete Cut"),
				 tr("Are you sure you want to delete")+" \""+
				 rdcart_cut_model->data(rdcart_cut_model->index(rows.first().row(),1)).toString()+"\"?",
				 QMessageBox::Yes,QMessageBox::No)) {

    case QMessageBox::No:
    case Qt::NoButton:
      return;
      
    default:
      break;
    }
  }
  else {
    if(QMessageBox::question(this,"RDLibrary - "+tr("Delete Cuts"),
			     tr("Are you sure you want to delete")+
			     QString::asprintf(" %d ",cutnames.size())+
			     tr("cuts")+"?",QMessageBox::Yes,
			     QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }
  }

  //
  // Check for RDCatch Events
  //
  for(int i=0;i<cutnames.size();i++) {
    QString sql=QString("select `CUT_NAME` from `RECORDINGS` where ")+
      "`CUT_NAME`='"+RDEscapeString(cutnames.at(i))+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      if(QMessageBox::warning(this,tr("RDCatch Event Exists"),
			      tr("One or more cuts are used in one or more RDCatch events!\nDo you still want to delete?"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::No) {
	delete q;
	return;
      }
    }
    delete q;
  }

  //
  // Check Clipboard
  //
  if(cut_clipboard!=NULL) {
    for(int i=0;i<cutnames.size();i++) {
      if(cutnames.at(i)==cut_clipboard->cutName()) {
	if(QMessageBox::question(this,tr("Empty Clipboard"),
				 tr("Deleting this cut will also empty the clipboard.\nDo you still want to proceed?"),QMessageBox::Yes,QMessageBox::No)==
	   QMessageBox::No) {
	  return;
	}
	delete cut_clipboard;
	cut_clipboard=NULL;
	paste_cut_button->setDisabled(true);
	break;
      }
    }
  }

  //
  // Delete Cuts
  //
  for(int i=0;i<cutnames.size();i++) {
    if(!rdcart_cart->removeCut(rda->station(),rda->user(),cutnames.at(i),
			       rda->config())) {
      QMessageBox::warning(this,tr("RDLibrary"),
			   tr("Unable to delete audio for cut")+
			   QString::asprintf(" %d!",RDCut::cutNumber(cutnames.at(i))));
      return;
    }
    rdcart_cut_model->removeCut(cutnames.at(i));
  }
  rdcart_cart->updateLength(rdcart_controls->enforce_length_box->isChecked(),
			    QTime(0,0,0).msecsTo(rdcart_controls->
					    forced_length_edit->time()));
  rdcart_cart->resetRotation();
  disk_gauge->update();

  emit cartDataChanged();
}


void AudioCart::copyCutData()
{
  QModelIndex row=SingleSelectedLine();
  if(!row.isValid()) {
    return;
  }
  if(cut_clipboard!=NULL) {
    delete cut_clipboard;
  }
  cut_clipboard=new RDCut(rdcart_cut_model->cutName(row));
  paste_cut_button->setEnabled(rdcart_modification_allowed);
}


void AudioCart::pasteCutData()
{
  QModelIndex row=SingleSelectedLine();

  if(!row.isValid()) {
    return;
  }
  if(!cut_clipboard->exists()) {
    QMessageBox::information(this,tr("Clipboard Empty"),
			     tr("Clipboard is currently empty."));
    delete cut_clipboard;
    cut_clipboard=NULL;
    paste_cut_button->setDisabled(true);
    return;
  }
  if(QFile::exists(RDCut::pathName(rdcart_cut_model->cutName(row)))) {
    if(QMessageBox::warning(this,tr("Audio Exists"),
			    tr("This will overwrite the existing recording.\nDo you want to proceed?"),
			    QMessageBox::Yes,
			    QMessageBox::No)==QMessageBox::No) {
      return;
    }
  }
  cut_clipboard->copyTo(rda->station(),rda->user(),
			rdcart_cut_model->cutName(row),rda->config());
  rdcart_cart->updateLength(rdcart_controls->enforce_length_box->isChecked(),
			    QTime(0,0,0).msecsTo(rdcart_controls->
					    forced_length_edit->time()));
  rdcart_cart->resetRotation();
  rdcart_cut_model->refresh(row);
  disk_gauge->update();

  emit cartDataChanged();
}


void AudioCart::editCutData()
{
  QModelIndex row=SingleSelectedLine();

  if(!row.isValid()) {
    return;
  }
  QString cutname=rdcart_cut_model->cutName(row);
  if(!RDAudioExists(cutname)) {
    QMessageBox::information(this,"RDLibrary",
                            tr("No audio is present in the cut!"));
    return;
  }
  if(rdcart_marker_dialog->
     exec(RDCut::cartNumber(cutname),RDCut::cutNumber(cutname),
	  !rda->user()->editAudio())) {
    emit cartDataChanged();
    rdcart_cart->updateLength(rdcart_controls->enforce_length_box->isChecked(),
			      QTime(0,0,0).msecsTo(rdcart_controls->
					      forced_length_edit->time()));
    QModelIndex index=rdcart_cut_model->refresh(row);
    if(index.isValid()) {
      rdcart_cut_view->selectRow(index.row());
    }
  }  
}


void AudioCart::recordCutData()
{
  QModelIndex row=SingleSelectedLine();

  if(!row.isValid()) {
    return;
  }
  QString cutname=rdcart_cut_model->cutName(row);
  RecordCut *cut=new RecordCut(rdcart_cart,cutname,rdcart_use_weighting,this);
  cut->exec();
  delete cut;
  rdcart_cut_model->refresh(cutname);
  rdcart_cut_view->selectRow(rdcart_cut_model->row(cutname).row());
  if(cut_clipboard==NULL) {
    paste_cut_button->setDisabled(true);
  }
  disk_gauge->update();
  emit cartDataChanged();
  rdcart_cart->updateLength(rdcart_controls->enforce_length_box->isChecked(),
			    QTime(0,0,0).msecsTo(rdcart_controls->
					    forced_length_edit->time()));
}


void AudioCart::doubleClickedData(const QModelIndex &index)
{
  recordCutData();
}


void AudioCart::selectionChangedData(const QItemSelection &before,
				     const QItemSelection &after)
{
  UpdateButtons();
}


void AudioCart::ripCutData()
{
  int track;
  QString cutname;
  QString title;
  QString artist;
  QString album;
  QString label;

  QModelIndex row=SingleSelectedLine();

  if(!row.isValid()) {
    return;
  }

  cutname=rdcart_cut_model->cutName(row);
  RDDiscRecord *rec=new RDDiscRecord();
  CdRipper *ripper=new CdRipper(cutname,rec,rda->libraryConf(),rdcart_profile_rip);
  if((track=ripper->exec(&title,&artist,&album,&label))>=0) {
    if((rdcart_controls->title_edit->text().isEmpty()||
	(rdcart_controls->title_edit->text()==tr("[new cart]")))&&
       (!title.isEmpty())) {
      rdcart_controls->title_edit->setText(title);
    }
    rdcart_controls->artist_edit->setText(artist);
    rdcart_controls->album_edit->setText(album);
    rdcart_controls->label_edit->setText(label);
    RDCut *cut=new RDCut(cutname);
    cut->setIsrc(rec->isrc(track));
    cut->setRecordingMbId(rec->trackRecordingMbId(track));
    cut->setReleaseMbId(rec->discReleaseMbId());
    delete cut;
  }
  if(cut_clipboard==NULL) {
    paste_cut_button->setDisabled(true);
  }
  emit cartDataChanged();
  delete ripper;
  delete rec;
  disk_gauge->update();
  rdcart_cart->updateLength(rdcart_controls->enforce_length_box->isChecked(),
			    QTime(0,0,0).msecsTo(rdcart_controls->
					    forced_length_edit->time()));
  row=rdcart_cut_model->refresh(row);
  if(row.isValid()) {
    rdcart_cut_view->selectRow(row.row());
  }
}


void AudioCart::importCutData()
{
  QString cutname;
  RDWaveData wavedata;
  std::vector<QString> cutnames;

  QModelIndex row=SingleSelectedLine();

  if(!row.isValid()) {
    return;
  }

  cutname=rdcart_cut_model->cutName(row);
  RDSettings settings;
  rda->libraryConf()->getSettings(&settings);
  RDImportAudio *import=new RDImportAudio(cutname,rdcart_import_path,
					  &settings,&rdcart_import_metadata,
					  &wavedata,cut_clipboard,
					  &import_active,"RDLibrary",this);
  import->enableAutotrim(rda->libraryConf()->defaultTrimState());
  import->setAutotrimLevel(rda->libraryConf()->trimThreshold());
  import->enableNormalization(rda->libraryConf()->ripperLevel()!=0);
  import->setNormalizationLevel(rda->libraryConf()->ripperLevel());
  if(import->exec(true,true)==0) {
    if(rdcart_controls->title_edit->text().isEmpty()||
       (rdcart_controls->title_edit->text()==tr("[new cart]"))) {
      rdcart_controls->title_edit->setText(wavedata.title());
    }
    if(rdcart_controls->artist_edit->text().isEmpty()) {
      rdcart_controls->artist_edit->setText(wavedata.artist());
    }
    if(rdcart_controls->album_edit->text().isEmpty()) {
      rdcart_controls->album_edit->setText(wavedata.album());
    }

    if(rdcart_controls->year_edit->text().isEmpty()&&
       wavedata.releaseYear()>0) {
      rdcart_controls->year_edit->
	setText(QString::asprintf("%d",wavedata.releaseYear()));
    }
    if(rdcart_controls->song_id_edit->text().isEmpty()) {
      rdcart_controls->song_id_edit->setText(wavedata.songId());
    }
    if(rdcart_controls->label_edit->text().isEmpty()) {
      rdcart_controls->label_edit->setText(wavedata.label());
    }
    if(rdcart_controls->client_edit->text().isEmpty()) {
      rdcart_controls->client_edit->setText(wavedata.client());
    }
    if(rdcart_controls->agency_edit->text().isEmpty()) {
      rdcart_controls->agency_edit->setText(wavedata.agency());
    }
    if(rdcart_controls->publisher_edit->text().isEmpty()) {
      rdcart_controls->publisher_edit->setText(wavedata.publisher());
    }
    if(rdcart_controls->conductor_edit->text().isEmpty()) {
      rdcart_controls->conductor_edit->setText(wavedata.conductor());
    }
    if(rdcart_controls->composer_edit->text().isEmpty()) {
      rdcart_controls->composer_edit->setText(wavedata.composer());
    }
    if(rdcart_controls->user_defined_edit->text().isEmpty()) {
      rdcart_controls->user_defined_edit->setText(wavedata.userDefined());
    }
  }
  if(cut_clipboard==NULL) {
    paste_cut_button->setDisabled(true);
  }
  emit cartDataChanged();
  delete import;
  disk_gauge->update();
  rdcart_cart->updateLength(rdcart_controls->enforce_length_box->isChecked(),
			    QTime(0,0,0).msecsTo(rdcart_controls->
					    forced_length_edit->time()));
  row=rdcart_cut_model->refresh(row);
  if(row.isValid()) {
    rdcart_cut_view->selectRow(row.row());
  }
}


void AudioCart::UpdateButtons()
{
  QModelIndex row=SingleSelectedLine();

  add_cut_button->setEnabled(rdcart_modification_allowed);
  delete_cut_button->setEnabled(row.isValid()&&rdcart_modification_allowed);
  copy_cut_button->setEnabled(row.isValid());
  paste_cut_button->
    setEnabled(row.isValid()&&(cut_clipboard!=NULL)&&
	       rdcart_modification_allowed);
  record_cut_button->setEnabled(row.isValid());
  edit_cut_button->setEnabled(row.isValid());
  import_cut_button->setEnabled(row.isValid()&&rdcart_modification_allowed);
  rip_cut_button->setEnabled(row.isValid()&&rdcart_modification_allowed);
}


QModelIndex AudioCart::SingleSelectedLine() const
{
  if(rdcart_cut_view->selectionModel()->selectedRows().size()!=1) {
    return QModelIndex();
  }
  return rdcart_cut_view->selectionModel()->selectedRows().first();
}
