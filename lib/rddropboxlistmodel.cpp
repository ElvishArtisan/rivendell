// rddropboxlistmodel.cpp
//
// Data model for Rivendell dropboxes
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
#include "rddropboxlistmodel.h"

RDDropboxListModel::RDDropboxListModel(const QString &hostname,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_hostname=hostname;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("ID"));                // 00
  d_alignments.push_back(right);

  d_headers.push_back(tr("Group"));             // 01
  d_alignments.push_back(left);

  d_headers.push_back(tr("Path"));              // 02
  d_alignments.push_back(left);

  d_headers.push_back(tr("Norm. Level"));       // 03
  d_alignments.push_back(right);

  d_headers.push_back(tr("Autotrim Level"));    // 04
  d_alignments.push_back(right);

  d_headers.push_back(tr("To Cart"));           // 05
  d_alignments.push_back(right);

  d_headers.push_back(tr("Force Mono"));        // 06
  d_alignments.push_back(center);

  d_headers.push_back(tr("Use CC ID"));         // 07
  d_alignments.push_back(center);

  d_headers.push_back(tr("Delete Cuts"));       // 08
  d_alignments.push_back(center);

  d_headers.push_back(tr("Metadata Pattern"));  // 09
  d_alignments.push_back(center);

  d_headers.push_back(tr("User Defined"));      // 10
  d_alignments.push_back(center);

  updateModel();
}


RDDropboxListModel::~RDDropboxListModel()
{
}


QPalette RDDropboxListModel::palette()
{
  return d_palette;
}


void RDDropboxListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDDropboxListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDDropboxListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDDropboxListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDDropboxListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDDropboxListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      // Nothing to do!
      break;

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      if(col==1) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      if(col==1) {
	return d_group_colors.at(row);
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


int RDDropboxListModel::dropboxId(const QModelIndex &row) const
{
  return d_box_ids.at(row.row());
}


QModelIndex RDDropboxListModel::addDropbox(int box_id)
{
  //
  // Find the insertion offset
  //
  int offset=d_box_ids.size();
  for(int i=0;i<d_box_ids.size();i++) {
    if(box_id<d_box_ids.at(i)) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  d_box_ids.insert(offset,box_id);
  d_group_colors.insert(offset,QVariant());
  list[0]=QString().sprintf("%d",box_id);
  d_texts.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDDropboxListModel::removeDropbox(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_box_ids.removeAt(row.row());
  d_group_colors.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDDropboxListModel::removeDropbox(int box_id)
{
  for(int i=0;i<d_box_ids.size();i++) {
    if(d_box_ids.at(i)==box_id) {
      removeDropbox(createIndex(i,0));
      return;
    }
  }
}


void RDDropboxListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("DROPBOXES.ID=%d",d_box_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDDropboxListModel::refresh(int box_id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_box_ids.at(i)==box_id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDDropboxListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where DROPBOXES.STATION_NAME=\""+RDEscapeString(d_hostname)+"\" "+
    "order by DROPBOXES.ID ";
  beginResetModel();
  d_box_ids.clear();
  d_group_colors.clear();
  d_texts.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_box_ids.push_back(-1);
    d_group_colors.push_back(QVariant());
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDDropboxListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("DROPBOXES.ID=%d",d_box_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDDropboxListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // ID
  d_box_ids.back()=q->value(0).toInt();
  texts.push_back(q->value(0));

  // Group
  texts.push_back(q->value(1));
  d_group_colors.back()=QColor(q->value(2).toString());

  // Path
  texts.push_back(q->value(3));

  // Norm. Level
  if(q->value(4).toInt()==0) {
    texts.push_back(tr("[off]"));
  }
  else {
    texts.push_back(QString().sprintf("%d dBFS",q->value(4).toInt()));
  }

  // Autotrim Level
  if(q->value(5).toInt()==0) {
    texts.push_back(tr("[off]"));
  }
  else {
    texts.push_back(QString().sprintf("%d dBFS",q->value(5).toInt()));
  }

  // To Cart
  if(q->value(6).toUInt()==0) {
    texts.push_back(tr("[auto]"));
  }
  else {
    texts.push_back(QString().sprintf("%06u",q->value(6).toUInt()));
  }

  // Force to Mono
  texts.push_back(q->value(7));

  // Use CC ID
  texts.push_back(q->value(8));

  // Delete Cuts
  texts.push_back(q->value(9));

  // Metadata Pattern
  texts.push_back(q->value(10));

  // User Defined
  texts.push_back(q->value(11));

  d_texts[row]=texts;
}


QString RDDropboxListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "DROPBOXES.ID,"+                   // 00
    "DROPBOXES.GROUP_NAME,"+           // 01
    "GROUPS.COLOR,"+                   // 02
    "DROPBOXES.PATH,"+                 // 03
    "DROPBOXES.NORMALIZATION_LEVEL,"+  // 04
    "DROPBOXES.AUTOTRIM_LEVEL,"+       // 05
    "DROPBOXES.TO_CART,"+              // 06
    "DROPBOXES.FORCE_TO_MONO,"+        // 07
    "DROPBOXES.USE_CARTCHUNK_ID,"+     // 08
    "DROPBOXES.DELETE_CUTS,"+          // 09
    "DROPBOXES.METADATA_PATTERN,"+     // 10
    "DROPBOXES.SET_USER_DEFINED "+         // 11
    "from DROPBOXES left join GROUPS "+
    "on DROPBOXES.GROUP_NAME=GROUPS.NAME ";

    return sql;
}
