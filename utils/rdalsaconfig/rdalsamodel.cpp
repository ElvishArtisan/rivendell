// rdalsamodel.cpp
//
// Abstract an ALSA configuration. 
//
//   (C) Copyright 2009-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <alsa/asoundlib.h>

#include <qstringlist.h>

#include <rdalsamodel.h>

RDAlsaModel::RDAlsaModel(unsigned samprate,QObject *parent)
  : QAbstractListModel(parent)
{
  model_sample_rate=samprate;

  LoadSystemConfig();
}


int RDAlsaModel::rowCount(const QModelIndex &parent) const
{
  int rows=0;

  for(int i=0;i<model_alsa_cards.size();i++) {
    rows+=model_alsa_cards.at(i)->pcmQuantity();
  }

  return rows;
}


Qt::ItemFlags RDAlsaModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags=QAbstractListModel::flags(index);

  if((model_alsa_cards.at(model_card_index.at(index.row()))->id()=="Axia")&&
     (model_sample_rate!=48000)) {
    flags=flags&Qt::ItemIsEnabled;
  }

  return flags;
}


QVariant RDAlsaModel::data(const QModelIndex &index,int role) const
{
  int row=index.row();

  switch((Qt::ItemDataRole)role) {
  case Qt::DisplayRole:
    return QVariant(model_alsa_cards.at(model_card_index.at(row))->name()+" - "+
    		    model_alsa_cards.at(model_card_index.at(row))->
		    pcmName(model_pcm_index.at(row)));
    break;

  case Qt::DecorationRole:
  case Qt::EditRole:
  case Qt::ToolTipRole:
  case Qt::StatusTipRole:
  case Qt::WhatsThisRole:
  case Qt::SizeHintRole:
  case Qt::FontRole:
  case Qt::TextAlignmentRole:
  case Qt::BackgroundColorRole:
  case Qt::TextColorRole:
  case Qt::CheckStateRole:
  case Qt::AccessibleTextRole:
  case Qt::AccessibleDescriptionRole:
  case Qt::InitialSortOrderRole:
  case Qt::DisplayPropertyRole:
  case Qt::DecorationPropertyRole:
  case Qt::ToolTipPropertyRole:
  case Qt::StatusTipPropertyRole:
  case Qt::WhatsThisPropertyRole:
  case Qt::UserRole:
    break;
  }

  return QVariant();
}


QVariant RDAlsaModel::headerData(int section,Qt::Orientation orient,
				 int role) const
{
  switch(orient) {
  case Qt::Horizontal:
    return QVariant(tr("ALSA Devices"));

  case Qt::Vertical:
    break;
  }

  return QVariant();
}


QModelIndex RDAlsaModel::indexOf(const QString &card_id,int pcm_num) const
{
  bool ok=false;
  int cardnum=card_id.toUInt(&ok);

  if(ok) {
    for(int i=0;i<model_card_index.size();i++) {
      if((model_card_index.at(i)==cardnum)&&
	 (model_pcm_index.at(i)==pcm_num)) {
	return createIndex(i,0);
      }
    }
  }
  else {
    for(int i=0;i<model_card_index.size();i++) {
      if((model_alsa_cards.at(model_card_index.at(i))->id()==card_id)&&
	 (model_pcm_index.at(i)==pcm_num)) {
	return createIndex(i,0);
      }
    }
  }

  return QModelIndex();
}


RDAlsaCard *RDAlsaModel::card(const QModelIndex &index) const
{
  return model_alsa_cards.at(model_card_index.at(index.row()));
}


int RDAlsaModel::pcmNumber(const QModelIndex &index) const
{
  return model_pcm_index.at(index.row());
}


void RDAlsaModel::LoadSystemConfig()
{
  snd_ctl_t *snd_ctl=NULL;
  int index=0;

  while(snd_ctl_open(&snd_ctl,QString().sprintf("hw:%d",index),0)>=0) {
    model_alsa_cards.push_back(new RDAlsaCard(snd_ctl,index));
    for(int i=0;i<model_alsa_cards.back()->pcmQuantity();i++) {
      model_card_index.push_back(index);
      model_pcm_index.push_back(i);
    }
    snd_ctl_close(snd_ctl);
    index++;
  }
}
