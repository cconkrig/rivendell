// rdlogplay.h
//
// Rivendell Log Playout Machine
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOGPLAY_H
#define RDLOGPLAY_H

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QSignalMapper>
#include <QTimer>

#include <rd.h>
#include <rdairplay_conf.h>
#include <rdapplication.h>
#include <rdevent_player.h>
#include <rdlog.h>
#include <rdlogmodel.h>
#include <rdmacro_event.h>
#include <rdplay_deck.h>
#include <rdsimpleplayer.h>
#include <rdunixsocket.h>

//
// Widget Settings
//
#define LOGPLAY_MAX_PLAYS 12
#define TRANSPORT_QUANTITY 12
#define LOGPLAY_LOOKAHEAD_EVENTS 20
#define LOGPLAY_RESCAN_INTERVAL 5000
#define LOGPLAY_RESCAN_SIZE 30

class RDLogPlay : public RDLogModel
{
 Q_OBJECT
 public:
  RDLogPlay(int id,RDEventPlayer *player,bool enable_cue,QObject *parent);
  QString serviceName() const;
  void setServiceName(const QString &svcname);
  QString defaultServiceName() const;
  void setDefaultServiceName(const QString &svcname);
  int card(int channum) const;
  int port(int channum) const;
  bool channelsValid() const;
  RDAirPlayConf::OpMode mode() const;
  void setOpMode(RDAirPlayConf::OpMode mode);
  void setLogName(QString name);
  void setChannels(int cards[2],int ports[2],QString labels[2],
		   const QString start_rml[2],const QString stop_rml[2]);
  void setNowCart(unsigned cartnum);
  void setNextCart(unsigned cartnum);
  void auditionHead(int line);
  void auditionTail(int line);
  void auditionStop();
  bool play(int line,RDLogLine::StartSource src,
	    int mport=-1,bool skip_meta=false);
  bool channelPlay(int mport);
  bool stop(bool all=true,int port=0,int fade=0);
  bool stop(int line,int fade=0);
  bool channelStop(int mport);
  bool pause(int line);
  void duckVolume(int level,int fade,int mport=-1);
  void makeNext(int line,bool refresh_status=true);
  void load();
  void append(const QString &log_name);
  bool refresh();
  void save(int line=-1);
  void clear();
  void insert(int line,int cartnum,RDLogLine::TransType next_type,
	      RDLogLine::TransType type=RDLogLine::Play);
  void insert(int line,RDLogLine *logline,bool update=true,
	      bool preserv_custom_transition=false);
  void remove(int line,int num_lines,bool update=true,
	      bool preserv_custom_transition=false);
  void move(int from_line,int to_line);
  void copy(int from_line,int to_line,
	    RDLogLine::TransType type=RDLogLine::Play);
  int topLine();
  int currentLine() const;
  int nextLine() const;
  int nextLine(int line);
  RDLogLine *nextEvent();
  RDLogLine::TransType nextTrans();
  RDLogLine::TransType nextTrans(int line);
  void transportEvents(int line[]);
  int runningEvents(int *line, bool include_paused=true);
  void lineModified(int line);
  RDLogLine::Status status(int line);
  QTime startTime(int line);
  QTime nextStop() const;
  int startOfHour(int hour) const;
  bool running(bool include_paused=true);
  void resync();
  bool isRefreshable() const;

 public slots:
  void setSlotQuantity(int slot_quan);

 private slots:
  void transTimerData();
  void graceTimerData();
  void playStateChangedData(int id,RDPlayDeck::State state);
  void onairFlagChangedData(bool state);
  void segueStartData(int);
  void segueEndData(int);
  void talkStartData(int);
  void talkEndData(int);
  void positionData(int,int);
  void macroStartedData();
  void macroFinishedData();
  void macroStoppedData();
  void timescalingSupportedData(int card,bool state);
  void auditionStartedData();
  void auditionStoppedData();
  void notificationReceivedData(RDNotification *notify);

 signals:
  void renamed();
  void reloaded();
  void transportChanged();
  void inserted(int line);
  void removed(int line,int num,bool moving);
  void modified(int line);
  void hourChanged(int hour,bool state);
  void auditionHeadPlayed(int line);
  void auditionTailPlayed(int line);
  void auditionStopped(int line);
  void played(int line);
  void paused(int line);
  void stopped(int line);
  void position(int line,int point);
  void topEventChanged(int line);
  void nextEventChanged(int line);
  void activeEventChanged(int line,RDLogLine::TransType trans);
  void nextStopChanged(QTime time);
  void postPointChanged(QTime point,int offset,bool offset_valid,bool running);
  void runStatusChanged(bool running);
  void refreshabilityChanged(bool state);
  void refreshStatusChanged(bool active);
  void channelStarted(int id,int mport,int card,int port);
  void channelStopped(int id,int mport,int card,int port);

 protected:
  QString cellText(int col,int line,RDLogLine *ll) const;
  QFont cellTextFont(int col,int row,RDLogLine *ll) const;
  QColor cellTextColor(int col,int row,RDLogLine *ll) const;
  QColor rowBackgroundColor(int row,RDLogLine *ll) const;

 private:
  bool StartEvent(int line,RDLogLine::TransType trans_type,int trans_length,
		  RDLogLine::StartSource src,int mport=-1,int duck_length=0);
  bool StartAudioEvent(int line);
  void CleanupEvent(int id);
  void UpdateStartTimes();
  void FinishEvent(int line);
  QTime GetStartTime(QTime sched_time,RDLogLine::TransType trans_type,
		     RDLogLine::TimeType time_type,QTime prev_time,
		     int prev_total_length,int prev_segue_length,bool *stop,
		     int running_events);
  QTime GetNextStop(int line);
  void UpdatePostPoint();
  void UpdatePostPoint(int line);
  void AdvanceActiveEvent();
  void SetTransTimer(QTime current_time=QTime(),bool stop=true);
  QString GetPortName(int card,int port);
  int GetNextChannel(int mport,int *card,int *port);
  int GetLineById(int id);
  RDPlayDeck *GetPlayDeck();
  void FreePlayDeck(RDPlayDeck *);
  bool GetNextPlayable(int *line,bool skip_meta,bool forced_start=false);
  void LogPlayEvent(RDLogLine *logline);
  void RefreshEvents(int line,int line_quan,bool force_update=false);
  void ChangeTransport();
  void Playing(int id);
  void Paused(int id);
  void Stopping(int id);
  void Stopped(int id);
  void Finished(int id);
  void ClearChannel(int deckid);
  RDLogLine::TransType GetTransType(const QString &logname,int line);
  bool ClearBlock(int start_line);
  void SendNowNext();
  void UpdateRestartData();
  QJsonValue GetPadJson(const QString &name,RDLogLine *ll,
			const QDateTime &start_datetime,int line) const;
  void LogTraffic(RDLogLine *logline,RDLogLine::PlaySource src,
		  RDAirPlayConf::TrafficAction action,bool onair_flag) const;
  void DumpToSyslog(int prio_lvl,const QString &hdr) const;
  RDCae *play_cae;
  RDAirPlayConf::OpMode play_op_mode;
  int play_slot_id[LOGPLAY_MAX_PLAYS];
  int play_trans_length;
  int play_next_line;
  int play_line_counter;
  bool play_start_next;
  int play_id;
  QTime play_next_stop;
  bool play_running;
  QTime play_post_time;
  int play_post_offset;
  int play_active_line;
  RDLogLine::TransType play_active_trans;
  RDMacroEvent *play_macro_deck;
  bool play_macro_running;
  bool play_refresh_pending;
  QTimer *play_trans_timer;
  QTimer *play_grace_timer;
  int play_trans_line;
  int play_grace_line;
  int play_card[2];
  int play_port[2];
  QString play_label[2];
  QString play_start_rml[2];
  QString play_stop_rml[2];
  bool play_timescaling_available;
  RDPlayDeck *play_deck[RD_MAX_STREAMS];
  bool play_deck_active[RD_MAX_STREAMS];
  int next_channel;
  QString play_nownext_rml;
  bool play_timescaling_supported[RD_MAX_CARDS];
  QString play_svc_name;
  QString play_defaultsvc_name;
  int play_rescan_pos;
  RDLog *play_log;
  QDateTime play_link_datetime;
  QDateTime play_modified_datetime;
  bool play_refreshable;
  bool play_onair_flag;
  int play_duck_volume_port1;
  int play_duck_volume_port2;
  unsigned play_now_cartnum;
  unsigned play_next_cartnum;
  unsigned play_prevnow_cartnum;
  unsigned play_prevnext_cartnum;
  RDSimplePlayer *play_audition_player;
  int play_audition_line;
  bool play_audition_head_played;
  int play_audition_preroll;
  RDEventPlayer *play_event_player;
  RDUnixSocket *play_pad_socket[2];
  bool play_hours[24];
  int play_slot_quantity;
};


#endif  //  RDLOGPLAY_H
