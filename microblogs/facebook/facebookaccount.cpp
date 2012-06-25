/*
    This file is part of Choqok, the KDE micro-blogging client

    Copyright (C) 2011-2012 Pankaj Bhambhani <pankajb64@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see http://www.gnu.org/licenses/
*/

#include "facebookaccount.h"
#include "facebookmicroblog.h"
#include <KDebug>

class FacebookAccount::Private
{
public:
    QString accessToken;
};

FacebookAccount::FacebookAccount(FacebookMicroBlog* parent, const QString& alias) : Account(parent, alias), d(new Private)
{
    kDebug()<<alias;
   d->accessToken = configGroup()->readEntry("AccessToken", QString());	
}    

FacebookAccount::~FacebookAccount()
{
  delete d;
}

void FacebookAccount::writeConfig()
{
    configGroup()->writeEntry ("AccessToken", d->accessToken);
    Choqok::Account::writeConfig();
    //configGroup()->sync();
    //emit modified(this);
}

void FacebookAccount::setAccessToken (const QString& accessToken)
{
  d->accessToken = accessToken;
}

QString FacebookAccount::accessToken() const
{
  return d->accessToken;
}

#include "facebookaccount.moc"
