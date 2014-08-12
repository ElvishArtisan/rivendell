// rddiscimport.cpp
//
// A Qt-based application for importing TM Century GoldDisc CDs
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddiscimport.cpp,v 1.1.2.10 2014/01/21 21:59:33 cvs Exp $
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
#include <stdio.h>
#include <errno.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qtextcodec.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qfiledialog.h>

#include <rdescape_string.h>
#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rddatedialog.h>
#include <rdgroup.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdaudioimport.h>
#include <rddatedecode.h>
#include <rdlistviewitem.h>
#include <rdprofile.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdwavedata.h>
#include <rdsystem.h>

#include <rddiscimport.h>

MainWidget::MainWidget(QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  dg_user=NULL;
  dg_group=NULL;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"RDDiscImport","\n");
  delete cmd;

  //
  // Set Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  SetCaption();

  //
  // Load Local Configs
  //
  dg_config=new RDConfig();
  dg_config->load();

  //
  // Get Temporary File
  //
  dg_tempfile=RDTempFile();

  //
  // Open Database
  //
  dg_db=QSqlDatabase::addDatabase(dg_config->mysqlDriver());
  if(!dg_db) {
    QMessageBox::warning(this,tr("Database Error"),
		    tr("Can't Connect","Unable to connect to mySQL Server!"));
    exit(0);
  }
  dg_db->setDatabaseName(dg_config->mysqlDbname());
  dg_db->setUserName(dg_config->mysqlUsername());
  dg_db->setPassword(dg_config->mysqlPassword());
  dg_db->setHostName(dg_config->mysqlHostname());
  if(!dg_db->open()) {
    QMessageBox::warning(this,tr("Can't Connect"),
			 tr("Unable to connect to mySQL Server!"));
    dg_db->removeDatabase(dg_config->mysqlDbname());
    exit(0);
  }

  //
  // Fonts
  //
  QFont main_font("helvetica",12,QFont::Normal);
  main_font.setPixelSize(12);
  setFont(main_font);
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  setFont(main_font);

  //
  // Configuration Elements
  //
  dg_system=new RDSystem();
  dg_station=new RDStation(dg_config->stationName(),this);
  dg_library_conf=new RDLibraryConf(dg_config->stationName(),0);
  dg_ripc=new RDRipc(dg_config->stationName(),this);
  connect(dg_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  dg_ripc->connectHost("localhost",RIPCD_TCP_PORT,dg_config->password());

  //
  // Metadata Index Library
  //
  dg_metalibrary=new MetaLibrary();

  //
  // CD Player
  //
  dg_player=new RDCdPlayer(NULL,this);
  connect(dg_player,SIGNAL(mediaChanged()),this,SLOT(mediaChangedData()));
  connect(dg_player,SIGNAL(ejected()),this,SLOT(ejectData()));
  dg_player->setDevice(dg_library_conf->ripperDevice());
  dg_ripper=new RDCdRipper(NULL,this);
  dg_ripper->setDevice(dg_library_conf->ripperDevice());
  dg_ripper->setDestinationFile(dg_tempfile);
  dg_importer=new RDAudioImport(dg_station,dg_config,this);
  dg_importer->setSourceFile(dg_tempfile);
  dg_importer->setUseMetadata(false);
  
  //
  // Index File
  //
  dg_indexfile_edit=new QLineEdit(this);
  dg_indexfile_label=new QLabel(dg_indexfile_edit,tr("Index File")+":",this);
  dg_indexfile_label->setFont(label_font);
  dg_indexfile_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dg_indexfile_button=new QPushButton(tr("Select"),this);
  dg_indexfile_button->setFont(main_font);
  connect(dg_indexfile_button,SIGNAL(clicked()),
	  this,SLOT(indexFileSelectedData()));

  //
  // Group List
  //
  dg_group_box=new QComboBox(this);
  dg_group_label=new QLabel(dg_group_box,tr("Destination Group")+":",this);
  dg_group_label->setFont(label_font);
  dg_group_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(dg_group_box,SIGNAL(activated(int)),
	  this,SLOT(groupActivatedData(int)));

  //
  // User Defined Field
  //
  dg_userdef_edit=new QLineEdit(this);
  dg_userdef_label=new QLabel(dg_userdef_edit,tr("User Defined")+":",this);
  dg_userdef_label->setFont(label_font);
  dg_userdef_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Track List
  //
  dg_track_list=new RDListView(this);
  //db_track_list->setFont(default_font);
  dg_track_list->setAllColumnsShowFocus(true);
  dg_track_list->setItemMargin(5);
  dg_track_list->setSelectionMode(QListView::Single);
  dg_track_list->setSortColumn(-1);
  connect(dg_track_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(trackDoubleClickedData(QListViewItem *,const QPoint &,int)));

  dg_track_list->addColumn("#");
  dg_track_list->setColumnAlignment(0,Qt::AlignHCenter);

  dg_track_list->addColumn(tr("Title"));
  dg_track_list->setColumnAlignment(1,Qt::AlignLeft);

  dg_track_list->addColumn(tr("Artist"));
  dg_track_list->setColumnAlignment(2,Qt::AlignLeft);

  dg_track_list->addColumn(tr("Length"));
  dg_track_list->setColumnAlignment(3,Qt::AlignRight);

  //
  // Progress Bars
  //
  dg_disc_label=new QLabel(tr("Disk Progress"),this);
  dg_disc_label->setFont(label_font);
  dg_disc_label->setDisabled(true);
  dg_disc_bar=new QProgressBar(this);
  dg_disc_bar->setDisabled(true);

  dg_track_label=new QLabel(tr("Track Progress"),this);
  dg_track_label->setFont(label_font);
  dg_track_label->setDisabled(true);
  dg_track_bar=new QProgressBar(this);
  dg_track_bar->setTotalSteps(dg_ripper->totalSteps()+1);
  dg_track_bar->setDisabled(true);
  connect(dg_ripper,SIGNAL(progressChanged(int)),
	  dg_track_bar,SLOT(setProgress(int)));

  //
  // Disc ID
  //
  dg_discid_edit=new QLineEdit(this);
  dg_discid_label=new QLabel(dg_discid_edit,tr("Disc ID")+":",this);
  dg_discid_label->setFont(label_font);
  dg_discid_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(dg_discid_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(discIdChangedData(const QString &)));

  //
  // Rip Button
  //
  dg_rip_button=new QPushButton(tr("Rip Disc"),this);
  dg_rip_button->setFont(label_font);
  connect(dg_rip_button,SIGNAL(clicked()),this,SLOT(ripData()));

  //
  // Channels
  //
  dg_channels_box=new QComboBox(this);
  dg_channels_box->insertItem("1");
  dg_channels_box->insertItem("2");
  dg_channels_box->setCurrentItem(dg_library_conf->defaultChannels()-1);
  dg_channels_label=new QLabel(dg_channels_box,tr("Channels")+":",this);
  dg_channels_label->setFont(label_font);
  dg_channels_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Autotrim Check Box
  //
  dg_autotrim_box=new QCheckBox(tr("Autotrim"),this);
  dg_autotrim_box->setChecked(true);
  dg_autotrim_box->setFont(label_font);
  dg_autotrim_box->setChecked(dg_library_conf->trimThreshold()!=0);
  connect(dg_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimCheckData(bool)));

  //
  // Autotrim Level
  //
  dg_autotrim_spin=new QSpinBox(this);
  dg_autotrim_spin->setRange(-99,0);
  dg_autotrim_spin->setValue(dg_library_conf->trimThreshold()/100);
  dg_autotrim_label=new QLabel(dg_autotrim_spin,tr("Level")+":",this);
  dg_autotrim_label->setFont(label_font);
  dg_autotrim_label->setAlignment(AlignRight|AlignVCenter);
  dg_autotrim_unit=new QLabel(tr("dBFS"),this);
  dg_autotrim_unit->setFont(label_font);
  dg_autotrim_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Normalize Check Box
  //
  dg_normalize_box=new QCheckBox(tr("Normalize"),this);
  dg_normalize_box->setChecked(true);
  dg_normalize_box->setFont(label_font);
  dg_normalize_box->setChecked(dg_library_conf->ripperLevel()!=0);
  connect(dg_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  dg_normalize_spin=new QSpinBox(this);
  dg_normalize_spin->setRange(-30,0);
  dg_normalize_spin->setValue(dg_library_conf->ripperLevel()/100);
  dg_normalize_label=new QLabel(dg_normalize_spin,tr("Level:"),this);
  dg_normalize_label->setFont(label_font);
  dg_normalize_label->setAlignment(AlignRight|AlignVCenter);
  dg_normalize_unit=new QLabel(tr("dBFS"),this);
  dg_normalize_unit->setFont(label_font);
  dg_normalize_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Eject Button
  //
  dg_eject_button=new RDTransportButton(RDTransportButton::Eject,this,"");
  connect(dg_eject_button,SIGNAL(clicked()),dg_player,SLOT(eject()));

  //
  // Close Button
  //
  dg_close_button=new QPushButton(tr("Close"),this);
  dg_close_button->setFont(label_font);
  connect(dg_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  LoadConfig();
  dg_group=new RDGroup(dg_group_name);
  if(!dg_player->open()) {
    QMessageBox::warning(this,"RDDiscImport - "+tr("Ripper Error"),
			 tr("Unable to open CD-ROM device at")+" "+
			 " \""+dg_library_conf->ripperDevice()+"\".");
    exit(256);
  }
  if(dg_metalibrary->load(dg_indexfile_edit->text())&&
			  (!dg_indexfile_edit->text().isEmpty())<0) {
    QMessageBox::warning(this,"RDDiscImport - "+tr("Read Error"),
			 tr("Unable to read index file!"));
    return;
  }
}


QSize MainWidget::sizeHint() const
{
  return QSize(700,700);
}


void MainWidget::indexFileSelectedData()
{
  QString filename;
  int lines;

  filename=QFileDialog::getOpenFileName(dg_indexfile_edit->text(),
					"CSV Files *.csv",this,"",
					tr("RDDiscImport - Open Index File"));
  dg_metalibrary->clear();
  if((lines=dg_metalibrary->load(filename))<0) {
    QMessageBox::warning(this,"RDDiscImport - "+tr("Read Error"),
			 tr("Unable to read index file!"));
    return;
  }
  else {
    dg_indexfile_edit->setText(filename);
    QMessageBox::information(this,"RDDiscImport - "+tr("File Read"),
			     tr("Loaded")+QString().sprintf(" %d ",lines)+
			     tr("records."));
  }
  SaveConfig();
}


void MainWidget::groupActivatedData(int)
{
  if(dg_group!=NULL) {
    delete dg_group;
  }
  dg_group=new RDGroup(dg_group_box->currentText());
  SaveConfig();
}


void MainWidget::autotrimCheckData(bool state)
{
  dg_autotrim_spin->setEnabled(state);
  dg_autotrim_label->setEnabled(state);
  dg_autotrim_unit->setEnabled(state);
}


void MainWidget::trackDoubleClickedData(QListViewItem *it,const QPoint &pt,
					int row)
{
  RDListViewItem *item=(RDListViewItem *)it;
  if(item->id()) {
    item->setTextColor(Qt::gray);
    item->setId(false);
    dg_rip_enableds[item->text(0).toInt()-1]=false;
  }
  else {
    item->setTextColor(Qt::black);
    item->setId(true);
    dg_rip_enableds[item->text(0).toInt()-1]=true;
  }
}


void MainWidget::normalizeCheckData(bool state)
{
  dg_normalize_spin->setEnabled(state);
  dg_normalize_label->setEnabled(state);
  dg_normalize_unit->setEnabled(state);
}


void MainWidget::mediaChangedData()
{
  dg_rip_enableds.clear();
  dg_track_list->clear();
  for(int i=(dg_player->tracks()-1);i>=0;i--) {
    dg_rip_enableds.push_back(false);
    RDListViewItem *item=new RDListViewItem(dg_track_list);
    item->setId(false);
    item->setTextColor(Qt::gray);
    item->setText(0,QString().sprintf("%d",i+1));
    item->setText(1,tr("Track")+QString().sprintf(" %d",i+1));
    item->setText(3,RDGetTimeLength(dg_player->trackLength(i+1),false,false));
  }
  dg_discid_label->setDisabled(dg_player->tracks()==0);
  dg_discid_edit->setDisabled(dg_player->tracks()==0);
}


void MainWidget::discIdChangedData(const QString &str)
{
  MetaRecord *meta=NULL;
  RDListViewItem *item=NULL;
  bool matched=false;

  item=(RDListViewItem *)dg_track_list->firstChild();
  while(item!=NULL) {
    if((meta=dg_metalibrary->track(str,item->text(0).toInt()-1))==NULL) {
      item->setText(1,tr("Track")+
		    QString().sprintf(" %d",item->text(0).toInt()));
      item->setText(2,"");
      item->setTextColor(Qt::gray);
      item->setId(false);
      dg_rip_enableds[item->text(0).toInt()-1]=false;
    }
    else {
      item->setText(1,meta->title());
      item->setText(2,meta->artist());
      item->setTextColor(Qt::black);
      item->setId(true);
      dg_rip_enableds[item->text(0).toInt()-1]=true;
      matched=true;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  dg_rip_button->setEnabled(matched);
}


void MainWidget::ripData()
{
  RDCart *cart=NULL;
  RDCut *cut=NULL;
  RDWaveData *data=NULL;
  unsigned cartnum;
  RDAudioImport::ErrorCode import_err;
  RDAudioConvert::ErrorCode conv_err;

  //
  // Sanity Checks
  //
  if(dg_group->freeCartQuantity()<dg_player->tracks()) {
    QMessageBox::warning(this,"RDDiscImport - "+tr("Ripper Error"),
	tr("There are insufficient free cart numbers available in the")+
			 "\""+dg_group->name()+"\" "+tr("group")+"!");
    return;
  }

  //
  // Lock Down the GUI
  //
  LockGui(false);

  //
  // Load Importer Settings
  //
  RDSettings *s=new RDSettings();
  if(dg_library_conf->defaultFormat()==1) {
    s->setFormat(RDSettings::MpegL2Wav);
  }
  else {
    s->setFormat(RDSettings::Pcm16);
  }
  s->setChannels(dg_channels_box->currentItem()+1);
  s->setSampleRate(dg_system->sampleRate());
  s->setBitRate(dg_library_conf->defaultBitrate());
  if(dg_normalize_box->isChecked()) {
    s->setNormalizationLevel(dg_normalize_spin->value());
  }
  if(dg_autotrim_box->isChecked()) {
    s->setAutotrimLevel(dg_autotrim_spin->value());
  }
  dg_importer->setDestinationSettings(s);

  //
  // Rip and Import
  //
  dg_disc_bar->setTotalSteps(dg_player->tracks());
  for(int i=0;i<dg_player->tracks();i++) {
    if(dg_rip_enableds[i]) {
      MetaRecord *r=dg_metalibrary->track(dg_discid_edit->text(),i);
      if(r!=NULL) {
	dg_disc_bar->setProgress(i);
	dg_track_label->setText(QString().sprintf("Track %d: ",i+1)+
				r->title()+" - "+r->artist());
	dg_ripper->rip(i);
	if((cartnum=dg_group->nextFreeCart())>0) {
	  cart=new RDCart(cartnum);
	  cart->create(dg_group->name(),RDCart::Audio);
	  cart->addCut(dg_library_conf->defaultFormat(),
		       dg_library_conf->defaultBitrate(),
		       dg_channels_box->currentItem()+1,"",r->discId());
	  cut=new RDCut(cartnum,1);
	  dg_importer->setCartNumber(cartnum);
	  dg_importer->setCutNumber(1);
	  if((import_err=dg_importer->
	      runImport(dg_user->name(),dg_user->password(),&conv_err))==
	     RDAudioImport::ErrorOk) {
	    data=new RDWaveData();
	    r->getMetadata(data,dg_player->trackLength(i+1));
	    if(!dg_autotrim_box->isChecked()) {
	      data->setStartPos(0);
	      data->setEndPos(dg_player->trackLength(i+1));
	    }
	    data->setUserDefined(dg_userdef_edit->text().
				 replace("%d",dg_discid_edit->text()).
				 replace("%t",QString().sprintf("%d",i+1)));
	    cart->setMetadata(data);
	    cut->setMetadata(data);
	    delete data;
	  }
	  else {
	    QMessageBox::warning(this,"RDDiscImport - "+tr("Import Error"),
				 tr("Unable to import track audio!")+"\n"+
				 "["+
				 RDAudioImport::errorText(import_err,conv_err)+
				 "].");
	    return;
	  }
	  delete cut;
	  delete cart;
	}
      }
    }
  }
  delete s;

  //
  // Reset Progress Bars
  //
  dg_disc_bar->reset();
  dg_track_bar->reset();

  //
  // Unlock the GUI
  //
  LockGui(true);

  dg_player->eject();
  qApp->processEvents();

  QMessageBox::information(this,"RDDiscImport - "+tr("Ripper Status"),
			   tr("Rip Complete!"));
}


void MainWidget::ejectData()
{
  dg_track_list->clear();
  dg_discid_label->setDisabled(true);
  dg_discid_edit->setDisabled(true);
  dg_discid_edit->clear();
  dg_rip_button->setDisabled(true);
}


void MainWidget::userChangedData()
{
  QStringList groups;

  if(dg_user!=NULL) {
    delete dg_user;
  }
  dg_group_box->clear();

  dg_user=new RDUser(dg_ripc->user());
  
  groups=dg_user->groups();
  for(unsigned i=0;i<groups.size();i++) {
    dg_group_box->insertItem(groups[i]);
    if(dg_group_name==groups[i]) {
      dg_group_box->setCurrentItem(i);
    }
  }

  SetCaption();
}


void MainWidget::quitMainWidget()
{
  if(dg_close_button->isEnabled()) {
    SaveConfig();
    unlink(dg_tempfile);
    qApp->quit();
  }
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  dg_indexfile_label->setGeometry(10,10,115,20);
  dg_indexfile_edit->setGeometry(130,10,size().width()-220,20);
  dg_indexfile_button->setGeometry(size().width()-70,7,60,26);

  dg_group_label->setGeometry(10,40,115,20);
  dg_group_box->setGeometry(130,40,200,20);

  dg_userdef_label->setGeometry(350,40,95,20);
  dg_userdef_edit->setGeometry(450,40,size().width()-460,20);

  dg_track_list->setGeometry(10,72,size().width()-20,size().height()-232);

  dg_disc_label->setGeometry(15,size().height()-157,size().width()-20,20);
  dg_disc_bar->setGeometry(10,size().height()-140,size().width()-20,20);

  dg_track_label->setGeometry(15,size().height()-117,size().width()-20,20);
  dg_track_bar->setGeometry(10,size().height()-100,size().width()-20,20);

  dg_discid_label->setGeometry(10,size().height()-70,60,20);
  dg_discid_edit->setGeometry(75,size().height()-70,60,20);

  dg_rip_button->setGeometry(10,size().height()-43,130,30);

  dg_channels_box->setGeometry(340,size().height()-75,50,20);
  dg_channels_label->setGeometry(250,size().height()-75,75,20);

  dg_autotrim_box->setGeometry(200,size().height()-48,80,15);
  dg_autotrim_spin->setGeometry(340,size().height()-50,40,20);
  dg_autotrim_label->setGeometry(290,size().height()-50,45,20);
  dg_autotrim_unit->setGeometry(385,size().height()-50,40,20);

  dg_normalize_box->setGeometry(200,size().height()-21,113,15);
  dg_normalize_spin->setGeometry(340,size().height()-23,40,20);
  dg_normalize_label->setGeometry(290,size().height()-23,45,20);
  dg_normalize_unit->setGeometry(385,size().height()-23,40,20);

  dg_eject_button->setGeometry(size().width()-200,size().height()-60,80,50);

  dg_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void MainWidget::LockGui(bool state)
{
  dg_indexfile_edit->setEnabled(state);
  dg_indexfile_button->setEnabled(state);
  dg_group_box->setEnabled(state);

  dg_disc_label->setDisabled(state);
  dg_disc_bar->setDisabled(state);

  dg_track_label->setDisabled(state);
  dg_track_bar->setDisabled(state);

  dg_discid_edit->setEnabled(state);
  dg_eject_button->setEnabled(state);

  dg_channels_box->setEnabled(state);

  if(state) {
    dg_autotrim_box->setEnabled(true);
    if(dg_autotrim_box->isChecked()) {
      dg_autotrim_spin->setEnabled(true);
    }
    dg_normalize_box->setEnabled(true);
    if(dg_normalize_box->isChecked()) {
      dg_normalize_spin->setDisabled(true);
    }
  }
  else {
    dg_autotrim_box->setDisabled(true);
    dg_autotrim_spin->setDisabled(true);
    dg_normalize_box->setDisabled(true);
    dg_normalize_spin->setDisabled(true);
  }

  dg_rip_button->setEnabled(state);

  dg_close_button->setEnabled(state);
}


void MainWidget::SetCaption()
{
  QString username=tr("[unknown]");
  if(dg_user!=NULL) {
    username=dg_user->name();
  }
  setCaption(tr("RDDiscImport")+" v"+VERSION+" "+tr("User")+": "+username);
}


void MainWidget::LoadConfig()
{
  RDProfile *p=new RDProfile();
  p->setSource(RDHomeDir()+"/.rddiscimportrc");
  dg_indexfile_edit->setText(p->stringValue("RDDiscImport","IndexFile"));
  dg_group_name=p->stringValue("RDDiscImport","Group");
  dg_userdef_edit->setText(p->stringValue("RDDiscImport","UserDefTemplate",
					  "Ripped from disc %d track %t"));
  delete p;
}


void MainWidget::SaveConfig()
{
  FILE *f=NULL;

  if((f=fopen(RDHomeDir()+"/.rddiscimportrc","w"))==NULL) {
    return;
  }
  fprintf(f,"[RDDiscImport]\n");
  fprintf(f,"IndexFile=%s\n",(const char *)dg_indexfile_edit->text());
  fprintf(f,"Group=%s\n",(const char *)dg_group_box->currentText());
  fprintf(f,"UserDefTemplate=%s\n",(const char *)dg_userdef_edit->text());
  dg_group_name=dg_group_box->currentText();

  fclose(f);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rddiscimport_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
