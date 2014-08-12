// rdcheckcuts.cpp
//
// Check Rivendell Cuts for Valid Audio
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcheckcuts.cpp,v 1.1.2.3 2013/11/13 23:36:38 cvs Exp $
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

#include <qapplication.h>

#include <rddb.h>
#include <rdcmd_switch.h>
#include <rdstation.h>
#include <rdaudioinfo.h>
#include <rdcart.h>
#include <rdcut.h>

#include <rdcheckcuts.h>

MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  std::vector<QString> group_names;
  std::vector<QString> bad_cuts;
  QString sql;
  RDSqlQuery *q;
  
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcheckcuts",RDCHECKCUTS_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--group") {
      group_names.push_back(cmd->value(i));
      cmd->setProcessed(i,true);
    }
  }
  if(!cmd->allProcessed()) {
    fprintf(stderr,"rdcheckcuts: unknown option\n");
    exit(256);
  }

  //
  // Open Configuration
  //
  cut_config=new RDConfig();
  cut_config->load();

  //
  // Open Database
  //
  if(!OpenDb()) {
    fprintf(stderr,"rdcheckcuts: unable to open database\n");
    exit(256);
  }

  //
  // Build Group List
  //
  if(group_names.size()==0) {
    sql="select NAME from GROUPS order by NAME";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      group_names.push_back(q->value(0).toString());
    }
    delete q;
  }

  //
  // Scan Cuts
  //
  for(unsigned i=0;i<group_names.size();i++) {
    ValidateGroup(group_names[i],&bad_cuts);
  }

  //
  // Render Output
  //
  for(unsigned i=0;i<bad_cuts.size();i++) {
    RenderCut(bad_cuts[i]);
  }

  exit(0);
}


void MainObject::RenderCut(const QString &cutname)
{
  RDCut *cut=new RDCut(cutname);
  RDCart *cart=new RDCart(cut->cartNumber());

  printf("Cut %03d [%s] in cart %06u [%s] is missing audio\n",
	 cut->cutNumber(),
	 (const char *)cut->description(),
	 cart->number(),
	 (const char *)cart->title());
  delete cart;
  delete cut;
}


bool MainObject::ValidateGroup(const QString &groupname,
			       std::vector<QString> *cutnames)
{
  bool ret=true;
  QString sql;
  RDSqlQuery *q;
  RDStation *station=new RDStation(cut_config->stationName());
  RDAudioInfo *info=new RDAudioInfo(station,cut_config);
  RDAudioInfo::ErrorCode err_code;
  
  sql=QString("select CUTS.CUT_NAME,CUTS.CART_NUMBER,CUTS.LENGTH ")+
    "from CUTS left join CART on CUTS.CART_NUMBER=CART.NUMBER "+
    "where CART.GROUP_NAME=\""+groupname+"\" order by CART_NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(2).toInt()>0) {
      info->setCartNumber(q->value(1).toUInt());
      info->setCutNumber(RDCut::cutNumber(q->value(0).toString()));
      err_code=info->runInfo("user","");
      switch(err_code) {
      case RDAudioInfo::ErrorOk:
	break;
	
      case RDAudioInfo::ErrorNoAudio:
	cutnames->push_back(q->value(0).toString());
	ret=false;
	break;
	
      case RDAudioInfo::ErrorInternal:
      case RDAudioInfo::ErrorUrlInvalid:
      case RDAudioInfo::ErrorService:
      case RDAudioInfo::ErrorInvalidUser:
	ret=false;
	break;
      }
    }
  }
  delete info;
  delete station;
  
  return ret;
}


bool MainObject::OpenDb()
{
  unsigned schema=0;

  QString err(tr("rdcheckcuts: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    return false;
  }
  return true;
}


void MainObject::CloseDb()
{
  QSqlDatabase::removeDatabase(cut_config->mysqlDbname());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
