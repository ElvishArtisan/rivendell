// local_macros.cpp
//
// Local macros for the Rivendell netcatcher daemon
//
//   (C) Copyright 2002-2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: local_macros.cpp,v 1.4 2010/07/29 19:32:36 cvs Exp $
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

#include <rdcart.h>
#include <rduser.h>

#include <rdcatchd.h>

void MainObject::RunLocalMacros(RDMacro *rml)
{
  RDCart *cart;
  RDCut *cut;
  RDDeck *deck;
  int chan;
  unsigned cartnum;
  unsigned cutnum;
  unsigned dst_cartnum;
  unsigned dst_cutnum;
  unsigned len;
  unsigned decknum;
  unsigned eventnum;
  QDateTime dt;
  RDUser *user;
  bool ok=false;
  CatchEvent e;
  unsigned event_ptr=0;
  QString sql;
  RDSqlQuery *q;

  switch(rml->command()) {
  case RDMacro::CE:
    if(rml->argQuantity()==2) {
      decknum=rml->arg(0).toUInt()-1;
      if(decknum<MAX_DECKS) {
	eventnum=rml->arg(1).toUInt();
	if((eventnum>0)&&(eventnum<=RD_CUT_EVENT_ID_QUAN)) {
	  if(catch_record_deck_status[decknum]==RDDeck::Recording) {
	    if((event_ptr=GetEvent(catch_record_id[decknum]))<
	       catch_events.size()) {
	      e=catch_events[event_ptr];
	      sql=QString("insert into CUT_EVENTS set ")+
		"CUT_NAME=\""+e.cutName()+"\","+
		QString().sprintf("NUMBER=%u,",eventnum)+
		QString().sprintf("POINT=%u",e.startTime().
				  msecsTo(QTime::currentTime()));
	      q=new RDSqlQuery(sql);
	      delete q;
	    }
	  }
	  if(rml->echoRequested()) {
	    rml->acknowledge(true);
	    catch_ripc->sendRml(rml);
	  }
	  return;
	}
      }
    }
    if(rml->echoRequested()) {
      rml->acknowledge(false);
      catch_ripc->sendRml(rml);
    }
    break;

  case RDMacro::CP:
    cartnum=rml->arg(0).toUInt();
    cutnum=rml->arg(1).toUInt();
    dst_cartnum=rml->arg(2).toUInt();
    dst_cutnum=rml->arg(3).toUInt();
    if((cartnum<1)||(cartnum>RD_MAX_CART_NUMBER)||
       (dst_cartnum<1)||(dst_cartnum>RD_MAX_CART_NUMBER)||
       (cutnum<1)||(cutnum>RD_MAX_CUT_NUMBER)||
       (dst_cutnum<1)||(dst_cutnum>RD_MAX_CUT_NUMBER)||
       (catch_ripc->user().isEmpty())) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	catch_ripc->sendRml(rml);
      }
      return;
    }
    user=new RDUser(catch_ripc->user());
    cut=new RDCut(cartnum,cutnum);
    ok=cut->copyTo(catch_rdstation,user,RDCut::cutName(dst_cartnum,dst_cutnum),
		   catch_config);
    delete cut;
    delete user;
    if(rml->echoRequested()) {
      rml->acknowledge(ok);
      catch_ripc->sendRml(rml);
    }
    break;

  case RDMacro::EX:
    cart=new RDCart(rml->arg(0).toUInt());
    if(cart->exists()) {
      if(ExecuteMacroCart(cart)) {
	if(rml->echoRequested()) {
	  rml->acknowledge(true);
	  catch_ripc->sendRml(rml);
	}
	delete cart;
	return;
      }
    }
    else {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	catch_ripc->sendRml(rml);
      }
    }
    delete cart;
    return;
    break;

  case RDMacro::RS:
    //
    // Validate Parameters
    //
    if(rml->argQuantity()!=4) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	catch_ripc->sendRml(rml);
      }
      return;
    }
    chan=rml->arg(0).toInt();
    cartnum=rml->arg(1).toUInt();
    cutnum=rml->arg(2).toUInt();
    len=rml->arg(3).toUInt();
    if((chan<=0)||(chan>MAX_DECKS)||(cartnum>999999)||(cutnum>999)
       ||len==0) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	catch_ripc->sendRml(rml);
      }
      return;
    }
    cut=new RDCut(cartnum,cutnum);
    deck=new RDDeck(catch_config->stationName(),chan);
    if((!cut->exists())||(!deck->isActive())) {
      delete cut;
      delete deck;
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	catch_ripc->sendRml(rml);
      }
      return;
    }
    delete cut;
    delete deck;
    if(catch_record_deck_status[chan-1]!=RDDeck::Idle) {
      if(catch_record_aborting[chan-1]&&
	 (catch_record_pending_cartnum[chan-1]==0)) {
	// Cache Event
	catch_record_pending_cartnum[chan-1]=cartnum;
	catch_record_pending_cutnum[chan-1]=cutnum;
	catch_record_pending_maxlen[chan-1]=len;
	if(rml->echoRequested()) {
	  rml->acknowledge(true);
	  catch_ripc->sendRml(rml);
	}
	return;
      }
      else {
	LogLine(RDConfig::LogWarning,QString().
		sprintf("unable to handle RS macro for deck %d: device busy",
			chan));
	if(rml->echoRequested()) {
	  rml->acknowledge(false);
	  catch_ripc->sendRml(rml);
	}
	return;
      }
    }

    //
    // Create Event
    //
    StartRmlRecording(chan,cartnum,cutnum,len);
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      catch_ripc->sendRml(rml);
    }
    break;

  case RDMacro::RR:
    if(rml->argQuantity()!=1) {
      if(rml->echoRequested()) {
	rml->acknowledge(false);
	catch_ripc->sendRml(rml);
      }
      return;
    }
    chan=rml->arg(0).toInt();
    if((chan>0)&&(chan<(MAX_DECKS+1))) {
      switch(catch_record_deck_status[chan-1]) {
      case RDDeck::Recording:
	catch_record_aborting[chan-1]=true;
	catch_cae->stopRecord(catch_record_card[chan-1],
			      catch_record_stream[chan-1]);
	break;
	
      case RDDeck::Waiting:
	startTimerData(catch_record_id[chan-1]);
	break;
	
      default:
	break;
      }
    }
    if(rml->echoRequested()) {
      rml->acknowledge(true);
      catch_ripc->sendRml(rml);
    }
    break;

  default:
    break;
  }
}
