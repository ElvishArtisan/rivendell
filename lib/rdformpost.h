// rdformpost.h
//
// Handle POST data from an HTML form.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdformpost.h,v 1.2 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDFORMPOST_H
#define RDFORMPOST_H

#include <map>

#include <qstring.h>
#include <qstringlist.h>
#include <qvariant.h>
#include <qhostaddress.h>

class RDFormPost
{
 public:
  enum Encoding {UrlEncoded=0,MultipartEncoded=1,AutoEncoded=2};
  enum Error {ErrorOk=0,ErrorNotPost=1,ErrorNoTempDir=2,ErrorMalformedData=3,
	      ErrorPostTooLarge=4,ErrorInternal=5,ErrorNotInitialized=6};
  RDFormPost(RDFormPost::Encoding encoding,unsigned maxsize=0,
	     bool auto_delete=true);
  ~RDFormPost();
  RDFormPost::Error error() const;
  QStringList names() const;
  QVariant value(const QString &name,bool *ok=NULL);
  bool getValue(const QString &name,QHostAddress *addr);
  bool getValue(const QString &name,QString *str);
  bool getValue(const QString &name,int *n);
  bool getValue(const QString &name,long *n);
  bool getValue(const QString &name,QDateTime *datetime);
  bool getValue(const QString &name,QTime *time);
  bool isFile(const QString &name);
  QString tempDir() const;
  void dump();
  static QString errorString(RDFormPost::Error err);
  static QString urlEncode(const QString &str);
  static QString urlDecode(const QString &str);

 private:
  void LoadUrlEncoding(char first);
  void LoadMultipartEncoding(char first);
  RDFormPost::Encoding post_encoding;
  RDFormPost::Error post_error;
  std::map<QString,QVariant> post_values;
  std::map<QString,bool> post_filenames;
  QString post_tempdir;
  bool post_auto_delete;
  unsigned post_content_length;
};


#endif  // RDFORMPOST_H
