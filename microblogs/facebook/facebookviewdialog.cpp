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

#include "facebookviewdialog.h"
#include <KLocale>
#include <KMessageBox>
#include <KDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QWebFrame>
#include <QWebElement>

FacebookViewDialog::FacebookViewDialog ( const QUrl& link, QWidget* parent) : KDialog(parent)
{
	mLink = link;
}


void FacebookViewDialog::initUi()
{
  setButtons( KDialog::Cancel );
  setCaption( i18n( "Facebook Post" ) );
  setAttribute( Qt::WA_DeleteOnClose, true );
  
  QWidget * const widget = new QWidget( this );
  QVBoxLayout * const layout = new QVBoxLayout( widget );
  QWidget * const progressWidget = new QWidget( this );
  QHBoxLayout * const progressLayout = new QHBoxLayout( progressWidget );
  progressLayout->setMargin( 0 );
  layout->setMargin( 0 );
  setMainWidget( widget );
  mWebView = new KWebView( this );
  
  mProgressBar = new QProgressBar( this );
  mProgressBar->setRange( 0, 100 );
  QLabel * const progressLabel = new QLabel( i18n( "Loading Page:" ), this );
  progressLayout->addWidget( progressLabel );
  progressLayout->addWidget( mProgressBar ); 
  
  layout->addWidget( progressWidget );
  layout->addWidget( mWebView );
  
  connect( this, SIGNAL(cancelClicked()), SIGNAL(canceled()) );
  connect( mWebView, SIGNAL(loadStarted()), progressWidget, SLOT(show()) );
  connect( mWebView, SIGNAL(loadFinished(bool)), progressWidget, SLOT(hide()) );
  connect( mWebView, SIGNAL(loadProgress(int)), mProgressBar, SLOT(setValue(int)) );
}

void FacebookViewDialog::setLink( const QUrl& link)
{
	mLink = link;
}

void FacebookViewDialog::start()
{
	Q_ASSERT( !mLink.isEmpty() );
	
	//kDebug() << "Showing" << mLink.toString();
    mWebView->setUrl( mLink  );
    show();
}



