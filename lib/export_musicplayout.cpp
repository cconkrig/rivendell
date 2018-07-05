// export_musicplayout.cpp
//
// Export a Rivendell Report to an ASCII Text File.
//
//   (C) Copyright 2012,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <qfile.h>
#include <qmessagebox.h>

#include <rdairplay_conf.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlog_line.h>
#include <rdreport.h>

bool RDReport::ExportMusicPlayout(const QString &filename,
				  const QDate &startdate,const QDate &enddate,
				  const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString cut;
  QString str;
  QString cart_fmt;
  QString cart_num;

  QFile file(filename);
  if((f=fopen((const char *)filename,"w"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }
  if(useLeadingZeros()) {
    cart_fmt=QString().sprintf("%%0%uu",cartDigits());
  }
  else {
    cart_fmt="%6u";
  }
  sql=QString("select ")+
    "ELR_LINES.LENGTH,"+
    "ELR_LINES.CART_NUMBER,"+
    "ELR_LINES.EVENT_DATETIME,"+
    "ELR_LINES.EXT_EVENT_ID,"+
    "ELR_LINES.TITLE,"+
    "ELR_LINES.CUT_NUMBER,"+
    "ELR_LINES.ARTIST,"+
    "ELR_LINES.ALBUM,"+
    "ELR_LINES.LABEL "+
    "from ELR_LINES left join CART "+
    "on ELR_LINES.CART_NUMBER=CART.NUMBER where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    fprintf(f,"                                                             Rivendell RDAirPlay Music Playout Report for %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"));
  }
  else {
    fprintf(f,"                                              Rivendell RDAirPlay Music Playout Report for %s - %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"),
	    (const char *)enddate.toString("MM/dd/yyyy"));
  }
  str=QString().sprintf("%s -- %s\n",(const char *)name(),
			(const char *)description());
  for(unsigned i=0;i<(180-str.length())/2;i++) {
    fprintf(f," ");
  }
  fprintf(f,"%s\n",(const char *)str);
  fprintf(f,"--Time--  -Cart-  Cut  A-Len  --Title-----------------------   --Artist----------------------   --Album------------------   --Label-------------\n");

  //
  // Write Data Rows
  //
  while(q->next()) {
    if(q->value(5).toInt()>0) {
      cut=QString().sprintf("%03d",q->value(5).toInt());
    }
    else {
      if((RDAirPlayConf::TrafficAction)q->value(6).toInt()==
	 RDAirPlayConf::TrafficMacro) {
	cut="rml";
      }
      else {
	cut="   ";
      }
    }
    cart_num=QString().sprintf(cart_fmt,q->value(1).toUInt());
    fprintf(f,"%8s  %6s  %3s  %5s  %-30s   %-30s   %-25s   %-20s\n",
	    (const char *)q->value(2).toDateTime().time().toString("hh:mm:ss"),
	    (const char *)cart_num,
	    (const char *)cut,
	    (const char *)RDGetTimeLength(q->value(0).toInt(),true,false).
	    right(5),
	    (const char *)StringField(q->value(4).toString().left(30)),
	    (const char *)StringField(q->value(6).toString().left(30)),
	    (const char *)StringField(q->value(7).toString().left(25)),
	    (const char *)StringField(q->value(8).toString().left(20)));
  }
  delete q;
  fclose(f);
  report_error_code=RDReport::ErrorOk;

  return true;
}
