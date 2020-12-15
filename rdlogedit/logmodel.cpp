// logmodel.cpp
//
// Data model for Rivendell logs in RDLogEdit
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include "logmodel.h"

LogModel::LogModel(const QString &logname,QObject *parent)
  : RDLogModel(logname,false,parent)
{
  d_group_list=new RDGroupList();
}


LogModel::~LogModel()
{
  delete d_group_list;
}


QString LogModel::serviceName() const
{
  return d_group_list->serviceName();
}


bool LogModel::groupIsValid(const QString &grpname) const
{
  return d_group_list->groupIsValid(grpname);
}


bool LogModel::allGroupsValid() const
{
  RDLogLine *ll;

  for(int i=0;i<lineCount();i++) {
    if((ll=logLine(i))!=NULL) {
      if((ll->type()==RDLogLine::Cart)||(ll->type()==RDLogLine::Macro)) {
	if(!d_group_list->groupIsValid(ll->groupName())) {
	  return false;
	}
      }
    }
  }

  return true;
}


void LogModel::setServiceName(const QString &str)
{
  if(d_group_list->serviceName()!=str) {
    d_group_list->setServiceName(str);
    emitAllDataChanged();
  }
}


QColor LogModel::backgroundColor(int line,RDLogLine *ll) const
{
  QDateTime now=QDateTime(QDate::currentDate(),QTime::currentTime());

  switch(ll->type()) {
  case RDLogLine::Cart:
    switch(ll->validity(now)) {
    case RDCart::AlwaysValid:
      if(d_group_list->groupIsValid(ll->groupName())||
	 ll->groupName().isEmpty()) {
	return RDLogModel::backgroundColor(line,ll);
      }
      return RD_CART_INVALID_SERVICE_COLOR;
	      
    case RDCart::ConditionallyValid:
      return RD_CART_CONDITIONAL_COLOR;
	      
    case RDCart::FutureValid:
      return RD_CART_FUTURE_COLOR;
	      
    case RDCart::EvergreenValid:
      return RD_CART_EVERGREEN_COLOR;
	      
    case RDCart::NeverValid:
      return RD_CART_ERROR_COLOR;
    }
    break;

  default:
    if(d_group_list->groupIsValid(ll->groupName())||ll->groupName().isEmpty()) {
      return RDLogModel::backgroundColor(line,ll);
    }
    return RD_CART_INVALID_SERVICE_COLOR;
  }
  return RDLogModel::backgroundColor(line,ll);
}
