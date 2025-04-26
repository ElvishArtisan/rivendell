// check.cpp
//
// Routines for --check for rddbmgr(8)
//
//   (C) Copyright 2018-2025 Fred Gleason <fredg@paravelsystems.com>
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
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QDir>
#include <QProcess>

#include <dbversion.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdhash.h>
#include <rdlog.h>

#include "rddbmgr.h"

#define RDDBMGR_CHARSET_STRING "pRiKaZiDatFoO"
#define RDDBMGR_COLLATION_STRING "pRiKaZiDatBaR"

bool MainObject::Check(QString *err_msg)
{
  if(GetCurrentSchema()!=RD_VERSION_DATABASE) {
    *err_msg="unsupported schema for checking";
    return false;
  }

  //
  // Recover Audio
  //
  if((db_check_all)&&!db_relink_audio.isEmpty()) {
    RelinkAudio(db_relink_audio);
    return true;
  }

  //
  // Check Table Attributes
  //
  if(db_check_all) {
    printf("Checking DB/table attributes...\n");
    CheckTableAttributes();
    printf("done.\n\n");
  }

  //
  // Check for Orphaned Tables
  //
  if(db_check_all||db_check_orphaned_tables) {
    printf("Checking DB tables...\n");
    CheckOrphanedTables();
    printf("done.\n\n");
  }

  //
  // Check for Orphaned Voice Tracks
  //
  if(db_check_all||db_check_orphaned_tracks) {
    printf("Checking voice tracks...\n");
    CheckOrphanedTracks();
    printf("done.\n\n");
  }

  //
  // Check for stale reservations
  //
  if(db_check_all) {
    printf("Checking for stale cart reservations...\n");
    CheckPendingCarts();
    printf("done.\n\n");
  }

  //
  // Check Schedule Code Rules
  //
  if(db_check_all) {
    printf("Checking schedule code rules...\n");
    CheckSchedCodeRules(true);
    printf("done.\n\n");
  }
    
  //
  // Check for orphaned carts
  //
  if(db_check_all||db_check_orphaned_carts) {
    printf("Checking for orphaned carts...\n");
    CheckOrphanedCarts();
    printf("done.\n\n");
  }

  //
  // Check for orphaned cuts
  //
  if(db_check_all||db_check_orphaned_cuts) {
    printf("Checking for orphaned cuts...\n");
    CheckOrphanedCuts();
    printf("done.\n\n");
  }

  //
  // Check Cart->Cut Counts
  //
  if(db_check_all) {
    printf("Checking cart->cuts counters...\n");
    CheckCutCounts();
    printf("done.\n\n");
  }

  //
  // Check Orphaned Audio
  //
  if(db_check_all||db_check_orphaned_audio) {
    printf("Checking for orphaned audio...\n");
    CheckOrphanedAudio();
    printf("done.\n\n");
  }

  //
  // Validating Audio Lengths
  //
  if(db_check_all) {
    printf("Validating audio lengths (this may take some time)...\n");
    ValidateAudioLengths();
    printf("done.\n\n");
  }

  //
  // Validate Strings
  //
  if(db_check_all||db_check_strings) {
    printf("Validating character strings (this may take some time)...\n");
    ValidateDbStrings();
    printf("done.\n\n");
  }

  //
  // Rehash
  //
  if((db_check_all)&&!db_rehash.isEmpty()) {
    printf("Checking hashes...\n");
    Rehash(db_rehash);
    printf("done.\n\n");
  }

  //
  // Check Log Line IDs
  //
  if(db_check_all||db_check_log_line_ids) {
    printf("Checking log line IDs...\n");
    QString sql=QString("select ")+
      "`NAME` "+  // 00
      "from `LOGS` "+
      "order by `NAME`";
    RDSqlQuery *q=new RDSqlQuery(sql);
    while(q->next()) {
      CheckLogLineIds(q->value(0).toString());
    }
    delete q;
    printf("done.\n\n");
  }

  *err_msg="ok";
  return true;
}


void MainObject::CheckTableAttributes()
{
  QString sql;
  RDSqlQuery *q;

  //
  // Per-table Attributes
  //
  sql=QString("select ")+
    "`TABLE_NAME`,"+       // 00
    "`ENGINE`,"+           // 01
    "`TABLE_COLLATION` "+  // 02
    "from `information_schema`.`TABLES` where "+
    "`TABLE_SCHEMA`='"+RDEscapeString(db_mysql_database)+"'";
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    QStringList f0=q->value(2).toString().split("_");
    QString charset=f0.at(0);
    if(q->value(1).toString().toLower()!=db_mysql_engine.toLower()) {
      printf("  Table \"%s\" uses engine type %s, should be %s. Fix? (y/N) ",
	     q->value(0).toString().toUtf8().constData(),
	     q->value(1).toString().toUtf8().constData(),
	     db_mysql_engine.toUtf8().constData());
      fflush(NULL);
      if(UserResponse()) {
	sql=QString("alter table `")+q->value(0).toString()+"` "+
	  "ENGINE="+db_mysql_engine;
	printf("*****************\n");
	printf("SQL: %s\n",sql.toUtf8().constData());
	printf("*****************\n");
	RDSqlQuery::apply(sql);
      }
    }
    if(q->value(2).toString().toLower()!=db_config->mysqlCollation()) {
      printf("  Table \"%s\" uses charset/collation %s/%s, should be utf8mb4/%s. Fix? (y/N) ",
	     q->value(0).toString().toUtf8().constData(),
	     charset.toUtf8().constData(),
	     q->value(2).toString().toUtf8().constData(),
	     db_config->mysqlCollation().toUtf8().constData());
      fflush(NULL);
      if(UserResponse()) {
	RewriteTable(q->value(0).toString(),
		     charset,q->value(2).toString(),
		     "utf8mb4",db_config->mysqlCollation());
      }
    }
  }
  delete q;

  //
  // Database Attributes
  //
  sql=QString("select ")+
    "`SCHEMA_NAME`,"+                 // 00
    "`DEFAULT_CHARACTER_SET_NAME`,"+  // 01
    "`DEFAULT_COLLATION_NAME` "+      // 02
    "from `information_schema`.`SCHEMATA`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(0).toString()==db_mysql_database) {
      if((q->value(1).toString().toLower()!="utf8mb4")||
	 (q->value(2).toString().toLower()!=db_config->mysqlCollation())) {
	printf("  Database uses default charset/collation %s/%s, should be utf8mb4/%s. Fix? (y/N) ",
	       q->value(1).toString().toUtf8().constData(),
	       q->value(2).toString().toUtf8().constData(),
	       db_config->mysqlCollation().toUtf8().constData());
	fflush(NULL);
	if(UserResponse()) {
	  sql=QString("alter database `")+db_mysql_database+"` "+
	    "character set utf8mb4 collate "+db_config->mysqlCollation();
	  RDSqlQuery::apply(sql);	
	}
      }
    }
  }
  delete q;
}


void MainObject::RewriteTable(const QString &tblname,
			      const QString &old_charset,
			      const QString &old_collation,
			      const QString &new_charset,
			      const QString &new_collation)
{
  /*
  printf("RewriteTable('%s','%s','%s','%s','%s')\n",
	 tblname.toUtf8().constData(),
	 old_charset.toUtf8().constData(),
	 old_collation.toUtf8().constData(),
	 new_charset.toUtf8().constData(),
	 new_collation.toUtf8().constData());
  */  
  QString err_msg;
  QProcess *proc=NULL;
  QStringList args;
  QString tempdir=RDTempDir();
  if(tempdir.isEmpty()) {
    return;
  }
  QString filename=tempdir+"/table.sql";
  QString temp1_filename=tempdir+"/table-temp1.sql";
  QString temp2_filename=tempdir+"/table-temp2.sql";
  QString temp3_filename=tempdir+"/table-temp3.sql";
  QString out_filename=tempdir+"/table-out.sql";
  /*
  printf("table \"%s\" using: %s\n",
	 (const char *)tblname.toUtf8(),
	 (const char *)tempdir.toUtf8());
  */

  //
  // Dump Table
  //
  args.clear();
  args.push_back("--opt");
  args.push_back("-h");
  args.push_back(db_mysql_hostname);
  args.push_back("-u");
  args.push_back(db_mysql_loginname);
  args.push_back("-p"+db_mysql_password);
  args.push_back(db_mysql_database);
  args.push_back(tblname);
  proc=new QProcess(this);
  proc->setStandardOutputFile(filename);
  proc->start("mysqldump",args);
  proc->waitForFinished(-1);
  delete proc;

  //
  // Stage 1
  //
  if(!RewriteFile(filename,old_collation,
		  temp1_filename,RDDBMGR_COLLATION_STRING,
		  &err_msg)) {
    fprintf(stderr,"rddbmgr: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }
  if(!RewriteFile(temp1_filename,old_charset,
		  temp2_filename,RDDBMGR_CHARSET_STRING,
		  &err_msg)) {
    fprintf(stderr,"rddbmgr: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Stage 2
  //
  if(!RewriteFile(temp2_filename,RDDBMGR_COLLATION_STRING,
		  temp3_filename,new_collation,
		  &err_msg)) {
    fprintf(stderr,"rddbmgr: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }
  if(!RewriteFile(temp3_filename,RDDBMGR_CHARSET_STRING,
		  out_filename,new_charset,
		  &err_msg)) {
    fprintf(stderr,"rddbmgr: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Push Back Modified Table
  //
  args.clear();
  args.push_back("-h");
  args.push_back(db_mysql_hostname);
  args.push_back("-u");
  args.push_back(db_mysql_loginname);
  args.push_back("-p"+db_mysql_password);
  args.push_back(db_mysql_database);
  proc=new QProcess(this);
  proc->setStandardInputFile(out_filename);
  proc->start("mysql",args);
  proc->waitForFinished(-1);
  if(proc->exitStatus()!=QProcess::NormalExit) {
    fprintf(stderr,
	    "rddbmgr: \"mysql %s\" crashed when rewriting table \"%s\"\n",
	    args.join(" ").toUtf8().constData(),tblname.toUtf8().constData());
    fprintf(stderr,"rddbmgr: source SQL data in \"%s\"\n",
	    out_filename.toUtf8().constData());
    exit(1);
    }
  if(proc->exitCode()!=0) {
    fprintf(stderr,
	    "rddbmgr: \"mysql %s\" returned exit code %d [%s] when rewriting table \"%s\"\n",
	    args.join(" ").toUtf8().constData(),
	    proc->exitCode(),proc->readAllStandardError().constData(),
	    tblname.toUtf8().constData());
    fprintf(stderr,"rddbmgr: source SQL data in \"%s\"\n",
	    out_filename.toUtf8().constData());
    exit(1);
  }
  delete proc;

  //
  // Clean Up
  //
  unlink(filename.toUtf8());
  unlink(temp1_filename.toUtf8());
  unlink(temp2_filename.toUtf8());
  unlink(temp3_filename.toUtf8());
  unlink(out_filename.toUtf8());
  rmdir(tempdir.toUtf8());
}


bool MainObject::RewriteFile(const QString &old_filename,
			     const QString &old_str, 
			     const QString &new_filename,
			     const QString &new_str,
			     QString *err_msg)
{
  QStringList args;
  QProcess *proc=NULL;

  args.clear();
  args.push_back(QString("s/")+old_str+"/"+new_str+"/g");
  args.push_back(old_filename);
  proc=new QProcess(this);
  proc->setStandardOutputFile(new_filename);
  proc->start("sed",args);
  proc->waitForFinished(-1);
  if(proc->exitStatus()!=QProcess::NormalExit) {
    *err_msg=QString("\"sed ")+args.join(" ")+"\" "+
      "crashed when rewriting file \""+old_filename+"\" "+
      "to \""+new_filename+"\"";
    delete proc;
    return false;
  }
  if(proc->exitCode()!=0) {
    *err_msg=QString("\"sed ")+args.join(" ")+"\" "+
      "returned exit code "+QString::asprintf("%d",proc->exitCode())+
      "["+QString::fromUtf8(proc->readAllStandardError())+"] "+
      " when rewriting file \""+old_filename+"\" "+
      "to \""+new_filename+"\"";
    delete proc;
    return false;
  }
  delete proc;

  return true;
}


void MainObject::RelinkAudio(const QString &srcdir) const
{
  QString sql;
  RDSqlQuery *q;

  QDir dir(srcdir);
  QStringList files=dir.entryList(QDir::Files|QDir::Readable|QDir::Hidden);
  for(int i=0;i<files.size();i++) {
    QString filename=dir.path()+"/"+files[i];
    QString hash=RDSha1HashFile(filename);
    QString firstdest;
    bool delete_source=true;

    //
    // Check against audio cuts
    //
    sql=QString("select ")+
      "`CUTS`.`CUT_NAME`,"+  // 00
      "`CART`.`TITLE` "+     // 01
      "from `CUTS` left join `CART` "+
      "on `CUTS`.`CART_NUMBER`=`CART`.`NUMBER` where "+
      "`CUTS`.`SHA1_HASH`='"+RDEscapeString(hash)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      RelinkCut(filename,q->value(0).toString(),q->value(1).toString(),
		&firstdest,&delete_source);
    }

    //
    // Check against RSS posts
    //
    sql=QString("select ")+
      "`FEEDS`.`KEY_NAME`,"+           // 00
      "`PODCASTS`.`ID`,"+              // 01
      "`PODCASTS`.`ITEM_TITLE`,"+      // 02
      "`PODCASTS`.`AUDIO_FILENAME` "+  // 03
      "from `PODCASTS` left join `FEEDS` "+
      "on `FEEDS`.`ID`=`PODCASTS`.`FEED_ID` where "+
      "`PODCASTS`.`SHA1_HASH`='"+RDEscapeString(hash)+"'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      RelinkCast(filename,q->value(0).toString(),q->value(1).toUInt(),
		 q->value(2).toString(),q->value(3).toString(),
		 &firstdest,&delete_source);
    }
    delete q;

    //
    // (Perhaps) delete the source file
    //
    if(db_relink_audio_move&&delete_source) {
      unlink(filename.toUtf8());
    }
  }
}


void MainObject::RelinkCut(const QString &src_filename,const QString &cutname,
			   const QString &title,
			   QString *firstdest,bool *delete_src) const
{
  printf("  Recovering %06u/%03d [%s]...",
	 RDCut::cartNumber(cutname),RDCut::cutNumber(cutname),
	 title.toUtf8().constData());
  fflush(stdout);

  if(db_relink_audio_move) {
    unlink(RDCut::pathName(cutname).toUtf8());
    if(link(src_filename.toUtf8(),RDCut::pathName(cutname).toUtf8())<0) {
      if(errno==EXDEV) {  // We're crossing filesystems, so do a copy
	if(firstdest->isEmpty()) {
	  if(CopyToAudioStore(RDCut::pathName(cutname),src_filename)) {
	    *firstdest=RDCut::pathName(cutname);
	  }
	  else {
	    fprintf(stderr,"unable to copy file \"%s\"\n",
		    src_filename.toUtf8().constData());
	    *delete_src=false;
	  }
	}
	else {
	  unlink(RDCut::pathName(cutname).toUtf8());
	  RDCheckExitCode("RelinkCut() link",
		link((*firstdest).toUtf8(),RDCut::pathName(cutname).toUtf8()));
	}
      }
      else {
	fprintf(stderr,"unable to move file \"%s\" [%s]\n",
		src_filename.toUtf8().constData(),strerror(errno));
	*delete_src=false;
      }
    }
    else {
      RDCheckExitCode("RelinkCut() chown",
		      chown(RDCut::pathName(cutname).toUtf8(),db_config->uid(),
			    db_config->gid()));
      RDCheckExitCode("RelinkCut() chmod",
		      chmod(RDCut::pathName(cutname).toUtf8(),
			    S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP));
    }
  }
  else {
    if(firstdest->isEmpty()) {
      if(CopyToAudioStore(RDCut::pathName(cutname),src_filename)) {
	*firstdest=RDCut::pathName(cutname);
      }
      else {
	fprintf(stderr,"unable to copy file \"%s\"\n",
		src_filename.toUtf8().constData());
      }
    }
    else {
      RDCheckExitCode("RelinkCut() unlink",
		      unlink(RDCut::pathName(cutname).toUtf8()));
      RDCheckExitCode("RelinkCut() link",
		link((*firstdest).toUtf8(),RDCut::pathName(cutname).toUtf8()));
    }
  }
  printf("  done.\n");
}


void MainObject::RelinkCast(const QString &src_filename,const QString &keyname,
			    unsigned cast_id,const QString &title,
			    const QString &audio_filename,
			    QString *firstdest,bool *delete_src) const
{
  QString destpath=QString(RD_AUDIO_ROOT)+"/"+audio_filename;

  printf("  Recovering RSS item %s:%u [%s]...",
	 keyname.toUtf8().constData(),cast_id,title.toUtf8().constData());
  fflush(stdout);

  if(db_relink_audio_move) {
    unlink(destpath.toUtf8());
    if(link(src_filename.toUtf8(),destpath.toUtf8())<0) {
      if(errno==EXDEV) {  // We're crossing filesystems, so do a copy
	if(firstdest->isEmpty()) {
	  if(CopyToAudioStore(destpath,src_filename)) {
	    *firstdest=destpath;
	  }
	  else {
	    fprintf(stderr,"unable to copy file \"%s\"\n",
		    src_filename.toUtf8().constData());
	    *delete_src=false;
	  }
	}
	else {
	  unlink(destpath.toUtf8());
	  RDCheckExitCode("RelinkCast() link",
			  link((*firstdest).toUtf8(),destpath.toUtf8()));
	}
      }
      else {
	fprintf(stderr,"unable to move file \"%s\" [%s]\n",
		src_filename.toUtf8().constData(),strerror(errno));
	*delete_src=false;
      }
    }
    else {
      RDCheckExitCode("RelinkCast() chown",
		      chown(destpath.toUtf8(),db_config->uid(),
			    db_config->gid()));
      chmod(destpath.toUtf8(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    }
  }
  else {
    if(firstdest->isEmpty()) {
      RDCheckExitCode("RelinkCast() unlink",unlink(destpath.toUtf8()));
      if(CopyToAudioStore(destpath,src_filename)) {
	*firstdest=destpath;
      }
      else {
	fprintf(stderr,"unable to copy file \"%s\" [%s]\n",
		src_filename.toUtf8().constData(),strerror(errno));
      }
    }
    else {
      RDCheckExitCode("RelinkCast() unlink",unlink(destpath.toUtf8()));
      RDCheckExitCode("RelinkCast() link",
		      link((*firstdest).toUtf8(),destpath.toUtf8()));
    }
  }
  printf("  done.\n");
}


void MainObject::CheckOrphanedTracks() const
{
  QString sql=QString("select ")+
    "`NUMBER`,"+  // 00
    "`TITLE`,"+   // 01
    "`OWNER` "+   // 02
    "from `CART` where "+
    "(`OWNER`!='')&&"+
    "(`OWNER` is not null)";
  QSqlQuery *q=new QSqlQuery(sql);
  QSqlQuery *q1;

  while(q->next()) {
    sql=QString("select `LINE_ID` from `LOG_LINES` where ")+
      "`LOG_NAME`='"+RDEscapeString(q->value(2).toString())+"' && "+
      QString::asprintf("`CART_NUMBER`=%u",q->value(0).toUInt());
    q1=new QSqlQuery(sql);
    if(!q1->first()) {
      printf("  Found orphaned track %u - \"%s\".  Delete? (y/N) ",
	     q->value(0).toUInt(),q->value(1).toString().toUtf8().constData());
      fflush(NULL);
      if(UserResponse()) {
	RDCart *cart=new RDCart(q->value(0).toUInt());
	cart->remove(NULL,NULL,db_config);
	delete cart;
	RDLog *log=new RDLog(q->value(2).toString());
	if(log->exists()) {
	  log->updateTracks();
	}
	delete log;
      }
    }
    delete q1;
  }
  delete q;
}


void MainObject::CheckCutCounts() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;

  sql=QString("select ")+
    "`NUMBER`,"+        // 00
    "`CUT_QUANTITY`,"+  // 01
    "`TITLE` "+         // 02
    "from `CART`";
  q=new QSqlQuery(sql);
  while(q->next()) {
    sql=QString("select `CUT_NAME` from `CUTS` where ")+
      QString::asprintf("(`CART_NUMBER`=%u)&&",q->value(0).toUInt())+
      "(`LENGTH`>0)";
    q1=new QSqlQuery(sql);
    if(q1->size()!=q->value(1).toInt()) {
      printf("  Cart %u [%s] has invalid cut count, fix (y/N)?",
	     q->value(0).toUInt(),q->value(2).toString().toUtf8().constData());
      if(UserResponse()) {
	RDCart *cart=new RDCart(q->value(0).toUInt());
	cart->updateLength();
	cart->resetRotation();
	delete cart;
      }
    }
    delete q1;
  }
  delete q;
}


void MainObject::CheckPendingCarts() const
{
  QString sql;
  QSqlQuery *q;
  QDateTime now(QDate::currentDate(),QTime::currentTime());

  sql=QString("select `NUMBER` from `CART` where ")+
    "(`PENDING_STATION` is not null)&&"+
    "(`PENDING_DATETIME`<'"+now.addDays(-1).
    toString("yyyy-MM-dd hh:mm:ss")+"')";
  q=new QSqlQuery(sql);
  while(q->next()) {
    printf("  Cart %06u has stale reservation, delete cart(y/N)?",
	   q->value(0).toUInt());
    if(UserResponse()) {
      RDCart::removeCart(q->value(0).toUInt(),NULL,NULL,db_config);
    }
  }
  delete q;
}


void MainObject::CheckOrphanedCarts() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;

  sql=QString("select ")+
    "`CART`.`NUMBER`,"+
    "`CART`.`TITLE` "+
    "from `CART` left join `GROUPS` "+
    "on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` where "+
    "`GROUPS`.`NAME` is null";
  q=new QSqlQuery(sql);
  while(q->next()) {
    printf("  Cart %06u [%s] has missing/invalid group.\n",
	   q->value(0).toUInt(),q->value(1).toString().toUtf8().constData());
    if(db_orphan_group_name.isEmpty()) {
      printf("  Rerun rddbcheck with the --orphan-group= switch to fix.\n\n");
    }
    else {
      printf("  Assign to group \"%s\" (y/N)?",
	     db_orphan_group_name.toUtf8().constData());
      if(UserResponse()) {
	sql=QString("update `CART` set ")+
	  "`GROUP_NAME`='"+RDEscapeString(db_orphan_group_name)+"' "+
	  QString::asprintf("where `NUMBER`=%u",q->value(0).toUInt());
	q1=new QSqlQuery(sql);
	delete q1;
      }
      printf("\n");
    }
  }
  delete q;
}


void MainObject::CheckOrphanedCuts() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;
  QSqlQuery *q2;
  QFileInfo *file=NULL;

  sql=QString("select ")+
    "`CUTS`.`CUT_NAME`,"+     // 00
    "`CUTS`.`DESCRIPTION` "+  // 01
    "from `CUTS` left join `CART` "+
    "on `CUTS`.`CART_NUMBER`=`CART`.`NUMBER` "+
    "where `CART`.`NUMBER` is null";
  q=new QSqlQuery(sql);
  while(q->next()) {
    printf("  Cut %s [%s] is orphaned.\n",
	   q->value(0).toString().toUtf8().constData(),
	   q->value(1).toString().toUtf8().constData());
    //
    // Try to repair it
    //
    sql=QString::asprintf("select `NUMBER` from `CART` where `NUMBER`=%d",
			  q->value(0).toString().left(6).toUInt());
    q1=new QSqlQuery(sql);
    if(q1->first()) {
      printf("  Repair it (y/N)?");
      if(UserResponse()) {
	//
	// FIXME: Regen Cart Data
	//
	sql=QString::asprintf("update `CUTS` set `CART_NUMBER`=%u where `CUT_NAME`='%s'",
		  q1->value(0).toUInt(),
		  q->value(0).toString().toUtf8().constData());
	q2=new QSqlQuery(sql);
	delete q2;
	delete q1;
	printf("\n");
	continue;
      }
    }
    printf("\n");
    delete q1;

    //
    // Try to recover audio
    //
    file=new QFileInfo(RDCut::pathName(q->value(0).toString()));
    if(!file->exists()) {
      printf("  Clear it (y/N)?");
      if(UserResponse()) {
	sql=QString::asprintf("delete from `CUTS` where `CUT_NAME`='%s'",
			      q->value(0).toString().toUtf8().constData());
	q1=new QSqlQuery(sql);
	delete q1;
      }
    }
    else {
      if(db_dump_cuts_dir.isEmpty()) {
	printf("  Rerun rddbcheck with the --dump-cuts-dir= switch to fix.\n");
      }
      else {
	printf("  Clear it (y/N)?");
	if(UserResponse()) {
	  RDCheckExitCode("CheckOrphanedCuts() system",
			  system(("mv "+file->filePath()+" "+
				  db_dump_cuts_dir+"/").toUtf8()));
	  sql=QString::asprintf("delete from `CUTS` where `CUT_NAME`='%s'",
				q->value(0).toString().toUtf8().constData());
	  q1=new QSqlQuery(sql);
	  delete q1;
	  printf("  Saved audio in \"%s/%s\"\n",
		 db_dump_cuts_dir.toUtf8().constData(),
		 file->fileName().toUtf8().constData());
	}
      }
    }
    printf("\n");
    delete file;
    file=NULL;
  }
  delete q;
}


void MainObject::CheckOrphanedAudio() const
{
  QDir dir(db_config->audioRoot());
  QStringList filters;
  filters.push_back("??????_???.wav");
  QStringList list=dir.entryList(filters);
  for(int i=0;i<list.size();i++) {
    bool ok=false;
    list[i].left(6).toUInt(&ok);
    if(ok) {
      list[i].mid(7,3).toInt(&ok);
      if(ok) {
	QString sql=QString("select ")+
	  "`CUT_NAME` "+
	  "from `CUTS` where "+
	  "`CUT_NAME`='"+RDEscapeString(list.at(i).left(10))+"'";
	QSqlQuery *q=new QSqlQuery(sql);
	if(!q->first()) {
	  printf("  File \"%s/%s\" is orphaned.\n",
		 db_config->audioRoot().toUtf8().constData(),
		 list.at(i).toUtf8().constData());
	  if(db_dump_cuts_dir.isEmpty()) {
	    printf(
	     "  Rerun rddbcheck with the --dump-cuts-dir= switch to fix.\n\n");
	  }
	  else {
	    printf("  Move to \"%s\" (y/N)? ",
		   db_dump_cuts_dir.toUtf8().constData());
	    if(UserResponse()) {
	      RDCheckExitCode("CheckOrphanedAudio() system",
			      system(QString::asprintf("mv %s/%s %s/",
				   db_config->audioRoot().toUtf8().constData(),
				   list.at(i).toUtf8().constData(),
				   db_dump_cuts_dir.toUtf8().constData()).
				   toUtf8()));
	      printf("  Saved audio in \"%s/%s\"\n",
		     db_dump_cuts_dir.toUtf8().constData(),
		     list.at(i).toUtf8().constData());
	    }
	  }
	}
	delete q;
      }
    }
  }
}


void MainObject::CheckOrphanedTables() const
{
  QStringList missing;
  QStringList extra;
  QStringList canonical=GetCanonicalTables(db_current_schema);

  if(canonical.size()==0) {
    printf("  Unsupported schema for analysis, skipping\n");
    return;
  }  
  if(!CheckTableNames(canonical,missing,extra)) {
    for(int i=0;i<missing.size();i++) {
      printf("  WARNING: missing table \"%s\"\n",
	     missing.at(i).toUtf8().constData());
    }
    for(int i=0;i<extra.size();i++) {
      printf("  Found orphaned table \"%s\". Remove (y/N)?",
	     extra.at(i).toUtf8().constData());
      if(UserResponse()) {
	DropTable(extra.at(i));
      }
    }
  }
}


bool MainObject::CheckTableNames(const QStringList &canonical,
				 QStringList &missing,QStringList &extra) const
{
  //
  // Get Existing Tables
  //
  QStringList existing;
  QString sql=QString("show tables");
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    existing.push_back(q->value(0).toString());
  }
  delete q;

  //
  // Find Missing Tables
  //
  for(int i=0;i<canonical.size();i++) {
    if(canonical.at(i).left(1)!="*") {  // Don't look for ephemeral tables
      if(!existing.contains(canonical.at(i))) {
	missing.push_back(canonical.at(i));
      }
    }
  }

  //
  // Find Extra Tables
  //
  bool found;
  for(int i=0;i<existing.size();i++) {
    found=false;
    for(int j=0;j<canonical.size();j++) {
      QRegExp exp(canonical.at(j));
      exp.setPatternSyntax(QRegExp::Wildcard);
      if(exp.indexIn(existing.at(i),0)>=0) {
	found=true;
      }
    }
    if(!found) {
      extra.push_back(existing.at(i));
    }
  }
  return (missing.size()==0)&&(extra.size()==0);
}


QStringList MainObject::GetCanonicalTables(int schema) const
{
  QStringList tables;

  switch(schema) {
  case 275:   // v2.19.x
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUDIO_PORTS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ENCODER_CHANNELS");
    tables.push_back("ENCODER_BITRATES");
    tables.push_back("ENCODER_SAMPLERATES");
    tables.push_back("ENCODERS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("OUTPUTS");
    tables.push_back("NOWNEXT_PLUGINS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STATIONS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    tables.push_back("*_CLK");
    tables.push_back("*_FLG");
    tables.push_back("*_LOG");
    tables.push_back("*_PRE");
    tables.push_back("*_POST");
    tables.push_back("*_RULES");
    tables.push_back("*_SRT");
    tables.push_back("*_STACK");
    break;

  case 308:   // v3.0.x
    tables.push_back("AUDIO_CARDS");
    tables.push_back("AUDIO_INPUTS");
    tables.push_back("AUDIO_OUTPUTS");
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CART_SCHED_CODES");
    tables.push_back("CAST_DOWNLOADS");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_LINES");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ELR_LINES");
    tables.push_back("ENCODER_CHANNELS");
    tables.push_back("ENCODER_BITRATES");
    tables.push_back("ENCODER_SAMPLERATES");
    tables.push_back("ENCODERS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_LINES");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORTER_LINES");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_LINES");
    tables.push_back("LOG_MACHINES");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("OUTPUTS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("PYPAD_INSTANCES");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("RULE_LINES");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STACK_LINES");
    tables.push_back("STACK_SCHED_CODES");
    tables.push_back("STATIONS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    break;

  case 310:   // v3.1.x
    tables.push_back("AUDIO_CARDS");
    tables.push_back("AUDIO_INPUTS");
    tables.push_back("AUDIO_OUTPUTS");
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CART_SCHED_CODES");
    tables.push_back("CAST_DOWNLOADS");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_LINES");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ELR_LINES");
    tables.push_back("ENCODER_CHANNELS");
    tables.push_back("ENCODER_BITRATES");
    tables.push_back("ENCODER_SAMPLERATES");
    tables.push_back("ENCODERS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_LINES");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORTER_LINES");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_LINES");
    tables.push_back("LOG_MACHINES");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("NEXUS_FIELDS");
    tables.push_back("NEXUS_QUEUE");
    tables.push_back("NEXUS_SERVER");
    tables.push_back("NEXUS_STATIONS");
    tables.push_back("OUTPUTS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("PYPAD_INSTANCES");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("RULE_LINES");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STACK_LINES");
    tables.push_back("STACK_SCHED_CODES");
    tables.push_back("STATIONS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    break;

  case 375:   // v3.2.x
    tables.push_back("AUDIO_CARDS");
    tables.push_back("AUDIO_INPUTS");
    tables.push_back("AUDIO_OUTPUTS");
    tables.push_back("AUDIO_PERMS");
    tables.push_back("AUTOFILLS");
    tables.push_back("AUX_METADATA");
    tables.push_back("CART");
    tables.push_back("CARTSLOTS");
    tables.push_back("CART_SCHED_CODES");
    tables.push_back("CLIPBOARD");
    tables.push_back("CLOCKS");
    tables.push_back("CLOCK_LINES");
    tables.push_back("CLOCK_PERMS");
    tables.push_back("CUTS");
    tables.push_back("CUT_EVENTS");
    tables.push_back("DECKS");
    tables.push_back("DECK_EVENTS");
    tables.push_back("DROPBOXES");
    tables.push_back("DROPBOX_PATHS");
    tables.push_back("DROPBOX_SCHED_CODES");
    tables.push_back("ELR_LINES");
    tables.push_back("ENCODER_PRESETS");
    tables.push_back("EVENTS");
    tables.push_back("EVENT_LINES");
    tables.push_back("EVENT_PERMS");
    tables.push_back("EXTENDED_PANELS");
    tables.push_back("EXTENDED_PANEL_NAMES");
    tables.push_back("FEEDS");
    tables.push_back("FEED_IMAGES");
    tables.push_back("FEED_PERMS");
    tables.push_back("GPIO_EVENTS");
    tables.push_back("GPIS");
    tables.push_back("GPOS");
    tables.push_back("GROUPS");
    tables.push_back("HOSTVARS");
    tables.push_back("IMPORTER_LINES");
    tables.push_back("IMPORT_TEMPLATES");
    tables.push_back("INPUTS");
    tables.push_back("ISCI_XREFERENCE");
    tables.push_back("JACK_CLIENTS");
    tables.push_back("LIVEWIRE_GPIO_SLOTS");
    tables.push_back("LOGS");
    tables.push_back("LOGS");
    tables.push_back("LOG_LINES");
    tables.push_back("LOG_MACHINES");
    tables.push_back("LOG_MODES");
    tables.push_back("MATRICES");
    tables.push_back("NEXUS_FIELDS");
    tables.push_back("NEXUS_QUEUE");
    tables.push_back("NEXUS_SERVER");
    tables.push_back("NEXUS_STATIONS");
    tables.push_back("OUTPUTS");
    tables.push_back("PANELS");
    tables.push_back("PANEL_NAMES");
    tables.push_back("PODCASTS");
    tables.push_back("PYPAD_INSTANCES");
    tables.push_back("RDAIRPLAY");
    tables.push_back("RDAIRPLAY_CHANNELS");
    tables.push_back("RDCATCH");
    tables.push_back("RDHOTKEYS");
    tables.push_back("RDLIBRARY");
    tables.push_back("RDLOGEDIT");
    tables.push_back("RDPANEL");
    tables.push_back("RDPANEL_CHANNELS");
    tables.push_back("RECORDINGS");
    tables.push_back("REPLICATORS");
    tables.push_back("REPLICATOR_MAP");
    tables.push_back("REPL_CART_STATE");
    tables.push_back("REPL_CUT_STATE");
    tables.push_back("REPORTS");
    tables.push_back("REPORT_GROUPS");
    tables.push_back("REPORT_SERVICES");
    tables.push_back("REPORT_STATIONS");
    tables.push_back("RULE_LINES");
    tables.push_back("SCHED_CODES");
    tables.push_back("SERVICES");
    tables.push_back("SERVICE_CLOCKS");
    tables.push_back("SERVICE_PERMS");
    tables.push_back("STACK_LINES");
    tables.push_back("STACK_SCHED_CODES");
    tables.push_back("STATIONS");
    tables.push_back("SUPERFEED_MAPS");
    tables.push_back("SWITCHER_NODES");
    tables.push_back("SYSTEM");
    tables.push_back("TRIGGERS");
    tables.push_back("TTYS");
    tables.push_back("USERS");
    tables.push_back("USER_PERMS");
    tables.push_back("USER_SERVICE_PERMS");
    tables.push_back("VERSION");
    tables.push_back("VGUEST_RESOURCES");
    tables.push_back("WEBAPI_AUTHS");
    tables.push_back("WEB_CONNECTIONS");
    break;
  }

  return tables;
}


void MainObject::CheckLogLineIds(const QString &logname) const
{
  QString sql;
  RDSqlQuery *q=NULL;
  int prev_line_id=-1;

  sql=QString("select ")+
    "`ID`,"+       // 00
    "`COUNT`,"+    // 01
    "`LINE_ID` "+  // 02
    "from `LOG_LINES` where "+
    "`LOG_NAME`='"+RDEscapeString(logname)+"' "+
    "order by `LINE_ID`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(2).toInt()==prev_line_id) {
      printf("  Duplicate line ID found in log \"%s\" at line %d\n",
	      logname.toUtf8().constData(),q->value(1).toInt());
      printf("  Repair it (y/N)?");
      if(UserResponse()) {
	//
	// Calculate next unused line ID
	//
	int next_line_id=1;
	sql=QString("select ")+
	  "`LINE_ID` "+
	  "from `LOG_LINES` where "+
	  "`LOG_NAME`='"+RDEscapeString(logname)+"' "+
	  "order by `LINE_ID` desc";
	RDSqlQuery *q1=new RDSqlQuery(sql);
	if(q1->first()) {
	  next_line_id=q1->value(0).toInt()+1;
	}
	delete q1;

	//
	// Rewrite line ID
	//
	sql=QString("update `LOG_LINES` set ")+
	  QString::asprintf("`LINE_ID`=%d ",next_line_id)+
	  "where "+
	  QString::asprintf("`ID`=%d",q->value(0).toInt());
	RDSqlQuery::apply(sql);
	next_line_id++;

	sql=QString("update `LOGS` set ")+
	  QString::asprintf("`NEXT_ID`=%d ",next_line_id)+
	  "where "+
	  "`NAME`='"+RDEscapeString(logname)+"'";
	RDSqlQuery::apply(sql);
	next_line_id++;
      }
    }
    prev_line_id=q->value(2).toInt();
  }
  delete q;
}


void MainObject::ValidateAudioLengths() const
{
  QString sql;
  QSqlQuery *q;
  RDWaveFile *wave=NULL;

  sql=QString("select ")+
    "`CUT_NAME`,"+
    "`CART_NUMBER`,"+
    "`LENGTH` "+
    "from `CUTS` order by `CART_NUMBER`";
  q=new QSqlQuery(sql);
  while(q->next()) {
    if(q->value(2).toInt()>0) {
      wave=new RDWaveFile(RDCut::pathName(q->value(0).toString()));
      if(wave->openWave()) {
	if((int)wave->getExtTimeLength()<(q->value(2).toInt()-100)) {
	  SetCutLength(q->value(0).toString(),wave->getExtTimeLength());
	}
      }
      else {
	SetCutLength(q->value(0).toString(),0);
      }
      delete wave;
    }
  }
  delete q;
}


void MainObject::Rehash(const QString &arg) const
{
  QString sql;
  QSqlQuery *q;
  unsigned cartnum;
  bool ok=false;

  if(arg.toLower()=="all") {
    sql=QString("select `NUMBER` from `CART` where ")+
      QString::asprintf("`TYPE`=%d ",RDCart::Audio)+
      "order by `NUMBER`";
    q=new QSqlQuery(sql);
    while(q->next()) {
      RehashCart(q->value(0).toUInt());
    }
    delete q;
    return;
  }
  cartnum=arg.toUInt(&ok);
  if(ok&&(cartnum>0)&&(cartnum<=RD_MAX_CART_NUMBER)) {
    RehashCart(cartnum);
    return;
  }
  RDCut *cut=new RDCut(arg);
  if(cut->exists()) {
    RehashCut(arg);
  }
  delete cut;
}


void MainObject::RehashCart(unsigned cartnum) const
{
  RDCart *cart=new RDCart(cartnum);
  if(cart->exists()) {
    if(cart->type()==RDCart::Audio) {
      QString sql=QString("select `CUT_NAME` from `CUTS` where ")+
	QString::asprintf("`CART_NUMBER`=%u ",cartnum)+
	"order by `CUT_NAME`";
      QSqlQuery *q=new QSqlQuery(sql);
      while(q->next()) {
	RehashCut(q->value(0).toString());
      }
      delete q;
    }
  }
  else {
    printf("  Cart %06u does not exist.\n",cartnum);
  }
}


void MainObject::RehashCut(const QString &cutnum) const
{
  QString hash=RDSha1HashFile(RDCut::pathName(cutnum),true);
  if(hash.isEmpty()) {
    printf("  Unable to generate hash for \"%s\"\n",
	   RDCut::pathName(cutnum).toUtf8().constData());
  }
  else {
    RDCut *cut=new RDCut(cutnum);
    if(cut->exists()) {
      if(cut->sha1Hash().isEmpty()) {
	cut->setSha1Hash(hash);
      }
      else {
	if(cut->sha1Hash()!=hash) {
	  RDCart *cart=new RDCart(RDCut::cartNumber(cutnum));
	  printf("  Cut %d [%s] in cart %06u [%s] has inconsistent SHA1 hash.  Fix? (y/N) ",
		 cut->cutNumber(),
		 cut->description().toUtf8().constData(),
		 cart->number(),
		 cart->title().toUtf8().constData());
	  fflush(NULL);
	  if(UserResponse()) {
	    cut->setSha1Hash(hash);
	  }
	  delete cart;
	}
      }
    }
    else {
      printf("  Cut \"%s\" does not exist.\n",cutnum.toUtf8().constData());
    }
    delete cut;
  }
}


void MainObject::SetCutLength(const QString &cutname,int len) const
{
  QString sql;
  QSqlQuery *q;
  RDCut *cut=new RDCut(cutname);
  RDCart *cart=new RDCart(cut->cartNumber());

  printf("  Cut %d [%s] in cart %06u [%s] has invalid length.  Correct? (y/N) ",
	 cut->cutNumber(),
	 cut->description().toUtf8().constData(),
	 cart->number(),
	 cart->title().toUtf8().constData());
  fflush(NULL);
  if(UserResponse()) {
    fflush(NULL);
    sql=QString("update `CUTS` set ")+
      "`START_POINT`=0,"+
      QString::asprintf("`END_POINT`=%d,",len)+
      "`FADEUP_POINT`=-1,"+
      "`FADEDOWN_POINT`=-1,"+
      "`SEGUE_START_POINT`=-1,"+
      "`SEGUE_END_POINT`=-1,"+
      "`TALK_START_POINT`=-1,"+
      "`TALK_END_POINT`=-1,"+
      "`HOOK_START_POINT`=-1,"+
      "`HOOK_END_POINT`=-1,"+
      "`PLAY_GAIN`=0,"+
      QString::asprintf("`LENGTH`=%d where ",len)+
      "`CUT_NAME`='"+RDEscapeString(cutname)+"'";
    q=new QSqlQuery(sql);
    delete q;
    cart->updateLength();
    cart->resetRotation();
  }
  delete cart;
  delete cut;
}


bool MainObject::UserResponse() const
{
  char c=0;

  if(db_yes) {
    printf("y\n");
    return true;
  }
  if(db_no) {
    printf("n\n");
    return false;
  }
  while((c!='y')&&(c!='Y')&&(c!='n')&&(c!='N')) {
    RDCheckReturnCode("UserResponse()",scanf("%c",&c),1);
    if((c=='y')||(c=='Y')) {
      RDCheckReturnCode("UserResponse()",scanf("%c",&c),1);
      return true;
    }
    if(c=='\n') {
      return false;
    }
  }
  RDCheckReturnCode("UserResponse()",scanf("%c",&c),1);
  return false;
}


bool MainObject::CopyToAudioStore(const QString &destfile,
				  const QString &srcfile) const
{
  int src_fd=-1;
  struct stat src_stat;
  int dest_fd=-1;
  struct stat dest_stat;
  int n;
  int blksize;
  char *data=NULL;

  if((src_fd=open(srcfile.toUtf8(),O_RDONLY))<0) {
    return false;
  }
  fstat(src_fd,&src_stat);
  mode_t mask=umask(S_IRWXO);
  if((dest_fd=open(destfile.toUtf8(),O_WRONLY|O_CREAT|O_TRUNC,
		   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))<0) {
    close(src_fd);
    return false;
  }
  umask(mask);
  fstat(dest_fd,&dest_stat);
  blksize=src_stat.st_blksize;
  if(dest_stat.st_blksize<blksize) {
    blksize=dest_stat.st_blksize;
  }
  data=(char *)malloc(blksize);
  while((n=read(src_fd,data,blksize))!=0) {
    RDCheckReturnCode("CopyToAudioStore() write",write(dest_fd,data,n),n);
  }
  free(data);
  RDCheckExitCode("CopyToAudioStore() fchown",
		  fchown(dest_fd,db_config->uid(),db_config->gid()));
  fchmod(dest_fd,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
  close(dest_fd);
  close(src_fd);

  return true;
}


void MainObject::CheckSchedCodeRules(bool prompt_user) const
{
  QString sql;
  RDSqlQuery *q;

  //
  // Check that we have at least one schedule code
  //
  sql=QString("select ")+
    "`CODE` "+  // 00
    "from `SCHED_CODES`";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }
  delete q;

  //
  // Check for orphaned rules
  //
  int clock_quan=0;
  sql=QString("select ")+
    "`NAME` "+  // 00
    "from `CLOCKS` order by `NAME`";
  q=new RDSqlQuery(sql);
  QString where_sql="";
  while(q->next()) {
    where_sql+="(`CLOCK_NAME`!='"+RDEscapeString(q->value(0).toString())+"')&&";
    clock_quan++;
  }
  delete q;
  if(clock_quan>0) {
    where_sql=QString("where ")+where_sql.left(where_sql.length()-2);
  }

  sql=QString("select ")+
    "`ID` "  // 00
    "from `RULE_LINES` "+
    where_sql;
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(prompt_user) {
      printf("  Found orphaned scheduler code rules. Fix? (y/N) ");
      fflush(stdout);
    }
    if((!prompt_user)||UserResponse()) {
      sql=QString("delete from `RULE_LINES` where ")+
	QString::asprintf("`ID`=%u || ",q->value(0).toUInt());
      while(q->next()) {
	sql+=QString::asprintf("`ID`=%u || ",q->value(0).toUInt());
      }
      sql=sql.left(sql.length()-4);
    }
    RDSqlQuery::apply(sql);
  }

  //
  // Check for missing rules
  //
  sql=QString("select ")+
    "`NAME` "+  // 00
    "from `CLOCKS` order by `NAME`";
  RDSqlQuery *clock_q=new RDSqlQuery(sql);
  while(clock_q->next()) {
    QString clkname=clock_q->value(0).toString();
    sql=QString("select ")+
      "`CODE` "+  // 00
      "from `SCHED_CODES` order by CODE";
    RDSqlQuery *code_q=new RDSqlQuery(sql);
    while(code_q->next()) {
      QString code=code_q->value(0).toString();
      sql=QString("select ")+
	"`ID` "+  // 00
	"from `RULE_LINES` where "+
	"`CLOCK_NAME`='"+RDEscapeString(clkname)+"' && "+
	"`CODE`='"+RDEscapeString(code)+"'";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	sql=QString("insert into `RULE_LINES` set ")+
	  "`CLOCK_NAME`='"+RDEscapeString(clkname)+"',"+
	  "`CODE`='"+RDEscapeString(code)+"'";
	RDSqlQuery::apply(sql);
      }
      delete q;
    }
    delete code_q;
  }
  delete clock_q;
}


void MainObject::ValidateDbStrings() const
{
  QString sql;
  RDSqlQuery *q;

  sql="show tables";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ValidateTableStrings(q->value(0).toString());
  }
  delete q;
}


void MainObject::ValidateTableStrings(const QString &tbl_name) const
{
  QString sql;
  RDSqlQuery *q;
  QString pri_col;
  QString pri_type;

  sql=QString("describe ")+"`"+tbl_name+"`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(3).toString()=="PRI") {
      pri_col=q->value(0).toString();
      pri_type=q->value(1).toString();
    }
    if(q->value(1).toString().left(8)=="varchar(") {
      ValidateFieldString(pri_col,pri_type,tbl_name,q->value(0).toString());
    }
  }
  delete q;
}


void MainObject::ValidateFieldString(const QString &pri_col,
				     const QString &pri_type,
				     const QString &tbl_name,
				     const QString &col_name) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select `")+col_name+"` from `"+tbl_name+"` where "+
    "`"+col_name+"` like '%\\0'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    FixFieldString(pri_col,pri_type,tbl_name,col_name);
  }
  delete q;
}


void MainObject::FixFieldString(const QString &pri_col,const QString &pri_type,
				const QString &tbl_name,
				const QString &col_name) const
{
  QString sql;
  RDSqlQuery *q;

  if(pri_col==col_name) {
    printf("  Field `%s`.`%s` contains invalid characters but is also primary key, unable to fix!\n",
	   tbl_name.toUtf8().constData(),
	   col_name.toUtf8().constData());
    return;
  }
  if(pri_col.isEmpty()||pri_type.isEmpty()) {
    printf("  Field `%s`.`%s` contains invalid characters but no primary key found, unable to fix!\n",
	   tbl_name.toUtf8().constData(),
	   col_name.toUtf8().constData());
    return;
  }
  printf("  Field `%s`.`%s` contains invalid characters. Fix? (y/N) ",
	 tbl_name.toUtf8().constData(),
	 col_name.toUtf8().constData());
  fflush(NULL);
  if(UserResponse()) {
    sql=QString("select ")+
      "`"+pri_col+"`,"+   // 00
      "`"+col_name+"` "+  // 01
      "from `"+tbl_name+"` where "+
      "`"+col_name+"` like '%\\0'";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("update `")+tbl_name+"` set "+
	"`"+col_name+"`='"+RDEscapeString(q->value(1).toByteArray())+"' ";
      if(pri_type.left(3)=="int") {
	sql+="where `"+pri_col+QString::asprintf("`=%d",q->value(0).toInt());
      }
      else {
	if(pri_type.left(7)=="varchar") {
	  sql+="where `"+pri_col+"`='"+
	    RDEscapeString(q->value(0).toString())+"'";
	}
	else {
	  printf("  Unknown primary key type \"%s\", skipping...\n",
		 pri_type.toUtf8().constData());
	  return;
	}
      }
      RDSqlQuery::apply(sql);
    }
  }
}
