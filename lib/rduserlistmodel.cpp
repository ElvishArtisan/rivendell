// rduserlistmodel.cpp
//
// Data model for Rivendell users
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdapplication.h"
#include "rdescape_string.h"
#include "rduserlistmodel.h"

RDUserListModel::RDUserListModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_type_filter=RDUser::TypeLast;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Login Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Full Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("E-Mail Address"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Phone Number"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Local Auth"));
  d_alignments.push_back(center);
}


RDUserListModel::~RDUserListModel()
{
}


QPalette RDUserListModel::palette()
{
  return d_palette;
}


void RDUserListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDUserListModel::setFont(const QFont &font)
{
  d_font=font;
}


int RDUserListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDUserListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDUserListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDUserListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      if(col==0) {
	return d_icons.at(row);
      }
      break;

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      return d_font;

    case Qt::TextColorRole:
      // Nothing to do!
      break;

    case Qt::BackgroundRole:
      // Nothing to do!
      break;

    default:
      break;
    }
  }

  return QVariant();
}


QString RDUserListModel::userName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QModelIndex RDUserListModel::addUser(const QString &username)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(username<d_texts.at(i).at(0).toString()) {
      offset=i;
      break;
    }
  }

  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[0]=username;
  d_texts.insert(offset,list);
  d_icons.insert(offset,QVariant());
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDUserListModel::removeUser(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();
}


void RDUserListModel::removeUser(const QString &username)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==username) {
      removeUser(createIndex(i,0));
      return;
    }
  }
}


void RDUserListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where LOGIN_NAME=\""+
      RDEscapeString(d_texts.at(row.row()).at(0).toString())+
      "\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDUserListModel::refresh(const QString &grpname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==grpname) {
      updateRowLine(i);
      return;
    }
  }
}


RDUser::Type RDUserListModel::typeFilter() const
{
  return d_type_filter;
}


void RDUserListModel::setTypeFilter(RDUser::Type type)
{
  if(type!=d_type_filter) {
    beginResetModel();
    d_type_filter=type;
    updateModel();
    endResetModel();
  }
}


void RDUserListModel::updateModel()
{
  QList<QVariant> texts; 
  QList<QVariant> icons;

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+filterSql("");
  sql+="order by LOGIN_NAME ";
  beginResetModel();
  d_texts.clear();
  d_icons.clear();
  printf("SQL: %s\n",sql.toUtf8().constData());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    d_icons.push_back(icons);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDUserListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      filterSql(d_texts.at(line).at(0).toString());
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDUserListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Login Name
  texts.push_back(q->value(0));
  if(q->value(1).toString()=="Y") {
    d_icons[row]=rda->iconEngine()->userIcon(RDUser::TypeAdminConfig);
  }
  else {
    if(q->value(2).toString()=="Y") {
      d_icons[row]=rda->iconEngine()->userIcon(RDUser::TypeAdminRss);
    }
    else {
      if(q->value(7).toString()=="Y") {
	d_icons[row]=rda->iconEngine()->userIcon(RDUser::TypeLocalUser);
      }
      else {
	d_icons[row]=rda->iconEngine()->userIcon(RDUser::TypeExternalUser);
      }
    }
  }

  // Full Name
  texts.push_back(q->value(3));

  // Description
  texts.push_back(q->value(4));

  // E-Mail Address
  texts.push_back(q->value(5));

  // Phone Number
  texts.push_back(q->value(6));

  // Local Auth
  texts.push_back(q->value(7));

  d_texts[row]=texts;
}


QString RDUserListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "USERS.LOGIN_NAME,"+         // 00
    "USERS.ADMIN_CONFIG_PRIV,"+  // 01
    "USERS.ADMIN_RSS_PRIV,"+     // 02
    "USERS.FULL_NAME,"+          // 03
    "USERS.DESCRIPTION,"+        // 04
    "USERS.EMAIL_ADDRESS,"+      // 05
    "USERS.PHONE_NUMBER,"+       // 06
    "USERS.LOCAL_AUTH "+         // 07
    "from USERS ";

    return sql;
}


QString RDUserListModel::filterSql(const QString &username) const
{
  QString sql=" ";

  switch(d_type_filter) {
  case RDUser::TypeAll:
    if(!username.isEmpty()) {
      sql=" where LOGIN_NAME=\""+RDEscapeString(username)+"\"";
    }
    break;

  case RDUser::TypeAdminConfig:
    sql=" where (ADMIN_CONFIG_PRIV=\"Y\")";
    if(!username.isEmpty()) {
      sql+=" && (LOGIN_NAME=\""+RDEscapeString(username)+"\")";
    }
    break;

  case RDUser::TypeAdminRss:
    sql=" where (ADMIN_RSS_PRIV=\"Y\")";
    if(!username.isEmpty()) {
      sql+=" && (LOGIN_NAME=\""+RDEscapeString(username)+"\")";
    }
    break;

  case RDUser::TypeLocalUser:
    sql=QString("where (ADMIN_CONFIG_PRIV='N')&&(ADMIN_RSS_PRIV='N')&&")+
      "(LOCAL_AUTH='Y')";
    if(!username.isEmpty()) {
      sql+=" && (LOGIN_NAME=\""+RDEscapeString(username)+"\")";
    }
    break;

  case RDUser::TypeExternalUser:
    sql=QString("where (ADMIN_CONFIG_PRIV='N')&&(ADMIN_RSS_PRIV='N')&&")+
      "(LOCAL_AUTH='N')";
    if(!username.isEmpty()) {
      sql+=" && (LOGIN_NAME=\""+RDEscapeString(username)+"\")";
    }
    break;

  case RDUser::TypeUser:
    sql=QString("where (ADMIN_CONFIG_PRIV='N')&&(ADMIN_RSS_PRIV='N')");
    if(!username.isEmpty()) {
      sql+=" && (LOGIN_NAME=\""+RDEscapeString(username)+"\")";
    }
    break;

  case RDUser::TypeAdmin:
    sql=QString("where ((ADMIN_CONFIG_PRIV='Y')||(ADMIN_RSS_PRIV='Y'))");
    if(!username.isEmpty()) {
      sql+=" && (LOGIN_NAME=\""+RDEscapeString(username)+"\")";
    }
    break;

  case RDUser::TypeLast:
    break;
  }

  return sql;
}
    
