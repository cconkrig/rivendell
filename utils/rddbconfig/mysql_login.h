// mysql_login.h
//
// mySQL Administrative Login Widget for rddbconfig(8).
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef MYSQL_LOGIN_H
#define MYSQL_LOGIN_H

#include <QDialog>
#include <QLineEdit>

#include <rddialog.h>

class MySqlLogin : public RDDialog
{
  Q_OBJECT
  public:
   MySqlLogin(QString *username,QString *password,RDConfig *c,
	      QWidget *parent=0);
   ~MySqlLogin();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void okData();
   void cancelData();

  private:
   QString *login_name;
   QLineEdit *login_name_edit;
   QString *login_password;
   QLineEdit *login_password_edit;
};


#endif  // MYSQL_LOGIN_H
