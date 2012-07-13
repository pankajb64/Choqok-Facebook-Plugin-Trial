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

#ifndef FACEBOOKPOSTWIDGET_H
#define FACEBOOKPOSTWIDGET_H
#include <postwidget.h>
#include "facebookpost.h"

using namespace KFacebook;

/*namespace Choqok
{
  namespace UI
  {
	class PostWidget;
  } 	
}*/

class FacebookPostWidget : public Choqok::UI::PostWidget {

    Q_OBJECT
    
    public:
    FacebookPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent = 0);
    protected:	
    virtual QString generateSign ();
    virtual QString prepareStatus( const QString &txt );
    
    protected slots:
    void slotImageFetched(QString& remoteUrl, QPixmap& pixmap) const;
    
    protected:
    void downloadImage(QString& linkUrl) const;  
    
    private:
    
    QString prepareLink (QString& link, QString& title, QString& caption, QString& description, QString& type ) const;
};
#endif
