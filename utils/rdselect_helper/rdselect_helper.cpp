// rdselect_help.cpp
//
// SETUID helper script for rdselect(1)
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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
  if(qApp->arguments().size()!=2) {
    fprintf(stderr,"rdselect_helper: you must pass exactly one argument\n");
    exit(RDConfig::RDSelectInvalidArguments);
  }
  if(QString(qApp->arguments().at(1)).contains("/")||
     QString(qApp->arguments().at(1)).contains("..")) {
    fprintf(stderr,"rdselect_helper: invalid configuration name\n");
    exit(RDConfig::RDSelectInvalidName);
  }
  helper_config_filename=
    QString(RD_DEFAULT_RDSELECT_DIR)+"/"+QString(qApp->arguments().at(1));

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
  FILE *f=NULL;

  if(!helper_config->audioStoreMountSource().isEmpty()) {
    //
    // Stop the Automounter
    //
    ControlAutomounter("stop");

    //
    // Update the Automounter
    //
    if((f=fopen(RDSELECT_AUTOMOUNT_CONFIG,"w"))==NULL) {
      fprintf(stderr,
       "rdselect_helper: unable to open automount configuration \"%s\" [%s]\n",
	      RDSELECT_AUTOMOUNT_CONFIG,
	      strerror(errno));
      exit(RDConfig::RDSelectCantAccessAutomount);
    }
    fprintf(f,"%s",RDSELECT_AUTOMOUNT_WARNING);
    QString options="-fstype="+helper_config->audioStoreMountType();
    if(!helper_config->audioStoreMountOptions().isEmpty()) {
      options+=","+helper_config->audioStoreMountOptions();
    }
    fprintf(f,"%s\t%s\t%s\n",
	    (const char *)helper_config->audioRoot().toUtf8(),
	    (const char *)options.toUtf8(),
	    (const char *)helper_config->audioStoreMountSource().toUtf8());
    fclose(f);

    //
    // Restart the Automounter
    //
    ControlAutomounter("start");
  }

  //
  // Start the rivendell service
  //
  unlink(RD_CONF_FILE);
  if(symlink(helper_config_filename.toUtf8(),RD_CONF_FILE)!=0) {
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
  FILE *f=NULL;

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
  // Stop the Automounter
  //
  ControlAutomounter("stop");

  //
  // Unmount the audio store
  //
  if(umount(helper_prev_config->audioRoot().toUtf8())!=0) {
    if(errno!=EINVAL) {  // Ignore the error if we're already unmounted
      fprintf(stderr,"rdselect_helper: unmount of \"%s\" failed [%s]\n",
	      helper_prev_config->audioRoot().toUtf8().constData(),
	      strerror(errno));
      exit(RDConfig::RDSelectAudioUnmountFailed);
    }
  }

  //
  // Update the Automounter
  //
  if((f=fopen(RDSELECT_AUTOMOUNT_CONFIG,"w"))==NULL) {
    fprintf(stderr,
      "rdselect_helper: unable to open automount configuration \"%s\" [%s]\n",
	    RDSELECT_AUTOMOUNT_CONFIG,
	    strerror(errno));
    exit(RDConfig::RDSelectCantAccessAutomount);
  }
  fprintf(f,"%s",RDSELECT_AUTOMOUNT_WARNING);
  fclose(f);

  //
  // Restart the Automounter
  //
  ControlAutomounter("start");
}


void MainObject::ControlAutomounter(const QString &cmd)
{
  QStringList args;

  args.push_back(cmd);
  args.push_back("autofs");
  QProcess *proc=new QProcess(this);
  proc->start("/bin/systemctl",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    fprintf(stderr,"rdselect_helper: systemctl(8) crashed\n");
    exit(RDConfig::RDSelectSystemctlCrashed);
  }
  if(proc->exitCode()!=0) {
    fprintf(stderr,
     "rdselect_helper: automounter control failed [systemctl exit code: %d]\n",
	    proc->exitCode());
  }
  delete proc;
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
      if((f=fopen((QString("/proc/")+dirs[i]+"/cmdline").toUtf8(),"r"))!=NULL) {
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
