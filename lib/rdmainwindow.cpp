// rdmainwindow.cpp
//
// Top-level window for Rivendell GUI modules.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include "rdmainwindow.h"

RDMainWindow::RDMainWindow(const QString &cmdname,Qt::WindowFlags f)
  : RDWidget(NULL,f)
{
  d_command_name=cmdname;
}


RDMainWindow::RDMainWindow(const QString &cmdname,RDConfig *c,
			    Qt::WindowFlags f)
  : RDWidget(c,NULL,f)
{
  d_command_name=cmdname;

  if(getenv("HOME")!=NULL) {
    d_conf_dir=new QDir(QString(getenv("HOME"))+"/.rivendell");
    d_conf_path=d_conf_dir->path()+"/"+d_command_name+"rc";
    if(!d_conf_dir->mkpath(d_conf_dir->path())) {
      delete d_conf_dir;
      d_conf_dir=NULL;
    }
  }
}


RDMainWindow::~RDMainWindow()
{
  if(d_conf_dir!=NULL) {
    delete d_conf_dir;
  }
}


bool RDMainWindow::loadSettings(bool apply_geometry)
{
  RDProfile *p=NULL;
  int x=0;
  int y=0;
  int w=0;
  int h=0;
  
  if(d_conf_dir==NULL) {
    return false;
  }
  p=new RDProfile();
  if(!p->setSource(d_conf_path)) {
    delete p;
    return false;
  }

  //
  // Set Geometry
  //
  if(apply_geometry) {
    x=p->intValue(d_command_name,"X");
    y=p->intValue(d_command_name,"Y");
    w=p->intValue(d_command_name,"Width");
    h=p->intValue(d_command_name,"Height");
    if((x>=0)&&(y>=0)&&(w>0)&&(h>0)) {
      setGeometry(x,y,w,h);
    }
  }

  loadLocalSettings(p);
  
  delete p;

  return true;
}


bool RDMainWindow::saveSettings() const
{
  if(d_conf_dir==NULL) {
    return false;
  }

  //
  // Save Geometry
  //
  QString temppath=d_conf_path+"-TEMP";
  FILE *f=NULL;
  
  if((f=fopen(temppath.toUtf8(),"w"))==NULL) {
    return false;
  }
  fprintf(f,"[%s]\n",d_command_name.toUtf8().constData());
  fprintf(f,"X=%d\n",geometry().x());
  fprintf(f,"Y=%d\n",geometry().y());
  fprintf(f,"Width=%d\n",geometry().width());
  fprintf(f,"Height=%d\n",geometry().height());

  saveLocalSettings(f);

  fclose(f);
  if(rename(temppath.toUtf8(),d_conf_path.toUtf8())!=0) {
    unlink(temppath.toUtf8());
    return false;
  }

  return true;
}


QString RDMainWindow::commandName() const
{
  return d_command_name;
}


void RDMainWindow::loadLocalSettings(RDProfile *p)
{
}


void RDMainWindow::saveLocalSettings(FILE *f) const
{
}
