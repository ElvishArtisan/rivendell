// rdselect_help.cpp
//
// SETUID helper script for rdselect(1)
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>

#include <qcoreapplication.h>
#include <qdir.h>
#include <qprocess.h>

#include "rd.h"
#include "rdselect_helper.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  setuid(geteuid());  // So the SETUID bit works as expected
  if(getuid()!=0) {
    fprintf(stderr,
	    "rdselect_helper: this program must be installed SETUID root\n");
    exit(RDConfig::RDSelectNotRoot);
  }

  //
  // Process argument
  //
  if(qApp->argc()!=2) {
    fprintf(stderr,"rdselect_helper: you must pass exactly one argument\n");
    exit(RDConfig::RDSelectInvalidArguments);
  }
  if(QString(qApp->argv()[1]).contains("/")||
     QString(qApp->argv()[1]).contains("..")) {
    fprintf(stderr,"rdselect_helper: invalid configuration name\n");
    exit(RDConfig::RDSelectInvalidName);
  }
  helper_config_filename=
    QString(RD_DEFAULT_RDSELECT_DIR)+"/"+QString(qApp->argv()[1]);

  //
  // Load Configurations
  //
  helper_config=new RDConfig();
  helper_config->setFilename(helper_config_filename);
  if(!helper_config->load()) {
    fprintf(stderr,"rdselect_helper: no such configuration\n");
    exit(RDConfig::RDSelectNoSuchConfiguration);
  }
  helper_prev_config=new RDConfig();
  if(!helper_prev_config->load()) {
    fprintf(stderr,"rdselect_helper: no current configuration found\n");
    exit(RDConfig::RDSelectNoCurrentConfig);
  }

  //
  // Check system state
  //
  if(ModulesActive()) {
    fprintf(stderr,"rdselect_helper: one or more rivendell modules active\n");
    exit(RDConfig::RDSelectModulesActive);
  }

  //
  // Shutdown old setup
  //
  Shutdown();

  //
  // Startup new setup
  //
  Startup();

  exit(RDConfig::RDSelectOk);
}


void MainObject::Startup()
{
  QStringList args;
  QProcess *proc=NULL;

  //
  // Mount the audio store
  //
  if(!helper_config->audioStoreMountSource().isEmpty()) {
    args.clear();
    if(!helper_config->audioStoreMountType().isEmpty()) { 
      args.push_back("-t");
      args.push_back(helper_config->audioStoreMountType());
    }
    args.push_back("-o");
    if(helper_config->audioStoreMountOptions().isEmpty()) {
      args.push_back("defaults");
    }
    else {
      args.push_back(helper_config->audioStoreMountOptions());
    }
    args.push_back(helper_config->audioStoreMountSource());
    args.push_back(helper_config->audioRoot());
    proc=new QProcess(this);
    proc->start("/bin/mount",args);
    proc->waitForFinished();
    if(proc->exitStatus()!=QProcess::NormalExit) {
      fprintf(stderr,"rdselect_helper: mount(8) command crashed\n");
      exit(RDConfig::RDSelectMountCrashed);
    }
    if(proc->exitCode()!=0) {
      fprintf(stderr,"rdselect_helper: mount exited with error code %d\n",
	      proc->exitCode());
      exit(RDConfig::RDSelectAudioMountFailed);
    }
    delete proc;
  }

  //
  // Start the rivendell service
  //
  unlink(RD_CONF_FILE);
  if(symlink(helper_config_filename,RD_CONF_FILE)!=0) {
    fprintf(stderr,"rdselect_helper: unable to create new symlink [%s]\n",
	    strerror(errno));
    exit(RDConfig::RDSelectSymlinkFailed);
  }
  args.clear();
  args.push_back("start");
  args.push_back("rivendell");
  proc=new QProcess(this);
  proc->start("/bin/systemctl",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    fprintf(stderr,"rdselect_helper: systemctl(8) crashed\n");
    exit(RDConfig::RDSelectSystemctlCrashed);
  }
  if(proc->exitCode()!=0) {
    fprintf(stderr,"rdselect_helper: rivendell service start failed\n");
    exit(RDConfig::RDSelectRivendellStartupFailed);
  }
  delete proc;
}


void MainObject::Shutdown()
{
  QStringList args;

  //
  // Stop Rivendell Service
  //
  args.push_back("stop");
  args.push_back("rivendell");
  QProcess *proc=new QProcess(this);
  proc->start("/bin/systemctl",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    fprintf(stderr,"rdselect_helper: systemctl(8) crashed\n");
    exit(RDConfig::RDSelectSystemctlCrashed);
  }
  if(proc->exitCode()!=0) {
    fprintf(stderr,"rdselect_helper: rivendell service shutdown failed\n");
    exit(RDConfig::RDSelectRivendellShutdownFailed);
  }
  delete proc;

  //
  // Unmount the audio store
  //
  if(umount(helper_prev_config->audioRoot())!=0) {
    if(errno!=EINVAL) {  // Ignore the error if we're already unmounted
      fprintf(stderr,"rdselect_helper: unmount of \"%s\" failed [%s]\n",
	      (const char *)helper_prev_config->audioRoot(),
	      strerror(errno));
      exit(RDConfig::RDSelectAudioUnmountFailed);
    }
  }
}


bool MainObject::ProcessActive(const QStringList &cmds) const
{
  QStringList dirs;
  QDir *proc_dir=new QDir("/proc");
  bool ok=false;
  FILE *f=NULL;
  char line[1024];
  QString cmdline;

  proc_dir->setFilter(QDir::Dirs);
  dirs=proc_dir->entryList();
  for(int i=0;i<dirs.size();i++) {
    dirs[i].toInt(&ok);
    if(ok) {
      if((f=fopen(QString("/proc/")+dirs[i]+"/cmdline","r"))!=NULL) {
	if(fgets(line,1024,f)!=NULL) {
	  QStringList f1=QString(line).split(" ",QString::SkipEmptyParts);
	  QStringList f2=f1[0].split("/",QString::SkipEmptyParts);
	  cmdline=f2[f2.size()-1];
	  for(int j=0;j<cmds.size();j++) {
	    if(cmdline==cmds[j]) {
	      fclose(f);
	      return true;
	    }
	  }
	}
	fclose(f);
      }
    }
  }

  delete proc_dir;
  return false;
}


bool MainObject::ModulesActive() const
{
  QStringList cmds;

  cmds.push_back("rdadmin");
  cmds.push_back("rdairplay");
  cmds.push_back("rdcartslots");
  cmds.push_back("rdcastmanager");
  cmds.push_back("rdcatch");
  cmds.push_back("rdlibrary");
  cmds.push_back("rdlogedit");
  cmds.push_back("rdlogin");
  cmds.push_back("rdlogmanager");
  cmds.push_back("rdpanel");
  cmds.push_back("rddbmgr");
  cmds.push_back("rdgpimon");
  return ProcessActive(cmds);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
