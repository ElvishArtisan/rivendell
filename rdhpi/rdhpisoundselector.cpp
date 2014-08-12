//   rdhpisoundselector.cpp
//
//   A selection widget for audio devices.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdhpisoundselector.cpp,v 1.3 2010/07/29 19:32:36 cvs Exp $
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
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qobject.h>
#include <qwidget.h>
#include <qstring.h>
#include <qdatetime.h>

#include <rdhpisoundselector.h>



RDHPISoundSelector::RDHPISoundSelector(RDHPISoundCard::DeviceClass dev_class,
			       QWidget *parent,
			       const char *name) :QListBox(parent,name)
{  
  sound_card=new RDHPISoundCard(this,"sound_card");

  if(dev_class==RDHPISoundCard::PlayDevice) {
    for(int i=0;i<sound_card->getCardQuantity();i++) {
      for(int j=0;j<sound_card->getCardOutputPorts(i);j++) {
	insertItem(sound_card->getOutputPortDescription(i,j),
		   i*HPI_MAX_NODES+j);
      }
    }
  }
  if(dev_class==RDHPISoundCard::RecordDevice) {
    for(int i=0;i<sound_card->getCardQuantity();i++) {
      for(int j=0;j<sound_card->getCardInputPorts(i);j++) {
	insertItem(sound_card->getInputPortDescription(i,j),
		   i*HPI_MAX_NODES+j);
      }
    }
  }
  connect(this,SIGNAL(highlighted(int)),this,SLOT(selection(int)));
}


void RDHPISoundSelector::selection(int selection)
{
  emit changed(selection/HPI_MAX_ADAPTERS,
	       selection-HPI_MAX_ADAPTERS*(selection/HPI_MAX_ADAPTERS));
  emit cardChanged(selection/HPI_MAX_ADAPTERS);
  emit portChanged(selection-HPI_MAX_ADAPTERS*(selection/HPI_MAX_ADAPTERS));
}
