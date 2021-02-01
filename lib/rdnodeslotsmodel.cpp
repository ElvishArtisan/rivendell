// rdnodeslotsmodel.cpp
//
// Data model for Rivendell switcher nodes.
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
#include "rdnodeslotsmodel.h"

RDNodeSlotsModel::RDNodeSlotsModel(bool is_src,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_is_source=is_src;
  d_base_output=0;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  if(d_is_source) {
    d_headers.push_back(tr("#"));
    d_alignments.push_back(right);

    d_headers.push_back(tr("Input #"));
    d_alignments.push_back(right);

    d_headers.push_back(tr("Name"));
    d_alignments.push_back(left);

    d_headers.push_back(tr("Active"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Shareable"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Chans"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Gain"));
    d_alignments.push_back(right);
  }
  else {
    d_headers.push_back(tr("#"));
    d_alignments.push_back(right);

    d_headers.push_back(tr("Output #"));
    d_alignments.push_back(right);

    d_headers.push_back(tr("Name"));
    d_alignments.push_back(left);

    d_headers.push_back(tr("Chans"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Load"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Gain"));
    d_alignments.push_back(right);
  }
}


RDNodeSlotsModel::~RDNodeSlotsModel()
{
}


QPalette RDNodeSlotsModel::palette()
{
  return d_palette;
}


void RDNodeSlotsModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDNodeSlotsModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDNodeSlotsModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDNodeSlotsModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDNodeSlotsModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDNodeSlotsModel::data(const QModelIndex &index,int role) const
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


void RDNodeSlotsModel::setBaseOutput(int output)
{
  d_base_output=output;
}


void RDNodeSlotsModel::setSlotQuantity(int slot_quan)
{
  beginResetModel();
  d_texts.clear();
  QList<QVariant> texts;
  for(int i=0;i<columnCount();i++) {
    texts.push_back(QVariant());
  }
  for(int i=0;i<slot_quan;i++) {
    d_texts.push_back(texts);
    d_texts.back()[0]=QString().sprintf("%d",i+1);
  }
  endResetModel();
}


void RDNodeSlotsModel::updateSource(unsigned id,RDLiveWireSource *src)
{
  if(d_is_source) {
    int slotnum=src->slotNumber()-1;
    if(src->channelNumber()<=0) {
      d_texts[slotnum][1]=tr("[unassigned]");
    }
    else {
      d_texts[slotnum][1]=QString().sprintf("%d",src->channelNumber());
    }
    d_texts[slotnum][2]=src->primaryName();
    if(src->rtpEnabled()) {
      d_texts[slotnum][3]=tr("Yes");
    }
    else {
      d_texts[slotnum][3]=tr("No");
    }
    if(src->shareable()) {
      d_texts[slotnum][4]=tr("Yes");
    }
    else {
      d_texts[slotnum][4]=tr("No");
    }
    d_texts[slotnum][5]=QString().sprintf("%d",src->channels());
    d_texts[slotnum][6]=QString().sprintf("%4.1f",(float)src->inputGain()/10.0);

    emit dataChanged(createIndex(slotnum,0),
		     createIndex(slotnum,columnCount()-1));
  }
}


void RDNodeSlotsModel::updateDestination(unsigned id,RDLiveWireDestination *dst)
{
  if(!d_is_source) {
    int slotnum=dst->slotNumber()-1;

    d_texts[slotnum][1]=
      QString().sprintf("%u",d_base_output+dst->slotNumber()-1);
    d_texts[slotnum][2]=dst->primaryName();
    d_texts[slotnum][3]=QString().sprintf("%d",dst->channels());
    d_texts[slotnum][4]=RDLiveWireDestination::loadString(dst->load());
    d_texts[slotnum][5]=
      QString().sprintf("%4.1f",(float)dst->outputGain()/10.0);

    emit dataChanged(createIndex(slotnum,0),
		     createIndex(slotnum,columnCount()-1));
  }
}
