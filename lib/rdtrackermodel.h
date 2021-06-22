// rdtrackermodel.h
//
// Data model for Rivendell logs RDTrackerWidget
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTRACKERMODEL_H
#define RDTRACKERMODEL_H

#include <rdgroup_list.h>
#include <rdlogmodel.h>

class RDTrackerModel : public RDLogModel
{
  Q_OBJECT
 public:
  RDTrackerModel(QObject *parent=0);
  ~RDTrackerModel();
  QString serviceName() const;
  bool groupIsValid(const QString &grpname) const;
  bool allGroupsValid() const;

 public slots:
   void setServiceName(const QString &str);

 protected:
  QColor rowBackgroundColor(int row,RDLogLine *ll) const;

 private:
  RDGroupList *d_group_list;
};


#endif  // RDTRACKERMODEL_H
