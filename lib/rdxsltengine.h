// rdxsltengine.h
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

#ifndef RDXSLTENGINE_H
#define RDXSLTENGINE_H

#include <QList>
#include <QObject>

#include <rdtempdirectory.h>

class RDXsltEngine : public QObject
{
  Q_OBJECT;
 public:
  RDXsltEngine(const QString stylesheet_pathname,QObject *parent=0);
  ~RDXsltEngine();
  QString stylesheetPathname() const;
  bool transformUrl(QString *output_filename,const QString &url,
  		    QString *err_msg);
  bool transformXml(QString *output_filename,const QString &src_xml,
  		    QString *err_msg);
  bool transform(QString *output,const QString &src_xml,QString *err_msg);

 private:
  QList<RDTempDirectory *> d_temp_directories;
  QString d_stylesheet_pathname;
};


#endif  // RDXSLTENGINE_H
