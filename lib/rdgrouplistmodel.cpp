// rdgrouplistmodel.cpp
//
// Data model for Rivendell groups
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
#include "rdgrouplistmodel.h"

RDGroupListModel::RDGroupListModel(bool show_all,bool user_is_admin,
				   QObject *parent)
  : QAbstractTableModel(parent)
{
  d_show_all=show_all;
  d_user_is_admin=user_is_admin;
  d_service_names.push_back(tr("ALL"));

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Start Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("End Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Enforce Range"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("E-Mail Addresses"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Traffic Report"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Music Report"));
  d_alignments.push_back(center);

  if(user_is_admin) {
    changeUser();
  }
}


RDGroupListModel::~RDGroupListModel()
{
}


QPalette RDGroupListModel::palette()
{
  return d_palette;
}


void RDGroupListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDGroupListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDGroupListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDGroupListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDGroupListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDGroupListModel::data(const QModelIndex &index,int role) const
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
      if(col==0) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      if(col==0) {
	return d_colors.at(row);
      }
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


QString RDGroupListModel::groupName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QStringList RDGroupListModel::allGroupNames() const
{
  QStringList ret;

  if(d_show_all) {
    for(int i=1;i<d_texts.size();i++) {
      ret.push_back(d_texts.at(i).at(0).toString());
    }
  }
  else {
    for(int i=0;i<d_texts.size();i++) {
      ret.push_back(d_texts.at(i).at(0).toString());
    }
  }

  return ret;
}


QModelIndex RDGroupListModel::addGroup(const QString &grpname)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(grpname<d_texts.at(i).at(0).toString()) {
      offset=i;
      break;
    }
  }

  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[0]=grpname;
  d_texts.insert(offset,list);
  d_colors.insert(offset,QVariant());
  d_icons.insert(offset,QVariant());
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


QModelIndex RDGroupListModel::renameGroup(const QString &oldname,
					  const QString &newname)
{
  removeGroup(oldname);
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==newname) {  // Merged to an existing group?
      updateRowLine(i);
      return createIndex(i,0);
    }
  }
  return addGroup(newname);
}


void RDGroupListModel::removeGroup(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());
  d_colors.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();
}


void RDGroupListModel::removeGroup(const QString &grpname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==grpname) {
      removeGroup(createIndex(i,0));
      return;
    }
  }
}


void RDGroupListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where NAME=\""+RDEscapeString(d_texts.at(row.row()).at(0).toString())+
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


void RDGroupListModel::refresh(const QString &grpname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==grpname) {
      updateRowLine(i);
      return;
    }
  }
}


void RDGroupListModel::changeUser()
{
  QString sql;
  RDSqlQuery *q=NULL;

  d_visible_groups.clear();
  d_visible_groups.push_back(tr("ALL"));
  if(d_user_is_admin) {
    sql=QString("select ")+
      "`NAME` "+  // 00
      "from `GROUPS` "+
      "order by `NAME`";
  }
  else {
    sql=QString("select ")+
      "`GROUP_NAME` "+  // 00
      "from `USER_PERMS` where "+
      "`USER_NAME`='"+RDEscapeString(rda->user()->name())+"' "+
      "order by `GROUP_NAME`";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_visible_groups.push_back(q->value(0).toString());
  }
  delete q;

  beginResetModel();
  updateModel();
  endResetModel();
}


void RDGroupListModel::updateModel()
{
  QList<QVariant> texts; 
  QList<QVariant> icons;

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+filterSql();
  sql+="order by `NAME` ";
  beginResetModel();
  d_texts.clear();
  d_colors.clear();
  d_icons.clear();

  if(d_show_all) {
    d_texts.push_back(texts);
    d_texts.back().push_back(tr("ALL"));
    d_colors.push_back(QVariant());
    d_icons.push_back(icons);    
  }

  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    d_colors.push_back(QVariant());
    d_icons.push_back(icons);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDGroupListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where `NAME`='"+RDEscapeString(d_texts.at(line).at(0).toString())+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDGroupListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Group Name
  texts.push_back(q->value(0));
  d_colors[row]=QColor(q->value(9).toString());
  if(q->value(5).toInt()==RDCart::Macro) {
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Macro);
  }
  else {
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Cart);
  }

  // Description
  texts.push_back(q->value(1));

  // Start Cart
  if(q->value(2).toUInt()==0) {
    texts.push_back(tr("[none]"));
  }
  else {
    texts.push_back(QString::asprintf("%06u",q->value(2).toUInt()));
  }

  // End Cart
  if(q->value(3).toUInt()==0) {
    texts.push_back(tr("[none]"));
  }
  else {
    texts.push_back(QString::asprintf("%06u",q->value(3).toUInt()));
  }

  // Enforce Range
  texts.push_back(q->value(4));

  // Notification E-Mail Addresses
  texts.push_back(q->value(6));

  // Traffic Report
  texts.push_back(q->value(7));

  // Music Report
  texts.push_back(q->value(8));

  d_texts[row]=texts;
}


QString RDGroupListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`NAME`,"+                // 00
    "`DESCRIPTION`,"+         // 01
    "`DEFAULT_LOW_CART`,"+    // 02
    "`DEFAULT_HIGH_CART`,"+   // 03
    "`ENFORCE_CART_RANGE`,"+  // 04
    "`DEFAULT_CART_TYPE`,"+   // 05
    "`NOTIFY_EMAIL_ADDRESS`," // 06
    "`REPORT_TFC`,"+          // 07
    "`REPORT_MUS`,"+          // 08
    "`COLOR` "+               // 09
    "from `GROUPS` ";

    return sql;
}


QString RDGroupListModel::filterSql() const
{
  QString sql=QString(" where (");

  for(int i=0;i<d_visible_groups.size();i++) {
    sql+=QString("(`GROUPS`.`NAME`='")+RDEscapeString(d_visible_groups.at(i))+"')||";
  }
  sql=sql.left(sql.length()-2);
  sql+=") ";

  return sql;
}
    
