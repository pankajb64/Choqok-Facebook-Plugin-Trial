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
#include <KAction>
#include <KMenu>
#include <klocalizedstring.h>
#include <KUrl>
#include <mediamanager.h>
#include <textbrowser.h>
#include "facebookutil.h"
#include "facebookviewdialog.h"
#include "facebookwhoiswidget.h"
#include "facebookaccount.h"
#include <KPushButton>
#include <QtGui/QtGui>
#include <QtGui/QLayout>

FacebookPostWidget::FacebookPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent): PostWidget(account, post, parent)
{
   
}

QString FacebookPostWidget::generateSign ()
{
	FacebookPost* post = static_cast<FacebookPost*>(currentPost());
	
    QString ss = "";
 
    
    ss = "<b><a href='"+ currentAccount()->microblog()->profileUrl( currentAccount(), post->author.userId ) 
		 +      +"' title=\"" +
    post->author.realName + "\">" ;
		
		 if (post->author.realName.isEmpty())
			ss += "Anonymous";
		else
			ss += post->author.realName;
		ss += "</a> - </b> via";

    //QStringList list = currentPost()->postId.split("_");
    
    /*ss += "<a href=\"http://www.facebook.com/" + list[0] + "/posts/" + list[1]
	 
	 + "\" title=\""
	 + currentPost().creationDateTime.toString(Qt::DefaultLocaleLongDate) + "\">%1</a>";*/
    
    if( !post->appId.isEmpty())
        ss += " <b> <a href=\"http://www.facebook.com/apps/application.php?id=" + post->appId.toString() + "\">" + post->appName + "</a></b> ";
    else
	    ss += " <b>web</b> ";	

    ss += ", <a href='"
	 + currentAccount()->microblog()->postUrl(currentAccount(), post->author.userName, post->postId)
 	 + "' title='"
	 + post->creationDateTime.toString(Qt::DefaultLocaleLongDate) + "'>%1</a>";	
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
    
    //status += post->likeCount + " , " + post->commentCount + " <br/> ";
    
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
	{
      downloadImage(post->iconUrl);
      QString imgUrl = getImageUrl(post->iconUrl);
      status += QString("<br/><a href = \"%1\"> <img align='left' src = \"%2\"/> </a><br/>").arg(imgUrl).arg(imgUrl);
    }

	  
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
            //title = type;
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

void FacebookPostWidget::slotImageFetched(const QString& linkUrl, const QPixmap& pixmap)
{
	
	QString imgUrl = getImageUrl(linkUrl);
	
	QPixmap pix = pixmap;
	
	if ( pixmap.width() > 200 )
	    pix = pixmap.scaledToWidth(200);
    else if ( pixmap.height() > 200 )
        pix = pixmap.scaledToHeight(200);
   
   Choqok::UI::PostWidget::mainWidget()->document()->addResource(QTextDocument::ImageResource, imgUrl, pix);
   
   /*QString content = currentPost()->content;
   
   content += QString("<a href = \"%1\"> <img src = \"%2\"/> </a>").arg(linkUrl).arg(imgUrl);
   
   currentPost()->content = content;
   
   Choqok::UI::PostWidget::updateUi();       */ 
}


void FacebookPostWidget::checkAnchor(const QUrl &link)
{
	QString scheme = link.scheme();
	
	if (scheme == "img")
		{
			QString postId = currentPost()->postId;
			QStringList list = postId.split("_");
			postId = list[1];
			QString userId = list[0];
			QString urlString = QString("https://www.facebook.com/%1/posts/%2").arg(userId).arg(postId);
			QUrl url(urlString);
			FacebookViewDialog* fdialog = new FacebookViewDialog(url, this);
			fdialog->start();
		}
	
	else if(scheme == "user")
	 {
        KMenu menu;
        KAction * info = new KAction( KIcon("user-identity"), i18nc("Who is user", "Who is %1", currentPost()->author.realName), &menu );
        KAction * openInBrowser = new KAction(KIcon("applications-internet"), i18nc("Open profile page in browser", "Open profile in browser"), &menu);

        menu.addAction(info);
        menu.addAction(openInBrowser);
        
        QAction * ret = menu.exec(QCursor::pos());
        if(ret == 0)
            return;
        if(ret == info) {
			QStringList list = currentPost()->postId.split("_");
			
			FacebookAccount* acc = qobject_cast<FacebookAccount *> (currentAccount());

            FacebookWhoisWidget *wd = new FacebookWhoisWidget(acc, link.host(),  currentPost(), /*ShowType::UserInfo ,*/ this);
            wd->show(QCursor::pos());
            return;
        } else if(ret == openInBrowser){
			FacebookMicroBlog* blog = qobject_cast < FacebookMicroBlog * > ( currentAccount()->microblog());
            Choqok::openUrl( QUrl( blog->facebookUrl(currentAccount(), link.host()) ) );
            return;
        }
	}else
        Choqok::UI::PostWidget::checkAnchor(link);
}

void FacebookPostWidget::initUi()
{
    Choqok::UI::PostWidget::initUi();
    
    FacebookPost* post = static_cast<FacebookPost*>(currentPost());

    KPushButton *btnComment = addButton( "btnComment",i18nc( "@info:tooltip", "Comment" ), "edit-undo" );
    KPushButton *btnViewComments = addButton( "btnViewComments",i18nc( "@info:tooltip", "View Commentss on this post" ), "" );
    KPushButton *btnLike = addButton( "btnLike",i18nc( "@info:tooltip", "Like" ), "rating" );
    KPushButton *btnViewLikes = addButton( "btnViewLikes",i18nc( "@info:tooltip", "View Likes on this post" ), "" );
    KPushButton *btn = buttons().value("btnResend");
    btnViewLikes->setText (post->likeCount);
    btnViewComments->setText (post->commentCount);
    
    QHBoxLayout layout;
    layout.setContentsMargins(0, 0, 0, 0);
    layout.setSizeConstraint(QLayout::SetFixedSize);
    
    layout.addWidget(btnComment);
    layout.addWidget(btnViewComments);
    layout.addWidget(btnLike);
    layout.addWidget(btnViewLikes);
    
    
    if ( btn)
      btn->setToolTip("Share");
    /*KMenu menu ;
    menu.addAction(btnComment);
    menu.addAction(btnLike);

    KAction *actRep = new KAction(KIcon("edit-undo"), i18n("Reply to %1", currentPost()->author.userName), menu);
    menu->addAction(actRep);
    connect( actRep, SIGNAL(triggered(bool)), SLOT(slotReply()) );
    connect( btnRe, SIGNAL(clicked(bool)), SLOT(slotReply()) );

    KAction *actWrite = new KAction( KIcon("document-edit"), i18n("Write to %1", currentPost()->author.userName),
                                     menu );
    menu->addAction(actWrite);
    connect( actWrite, SIGNAL(triggered(bool)), SLOT(slotWriteTo()) );

    if( !currentPost()->isPrivate ) {
        KAction *actReplytoAll = new KAction(i18n("Reply to all"), menu);
        menu->addAction(actReplytoAll);
        connect( actReplytoAll, SIGNAL(triggered(bool)), SLOT(slotReplyToAll()) );
    }

    menu->setDefaultAction(actRep);
    btnRe->setDelayedMenu(menu);

    if( !currentPost()->isPrivate ) {
        d->btnFav = addButton( "btnFavorite",i18nc( "@info:tooltip", "Favorite" ), "rating" );
        d->btnFav->setCheckable(true);
        connect( d->btnFav, SIGNAL(clicked(bool)), SLOT(setFavorite()) );
        updateFavStat();
    }*/
}
