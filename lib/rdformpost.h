// rdformpost.h
//
// Handle POST data from an HTML form.
//
//   (C) Copyright 2009-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDataStream>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QHostAddress>

#include <rdconfig.h>
#include <rdtempdirectory.h>

class RDFormPost
{
 public:
  enum Encoding {UrlEncoded=0,MultipartEncoded=1,AutoEncoded=2};
  enum Error {ErrorOk=0,ErrorNotPost=1,ErrorNoTempDir=2,ErrorMalformedData=3,
	      ErrorPostTooLarge=4,ErrorInternal=5,ErrorNotInitialized=6};
  RDFormPost(RDFormPost::Encoding encoding,bool auto_delete);
  ~RDFormPost();
  RDFormPost::Error error() const;
  QHostAddress clientAddress() const;
  QStringList names() const;
  QVariant value(const QString &name,bool *ok=NULL);
  bool getValue(const QString &name,QHostAddress *addr,bool *ok=NULL);
  bool getValue(const QString &name,QString *str,bool *ok=NULL);
  bool getValue(const QString &name,int *n,bool *ok=NULL);
  bool getValue(const QString &name,long *n,bool *ok=NULL);
  bool getValue(const QString &name,unsigned *n,bool *ok=NULL);
  bool getValue(const QString &name,QDateTime *datetime,bool *ok=NULL);
  bool getValue(const QString &name,QDate *date,bool *ok=NULL);
  bool getValue(const QString &name,QTime *time,bool *ok=NULL);
  bool getValue(const QString &name,bool *state,bool *ok=NULL);
  bool isFile(const QString &name);
  bool authenticate(bool *used_ticket=NULL);
  QString tempDir() const;
  unsigned headerContentLength() const;
  QString headerContentType() const;
  QByteArray rawPost() const;
  QString parsedPost() const;
  void dump();
  void dumpRawPost();
  static QString errorString(RDFormPost::Error err);
  static QString urlEncode(const QString &str);
  static QString urlDecode(const QString &str);

 private:
  void LoadUrlEncoding(char first);
  void LoadMultipartEncoding(char first);
  bool GetMimePart(QString *name,QString *value,bool *is_file,bool *ok);
  QByteArray GetLine(bool *ok);
  QHostAddress post_client_address;
  RDFormPost::Encoding post_encoding;
  RDFormPost::Error post_error;
  QMap<QString,QVariant> post_values;
  QMap<QString,bool> post_filenames;
  RDTempDirectory *post_tempdir;
  bool post_auto_delete;
  int64_t post_content_length;
  QString post_content_type;
  char *post_data;
  int64_t post_bytes_downloaded;
  QString post_separator;
  FILE *post_stream;
};


#endif  // RDFORMPOST_H
