// export_radiotraffic.cpp
//
// Export a Rivendell Report to RadioTraffic.com
//
//   (C) Copyright 2002-2005,2009,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rdescape_string.h>
#include <rdreport.h>

bool RDReport::ExportRadioTraffic(const QString &filename,
				  const QDate &startdate,const QDate &enddate,
				  const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString air_fmt;

  QFile file(filename);
  if((f=fopen((const char *)filename,"wb"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }

  if(useLeadingZeros()) {
    air_fmt=QString().sprintf("%%0%uu ",cartDigits());
  }
  else {
    air_fmt=QString().sprintf("%%%-uu ",cartDigits());
  }
  sql=QString("select ")+
    "ELR_LINES.LENGTH,"+
    "ELR_LINES.CART_NUMBER,"+
    "ELR_LINES.EVENT_DATETIME,"+
    "ELR_LINES.EVENT_TYPE,"+
    "ELR_LINES.EXT_START_TIME,"+
    "ELR_LINES.EXT_LENGTH,"+
    "ELR_LINES.EXT_DATA,"+
    "ELR_LINES.EXT_EVENT_ID,"+
    "ELR_LINES.EXT_ANNC_TYPE,"+
    "ELR_LINES.TITLE,"+
    "ELR_LINES.EXT_CART_NAME "+
    "from ELR_LINES left join CART "+
    "on ELR_LINES.CART_NUMBER=CART.NUMBER where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write Data Rows
  //
  while(q->next()) {
    fprintf(f,"%s ",(const char *)q->value(4).toTime().toString("hh:mm:ss"));
    fprintf(f,"%s ",(const char *)q->value(2).toDateTime().
	    toString("hh:mm:ss"));
    if(q->value(5).toInt()>0) {
      fprintf(f,"0%s ",(const char *)RDGetTimeLength(q->value(5).toInt(),
						     true,false));
    }
    else {
      fprintf(f,"00:00:00 ");
    }
    if(q->value(0).toInt()>0) {
      fprintf(f,"0%s ",(const char *)RDGetTimeLength(q->value(0).toInt(),
						     true,false));
    }
    else {
      fprintf(f,"00:00:00 ");
    }
    fprintf(f,air_fmt,q->value(1).toUInt());
    fprintf(f,"%-34s ",(const char *)q->value(9).toString().left(34));
    if(q->value(6).toString().isEmpty()) {
      fprintf(f,"                                ");
    }
    else {
      fprintf(f,"%-32s",(const char *)q->value(6).toString().left(32).
	      stripWhiteSpace());
    }
    fprintf(f,"\x0d\x0a");
  }

  delete q;
  fclose(f);
  report_error_code=RDReport::ErrorOk;

  return true;
}

