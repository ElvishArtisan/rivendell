// rdunittestdata.cpp
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

#include "rdunittestdata.h"

RDUnitTestData::RDUnitTestData()
{
}


QString RDUnitTestData::groupName() const
{
  return data_group_name;
}


void RDUnitTestData::setGroupName(const QString &str)
{
  data_group_name=str;
}


int RDUnitTestData::testQuantity() const
{
  return data_test_names.size();
}


QString RDUnitTestData::testName(int n) const
{
  return data_test_names[n];
}


QString RDUnitTestData::testResultDescription(int n) const
{
  return data_test_result_descriptions[n];
}


bool RDUnitTestData::testResult(int n) const
{
  return data_test_results[n];
}


void RDUnitTestData::addTest(const QString &name,bool result,
			     const QString &result_desc)
{
  data_test_names.push_back(name);
  data_test_result_descriptions.push_back(result_desc);
  data_test_results.push_back(result);
}


void RDUnitTestData::clear()
{
  data_group_name="";
  data_test_names.clear();
  data_test_result_descriptions.clear();
  data_test_results.clear();
}

