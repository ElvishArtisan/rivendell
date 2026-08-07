// rdalsamodel.cpp
//
// Abstract an ALSA configuration. 
//
//   (C) Copyright 2009-2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdalsamodel.h>

RDAlsaModel::RDAlsaModel(unsigned samprate,QObject *parent)
  : QAbstractListModel(parent)
{
  model_sample_rate=samprate;

  LoadSystemConfig();
}


int RDAlsaModel::rowCount(const QModelIndex &parent) const
{
  return model_alsa_cards.size();
}


Qt::ItemFlags RDAlsaModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags=QAbstractListModel::flags(index);

  if((model_alsa_cards.at(index.row())->id()=="Axia")&&
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
    return QVariant(model_alsa_cards.at(row)->prettyLongName());
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


QModelIndex RDAlsaModel::indexOf(const QString &card_id) const
{
  bool ok=false;

  if(ok) {
    for(int i=0;i<model_alsa_cards.size();i++) {
      if(model_alsa_cards.at(i)->id()==card_id) {
	return createIndex(i,0);
      }
    }
  }

  return QModelIndex();
}


bool RDAlsaModel::isEnabled(int row) const
{
  return model_alsa_cards.at(row)->isEnabled();
}


void RDAlsaModel::setEnabled(int row,bool state)
{
  return model_alsa_cards.at(row)->setEnabled(state);
}


bool RDAlsaModel::loadConfig(const QString &filename)
{
  FILE *f=NULL;
  char line[1024];
  int istate=0;
  int port=0;
  QString card_id=0;
  int card_num=0;
  bool ok=false;
  int device=0;
  QStringList list;
  bool active_line=false;
  QModelIndex index;

  if((f=fopen(filename.toUtf8(),"r"))==NULL) {
    return false;
  }
  while(fgets(line,1024,f)!=NULL) {
    QString str=line;
    str.replace("\n","");
    if(str==START_MARKER) {
      active_line=true;
    }
    if(str==END_MARKER) {
      active_line=false;
    }
    if((str!=START_MARKER)&&(str!=END_MARKER)) {
      if(active_line) {
	switch(istate) {
	case 0:
	  if(str.left(6)=="pcm.rd") {
	    port=str.mid(6,1).toInt();
	    istate=1;
	  }
	  else {
	    if(str.left(6)=="ctl.rd") {
	      istate=10;
	    }
	    else {
	      model_other_lines.push_back(str);
	    }
	  }
	  break;

	case 1:
	  list=str.split(" ",QString::SkipEmptyParts);
	  if(list[0]=="}") {
	    if((port>=0)&&(port<RD_MAX_CARDS)) {
	      for(int i=0;i<model_alsa_cards.size();i++) {
		RDAlsaCard *card=model_alsa_cards.at(i);
		card_num=card_id.toUInt(&ok);
		if(ok) {
		  if(card_num==card->index()) {
		    if(device>=0) {
		      card->setEnabled(true);
		    }
		  }
		}
		else {
		  if(card_id==card->id()) {
		    if(device>=0) {
		      card->setEnabled(true);
		    }
		  }
		}
	      }
	    }
	    card_id="";
	    device=0;
	    istate=0;
	  }
	  else {
	    if(list.size()==2) {
	      if(list[0]=="card") {
		card_id=list[1].trimmed();
	      }
	      if(list[0]=="device") {
		device=list[1].toInt();
	      }
	    }
	  }
	  break;

	case 10:
	  if(str.left(1)=="}") {
	    istate=0;
	  }
	  break;
	}
      }
      else {
	model_other_lines.push_back(str);
      }
    }
  }
  fclose(f);

  return true;
}


bool RDAlsaModel::saveConfig(const QString &filename)
{
  QString tempfile=filename+"-temp";
  FILE *f=NULL;
  int index=0;

  if((f=fopen(tempfile.toUtf8(),"w"))==NULL) {
    return false;
  }
  for(int i=0;i<model_other_lines.size();i++) {
    fprintf(f,"%s\n",model_other_lines.at(i).toUtf8().constData());
  }
  fprintf(f,"%s\n",START_MARKER);
  for(int i=0;i<model_alsa_cards.size();i++) {
    RDAlsaCard *card=model_alsa_cards.at(i);
    if(card->isEnabled()) {
      fprintf(f,"pcm.rd%d {\n",index);
      fprintf(f,"  type hw\n");
      fprintf(f,"  card %s\n",(const char *)card->id().toUtf8());
      fprintf(f,"  device 0\n");
      fprintf(f,"  rate %u\n",rda->system()->sampleRate());
      if(card->id()=="Axia") {
	fprintf(f,"  channels 2\n");
      }
      fprintf(f,"}\n");
      fprintf(f,"ctl.rd%d {\n",index);
      fprintf(f,"  type hw\n");
      fprintf(f,"  card %s\n",(const char *)card->id().toUtf8());
      fprintf(f,"}\n");
      index++;
    }
  }
  fprintf(f,"%s\n",END_MARKER);

  fclose(f);
  rename(tempfile.toUtf8(),filename.toUtf8());

  return true;
}


void RDAlsaModel::LoadSystemConfig()
{
  snd_ctl_t *snd_ctl=NULL;
  int index=0;

  while(snd_ctl_open(&snd_ctl,QString::asprintf("hw:%d",index).toUtf8(),0)>=0) {
    model_alsa_cards.push_back(new RDAlsaCard(snd_ctl,index));
    printf("[%d]: %s\n\n",index,model_alsa_cards.back()->dump().toUtf8().constData());
    snd_ctl_close(snd_ctl);
    index++;
  }
}
