// rddiscmodel.cpp
//
// Data model for Audio CD track information
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
#include "rdconf.h"
#include "rdescape_string.h"
#include "rddiscmodel.h"

RDDiscModel::RDDiscModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Track"));  // 00
  d_alignments.push_back(right);

  d_headers.push_back(tr("Length")); // 01
  d_alignments.push_back(right);

  d_headers.push_back(tr("Title"));  // 02
  d_alignments.push_back(left);

  d_headers.push_back(tr("Artist")); // 03
  d_alignments.push_back(left);

  d_headers.push_back(tr("Type"));   // 04
  d_alignments.push_back(left);

  d_headers.push_back(tr("Cut"));    // 05
  d_alignments.push_back(left);
}


RDDiscModel::~RDDiscModel()
{
}


QPalette RDDiscModel::palette()
{
  return d_palette;
}


void RDDiscModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDDiscModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDDiscModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDDiscModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDDiscModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDDiscModel::data(const QModelIndex &index,int role) const
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
      if(col==0) {
	return d_bold_font;
      }
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


int RDDiscModel::trackNumber(const QModelIndex &row) const
{
  return row.row();
}


bool RDDiscModel::trackContainsData(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(4)==tr("Data Track");
}


QString RDDiscModel::trackTitle(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(2).toString();
}


void RDDiscModel::setTrackTitle(const QModelIndex &row,const QString &str)
{
  if(str!=d_texts.at(row.row()).at(2).toString()) {
    d_texts[row.row()][2]=str;
    emit dataChanged(createIndex(row.row(),2),createIndex(row.row(),2));
  }
}


QString RDDiscModel::cutName(const QModelIndex &row) const
{
  return d_cut_names.at(row.row());
}


void RDDiscModel::setCutName(const QModelIndex &row,const QString &cutname)
{
  d_cut_names[row.row()]=cutname;
  if(cutname.isEmpty()) {
    d_texts[row.row()][5]=QString();
  }
  else {
    QString sql=QString("select ")+
      "CART.TITLE,"+        // 00
      "CUTS.DESCRIPTION "+  // 01
      "from CART left join CUTS "+
      "on CART.NUMBER=CUTS.CART_NUMBER "+
      "where CUTS.CUT_NAME=\""+RDEscapeString(cutname)+"\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      d_texts[row.row()][5]=q->value(0).toString()+"->"+q->value(1).toString();
    }
    else {
      d_texts[row.row()][5]="["+tr("New cart")+
	QString().sprintf(" %06u]",RDCut::cartNumber(cutname));
    }
    delete q;
  }
  emit dataChanged(createIndex(row.row(),5),createIndex(row.row(),5));
}


void RDDiscModel::clearCutNames()
{
  for(int i=0;i<d_cut_names.size();i++) {
    d_cut_names[i]=QString();
    d_texts[i][5]=QVariant();
  }
  emit dataChanged(createIndex(0,5),createIndex(rowCount(),5));
}


int RDDiscModel::leadTrack(const QModelIndex &row) const
{
  return d_lead_tracks.at(row.row());
}


void RDDiscModel::setLeadTrack(const QModelIndex &row,int track)
{
  if(track!=d_lead_tracks.at(row.row())) {
    d_lead_tracks[row.row()]=track;
    if(track<0) {
      d_texts[row.row()][5]=QString();
    }
    else {
      d_texts[row.row()][5]=tr("[continued]");
    }
    emit dataChanged(createIndex(row.row(),5),createIndex(row.row(),5));
  }
}


void RDDiscModel::clear()
{
  beginResetModel();
  d_texts.clear();
  d_cut_names.clear();
  d_lead_tracks.clear();
  endResetModel();
}


void RDDiscModel::setDisc(RDCdPlayer *player)
{
  beginResetModel();
  d_texts.clear();
  d_cut_names.clear();
  d_lead_tracks.clear();

  QList<QVariant> texts;

  for(int i=1;i<=player->tracks();i++) {
    d_texts.push_back(texts);
    d_cut_names.push_back(QString());
    d_lead_tracks.push_back(-1);

    // Track Number
    d_texts.back().push_back(QString().sprintf("%d",i));

    // Length
    d_texts.back().push_back(RDGetTimeLength(player->trackLength(i)));

    // Title
    d_texts.back().push_back(tr("Track")+QString().sprintf(" %d",i));

    // Other
    d_texts.back().push_back(QVariant());

    // Type
    if(player->isAudio(i)) {
      d_texts.back().push_back(tr("Audio Track"));
    }
    else {
      d_texts.back().push_back(tr("Data Track"));
    }

    // Cut
    d_texts.back().push_back(QVariant());
  }
  endResetModel();
}


void RDDiscModel::refresh(RDDiscRecord *rec)
{
  beginResetModel();
  for(int i=0;i<rec->tracks();i++) {
    if(!rec->trackTitle(i).isEmpty()) {
      d_texts[i][2]=rec->trackTitle(i);
    }
    if(rec->trackArtist(i).isEmpty()) {
      if(!rec->discArtist().isEmpty()) {
	d_texts[i][3]=rec->discArtist();
     }
    }
    else {
      d_texts[i][3]=rec->trackArtist(i);
    }
  }
  endResetModel();
}
