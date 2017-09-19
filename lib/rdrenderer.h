// rdrenderer.h
//
// Render a Rivendell log to a single audio object.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDRENDERER_H
#define RDRENDERER_H

#include <stdint.h>

#include <sndfile.h>

#include <qobject.h>
#include <qstringlist.h>

#include <rdconfig.h>
#include <rdlog_event.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rduser.h>

class __RDRenderLogLine : public RDLogLine
{
 public:
  __RDRenderLogLine(RDLogLine *ll,RDUser *user,RDStation *station,RDSystem *sys,
		    RDConfig *config,unsigned chans);
  RDCart *cart() const;
  RDCut *cut() const;
  SNDFILE *handle() const;
  double rampLevel() const;
  void setRampLevel(double lvl);
  double rampRate() const;
  void setRampRate(double lvl);
  void setRamp(RDLogLine::TransType next_trans);
  bool open(const QTime &time);
  void close();
  QString summary() const;

 private:
  bool GetCutFile(const QString &cutname,int start_pt,int end_pt,
		  QString *dest_filename) const;
  void DeleteCutFile(const QString &dest_filename) const;
  uint64_t FramesFromMsec(uint64_t msec);
  RDCart *ll_cart;
  RDCut *ll_cut;
  SNDFILE *ll_handle;
  RDLogLine *ll_logline;
  RDUser *ll_user;
  RDStation *ll_station;
  RDSystem *ll_system;
  RDConfig *ll_config;
  unsigned ll_channels;
  double ll_ramp_level;
  double ll_ramp_rate;
};




class RDRenderer : public QObject
{
  Q_OBJECT;
 public:
  RDRenderer(RDUser *user,RDStation *station,RDSystem *system,RDConfig *config,
	     QObject *parent=0);
  ~RDRenderer();
  bool renderToFile(const QString &outfile,RDLogEvent *log,unsigned chans,
		    RDSettings *s,const QTime &start_time,bool ignore_stops,
		    QString *err_msg,int first_line,int last_line,
		    const QTime &first_time=QTime(),
		    const QTime &last_time=QTime());
  bool renderToCart(unsigned cartnum,int cutnum,RDLogEvent *log,unsigned chans,
		    RDSettings *s,const QTime &start_time,bool ignore_stops,
		    QString *err_msg,int first_line,int last_line,
		    const QTime &first_time=QTime(),
		    const QTime &last_time=QTime());
  QStringList warnings() const;

 public slots:
  void abort();

 signals:
  void progressMessageSent(const QString &msg);
  void lineStarted(int linno);

 private:
  bool Render(const QString &outfile,RDLogEvent *log,unsigned chans,
	      RDSettings *s,const QTime &start_time,bool ignore_stops,
	      QString *err_msg,int first_line,int last_line,
	      const QTime &first_time,const QTime &last_time);
  void Sum(float *pcm_out,__RDRenderLogLine *ll,sf_count_t frames,
	   unsigned chans);
  bool ConvertAudio(const QString &srcfile,const QString &dstfile,
		    RDSettings *s,QString *err_msg);
  bool ImportCart(const QString &srcfile,unsigned cartnum,int cutnum,
		  QString *err_msg);
  uint64_t FramesFromMsec(uint64_t msec) const;
  void DeleteTempFile(const QString &filename) const;
  void ProgressMessage(const QString &msg);
  void ProgressMessage(const QTime &time,int line,const QString &trans,
		       const QString &msg);
  RDUser *render_user;
  RDStation *render_station;
  RDSystem *render_system;
  RDConfig *render_config;
  QStringList render_warnings;
  bool render_abort;
};


#endif  // RDRENDERER_H
