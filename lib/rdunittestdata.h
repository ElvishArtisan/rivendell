// rdunittestdata.h
//
// Contain class for unit test results.
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

#include <vector>

#include <QString>
#include <QStringList>

#ifndef RDUNITTESTDATA_H
#define RDUNITTESTDATA_H

class RDUnitTestData
{
 public:
  RDUnitTestData();
  QString groupName() const;
  void setGroupName(const QString &str);
  int testQuantity() const;
  QString testName(int n) const;
  QString testResultDescription(int n) const;
  bool testResult(int n) const;
  void addTest(const QString &name,bool result,const QString &result_desc="");
  void clear();

 private:
  QString data_group_name;
  QStringList data_test_names;
  QStringList data_test_result_descriptions;
  std::vector<bool> data_test_results;
};


#endif   // RDUNITTESTDATA_H
