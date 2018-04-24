// rdcheckcuts.cpp
//
// Check Rivendell Cuts for Valid Audio
//
//   (C) Copyright 2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include <rdapplication.h>
#include <rdaudioinfo.h>
#include <rdcart.h>
#include <rdcut.h>

#include "rdcheckcuts.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  new RDApplication(RDApplication::Console,"rdcheckcuts",RDCHECKCUTS_USAGE);

  std::vector<QString> group_names;
  std::vector<QString> bad_cuts;
  QString sql;
  RDSqlQuery *q;
  
  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--group") {
      group_names.push_back(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }
  }
  if(!rda->cmdSwitch()->allProcessed()) {
    fprintf(stderr,"rdcheckcuts: unknown option\n");
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
  RDAudioInfo *info=new RDAudioInfo(rda->station(),rda->config());
  RDAudioInfo::ErrorCode err_code;
  
  sql=QString("select ")+
    "CUTS.CUT_NAME,"+
    "CUTS.CART_NUMBER,"+
    "CUTS.LENGTH "+
    "from CUTS left join CART "+
    "on CUTS.CART_NUMBER=CART.NUMBER "+
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
  
  return ret;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
