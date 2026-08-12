// rdalsamodel.cpp
//
// Abstract an ALSA configuration. 
//
//   (C) Copyright 2009-2026 Fred Gleason <fredg@paravelsystems.com>
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

#ifdef HAVE_ALSA
#include <alsa/control.h>
#endif // HAVE_ALSA

#include <rdapplication.h>
#include <rdalsamodel.h>

RDAlsaModel::RDAlsaModel(unsigned samprate,QObject *parent)
  : QAbstractTableModel(parent)
{
  model_sample_rate=samprate;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("PCM"));          // 00
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));   // 01
  d_alignments.push_back(left);

  LoadDevicesList();
}


int RDAlsaModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDAlsaModel::rowCount(const QModelIndex &parent) const
{
  return d_pretty_ids.size();
}


Qt::ItemFlags RDAlsaModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags=QAbstractTableModel::flags(index);

  if((model_alsa_cards.at(model_card_index.at(index.row()))->id()=="Axia")&&
     (model_sample_rate!=48000)) {
    flags=flags&Qt::ItemIsEnabled;
  }

  return flags;
}


QVariant RDAlsaModel::data(const QModelIndex &index,int role) const
{
  int row=index.row();
  int col=index.column();

  switch((Qt::ItemDataRole)role) {
  case Qt::DisplayRole:
    switch(col) {
    case 0:
      return d_pretty_ids.at(row);

    case 1:
    return QVariant(model_alsa_cards.at(model_card_index.at(row))->name()+" - "+
		    model_alsa_cards.at(model_card_index.at(row))->
		    pcmName(model_pcm_index.at(row)));
    }
    break;

  case Qt::TextAlignmentRole:
    return d_alignments.at(col);

  case Qt::DecorationRole:
  case Qt::EditRole:
  case Qt::ToolTipRole:
  case Qt::StatusTipRole:
  case Qt::WhatsThisRole:
  case Qt::SizeHintRole:
  case Qt::FontRole:
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
  if(role==Qt::DisplayRole) {
    switch(orient) {
    case Qt::Horizontal:
      return d_headers.at(section);

    case Qt::Vertical:
      break;
    }
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


RDAlsaCard *RDAlsaModel::card(int row) const
{
  return model_alsa_cards.at(model_card_index.at(row));
}


bool RDAlsaModel::isEnabled(int row) const
{
  return model_alsa_cards.at(model_card_index.at(row))->
    isEnabled(model_pcm_index.at(row));
}


void RDAlsaModel::setEnabled(int row,bool state)
{
  model_alsa_cards.at(model_card_index.at(row))->
    setEnabled(model_pcm_index.at(row),state);
}


bool RDAlsaModel::loadSelections(const QString &filename)
{
  FILE *f=NULL;
  char line[1024];
  bool active_line=false;
  int rd_index=0;
  QString rd_class;
  QString rd_type;
  int rd_slot=0;
  QString rd_card;
  int rd_device=0;
  RDAlsaCard *card=NULL;
  bool ok=false;

  if((f=fopen(filename.toUtf8(),"r"))==NULL) {
    return false;
  }
  while(fgets(line,1024,f)!=NULL) {
    QString str=QString::fromUtf8(line).trimmed();
    if(str==START_MARKER) {
      active_line=true;
      continue;
    }
    if(str==END_MARKER) {
      active_line=false;
      continue;
    }
    if((str!=START_MARKER)&&(str!=END_MARKER)) {
      if(active_line) {
	QStringList f0=str.split(" ",QString::SkipEmptyParts);
	if(f0.size()==2) {
	  if(f0.last()=="{") {  // Open Bracket
	    QStringList f1=f0.first().split(".",QString::SkipEmptyParts);
	    if(f1.size()==2) {
	      rd_class=f1.first();
	      if(f1.last().left(2)=="rd") {
		unsigned num=f1.last().right(f1.last().length()-2).toUInt(&ok);
		if(ok) {
		  rd_index=num;
		}
	      }
	    }
	  }
	  if(rd_index>=0) {
	    /*
	    printf("EXAMINING: first: %s  last: %s\n",
		   f0.first().toUtf8().constData(),
		   f0.last().toUtf8().constData());
	    */
	    /*
	      if(f0.first()=="bits") {
	      rd_bits=f0.last().toUInt();
	      }
	    */
	    if(f0.first()=="card") {
	      rd_card=f0.last();
	    }
	    if(f0.first()=="device") {
	      rd_device=f0.last().toUInt();
	    }
	    /*
	      if(f0.first()=="rate") {
	      rd_rate=f0.last().toUInt();
	      }
	    */
	    if(f0.first()=="slot") {
	      rd_slot=f0.last().toUInt();
	    }
	    if(f0.first()=="type") {
	      rd_type=f0.last();
	    }
	  }
	}
	if(f0.first()=="}") {
	  /*
	  printf("PROCESSING: class: %s  type: %s  card: %s  device: %u\n",
		 rd_class.toUtf8().constData(),
		 rd_type.toUtf8().constData(),
		 rd_card.toUtf8().constData(),
		 rd_device);
	  */
	  if(rd_class=="pcm") {
	    if(rd_type=="lw") {   // LWSound virtual devices
	      for(int i=0;i<model_alsa_cards.size();i++) {
		card=model_alsa_cards.at(i);
		if((card->driver()=="LWSound")&&
		   (card->id()==QString::asprintf("lw32_%d",rd_slot))) {
		  card->setEnabled(rd_slot,true);
		}
	      }
	    }
	    if(rd_type=="hw") {   // Hardware devices
	      for(int i=0;i<rowCount();i++) {
		card=model_alsa_cards.at(model_card_index.at(i));
		if((card->id()==rd_card)&&(rd_device==model_pcm_index.at(i))) {
		  card->setEnabled(rd_device,true);
		}
	      }
	    }
	  }
	  if(rd_class=="ctl") {

	  }
	  rd_class="";
	  rd_type="";
	  rd_card="";
	  rd_device=0;
	}
      }
      else {
	model_other_lines.push_back(str);
      }
    }
  }
  return true;
}


bool RDAlsaModel::saveSelections(const QString &filename)
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
  for(int i=0;i<rowCount();i++) {
    RDAlsaCard *card=model_alsa_cards.at(model_card_index.at(i));
    if(card->isEnabled(model_pcm_index.at(i))) {
      if(card->driver()=="LWSound") {
	QStringList f0=
	  card->id().split("_",QString::SkipEmptyParts);
	fprintf(f,"pcm.rd%d {\n",index);
	fprintf(f,"  type lw\n");
	fprintf(f,"  slot %s\n",f0.last().toUtf8().constData());
	fprintf(f,"  bits 32\n");
	fprintf(f,"}\n");
	fprintf(f,"ctl.rd%d {\n",index);
	fprintf(f,"  type lw\n");
	fprintf(f,"  slot %s\n",f0.last().toUtf8().constData());
	fprintf(f,"}\n");
      }
      else {
	fprintf(f,"pcm.rd%d {\n",index);
	fprintf(f,"  type hw\n");
	fprintf(f,"  card %s\n",
		card->id().toUtf8().constData());
	fprintf(f,"  device %d\n",model_pcm_index.at(i));
	fprintf(f,"  rate %u\n",rda->system()->sampleRate());
	if(card->id().left(4)=="Axia") {
	  fprintf(f,"  channels 2\n");
	}
	fprintf(f,"}\n");
	fprintf(f,"ctl.rd%d {\n",index);
	fprintf(f,"  type hw\n");
	fprintf(f,"  card %s\n",card->id().toUtf8().constData());
	fprintf(f,"}\n");
      }
      index++;
    }
  }
  fprintf(f,"%s\n",END_MARKER);

  fclose(f);
  rename(tempfile.toUtf8(),filename.toUtf8());

  return true;
}


void RDAlsaModel::LoadDevicesList()
{
  snd_ctl_t *snd_ctl=NULL;
  int index=0;

  //
  // Hardware Devices
  //
  while(snd_ctl_open(&snd_ctl,QString::asprintf("hw:%d",index).toUtf8(),0)>=0) {
    model_alsa_cards.push_back(new RDAlsaCard(snd_ctl,index));
    if(model_alsa_cards.back()->driver()!="Axia") {
      if(model_alsa_cards.back()->pcmQuantity()>0) {
	printf("creating pretty_id: %s\n",model_alsa_cards.back()->id().toUtf8().constData());
	d_pretty_ids.push_back(model_alsa_cards.back()->id());
      }
    }
    for(int i=0;i<model_alsa_cards.back()->pcmQuantity();i++) {
      model_card_index.push_back(index);
      model_pcm_index.push_back(i);
      if(model_alsa_cards.back()->driver()=="Axia") {
	d_pretty_ids.push_back(model_alsa_cards.back()->id()+
			       QString::asprintf(",%d",i));
      }
    }
    snd_ctl_close(snd_ctl);
    index++;
  }

  //
  // AoIP Devices
  //
  index=0;
  while(snd_ctl_open(&snd_ctl,QString::asprintf("lw32_%d",index).toUtf8(),0)>=0) {
    model_alsa_cards.push_back(new RDAlsaCard(snd_ctl,index));
    d_pretty_ids.push_back(model_alsa_cards.back()->id());
    snd_ctl_close(snd_ctl);
    index++;
  }
}
