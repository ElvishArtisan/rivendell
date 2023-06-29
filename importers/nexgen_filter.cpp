// nexgen_filter.cpp
//
// A Library import filter for the Prophet NexGen system
//
//   (C) Copyright 2012-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rd.h>
#include <rdcart.h>
#include <rdconfig.h>
#include <rdconf.h>
#include <rdcmd_switch.h>
#include <rdcut.h>
#include <rddb.h>
#include <rdtempdirectory.h>
#include <rdwavefile.h>
#include <rdweb.h>

#include "nexgen_filter.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString group_name;
  QString audio_dir;
  QString reject_dir="/dev/null";
  QStringList xml_files;
  bool ok=false;
  char tempdir[PATH_MAX];
  QString err_msg;

  filter_cart_offset=0;
  filter_delete_cuts=false;
  filter_normalization_level=0;
  filter_verbose=false;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("nexgen_filter",
			     "nexgen_filter",NEXGEN_FILTER_USAGE,false,this));
  if(!rda->open(&err_msg,NULL,false,false)) {
    fprintf(stderr,"nexgen_filter: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Read Command Options
  //
  bool options=true;
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!options) {
      xml_files.push_back(rda->cmdSwitch()->key(i));
    }
    else {
      if(rda->cmdSwitch()->key(i)=="--verbose") {
	filter_verbose=true;
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--group") {
	group_name=rda->cmdSwitch()->value(i);
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--audio-dir") {
	audio_dir=rda->cmdSwitch()->value(i);
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--reject-dir") {
	reject_dir=rda->cmdSwitch()->value(i);
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--cart-offset") {
	filter_cart_offset=rda->cmdSwitch()->value(i).toInt(&ok);
	if(!ok) {
	  fprintf(stderr,"nexgen_filter: --cart-offset must be an integer\n");
	  exit(256);
	}
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--delete-cuts") {
	filter_delete_cuts=true;
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--normalization-level") {
	filter_normalization_level=rda->cmdSwitch()->value(i).toInt(&ok);
	if(!ok) {
	  fprintf(stderr,"nexgen_filter: --cart-offset must be an integer\n");
	  exit(256);
	}
	if(filter_normalization_level>0) {
	  fprintf(stderr,
		  "nexgen_filter: positive --normalization-level is invalid\n");
	  exit(256);
	}
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(!rda->cmdSwitch()->processed(i)) {
	options=false;
	xml_files.push_back(rda->cmdSwitch()->key(i));
      }
    }
  }

  //
  // RIPCD Connection
  //
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

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
	    group_name.toUtf8().constData());
    exit(256);
  }
  filter_audio_dir=new QDir(audio_dir);
  if(!audio_dir.isEmpty()) {
    if(!filter_audio_dir->exists()) {
      fprintf(stderr,"nexgen_filter: audio directory \"%s\" does not exist\n",
	      audio_dir.toUtf8().constData());
      exit(256);
    }
    if(!filter_audio_dir->isReadable()) {
      fprintf(stderr,"nexgen_filter: audio directory \"%s\" is not readable\n",
	      audio_dir.toUtf8().constData());
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
	      reject_dir.toUtf8().constData());
      exit(256);
    }
  }

  //
  // Create Temp Directory
  //
  strncpy(tempdir,(RDTempDirectory::basePath()+"/nexgen_filterXXXXXX").toUtf8(),
	  PATH_MAX-1);
  filter_temp_dir=new QDir(mkdtemp(tempdir));
  filter_temp_audiofile=filter_temp_dir->canonicalPath()+"/audio.dat";

  //
  // Main Loop
  //
  for(int i=0;i<xml_files.size();i++) {
    if(IsXmlFile(xml_files[i])) {
      if(audio_dir.isEmpty()) {
	fprintf(stderr,"unable to process \"%s\" [no --audio-dir specified]\n",
		xml_files[i].toUtf8().constData());
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
  rmdir(filter_temp_dir->canonicalPath().toUtf8());

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
  snprintf(tempdir,PATH_MAX,"%s/XXXXXX",
	   RDTempDirectory::basePath().toUtf8().constData());
  if(mkdtemp(tempdir)==NULL) {
    return;
  }
  dir=tempdir;

  //
  // Open Archive
  //
  if((fd_in=open(filename.toUtf8(),O_RDONLY))<0) {
    return;
  }

  //
  // Write Out File Components
  //
  while((read(fd_in,header,104)==104)&&(strncmp(header,"FR:",3)==0)) {
    files.push_back(dir+"/"+RDGetBasePart(QString(header+3).replace("\\","/")));
    if(files.back().right(4).toLower()==".xml") {
      xmlfile=files.back();
    }
    if(files.back().right(4).toLower()==".wav") {
      wavfile=files.back();
    }
    len=((0xFF&header[103])<<24)+((0xFF&header[102])<<16)+
      ((0xFF&header[101])<<8)+(0xFF&header[100]);
    if((fd_out=open(files.back().toUtf8(),O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR))<0) {
      fprintf(stderr,"unable to write temporary file \"%s\" [%s].\n",
	      files.back().toUtf8().constData(),strerror(errno));
      return;
    }
    if(fstat(fd_out,&stat)==0) {
      blksize=stat.st_blksize;
      data=(char *)realloc(data,blksize);
    }
    for(uint32_t i=blksize;i<len;i+=blksize) {
      n=read(fd_in,data,blksize);
      RDCheckExitCode("ProcessArchive() write",write(fd_out,data,n));
    }
    n=read(fd_in,data,len%blksize);
    RDCheckExitCode("ProcessArchive() write",write(fd_out,data,n));
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
  for(int i=0;i<files.size();i++) {
    unlink(files[i].toUtf8());
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
    fprintf(stderr,"unable to parse XML file \"%s\"\n",
	    xml.toUtf8().constData());
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
	      cartnum,filter_group->name().toUtf8().constData());
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
  Print(QString::asprintf("Importing cart %06d",cartnum));
  if(!data.title().isEmpty()) {
    Print(QString::asprintf(" [%s",data.title().toUtf8().constData()));
    if(!data.artist().isEmpty()) {
      Print(QString::asprintf("/%s",data.artist().toUtf8().constData()));
    }
    Print(QString::asprintf("]"));
  }
  if(arcname.isEmpty()) {
    Print(QString::asprintf(" from %s ...",filename.toUtf8().constData()));
  }
  else {
    Print(QString::asprintf(" from %s ...",arcname.toUtf8().constData()));
  }
  if(filter_delete_cuts) {
    delete_cuts_switch="--delete-cuts ";
  }
  QString cmd=QString("rdimport ")+
    "--autotrim-level=0 "+
    QString::asprintf("--normalization-level=%d ",filter_normalization_level)+
    QString::asprintf("--to-cart=%d ",cartnum)+
    delete_cuts_switch+
    filter_group->name()+" "+
    filter_temp_audiofile;
  if(system(cmd.toUtf8())!=0) {
    Print(QString::asprintf(" aborted.\n"));
    fprintf(stderr,"import of \"%s\" failed\n",filename.toUtf8().constData());
    WriteReject(xml);
    return;
  }
  Print(QString::asprintf(" done.\n"));
  unlink(filter_temp_audiofile.toUtf8());

  //
  // Apply Metadata
  //
  cart=new RDCart(cartnum);
  cart->setMetadata(&data);
  delete cart;
  sql=QString("select `CUT_NAME` from `CUTS` where ")+
    QString::asprintf("`CART_NUMBER`=%d ",cartnum)+
    "order by `ORIGIN_DATETIME` desc";
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

  if((f=fopen(xml.toUtf8(),"r"))==NULL) {
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
    line.mid(line.indexOf("<")+1,line.indexOf(">")-line.indexOf("<")-1).toLower();
  QString value=line.mid(line.indexOf(">")+1,line.lastIndexOf("<")-line.indexOf(">")-1);

  //  printf("%s: %s\n",(const char *)tag,(const char *)value);

  if(tag=="file_name") {
    *filename=filter_audio_dir->canonicalPath()+"/"+
      value.right(value.length()-value.lastIndexOf('\\')-1);
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
      data->setTalkStartPos(0);
      data->setTalkEndPos(value.toInt());
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

  if((fd=open(filename.toUtf8(),O_RDONLY))<0) {
    filename=SwapCase(filename);
    if((fd=open(filename.toUtf8(),O_RDONLY))<0) {
      fprintf(stderr,"unable to open audio file \"%s\"\n",
	      filename.toUtf8().constData());
      return false;
    }
    
  }
  lseek(fd,20,SEEK_SET);
  if(read(fd,&c,1)!=1) {
    close(fd);
    fprintf(stderr,"truncated audio file \"%s\"\n",filename.toUtf8().
	    constData());
    return false;
  }
  close(fd);
  if(c==80) {   // MPEG Audio
    if(system((QString("madplay -Q -o wave:")+filter_temp_audiofile+" "+filename).toUtf8())!=0) {
      fprintf(stderr,"MPEG converter error with file \"%s\"\n",
	      filename.toUtf8().constData());
      return false;
    }
  }
  else {    // PCM Audio
    if(symlink(filename.toUtf8(),filter_temp_audiofile.toUtf8())!=0) {
      fprintf(stderr,"unable to create symlink \"%s\"\n",
	      filter_temp_audiofile.toUtf8().constData());
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
		filter_reject_dir->path().toUtf8().constData());
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

  fields=str.split(" ");
  if(fields.size()==2) {
    dates=fields[0].split("/");
    if(dates.size()==3) {
      times=fields[1].split(":");
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
  QStringList parts=str.split(".");
  if(parts[parts.size()-1].
     contains(QRegExp("*[a-z]*",Qt::CaseSensitive,QRegExp::Wildcard))>0) {
    parts[parts.size()-1]=parts[parts.size()-1].toUpper();
  }
  else {
    parts[parts.size()-1]=parts[parts.size()-1].toLower();
  }
  return parts.join(".");
}


bool MainObject::IsXmlFile(const QString &filename)
{
  int fd=-1;
  char data[10];

  if((fd=open(filename.toUtf8(),O_RDONLY))<0) {
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
    printf("%s",msg.toUtf8().constData());
    fflush(stdout);
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
