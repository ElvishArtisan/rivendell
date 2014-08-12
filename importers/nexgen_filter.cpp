// nexgen_filter.cpp
//
// A Library import filter for the Prophet NexGen system
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: nexgen_filter.cpp,v 1.1.2.8 2013/06/20 20:24:45 cvs Exp $
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
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>

#include <qapplication.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qregexp.h>

#include <rddb.h>
#include <rd.h>
#include <rdconfig.h>
#include <rdconf.h>
#include <rdcmd_switch.h>
#include <rdcut.h>
#include <rdwavefile.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdweb.h>

#include <nexgen_filter.h>

//
// Global Variables
//
RDConfig *rdconfig;


MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  QString group_name;
  QString audio_dir;
  QString reject_dir="/dev/null";
  QStringList xml_files;
  bool ok=false;
  char tempdir[PATH_MAX];

  filter_cart_offset=0;
  filter_delete_cuts=false;
  filter_normalization_level=0;
  filter_verbose=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"nexgen_filter",
		    NEXGEN_FILTER_USAGE);
  bool options=true;
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!options) {
      xml_files.push_back(cmd->key(i));
    }
    else {
      if(cmd->key(i)=="--verbose") {
	filter_verbose=true;
	cmd->setProcessed(i,true);
      }
      if(cmd->key(i)=="--group") {
	group_name=cmd->value(i);
	cmd->setProcessed(i,true);
      }
      if(cmd->key(i)=="--audio-dir") {
	audio_dir=cmd->value(i);
	cmd->setProcessed(i,true);
      }
      if(cmd->key(i)=="--reject-dir") {
	reject_dir=cmd->value(i);
	cmd->setProcessed(i,true);
      }
      if(cmd->key(i)=="--cart-offset") {
	filter_cart_offset=cmd->value(i).toInt(&ok);
	if(!ok) {
	  fprintf(stderr,"nexgen_filter: --cart-offset must be an integer\n");
	  exit(256);
	}
	cmd->setProcessed(i,true);
      }
      if(cmd->key(i)=="--delete-cuts") {
	filter_delete_cuts=true;
	cmd->setProcessed(i,true);
      }
      if(cmd->key(i)=="--normalization-level") {
	filter_normalization_level=cmd->value(i).toInt(&ok);
	if(!ok) {
	  fprintf(stderr,"nexgen_filter: --cart-offset must be an integer\n");
	  exit(256);
	}
	if(filter_normalization_level>0) {
	  fprintf(stderr,
		  "nexgen_filter: positive --normalization-level is invalid\n");
	  exit(256);
	}
	cmd->setProcessed(i,true);
      }
      if(!cmd->processed(i)) {
	options=false;
	xml_files.push_back(cmd->key(i));
      }
    }
  }
  delete cmd;

  //
  // Open Config
  //
  rdconfig=new RDConfig(RD_CONF_FILE);
  rdconfig->load();

  //
  // Open Database
  //
  filter_db=QSqlDatabase::addDatabase(rdconfig->mysqlDriver());
  if(!filter_db) {
    fprintf(stderr,"nexgen_filter: can't open mySQL database\n");
    exit(1);
  }
  filter_db->setDatabaseName(rdconfig->mysqlDbname());
  filter_db->setUserName(rdconfig->mysqlUsername());
  filter_db->setPassword(rdconfig->mysqlPassword());
  filter_db->setHostName(rdconfig->mysqlHostname());
  if(!filter_db->open()) {
    fprintf(stderr,"nexgen_filter: unable to connect to mySQL Server\n");
    filter_db->removeDatabase(rdconfig->mysqlDbname());
    exit(1);
  }

  //
  // RIPCD Connection
  //
  filter_ripc=new RDRipc("");
  filter_ripc->connectHost("localhost",RIPCD_TCP_PORT,rdconfig->password());

  //
  // Station Configuration
  //
  filter_rdstation=new RDStation(rdconfig->stationName());

  //
  // Validate Arguments
  //
  if(group_name.isEmpty()) {
    fprintf(stderr,"nexgen_filter: missing group name\n");
    exit(256);
  }
  filter_group=new RDGroup(group_name);
  if(!filter_group->exists()) {
    fprintf(stderr,"nexgen_filter: group \"%s\" does not exist\n",
	    (const char *)group_name);
    exit(256);
  }
  filter_audio_dir=new QDir(audio_dir);
  if(!audio_dir.isEmpty()) {
    if(!filter_audio_dir->exists()) {
      fprintf(stderr,"nexgen_filter: audio directory \"%s\" does not exist\n",
	      (const char *)audio_dir);
      exit(256);
    }
    if(!filter_audio_dir->isReadable()) {
      fprintf(stderr,"nexgen_filter: audio directory \"%s\" is not readable\n",
	      (const char *)audio_dir);
      exit(256);
    }
  }

  //
  // Configure Reject Directory
  //
  if(reject_dir=="/dev/null") {
    filter_reject_dir=NULL;
  }
  else {
    filter_reject_dir=new QDir(reject_dir);
    if(!filter_reject_dir->exists()) {
      fprintf(stderr,"nexgen_filter: reject directory \"%s\" does not exist\n",
	      (const char *)reject_dir);
      exit(256);
    }
  }

  //
  // Create Temp Directory
  //
  strncpy(tempdir,RDTempDir()+"/nexgen_filterXXXXXX",PATH_MAX);
  filter_temp_dir=new QDir(mkdtemp(tempdir));
  filter_temp_audiofile=filter_temp_dir->canonicalPath()+"/audio.dat";

  //
  // Main Loop
  //
  for(unsigned i=0;i<xml_files.size();i++) {
    if(IsXmlFile(xml_files[i])) {
      if(audio_dir.isEmpty()) {
	fprintf(stderr,"unable to process \"%s\" [no --audio-dir specified]\n",
		(const char *)xml_files[i]);
      }
      else {
	ProcessXmlFile(xml_files[i]);
      }
    }
    else {
      ProcessArchive(xml_files[i]);
    }
  }

  //
  // Clean Up
  //
  rmdir(filter_temp_dir->canonicalPath());

  exit(0);
}


void MainObject::ProcessArchive(const QString &filename)
{
  int fd_in=-1;
  int fd_out=-1;
  char tempdir[PATH_MAX];
  char *data=NULL;
  QString dir;
  char header[105];
  uint32_t len;
  QString xmlfile;
  QString wavfile;
  QStringList files;
  struct stat stat;
  blksize_t blksize=1024;
  ssize_t n=0;

  //
  // Allocate Default Buffer
  //
  data=(char *)malloc(1024);

  //
  // Create temporary directory
  //
  snprintf(tempdir,PATH_MAX,"%s/XXXXXX",(const char *)RDTempDir());
  if(mkdtemp(tempdir)==NULL) {
    return;
  }
  dir=tempdir;

  //
  // Open Archive
  //
  if((fd_in=open(filename,O_RDONLY))<0) {
    return;
  }

  //
  // Write Out File Components
  //
  while((read(fd_in,header,104)==104)&&(strncmp(header,"FR:",3)==0)) {
    files.push_back(dir+"/"+RDGetBasePart(QString(header+3).replace("\\","/")));
    if(files.back().right(4).lower()==".xml") {
      xmlfile=files.back();
    }
    if(files.back().right(4).lower()==".wav") {
      wavfile=files.back();
    }
    len=((0xFF&header[103])<<24)+((0xFF&header[102])<<16)+
      ((0xFF&header[101])<<8)+(0xFF&header[100]);
    if((fd_out=open(files.back(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR))<0) {
      fprintf(stderr,"unable to write temporary file \"%s\" [%s].\n",
	      (const char *)files.back(),strerror(errno));
      return;
    }
    if(fstat(fd_out,&stat)==0) {
      blksize=stat.st_blksize;
      data=(char *)realloc(data,blksize);
    }
    for(uint32_t i=blksize;i<len;i+=blksize) {
      n=read(fd_in,data,blksize);
      write(fd_out,data,n);
    }
    n=read(fd_in,data,len%blksize);
    write(fd_out,data,n);
    close(fd_out);
  }
  close(fd_in);

  //
  // Run Import
  //
  if((!xmlfile.isEmpty())&&(!wavfile.isEmpty())) {
    ProcessXmlFile(xmlfile,wavfile,filename);
  }

  //
  // Clean Up
  //
  for(unsigned i=0;i<files.size();i++) {
    unlink(files[i]);
  }
  rmdir(tempdir);
  free(data);
}


void MainObject::ProcessXmlFile(const QString &xml,const QString &wavname,
				const QString &arcname)
{
  RDCart *cart=NULL;
  RDCut *cut=NULL;
  RDWaveData data;
  QString filename;
  int cartnum;
  QString sql;
  RDSqlQuery *q;
  QString delete_cuts_switch="";

  //
  // Read Metadata
  //
  if(!OpenXmlFile(xml,&data,&cartnum,&filename)) {
    fprintf(stderr,"unable to parse XML file \"%s\"\n",(const char *)xml);
    WriteReject(xml);
    return;
  }
  if(!wavname.isEmpty()) {
    filename=wavname;
  }

  //
  // Sanity Checks
  //
  if((cartnum<1)||(cartnum>999999)) {
    fprintf(stderr,"calculated cart number [%d] is invalid\n",cartnum);
    WriteReject(xml);
    return;
  }
  if(filter_group->enforceCartRange()) {
    if((cartnum<(int)filter_group->defaultLowCart())||
       (cartnum>(int)filter_group->defaultHighCart())) {
      fprintf(stderr,
	      "calculated cart number [%d] is invalid for group \"%s\"\n",
	      cartnum,(const char *)filter_group->name());
      WriteReject(xml);
      return;
    }
  }

  //
  // Prepare Audio
  //
  if(!PreprocessAudio(filename)) {
    WriteReject(xml);
    return;
  }

  //
  // Import Audio
  //
  Print(QString().sprintf("Importing cart %06d",cartnum));
  if(!data.title().isEmpty()) {
    Print(QString().sprintf(" [%s",(const char *)data.title()));
    if(!data.artist().isEmpty()) {
      Print(QString().sprintf("/%s",(const char *)data.artist()));
    }
    Print(QString().sprintf("]"));
  }
  if(arcname.isEmpty()) {
    Print(QString().sprintf(" from %s ...",(const char *)filename));
  }
  else {
    Print(QString().sprintf(" from %s ...",(const char *)arcname));
  }
  if(filter_delete_cuts) {
    delete_cuts_switch="--delete-cuts ";
  }
  if(system(QString().sprintf("rdimport --autotrim-level=0 --normalization-level=%d --to-cart=%d ",
			      filter_normalization_level,cartnum)+
	    +delete_cuts_switch+filter_group->name()+" "+
	    filter_temp_audiofile)!=0) {
    Print(QString().sprintf(" aborted.\n"));
    fprintf(stderr,"import of \"%s\" failed\n",(const char *)filename);
    WriteReject(xml);
    return;
  }
  Print(QString().sprintf(" done.\n"));
  unlink(filter_temp_audiofile);

  //
  // Apply Metadata
  //
  cart=new RDCart(cartnum);
  cart->setMetadata(&data);
  delete cart;
  sql=
    QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%d",cartnum)+
    " order by ORIGIN_DATETIME desc";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    cut=new RDCut(q->value(0).toString());
    cut->setMetadata(&data);
    delete cut;
  }
  delete q;
}


bool MainObject::OpenXmlFile(const QString &xml,RDWaveData *data,
			     int *cartnum,QString *filename)
{
  FILE *f=NULL;
  char line[1024];
  int crossfade=-1;
  int fadeup_start=-1;
  int fadeup_len=-1;

  if((f=fopen(xml,"r"))==NULL) {
    return false;
  }
  if(fgets(line,1024,f)==NULL) {
    fclose(f);
    return false;
  }
  if(!QString(line).contains("XMLDAT")) {
    fclose(f);
    return false;
  }
  while(fgets(line,1024,f)!=NULL) {
    ProcessXmlLine(line,data,cartnum,filename,&crossfade,&fadeup_start,
		   &fadeup_len);
  }
  if((fadeup_start>=0)&&(fadeup_len>=0)) {   // Calculate Start Marker
    data->setStartPos(fadeup_start);
    data->setEndPos(data->endPos()+fadeup_start);
    data->setFadeUpPos(fadeup_start+fadeup_len);
  }
  if((crossfade!=-1)&&(data->endPos()>0)) {  // Calculate Segue
    data->setSegueStartPos(data->endPos()-crossfade);
    data->setSegueEndPos(data->endPos());
  }

  fclose(f);
  return true;
}


void MainObject::ProcessXmlLine(const QString &line,RDWaveData *data,
				int *cartnum,QString *filename,
				int *crossfade,int *fadeup_start,
				int *fadeup_len)
{
  QString tag=
    line.mid(line.find("<")+1,line.find(">")-line.find("<")-1).lower();
  QString value=line.mid(line.find(">")+1,line.findRev("<")-line.find(">")-1);

  //  printf("%s: %s\n",(const char *)tag,(const char *)value);

  if(tag=="file_name") {
    *filename=filter_audio_dir->canonicalPath()+"/"+
      value.right(value.length()-value.findRev('\\')-1);
  }
  if(tag=="cart") {
    *cartnum=value.toUInt()+filter_cart_offset;
    data->setMetadataFound(true);
  }
  if(tag=="title") {
    data->setTitle(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="artist") {
    data->setArtist(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="album") {
    data->setAlbum(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="label") {
    data->setLabel(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="composer") {
    data->setComposer(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="licensor") {
    data->setLicensingOrganization(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="user_define") {
    data->setUserDefined(RDXmlUnescape(value));
    data->setMetadataFound(true);
  }
  if(tag=="isrc") {
    data->setIsrc(value.replace("-",""));
    data->setMetadataFound(true);
  }
  if(tag=="modified_time") {
    QDateTime dt=GetDateTime(value);
    data->setOriginationDate(dt.date());
    data->setOriginationTime(dt.time());
    data->setMetadataFound(true);
  }
  if(tag=="start_time") {
    QDateTime dt=GetDateTime(value);
    data->setStartDate(dt.date());
    data->setStartTime(dt.time());
    data->setMetadataFound(true);
  }
  if(tag=="end_time") {
    QDateTime dt=GetDateTime(value);
    data->setEndDate(dt.date());
    data->setEndTime(dt.time());
    data->setMetadataFound(true);
  }
  if(tag=="intro_start") {
    data->setStartPos(value.toInt());
    data->setMetadataFound(true);
  }
  if(tag=="intro_3") {
    if(value.toInt()>0) {
      data->setIntroStartPos(0);
      data->setIntroEndPos(value.toInt());
    }
  }
  if(tag=="hookstart") {
    if(value.toInt()>0) {
      data->setHookStartPos(value.toInt());
    }
  }
  if(tag=="hookend") {
    if(value.toInt()>0) {
      data->setHookEndPos(value.toInt());
    }
  }
  /*
  if(tag=="length") {
    data->setEndPos(value.toInt());
    data->setMetadataFound(true);
  }
  */
  if(tag=="runtime") {
    data->setEndPos(value.toInt());
    data->setMetadataFound(true);
  }
  if(tag=="isci_code") {
    data->setIsci(value);
    data->setMetadataFound(true);
  }

  //
  // Used for calculating segue markers later
  //
  if(tag=="cross_fade") {
    *crossfade=value.toInt();
  }
  if(tag=="fade_up_start") {
    *fadeup_start=value.toInt();
  }
  if(tag=="fade_up_length") {
    *fadeup_len=value.toInt();
  }
}


bool MainObject::PreprocessAudio(QString filename)
{
  int fd=-1;
  char c;

  if((fd=open(filename,O_RDONLY))<0) {
    filename=SwapCase(filename);
    if((fd=open(filename,O_RDONLY))<0) {
      fprintf(stderr,"unable to open audio file \"%s\"\n",
	      (const char *)filename);
      return false;
    }
    
  }
  lseek(fd,20,SEEK_SET);
  if(read(fd,&c,1)!=1) {
    close(fd);
    fprintf(stderr,"truncated audio file \"%s\"\n",(const char *)filename);
    return false;
  }
  close(fd);
  if(c==80) {   // MPEG Audio
    if(system(QString("madplay -Q -o wave:")+filter_temp_audiofile+" "+filename)!=0) {
      fprintf(stderr,"MPEG converter error with file \"%s\"\n",
	      (const char *)filename);
      return false;
    }
  }
  else {    // PCM Audio
    if(symlink(filename,filter_temp_audiofile)!=0) {
      fprintf(stderr,"unable to create symlink \"%s\"\n",
	      (const char *)filter_temp_audiofile);
      return false;
    }
  }
  return true;
}


void MainObject::WriteReject(const QString &filename)
{
  if(filter_reject_dir!=NULL) {
    if(QFile::exists(filename)) {
      if(!RDCopy(filename,filter_reject_dir->canonicalPath()+"/"+
		 RDGetBasePart(filename))) {
	fprintf(stderr,"Unable to write to \"%s\"\n",
		(const char *)filter_reject_dir->path());
      }
    }
  }
}


QDateTime MainObject::GetDateTime(const QString &str) const
{
  QStringList fields;
  QStringList dates;
  QStringList times;
  QDateTime ret;

  fields=fields.split(" ",str);
  if(fields.size()==2) {
    dates=dates.split("/",fields[0]);
    if(dates.size()==3) {
      times=times.split(":",fields[1]);
      if(times.size()==3) {
	ret=
	  QDateTime(QDate(dates[2].toInt(),dates[0].toInt(),dates[1].toInt()),
		    QTime(times[0].toInt(),times[1].toInt(),times[2].toInt()));
      }
    }
  }
  return ret;
}


QString MainObject::SwapCase(const QString &str) const
{
  QStringList parts=parts.split(".",str);
  if(parts[parts.size()-1].contains(QRegExp("*[a-z]*",true,true))>0) {
    parts[parts.size()-1]=parts[parts.size()-1].upper();
  }
  else {
    parts[parts.size()-1]=parts[parts.size()-1].lower();
  }
  return parts.join(".");
}


bool MainObject::IsXmlFile(const QString &filename)
{
  int fd=-1;
  char data[10];

  if((fd=open(filename,O_RDONLY))<0) {
    return false;
  }
  if(read(fd,data,8)!=8) {
    close(fd);
    return false;
  }
  close(fd);
  data[8]=0;
  return strncmp(data,"<XMLDAT>",8)==0;
}


void MainObject::Print(const QString &msg) const
{
  if(filter_verbose) {
    printf("%s",(const char *)msg);
    fflush(stdout);
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
