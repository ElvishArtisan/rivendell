// podcasts.cpp
//
// Rivendell web service portal -- Podcast services
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <curl/curl.h>

#include <QProcess>
#include <QProcessEnvironment>

#include <rdapplication.h>
#include <rdconf.h>
#include <rddelete.h>
#include <rdescape_string.h>
#include <rdformpost.h>
#include <rdgroup.h>
#include <rdhash.h>
#include <rdpodcast.h>
#include <rdupload.h>
#include <rduser.h>
#include <rdweb.h>

#include "rdxport.h"

// #define ENABLE_EXTENDED_CURL_LOGGING

int __PostRss_Debug_Callback(CURL *handle,curl_infotype type,char *data,
			     size_t size,void *userptr)
{
  QStringList *lines=(QStringList *)userptr;

  if(type==CURLINFO_TEXT) {
    lines->push_back(QString::fromUtf8(QByteArray(data,size)));
  }

  return 0;
}


size_t __PostRss_UploadFunction_Callback(char *buffer,size_t size,
					 size_t nitems,void *userdata)
{
  Xport *xport=(Xport *)userdata;

  int curlsize=size*nitems;
  int segsize=
    xport->xport_curl_upload_data.size()-xport->xport_curl_upload_data_ptr;
  if(segsize<curlsize) {
    curlsize=segsize;
  }
  memcpy(buffer,xport->xport_curl_upload_data.
	 mid(xport->xport_curl_upload_data_ptr,curlsize).constData(),
	 curlsize);
  xport->xport_curl_upload_data_ptr+=curlsize;
  return curlsize;
}


size_t __PostRss_DownloadFunction_Callback(char *buffer,size_t size,
					   size_t nitems,void *userdata)
{
  Xport *xport=(Xport *)userdata;

  (xport->xport_curl_download_data)+=QByteArray(buffer,size*nitems);

  return size*nitems;
}


void Xport::SavePodcast()  // Save posted audio to the Rivendell audio store
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  RDFeed *feed=NULL;
  QString filename;
  QString msg="OK";

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  if(!xport_post->getValue("FILENAME",&filename)) {
    XmlExit("Missing FILENAME",400,"podcasts.cpp",LINE_NUMBER);
  }
  if(!xport_post->isFile("FILENAME")) {
    XmlExit("Missing file data",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if(((!rda->user()->addPodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(keyname,rda->config(),this);
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();

  if(!RDCopy(filename,destpath)) {
    delete feed;
    delete cast;
    XmlExit("Internal server error [copy failed]",500,"podcasts.cpp",
	    LINE_NUMBER);
  }
  if(chmod(destpath.toUtf8(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)!=0) {
    err_msg=QString::asprintf("Internal server error [%s]",strerror(errno));
    unlink(destpath.toUtf8());
    delete feed;
    delete cast;
    XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
  }
  cast->setSha1Hash(RDSha1HashFile(destpath));

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,"saved podcast \"%s\"",destpath.toUtf8().constData());

  delete feed;
  delete cast;

  Exit(0);
}


void Xport::GetPodcast()  // Get posted podcast audio from the audio store
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  QString msg="OK";
  int fd=-1;
  struct stat st;
  ssize_t n=0;
  char *data=NULL;

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if(((!rda->user()->addPodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();

  if((fd=open(destpath.toUtf8(),O_RDONLY))<0) {
    err_msg=QString::asprintf("Internal server error [%s]",strerror(errno));
    delete cast;
    XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
  }
  memset(&st,0,sizeof(st));
  if(fstat(fd,&st)!=0) {
    err_msg=QString::asprintf("Internal server error [%s]",strerror(errno));
    delete cast;
    XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
  }

  printf("Content-type: audio/x-mpeg\n");
  printf("Content-length: %ld\n",st.st_size);
  printf("\n");
  fflush(stdout);
  data=new char[st.st_blksize];
  n=read(fd,data,st.st_blksize);
  while(n>0) {
    n=write(1,data,n);
    n=read(fd,data,st.st_blksize);
  }
  delete data;
  close(fd);

  rda->syslog(LOG_DEBUG,"served podcast \"%s\"",destpath.toUtf8().constData());

  delete cast;

  Exit(0);
}


void Xport::DeletePodcast() // Deleted posted podcast audio from the audio store
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  QString msg="OK";

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if(((!rda->user()->deletePodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();

  if(unlink(destpath.toUtf8())!=0) {
    if(errno!=ENOENT) {
      err_msg=QString::asprintf("Internal server error [%s]",strerror(errno));
      delete cast;
      XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
    }
  }
  cast->setSha1Hash();

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,"deleted podcast \"%s\"",destpath.toUtf8().constData());

  delete cast;

  Exit(0);
}


void Xport::PostPodcast()  // Upload podcast audio from the audio store to
                           // the remote archive
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  RDFeed *feed=NULL;
  QString msg="OK";
  RDUpload::ErrorCode upload_err;

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if(((!rda->user()->addPodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();
  feed=new RDFeed(keyname,rda->config(),this);

  RDUpload *upload=new RDUpload(rda->config(),this);
  upload->setSourceFile(destpath);
  QString desturl=feed->purgeUrl()+"/"+cast->audioFilename();
  upload->setDestinationUrl(desturl);
  if((upload_err=upload->
      runUpload(feed->purgeUsername(),feed->purgePassword(),
		rda->station()->sshIdentityFile(),feed->purgeUseIdFile(),
		&err_msg,rda->config()->logXloadDebugData()))!=
     RDUpload::ErrorOk) {
    delete upload;
    delete feed;
    delete cast;
    XmlExit(err_msg,500,"podcasts.cpp",LINE_NUMBER);
  }
  delete upload;

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,
	      "posted podcast audio \"%s\"",destpath.toUtf8().constData());

  delete feed;
  delete cast;

  Exit(0);
}


void Xport::RemovePodcast()  // Delete podcast audio from the remote archive
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  RDFeed *feed=NULL;
  QString msg="OK";
  RDDelete::ErrorCode del_err;

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if(((!rda->user()->deletePodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();
  feed=new RDFeed(keyname,rda->config(),this);

  RDDelete *del=new RDDelete(rda->config(),this);
  QString desturl=feed->purgeUrl()+"/"+cast->audioFilename();
  del->setTargetUrl(desturl);
  if((del_err=del->
      runDelete(feed->purgeUsername(),feed->purgePassword(),
		rda->station()->sshIdentityFile(),feed->purgeUseIdFile(),
		rda->config()->logXloadDebugData()))!=RDDelete::ErrorOk) {
    delete del;
    delete feed;
    delete cast;
    XmlExit(RDDelete::errorText(del_err),500,"podcasts.cpp",LINE_NUMBER);
  }
  delete del;

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,
	      "delete podcast audio \"%s\"",destpath.toUtf8().constData());

  delete feed;
  delete cast;

  Exit(0);
}


void Xport::DownloadRss()  // Download feed XML from the remote archive
{
  CURL *curl=NULL;
  CURLcode curl_err;
  char errstr[CURL_ERROR_SIZE];
  int feed_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDFeed *feed=NULL;
  QString msg="OK";

  QDateTime now=QDateTime::currentDateTime();

  if(!xport_post->getValue("ID",&feed_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(feed_id,rda->config(),this);
  if(!feed->exists()) {
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=feed->keyName();

  if(((!rda->user()->editPodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete feed;
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }

  if((curl=curl_easy_init())==NULL) {
    XmlExit("unable to get CURL handle",500,"podcasts.cpp",LINE_NUMBER);
  }
  //  xport_curl_data=feed->rssXml(err_msg,now).toUtf8();
  //  xport_curl_data_ptr=0;

  //
  // Authentication Parameters
  //
  if((QUrl(feed->feedUrl()).scheme().toLower()=="sftp")&&
     (!rda->station()->sshIdentityFile().isEmpty())&&feed->purgeUseIdFile()) {
    //
    // Enable host key verification
    //
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);

    curl_easy_setopt(curl,CURLOPT_USERNAME,
		     feed->purgeUsername().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_SSH_PRIVATE_KEYFILE,
		     rda->station()->sshIdentityFile().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_KEYPASSWD,
		     feed->purgePassword().toUtf8().constData());
    rda->syslog(LOG_DEBUG,"using ssh key at \"%s\"",
		rda->station()->sshIdentityFile().toUtf8().constData());
  }
  else {
    //
    // Disable host key verification
    //
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);

    curl_easy_setopt(curl,CURLOPT_USERNAME,
		     feed->purgeUsername().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_PASSWORD,
		     feed->purgePassword().toUtf8().constData());
  }

  //
  // Transfer Parameters
  //
  curl_easy_setopt(curl,CURLOPT_URL,feed->feedUrl().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_UPLOAD,0);
  xport_curl_download_data.clear();
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,
		   __PostRss_DownloadFunction_Callback);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,this);

  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   rda->config()->userAgent().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errstr);

  //
  // Execute it
  //
  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
  case CURLE_PARTIAL_FILE:
    feed->setLastBuildDateTime(now);
    rda->syslog(LOG_DEBUG,
		"posted RSS XML to \"%s\"",
		feed->feedUrl().toUtf8().constData());
    break;

  default:
    rda->syslog(LOG_ERR,"RSS XML upload failed: curl error %d [%s]",
		curl_err,curl_easy_strerror(curl_err));
    err_msg+=errstr;
    break;
  }
  curl_easy_cleanup(curl);

  printf("Content-type: application/rss+xml; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("%s",xport_curl_download_data.constData());

  Exit(0);
}


bool Xport::PostRssElemental(RDFeed *feed,const QDateTime &now,QString *err_msg)
{
  CURL *curl=NULL;
  CURLcode curl_err;
  char errstr[CURL_ERROR_SIZE];
  bool ret=false;
  bool ok=false;

  if((curl=curl_easy_init())==NULL) {
    XmlExit("unable to get CURL handle",500,"podcasts.cpp",LINE_NUMBER);
  }

  //
  // Avoid pointless uploads
  //
  // We use the date/time of the UNIX Epoch for <lastBuildDate> here so the
  // internal Rivendell hashes remain consistent.
  //
  QDateTime epoch(QDate(1970,1,1),QTime(0,0,0));
  QString new_hash=RDSha1HashData(xport_curl_upload_data=feed->
				  rssXml(err_msg,epoch,&ok).toUtf8());
  if(feed->sha1Hash()==new_hash) {
    rda->syslog(LOG_DEBUG,
		"SHA1 hash for feed \"%s\" is unchanged, skipping the XML upload",
		feed->keyName().toUtf8().constData());
    return true;
  }

  xport_curl_upload_data=feed->rssXml(err_msg,now,&ok).toUtf8();
  xport_curl_upload_data_ptr=0;


  //
  // Authentication Parameters
  //
  if((QUrl(feed->feedUrl()).scheme().toLower()=="sftp")&&
     (!rda->station()->sshIdentityFile().isEmpty())&&feed->purgeUseIdFile()) {
    //
    // Disable host key verification
    //
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);

    curl_easy_setopt(curl,CURLOPT_USERNAME,
		     feed->purgeUsername().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_SSH_PRIVATE_KEYFILE,
		     rda->station()->sshIdentityFile().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_KEYPASSWD,
		     feed->purgePassword().toUtf8().constData());
    rda->syslog(LOG_DEBUG,"using ssh key at \"%s\"",
		rda->station()->sshIdentityFile().toUtf8().constData());
  }
  else {
    //
    // Disable host key verification
    //
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);

    curl_easy_setopt(curl,CURLOPT_USERNAME,
		     feed->purgeUsername().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_PASSWORD,
		     feed->purgePassword().toUtf8().constData());
  }

  //
  // Transfer Parameters
  //
  curl_easy_setopt(curl,CURLOPT_URL,feed->feedUrl().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_UPLOAD,1);
  curl_easy_setopt(curl,CURLOPT_READFUNCTION, __PostRss_UploadFunction_Callback);
  curl_easy_setopt(curl,CURLOPT_READDATA,this);

  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   rda->config()->userAgent().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errstr);

  //
  // Execute it
  //
  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
  case CURLE_PARTIAL_FILE:
    feed->setLastBuildDateTime(now);
    feed->setSha1Hash(new_hash);
    rda->syslog(LOG_DEBUG,
		"posted RSS XML to \"%s\"",
		feed->feedUrl().toUtf8().constData());
    ret=true;
    break;

  default:
    rda->syslog(LOG_ERR,"RSS XML upload failed: curl error %d [%s]",
		curl_err,curl_easy_strerror(curl_err));
    *err_msg+=errstr;
    ret=false;
    break;
  }
  curl_easy_cleanup(curl);

  //
  // Cache Management
  //
  if(ret) {
    QString cdn_script=feed->cdnPurgePluginPath();
    if(!cdn_script.isEmpty()) {
      QStringList args;
      args.push_back(RDFeed::publicUrl(feed->baseUrl(""),feed->keyName()));
      RunCdnScript(cdn_script,args);
    }
  }

  return ret;
}


void Xport::PostRss()  // Post feed XML to the remote archive
{
  int feed_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDFeed *feed=NULL;
  QString msg="OK";
  bool ret=false;

  QDateTime now=QDateTime::currentDateTime();

  if(!xport_post->getValue("ID",&feed_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(feed_id,rda->config(),this);
  if(!feed->exists()) {
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=feed->keyName();

  if(((!rda->user()->editPodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {
    delete feed;
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }

  ret=PostRssElemental(feed,now,&err_msg);
  delete feed;

  //
  // Update Enclosing Superfeeds
  //
  QStringList superfeeds=feed->isSubfeedOf();
  for(int i=0;i<superfeeds.size();i++) {
    QString err_msg2;
    RDFeed *feed=new RDFeed(superfeeds.at(i),rda->config(),this);
    if(!PostRssElemental(feed,now,&err_msg)) {
      err_msg+="\nRepost of XML failed";
    }
    delete feed;
  }

  if(!ret) {
    XmlExit(err_msg,500,"podcasts.cpp",LINE_NUMBER);
  }

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  Exit(0);
}


void Xport::RemoveRss()  // Delete feed XML from the remote archive
{
  int feed_id=0;
  RDFeed *feed=NULL;
  QString keyname;
  QString destpath;
  QString err_msg;
  //  RDPodcast *cast=NULL;
  QString msg="OK";
  RDDelete::ErrorCode del_err;

  if(!xport_post->getValue("ID",&feed_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  feed=new RDFeed(feed_id,rda->config(),this);
  if(!feed->exists()) {
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=feed->keyName();
  if(((!rda->user()->deletePodcast())||
      (!rda->user()->feedAuthorized(keyname)))&&
     (!rda->user()->adminConfig())) {

    delete feed;
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }

  RDDelete *del=new RDDelete(rda->config(),this);
  QString desturl=feed->feedUrl();
  del->setTargetUrl(desturl);
  if((del_err=del->
      runDelete(feed->purgeUsername(),feed->purgePassword(),
		rda->station()->sshIdentityFile(),feed->purgeUseIdFile(),
		rda->config()->logXloadDebugData()))!=RDDelete::ErrorOk) {
    delete del;
    delete feed;
    XmlExit(RDDelete::errorText(del_err),500,"podcasts.cpp",LINE_NUMBER);
  }
  delete del;

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,
	      "deleted podcast RSS \"%s\"",destpath.toUtf8().constData());

  delete feed;

  Exit(0);
}


void Xport::PostImage()  // Upload podcast image to the remote archive
{
  int img_id=0;
  QString keyname;
  QString desturl;
  QString err_msg;
  unsigned feed_id=0;
  RDFeed *feed=NULL;
  QString file_ext;
  bool ret=false;
  QString sql;
  RDSqlQuery *q=NULL;
  CURL *curl=NULL;
  CURLcode curl_err;
  char errstr[CURL_ERROR_SIZE];
  QDateTime now=QDateTime::currentDateTime();

  if(!xport_post->getValue("ID",&img_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }
  sql=QString("select ")+
    "`FEED_ID`,"+         // 00
    "`DATA`,"+            // 01
    "`FILE_EXTENSION` "+  // 02
    "from FEED_IMAGES where "+
    QString::asprintf("ID=%d",img_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    feed_id=q->value(0).toUInt();
    xport_curl_upload_data=q->value(1).toByteArray();
    xport_curl_upload_data_ptr=0;
    file_ext=q->value(2).toString();
  }
  delete q;
  if(feed_id==0) {
    XmlExit("invalid image ID",400,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(feed_id,rda->config(),this);
  if(!feed->exists()) {
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=feed->keyName();

  if(!rda->user()->adminConfig()) {
    delete feed;
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }
  desturl=feed->purgeUrl()+"/"+RDFeed::imageFilename(feed_id,img_id,file_ext);
  if((curl=curl_easy_init())==NULL) {
    XmlExit("unable to get CURL handle",500,"podcasts.cpp",LINE_NUMBER);
  }

  //
  // Authentication Parameters
  //
  if((QUrl(feed->feedUrl()).scheme().toLower()=="sftp")&&
     (!rda->station()->sshIdentityFile().isEmpty())&&feed->purgeUseIdFile()) {
    //
    // Disable host key verification
    //
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);
    curl_easy_setopt(curl,CURLOPT_USERNAME,
		     feed->purgeUsername().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_SSH_PRIVATE_KEYFILE,
		     rda->station()->sshIdentityFile().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_KEYPASSWD,
		     feed->purgePassword().toUtf8().constData());
  }
  else {
    //
    // Disable host key verification
    //
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);
    curl_easy_setopt(curl,CURLOPT_USERNAME,
		     feed->purgeUsername().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_PASSWORD,
		     feed->purgePassword().toUtf8().constData());
  }

  //
  // Transfer Parameters
  //
  QStringList *err_msgs=SetupCurlLogging(curl);
  curl_easy_setopt(curl,CURLOPT_URL,desturl.toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_UPLOAD,1);
  curl_easy_setopt(curl,CURLOPT_READFUNCTION,__PostRss_UploadFunction_Callback);
  curl_easy_setopt(curl,CURLOPT_READDATA,this);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   (const char *)rda->config()->userAgent().toUtf8());
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errstr);

  //
  // Execute it
  //
  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
  case CURLE_PARTIAL_FILE:
    feed->setLastBuildDateTime(now);
    ret=true;
    break;

  default:
    err_msg=errstr;
    ret=false;
    break;
  }
  ProcessCurlLogging("RDFeed::postImage()",err_msgs);
  curl_easy_cleanup(curl);

  if(!ret) {
    rda->syslog(LOG_NOTICE,"errstr: %s  errnum: %d",errstr,curl_err);
    XmlExit(err_msg,500,"podcasts.cpp",LINE_NUMBER);
  }

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,
	      "posted image \"%s\"",desturl.toUtf8().constData());

  Exit(0);
}


void Xport::RemoveImage()  // Remove podcast image from the remote archive
{
  int img_id=0;
  QString keyname;
  unsigned feed_id=0;
  RDFeed *feed=NULL;
  QString desturl;
  QString file_ext;
  QString sql;
  RDSqlQuery *q=NULL;
  QDateTime now=QDateTime::currentDateTime();
  RDDelete::ErrorCode del_err;

  if(!xport_post->getValue("ID",&img_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }
  sql=QString("select ")+
    "`FEED_ID`,"+         // 00
    "`FILE_EXTENSION` "+  // 01
    "from `FEED_IMAGES` where "+
    QString::asprintf("`ID`=%d",img_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    feed_id=q->value(0).toUInt();
    file_ext=q->value(1).toString();
  }
  delete q;
  if(feed_id==0) {
    XmlExit("invalid image ID",400,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(feed_id,rda->config(),this);
  if(!feed->exists()) {
    XmlExit("No such feed",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=feed->keyName();
  if(!rda->user()->adminConfig()) {
    delete feed;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(keyname,rda->config(),this);
  desturl=feed->purgeUrl()+"/"+RDFeed::imageFilename(feed_id,img_id,file_ext);

  RDDelete *del=new RDDelete(rda->config(),this);
  del->setTargetUrl(desturl);
  if((del_err=del->
      runDelete(feed->purgeUsername(),feed->purgePassword(),
		rda->station()->sshIdentityFile(),feed->purgeUseIdFile(),
		rda->config()->logXloadDebugData()))!=RDDelete::ErrorOk) {
    delete del;
    delete feed;
    XmlExit(RDDelete::errorText(del_err),500,"podcasts.cpp",LINE_NUMBER);
  }
  delete del;

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,
	      "deleted image \"%s\"",desturl.toUtf8().constData());

  delete feed;

  Exit(0);
}


void Xport::RunCdnScript(const QString &cmd,const QStringList &args)
{
  QStringList f0=cmd.split("/",QString::KeepEmptyParts);
  f0.removeLast();
  QProcess *proc=new QProcess(this);
  QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
  QString path=env.value("PATH");
  env.remove("PATH");
  env.insert("PATH",f0.join("/")+":"+path);
  proc->setEnvironment(env.toStringList());

  proc->start(cmd,args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    rda->syslog(LOG_WARNING,"cdn script \"%s\" crashed",
		(cmd+" "+args.join(" ")).toUtf8().constData());
  }
  else {
    if(proc->exitCode()!=0) {
      rda->syslog(LOG_WARNING,"cdn script \"%s\" returned exit code %d [%s]",
		  (cmd+" "+args.join(" ")).toUtf8().constData(),
		  proc->exitCode(),
		  proc->readAllStandardError().constData());
    }
    else {
      rda->syslog(LOG_DEBUG,"ran cdn script \"%s\"",
		  (cmd+" "+args.join(" ")).toUtf8().constData());
    }
  }
  delete proc;
}


QStringList *Xport::SetupCurlLogging(CURL *curl) const
{
  QStringList *err_msgs=new QStringList();

#ifdef ENABLE_EXTENDED_CURL_LOGGING
  curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,__PostRss_Debug_Callback);
  curl_easy_setopt(curl,CURLOPT_DEBUGDATA,err_msgs);
  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
#endif  // ENABLE_EXTENDED_CURL_LOGGING

  return err_msgs;
}


void Xport::ProcessCurlLogging(const QString &label,
                               QStringList *err_msgs) const
{
#ifdef ENABLE_EXTENDED_CURL_LOGGING
  if(err_msgs->size()>0) {
    rda->syslog(LOG_ERR,"*** %s: extended CURL information begins ***",
               label.toUtf8().constData());
    for(int i=0;i<err_msgs->size();i++) {
      rda->syslog(LOG_ERR,"[%d]: %s",i,err_msgs->at(i).toUtf8().constData());
    }
    rda->syslog(LOG_ERR,"*** %s: extended CURL information ends ***",
               label.toUtf8().constData());
  }
#endif  // ENABLE_EXTENDED_CURL_LOGGING
  delete err_msgs;
}
