// rdexport.h
//
// A Batch Exporter for Rivendell.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEXPORT_H
#define RDEXPORT_H

#include <vector>

#include <qobject.h>

#include <rdcart.h>
#include <rdconfig.h>
#include <rdcut.h>
#include <rddb.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rduser.h>

#define RDEXPORT_USAGE "[options] <output-dir>\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userData();

 private:
  void ExportGroup(const QString &groupname);
  void ExportCart(unsigned cartnum);
  void ExportCut(RDCart *cart,RDCut *cut);
  QString ResolveOutputName(RDCart *cart,RDCut *cut,const QString &exten);
  QString SanitizePath(const QString &pathname) const;
  void Verbose(const QString &msg);
  std::vector<unsigned> export_start_carts;
  std::vector<unsigned> export_end_carts;
  std::vector<QString> export_groups;
  QString export_metadata_pattern;
  QString export_output_to;
  QString export_format;
  RDSettings::Format export_set_format;
  unsigned export_samplerate;
  unsigned export_bitrate;
  unsigned export_channels;
  int export_quality;
  bool export_xml;
  RDConfig *export_config;
  RDRipc *export_ripc;
  RDStation *export_station;
  RDUser *export_user;
  bool export_verbose;
  QString export_escape_string;
  bool export_continue_after_error;
  bool export_allow_clobber;
};


#endif  // RDEXPORT_H
