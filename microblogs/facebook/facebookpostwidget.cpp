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

#include "facebookpostwidget.h"
#include <KDebug>
#include <KLocalizedString>
#include <KUrl>
#include <mediamanager.h>
#include <textbrowser.h>

FacebookPostWidget::FacebookPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent): PostWidget(account, post, parent)
{

}

QString FacebookPostWidget::generateSign ()
{
	FacebookPost* post = static_cast<FacebookPost*>(currentPost());
	
    QString ss = "";
 
    
    ss = "<i><a href='"+ currentAccount()->microblog()->profileUrl( currentAccount(), post->author.userId ) 
		 +      +"' title=\"" +
    post->author.realName + "\">" ;
		
		 if (post->author.realName.isEmpty())
			ss += "Anonymous";
		else
			ss += post->author.realName;
		ss += "</a> - </i> via";

    //QStringList list = currentPost()->postId.split("_");
    
    /*ss += "<a href=\"http://www.facebook.com/" + list[0] + "/posts/" + list[1]
	 
	 + "\" title=\""
	 + currentPost().creationDateTime.toString(Qt::DefaultLocaleLongDate) + "\">%1</a>";*/
    
    if( !post->appId.isEmpty())
        ss += " <a href=\"http://www.facebook.com/apps/application.php?id=" + post->appId.toString() + "\">" + post->appName + "</a>";
    else
	    ss += " web";	

    ss += " <a href='"
	 + currentAccount()->microblog()->postUrl(currentAccount(), post->author.userName, post->postId)
 	 + "'>"
	 + "<b>" + post->creationDateTime.toString(Qt::DefaultLocaleLongDate) + "</b></a>";	
    return ss;

}

QString FacebookPostWidget::prepareStatus( const QString &txt ) 
{
	FacebookPost* post = static_cast<FacebookPost*>(currentPost());

    kDebug()<<"content: " << post->content;
    kDebug()<<"title: " << post->title;
    kDebug()<<"caption: " << post->caption;
    kDebug()<<"description: " << post->description;
    kDebug()<<"story: " << post->story;
    kDebug()<<"link: " << post->link;
	
	QString content = Choqok::UI::PostWidget::prepareStatus(post->content); 
	QString title = /*Choqok::UI::PostWidget::prepareStatus*/(post->title);
	QString caption = /*Choqok::UI::PostWidget::prepareStatus*/(post->caption);
	QString description = Choqok::UI::PostWidget::prepareStatus(post->description);
	QString story = Choqok::UI::PostWidget::prepareStatus(post->story);
	QString link = /*Choqok::UI::PostWidget::prepareStatus*/(post->link);
    QString status;
    if( !story.isEmpty() )
        status += story + " <br/> ";
    if( !link.isEmpty() )
        status += prepareLink(link, title, caption, description, post->type) + "<br/>";
    if( !content.isEmpty() )
        status += content;

    /* You cannot show an image this way in a QTextBrowser
     * You need to download it first, via Choqok::MediaManager and then add it as a resource
     * just like what we did in Image preview plugin.
     * I put the false in if, to prevent it to show for now*/
	if (!post->iconUrl.isEmpty())
	  downloadImage(post->iconUrl);

	  
   //QString status = Choqok::UI::PostWidget::prepareStatus(txt);
   kDebug()<< status;
   return status;
}

QString FacebookPostWidget::prepareLink(QString& link, QString& title, QString& caption, QString& description, QString& type) const
{
    if( title.isEmpty() )
    {
        if( !caption.isEmpty() ){
            title = caption;
            caption.clear();
        } else {
            title = type;
        }
    }    
    QString link_title = link;
    if( !caption.isEmpty() )
        link_title = caption;
	QString linkHtml = QString("<a href =\"%1\" title='%3' ><b> %2 </b></a>").arg(link).arg(title).arg(link_title);
    if( !description.isEmpty() )
        linkHtml.append(QString("<br/>%1").arg(description));
	return linkHtml;
}

void FacebookPostWidget::downloadImage(QString& linkUrl) const
{
	connect ( Choqok::MediaManager::self(), SIGNAL(imageFetched(QString, QPixmap)), SLOT(slotImageFetched(QString, QPixmap)) );
	
	Choqok::MediaManager::self()->fetchImage(linkUrl, Choqok::MediaManager::Async) ;
}

void FacebookPostWidget::slotImageFetched(QString& linkUrl, QPixmap& pixmap) const
{
	KUrl imgU(linkUrl);
	imgU.setScheme("img");
	QString imgUrl = imgU.prettyUrl();
	
	QPixmap pix = pixmap;
	
	if ( pixmap.width() > 200 )
	    pix = pixmap.scaledToWidth(200);
    else if ( pixmap.height() > 200 )
        pix = pixmap.scaledToHeight(200);
   
   Choqok::UI::PostWidget::mainWidget()->document()->addResource(QTextDocument::ImageResource, imgUrl, pix);
   
   QString content = Choqok::UI::PostWidget::content();
   
   content += QString("<a href = \"%1\"> <img src = \"%2\"/> </a>").arg(linkUrl).arg(imgUrl);
   
   Choqok::UI::PostWidget::setContent(content);
           
}
