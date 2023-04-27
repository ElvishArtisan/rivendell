// rdxsltengine.cpp
//
// Engine for performing XSLT transformations
//
//   (C) Copyright 2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <curl/curl.h>

#include <QProcess>
#include <QStringList>

#include "rdapplication.h"
#include "rdxsltengine.h"

size_t __RDXsltEngine_Download_Callback(char *ptr,size_t size,size_t nmemb,
			       void *userdata)
{
  QByteArray *buffer=(QByteArray *)userdata;

  buffer->append(QByteArray(ptr,size*nmemb));

  return size*nmemb;
}




RDXsltEngine::RDXsltEngine(const QString stylesheet_pathname,QObject *parent)
  : QObject(parent)
{
  d_stylesheet_pathname=stylesheet_pathname;
}


RDXsltEngine::~RDXsltEngine()
{
  for(int i=0;i<d_temp_directories.size();i++) {
    delete d_temp_directories.at(i);
  }
}


QString RDXsltEngine::stylesheetPathname() const
{
  return d_stylesheet_pathname;
}


bool RDXsltEngine::transformUrl(QString *output_filename,const QString &url,
			  QString *err_msg)
{
  QString err_msg2;
  bool ret=false;
  CURL *curl=NULL;
  CURLcode curl_err;
  long response_code;
  FILE *f=NULL;

  d_temp_directories.push_back(new RDTempDirectory("rivendell-rdxslt"));
  ret=d_temp_directories.back()->create(&err_msg2);
  if(ret) {
    // Download the source
    if((curl=curl_easy_init())==NULL) {
      *err_msg=tr("Unable to initialize CURL");
      return false;
    }
    QByteArray src_xml;
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__RDXsltEngine_Download_Callback);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&src_xml);
    curl_easy_setopt(curl,CURLOPT_USERAGENT,
		     rda->config()->userAgent().toUtf8().constData());
    curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
    curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
    curl_easy_setopt(curl,CURLOPT_URL,url.toUtf8().constData());

    curl_err=curl_easy_perform(curl);
    if((ret=(curl_err==CURLE_OK))) {
      curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
      if((ret=(response_code>=200)&&(response_code<300))) {
	// Generate the report
	QString output;
	RDXsltEngine *xslt=new RDXsltEngine(d_stylesheet_pathname);
	//	ret=RDFeed::generateReport(&output,src_xml,stylesheet_pathname,err_msg);
	ret=xslt->transform(&output,src_xml,err_msg);
	if(ret) {
	  *output_filename=
	    d_temp_directories.last()->path()+"/"+*output_filename;
	  if((f=fopen((*output_filename).toUtf8(),"w"))!=NULL) {
	    fprintf(f,"%s",output.toUtf8().constData());
	    fclose(f);
	  }
	  else {
	    *err_msg=tr("Unable to create output file")+" ["+
	      strerror(errno)+"].";
	  }
	}
      }
    }
    else {
      *err_msg=QObject::tr("Curl error")+" ["+curl_easy_strerror(curl_err)+"].";
      curl_easy_cleanup(curl);
    }
    curl_easy_cleanup(curl);
  }
  else {
    *err_msg=QObject::tr("Unable to create temporary directory.")+
      " ["+err_msg2+"]";
  }

  return ret;
}


bool RDXsltEngine::transformXml(QString *output_filename,const QString &src_xml,
			  QString *err_msg)
{
  QString err_msg2;
  FILE *f=NULL;
  bool ret=false;
  QString output;

  d_temp_directories.push_back(new RDTempDirectory("rivendell-rdxslt"));
  ret=d_temp_directories.back()->create(&err_msg2);
  if(ret) {
    *output_filename=d_temp_directories.last()->path()+"/"+*output_filename;
    ret=transform(&output,src_xml,err_msg);
    if(ret) {
      if((f=fopen((*output_filename).toUtf8(),"w"))!=NULL) {
	fprintf(f,"%s",output.toUtf8().constData());
	fclose(f);
      }
      else {
	*err_msg=tr("Unable to create output file")+" ["+strerror(errno)+"].";
      }
    }
  }
  else {
    *err_msg=QObject::tr("Unable to create temporary directory.")+
      "["+err_msg2+"]";
  }

  return ret;
}


bool RDXsltEngine::transform(QString *output,const QString &src_xml,QString *err_msg)
{
  QStringList args;
  QProcess *proc=NULL;

  args.push_back("--encoding");
  args.push_back("utf-8");
  args.push_back(d_stylesheet_pathname);
  args.push_back("-");
  proc=new QProcess(this);
  proc->start("xsltproc",args);
  if(!proc->waitForStarted()) {
    *err_msg=tr("unable to start xsltproc(1)");
    delete proc;
    return false;
  }
  proc->write(src_xml.toUtf8());
  proc->closeWriteChannel();
  if(!proc->waitForFinished()) {
    *err_msg=tr("xsltproc(1) is apparently hung!");
    proc->kill();
    delete proc;
    return false;
  }
  if(proc->exitStatus()!=QProcess::NormalExit) {
    *err_msg=tr("xsltproc(1) crashed!");
    delete proc;
    return false;
  }
  if(proc->exitCode()!=0) {
    *err_msg=tr("xsltproc(1) returned an error")+" ["+
      proc->readAllStandardError()+"].";
    delete proc;
    return false;
  }
  *output=QString::fromUtf8(proc->readAllStandardOutput());
  delete proc;

  return true;
}
