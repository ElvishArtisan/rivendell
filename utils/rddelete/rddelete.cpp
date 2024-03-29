// rddelete.cpp
//
// A Batch Deleter for Rivendell.
//
//   (C) Copyright 2013-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <dbversion.h>
#include <rdapplication.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rddelete.h>
#include <rdlog.h>

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  int obj_start=qApp->arguments().size();
  del_carts=false;
  del_logs=false;
  del_verbose=false;
  del_continue_after_error=false;
  del_dry_run=false;
  del_obj_ptr=0;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("rddelete","rddelete",
						   RDDELETE_USAGE,false,this));
  if(!rda->open(&err_msg,NULL,true,true)) {
    fprintf(stderr,"rddelete: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Read Command Options
  //
  if(rda->cmdSwitch()->keys()<1) {
    fprintf(stderr,"\n");
    fprintf(stderr,"%s",RDDELETE_USAGE);
    fprintf(stderr,"\n");
    delete rda->cmdSwitch();
    exit(256);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      del_verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--continue-after-error") {
      del_continue_after_error=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--dry-run") {
      del_dry_run=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--carts") {
      del_carts=true;
      obj_start=i+2;
      rda->cmdSwitch()->setProcessed(i,true);
      i=rda->cmdSwitch()->keys();
      break;
    }
    if(rda->cmdSwitch()->key(i)=="--logs") {
      del_logs=true;
      obj_start=i+2;
      rda->cmdSwitch()->setProcessed(i,true);
      i=rda->cmdSwitch()->keys();
      break;
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rddelete: unknown command option \"%s\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(2);
    }
  }

  //
  // Read Object List
  //
  for(int i=obj_start;i<qApp->arguments().size();i++) {
    del_obj_ids.push_back(qApp->arguments().at(i));
    if((del_obj_ids.back()=="--carts")||(del_obj_ids.back()=="--logs")) {
      fprintf(stderr,
	      "rddelete: --carts and --logs switches are mutually exclusive\n");
      exit(256);
    }
  }

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Print Status Messages
  //
  if(del_verbose) {
    printf("\n");
    printf("RDDelete v%s\n",VERSION);
    if(del_carts) {
      printf(" Deleting CARTS\n");
    }
    if(del_logs) {
      printf(" Deleting LOGS\n");
    }
    if(del_continue_after_error) {
      printf(" Continue after error mode is ON\n");
    }
    else {
      printf(" Continue after error mode is OFF\n");
    }
    if(del_dry_run) {
      printf(" Dry run mode is ON\n");
    }
    else {
      printf(" Dry run mode is OFF\n");
    }

    printf("\n");
  }
}


void MainObject::userData()
{
  //
  // Get User Context
  //
  disconnect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));

  //
  // Process Objects
  //
  if(del_carts) {
    if(!rda->user()->editAudio()) {
      fprintf(stderr,"rdimport: user \"%s\" has no edit audio permission\n",
	      rda->user()->name().toUtf8().constData());
      exit(256);
    }
    DeleteCarts();
  }

  if(del_logs) {
    if(!rda->user()->deleteLog()) {
      fprintf(stderr,"rdimport: user \"%s\" has no delete log permission\n",
	      rda->user()->name().toUtf8().constData());
      exit(256);
    }
    DeleteLogs();
  }

  exit(0);
}


void MainObject::DeleteCarts()
{
  unsigned cartnum=0;

  while(GetNextObject(&cartnum)) {
    if(cartnum==0) {
      if(!del_continue_after_error) {
	exit(256);
      }
    }
    else {
      RDCart *cart=new RDCart(cartnum);
      if(cart->exists()) {
	QString title=cart->title();
	if(!cart->owner().isEmpty()) {
	  fprintf(stderr,"rddelete: cart %06u [%s] is a voice track\n",
		  cartnum,title.toUtf8().constData());
	  return;
	}
	if(del_dry_run) {
	  if(del_verbose) {
	    printf("would delete cart %06u [%s]\n",cartnum,
		   title.toUtf8().constData());
	  }
	}
	else {
	  if(cart->remove(rda->station(),rda->user(),rda->config())) {
	    if(del_verbose) {
	      printf("deleted cart %06u [%s]\n",cartnum,
		     title.toUtf8().constData());
	    }
	  }
	  else {
	    fprintf(stderr,"unable to delete cart %06u [%s]\n",cartnum,
		    title.toUtf8().constData());
	  }
	}
      }
      else {
	fprintf(stderr,"cart %06u does not exist\n",cartnum);
	if(!del_continue_after_error) {
	  exit(256);
	}
      }
      delete cart;
    }
  }
}


void MainObject::DeleteLogs()
{
  QString logname;

  while(GetNextObject(&logname)) {
    if(logname.isEmpty()&&(!del_continue_after_error)) {
      exit(256);
    }
    else {
      RDLog *log=new RDLog(logname);
      if(log->exists()) {
	if(del_dry_run) {
	  if(del_verbose) {
	    printf("would delete log \"%s\"\n",logname.toUtf8().constData());
	  }
	}
	else {
	  if(log->remove(rda->station(),rda->user(),rda->config())) {
	    if(del_verbose) {
	      printf("deleted log \"%s\"\n",logname.toUtf8().constData());
	    }
	  }
	  else {
	    fprintf(stderr,"unable to delete log \"%s\".",
		    logname.toUtf8().constData());
	  }
	}
      }
      else {
	fprintf(stderr,"rddelete: log \"%s\" does not exist\n",
		logname.toUtf8().constData());
	if(!del_continue_after_error) {
	  exit(256);
	}
      }
      delete log;
    }
  }
}


bool MainObject::GetNextObject(unsigned *cartnum)
{
  QString id;
  bool ok=false;

  if(!GetNextObject(&id)) {
    return false;
  }
  *cartnum=id.toUInt(&ok);
  if((!ok)||(*cartnum==0)||(*cartnum>RD_MAX_CART_NUMBER)) {
    fprintf(stderr,"invalid cart id: %s\n",id.toUtf8().constData());
    *cartnum=0;
  }

  return true;
}


bool MainObject::GetNextObject(QString *logname)
{
  if(del_obj_ids.size()==0) {
    return GetNextStdinObject(logname);
  }
  if(del_obj_ptr>=del_obj_ids.size()) {
    return false;
  }
  *logname=del_obj_ids[del_obj_ptr++];

  return true;
}


bool MainObject::GetNextStdinObject(QString *logname)
{
  char c;

  *logname="";

  if(read(0,&c,1)<=0) {
    return false;
  }
  while(isspace(c)!=0) {
    if(read(0,&c,1)<=0) {
      return false;
    }
  }
  *logname+=c;
  if(read(0,&c,1)<=0) {
    return true;
  }
  *logname+=c;
  while(isspace(c)==0) {
    if(read(0,&c,1)<=0) {
      return true;
    }
    *logname+=c;
  }
  *logname=logname->trimmed();

  return true;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
