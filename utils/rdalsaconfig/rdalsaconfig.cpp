// rdalsaconfig.cpp
//
// A Qt-based application to display info about ALSA cards.
//
//   (C) Copyright 2009-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <unistd.h>
#include <sys/types.h>

#include <qapplication.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdcmd_switch.h>

#include <rdapplication.h>

#include <alsaitem.h>
#include <rdalsaconfig.h>

//
// Icons
//
#include "../../icons/rivendell-22x22.xpm"

//
// Globals
//
QString alsa_filename;
bool alsa_autogen=false;
bool alsa_rewrite=false;
bool alsa_manage_daemons=false;
bool alsa_daemon_start_needed=false;

void StopDaemons()
{
  if(alsa_manage_daemons) {
    if(system("systemctl --quiet is-active rivendell")==0) {
      system("systemctl --quiet stop rivendell");
      alsa_daemon_start_needed=true;
    }
  }
}


void StartDaemons()
{
  if(alsa_daemon_start_needed) {
    system("systemctl --quiet start rivendell");
  }
}


MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString err_msg;

  setWindowTitle(tr("RDAlsaConfig")+" v"+VERSION);

  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(rivendell_22x22_xpm));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Open the Database
  //
  rda=new RDApplication("RDAlsaConfig","rdalsaconfig",RDALSACONFIG_USAGE,this);
  if(!rda->open(&err_msg,NULL,false)) {
    QMessageBox::critical(this,"RDAlsaConfig - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // Generate Fonts
  //
  QFont font("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);

  //
  // ALSA Sound Devices
  //
  alsa_system_list=new QListView(this);
  alsa_system_list->setFont(font);
  alsa_system_list->setSelectionMode(QAbstractItemView::MultiSelection);
  alsa_system_label=
    new QLabel(alsa_system_list,tr("ALSA Sound Devices"),this);
  alsa_system_label->setFont(label_font);
  alsa_system_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  alsa_description_label=new QLabel(this);
  alsa_description_label->
    setText(tr("Select the audio devices to dedicate for use with Rivendell. (Devices so dedicated will be unavailable for use with other applications.)"));
  alsa_description_label->setFont(font);
  alsa_description_label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
  alsa_description_label->setWordWrap(true);

  //
  // Save Button
  //
  alsa_save_button=new QPushButton(tr("Save"),this);
  alsa_save_button->setFont(label_font);
  connect(alsa_save_button,SIGNAL(clicked()),this,SLOT(saveData()));

  //
  // Cancel Button
  //
  alsa_cancel_button=new QPushButton(tr("Cancel"),this);
  alsa_cancel_button->setFont(label_font);
  connect(alsa_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Available Devices and Configuration
  //
  alsa_system_model=new RDAlsaModel(rda->system()->sampleRate(),this);
  alsa_system_list->setModel(alsa_system_model);
  LoadConfig(alsa_filename);

  //
  // Daemon Management
  //
  if(alsa_manage_daemons) {
    if(geteuid()!=0) {
      QMessageBox::warning(this,tr("RDAlsaConfig error"),
	     tr("The \"--manage-daemons\" switch requires root permissions."));
      exit(256);
    }
    if(system("systemctl --quiet is-active rivendell")==0) {
      int r=QMessageBox::warning(this,tr("RDAlsaConfig warning"),
	    tr("Rivendell audio will be interrupted while running this program.\nContinue?"),
				     QMessageBox::Yes,QMessageBox::No);
      if(r!=QMessageBox::Yes) {
	exit(256);
      }       
    }
  }
  StopDaemons();
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,400);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::saveData()
{
  SaveConfig(alsa_filename);

  StartDaemons();

  qApp->quit();
}


void MainWidget::cancelData()
{
  StartDaemons();
  qApp->quit();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  alsa_system_label->setGeometry(10,5,size().width()-20,20);
  alsa_description_label->setGeometry(10,25,size().width()-20,50);
  alsa_system_list->
    setGeometry(10,75,size().width()-20,size().height()-130);
  alsa_save_button->
    setGeometry(size().width()-120,size().height()-40,50,30);
  alsa_cancel_button->
    setGeometry(size().width()-60,size().height()-40,50,30);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  int r=QMessageBox::question(this,tr("RDAlsaConfig quit"),
			      tr("Save configuration before exiting?"),
			      QMessageBox::Yes,QMessageBox::No,
			      QMessageBox::Cancel);
  switch(r) {
    case QMessageBox::Yes:
      saveData();
      break;

    case QMessageBox::No:
      cancelData();
      break;

    default:
      break;
  }
}


void MainWidget::LoadConfig(const QString &filename)
{
  if(!alsa_system_model->loadConfig(filename)) {
    return;
  }
  for(int i=0;i<alsa_system_model->rowCount();i++) {
    if(alsa_system_model->isEnabled(i)) {
      alsa_system_list->selectionModel()->
	select(alsa_system_model->index(i,0),QItemSelectionModel::Select);
    }
    else {
      alsa_system_list->selectionModel()->
	select(alsa_system_model->index(i,0),QItemSelectionModel::Deselect);
    }
  }

  /*
  FILE *f=NULL;
  char line[1024];
  int istate=0;
  int port=0;
  QString card_id=0;
  int device=0;
  QStringList list;
  bool active_line=false;
  QModelIndex index;

  if((f=fopen(filename.toUtf8(),"r"))==NULL) {
    return;
  }
  while(fgets(line,1024,f)!=NULL) {
    QString str=line;
    str.replace("\n","");
    if(str==START_MARKER) {
      active_line=true;
    }
    if(str==END_MARKER) {
      active_line=false;
    }
    if((str!=START_MARKER)&&(str!=END_MARKER)) {
      if(active_line) {
	switch(istate) {
	case 0:
	  if(str.left(6)=="pcm.rd") {
	    port=str.mid(6,1).toInt();
	    istate=1;
	  }
	  else {
	    if(str.left(6)=="ctl.rd") {
	      istate=10;
	    }
	    else {
	      alsa_other_lines.push_back(str+"\n");
	    }
	  }
	  break;

	case 1:
	  list=str.split(" ",QString::SkipEmptyParts);
	  if(list[0]=="}") {
	    if((port>=0)&&(port<RD_MAX_CARDS)) {
	      index=alsa_system_model->indexOf(card_id,device);
	      if(index.isValid()) {
		alsa_system_list->selectionModel()->
		  select(index,QItemSelectionModel::Select);
	      }
	    }
	    card_id="";
	    device=0;
	    istate=0;
	  }
	  else {
	    if(list.size()==2) {
	      if(list[0]=="card") {
		card_id=list[1].trimmed();
	      }
	      if(list[0]=="device") {
		device=list[1].toInt();
	      }
	    }
	  }
	  break;

	case 10:
	  if(str.left(1)=="}") {
	    istate=0;
	  }
	  break;
	}
      }
      else {
	alsa_other_lines.push_back(str+"\n");
      }
    }
  }
  fclose(f);
  */
}


void MainWidget::SaveConfig(const QString &filename) const
{
  for(int i=0;i<alsa_system_model->rowCount();i++) {
    QItemSelectionModel *sel=alsa_system_list->selectionModel();
    alsa_system_model->setEnabled(i,sel->isRowSelected(i,QModelIndex()));
  }
  alsa_system_model->saveConfig(filename);


  /*
  QString tempfile=filename+"-temp";
  FILE *f=NULL;

  if((f=fopen(tempfile.toUtf8(),"w"))==NULL) {
    return;
  }
  for(int i=0;i<alsa_other_lines.size();i++) {
    fprintf(f,alsa_other_lines.at(i));
  }
  fprintf(f,"%s\n",START_MARKER);
  QModelIndexList indexes=alsa_system_list->selectionModel()->selectedIndexes();
  for(int i=0;i<indexes.size();i++) {
    fprintf(f,"pcm.rd%d {\n",i);
    fprintf(f,"  type hw\n");
    fprintf(f,"  card %s\n",
	    (const char *)alsa_system_model->card(indexes.at(i))->id().toUtf8());
    fprintf(f,"  device %d\n",alsa_system_model->pcmNumber(indexes.at(i)));
    fprintf(f,"  rate %u\n",rda->system()->sampleRate());
    if(alsa_system_model->card(indexes.at(i))->id()=="Axia") {
      fprintf(f,"  channels 2\n");
    }
    fprintf(f,"}\n");
    fprintf(f,"ctl.rd%d {\n",i);
    fprintf(f,"  type hw\n");
    fprintf(f,"  card %s\n",
	    (const char *)alsa_system_model->card(indexes.at(i))->id().toUtf8());
    fprintf(f,"}\n");
  }
  fprintf(f,"%s\n",END_MARKER);

  fclose(f);
  rename(tempfile.toUtf8(),filename.toUtf8());
  */
}


Autogen::Autogen()
  : QObject()
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("RDAlsaConfig","rdalsaconfig",RDALSACONFIG_USAGE);
  if(!rda->open(&err_msg,NULL,false)) {
    fprintf(stderr,"rdalsaconfig: unable to open database [%s]\n",
	    (const char *)err_msg.toUtf8());
    exit(1);
  }

  StopDaemons();

  RDAlsaModel *model=new RDAlsaModel(rda->system()->sampleRate());
  if(alsa_rewrite) {
    if(!model->loadConfig(alsa_filename)) {
      fprintf(stderr,"rdalsaconfig: unable to load file \"%s\"\n",
	      (const char *)alsa_filename.toUtf8());
      StartDaemons();
      exit(1);
    }
  }
  if(alsa_autogen) {
    for(int i=0;i<model->rowCount();i++) {
      model->setEnabled(i,true);
    }
  }
  if(!model->saveConfig(alsa_filename)) {
    fprintf(stderr,"rdalsaconfig: unable to load file \"%s\"\n",
	    (const char *)alsa_filename.toUtf8());
    StartDaemons();
    exit(1);
  }

  StartDaemons();

  exit(0);
}


int main(int argc,char *argv[])
{
  //
  // Load the command-line arguments
  //
  alsa_filename=RD_ASOUNDRC_FILE;
  RDCmdSwitch *cmd=new RDCmdSwitch(argc,argv,"rdalsaconfig",
				   RDALSACONFIG_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--asoundrc-file") {
      alsa_filename=cmd->value(i);
    }
    if(cmd->key(i)=="--autogen") {
      alsa_autogen=true;
    }
    if(cmd->key(i)=="--rewrite") {
      alsa_rewrite=true;
    }
    if(cmd->key(i)=="--manage-daemons") {
      alsa_manage_daemons=true;
    }
  }

  if(alsa_autogen||alsa_rewrite) {
    QCoreApplication a(argc,argv);
    new Autogen();
    return a.exec();
  }

  //
  // Start GUI
  //
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
