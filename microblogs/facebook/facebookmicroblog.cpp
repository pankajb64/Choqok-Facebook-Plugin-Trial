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

#include "facebookmicroblog.h"
#include <KAboutData>
#include <KGenericFactory>
#include "facebookaccount.h"
#include <KMessageBox>
#include <kfacebook/postinfo.h>
#include <kfacebook/postjob.h>
#include <kfacebook/postaddjob.h>
#include <kfacebook/postslistjob.h>
#include "accountmanager.h"
#include "editaccountwidget.h"
#include "facebookeditaccountwidget.h"
#include "postwidget.h"
#include "facebookpostwidget.h"
#include <application.h>


K_PLUGIN_FACTORY( MyPluginFactory, registerPlugin < FacebookMicroBlog > (); )
K_EXPORT_PLUGIN( MyPluginFactory( "choqok_facebook" ) )

FacebookMicroBlog::FacebookMicroBlog( QObject *parent, const QList<QVariant> & args ): Choqok::MicroBlog( MyPluginFactory::componentData(), parent)
{
//...
}
 
FacebookMicroBlog::~FacebookMicroBlog()
{
}


Choqok::UI::PostWidget* FacebookMicroBlog::createPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent)
{
    return new Choqok::UI::PostWidget(account, post, parent);
}

void FacebookMicroBlog::createPost(Choqok::Account* theAccount, Choqok::Post* post) 
{
    kDebug()<<"Creating a new Post for " <<theAccount;
    FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount);
    PostAddJob* job = new PostAddJob(post->content, acc->accessToken());
    mJobsAccount.insert(job, acc);
    mJobsPost.insert(job, post);
    connect( job, SIGNAL(result(KJob*)), this, SLOT(slotCreatePost(KJob*)) );
    job->start();
 
}

void FacebookMicroBlog::slotCreatePost(KJob* job)
{
    FacebookAccount* acc = mJobsAccount.take((FacebookJob * )job);
    Choqok::Post* post = mJobsPost.take((FacebookJob * )job);
    emit postCreated ( acc, post );
}
void FacebookMicroBlog::abortCreatePost(Choqok::Account* theAccount, Choqok::Post* post)
{
    FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount); 
    Q_UNUSED(post);
    
    PostAddJob* job = (PostAddJob *)mJobsAccount.key(acc);
    
    if(job)
      job->abort();
}

void FacebookMicroBlog::fetchPost(Choqok::Account* theAccount, Choqok::Post* post)
{
    Q_UNUSED(theAccount);
    Q_UNUSED(post);
    KMessageBox::sorry(choqokMainWindow, i18n("Not Supported"));
}

void FacebookMicroBlog::removePost(Choqok::Account* theAccount, Choqok::Post* post)
{
    Q_UNUSED(theAccount);
    Q_UNUSED(post);
    KMessageBox::sorry(choqokMainWindow, i18n("Not Supported"));
}

Choqok::Account* FacebookMicroBlog::createNewAccount(const QString& alias)
{

kDebug() << "Creating a new Facebook Account";
FacebookAccount *acc = qobject_cast<FacebookAccount*>( Choqok::AccountManager::self()->findAccount(alias) );
    if(!acc) {
        return new FacebookAccount(this, alias);
    } else {
        return 0;//If there's an account with this alias, So We can't create a new one
    }
}

ChoqokEditAccountWidget* FacebookMicroBlog::createEditAccountWidget(Choqok::Account* account, QWidget* parent)
{
    kDebug();
    FacebookAccount *acc = qobject_cast<FacebookAccount*>(account);
    if(acc || !account)
        return new FacebookEditAccountWidget(this, acc, parent);
    else{
        kDebug()<<"Account passed here was not a valid Facebook Account!";
        return 0L;
    }
}    

Choqok::TimelineInfo* FacebookMicroBlog::timelineInfo(const QString& timelineName)
{
  if(timelineName == "Home") {
   
    Choqok::TimelineInfo * info = new Choqok::TimelineInfo;
    info->name = i18nc("Timeline Name", "Home");
    info->description = i18nc("Timeline description", "Your Facebook Home");
    info->icon = "user-home";
    return info;
  } else {
    
    kError()<<"timelineName is not valid!";
    return 0;
  }
}

QList< Choqok::Post* > FacebookMicroBlog::loadTimeline(Choqok::Account* account, const QString& timelineName)
{
    kDebug()<<timelineName;
    QList< Choqok::Post* > list;
    QString fileName = Choqok::AccountManager::generatePostBackupFileName(account->alias(), timelineName);
    KConfig postsBackup( "choqok/" + fileName, KConfig::NoGlobals, "data" );
    QStringList tmpList = postsBackup.groupList();

    QList<QDateTime> groupList;
    foreach(const QString &str, tmpList)
        groupList.append(QDateTime::fromString(str) );
    qSort(groupList);
    int count = groupList.count();
    if( count ) {
        Choqok::Post *st = 0;
        for ( int i = 0; i < count; ++i ) {
            st = new Choqok::Post;
            KConfigGroup grp( &postsBackup, groupList[i].toString() );
            st->creationDateTime = grp.readEntry( "creationDateTime", QDateTime::currentDateTime() );
	    st->postId = grp.readEntry( "postId", QString() );
            st->replyToPostId = grp.readEntry( "inReplyToPostId", QString() );
            st->title = grp.readEntry( "title", QString() );
            st->source = grp.readEntry( "source", QString() );
            st->link = grp.readEntry( "link", QString() );
            st->content = grp.readEntry( "text", QString() );
	    st->type = grp.readEntry( "type", QString() );
	    st->replyToUserId = grp.readEntry( "replyToUserId", QString() );

            st->author.userId = grp.readEntry( "authorId", QString() );
            st->author.realName = grp.readEntry( "authorRealName", QString() );
            st->author.profileImageUrl = grp.readEntry( "profileImageUrl", QString() );
            //st->isRead = grp.readEntry("isRead", true);

            list.append( st );
        }
    }
    return list;
}


void FacebookMicroBlog::saveTimeline(Choqok::Account* account, const QString& timelineName, const QList< Choqok::UI::PostWidget* >& timeline)
{
    kDebug();
    QString fileName = Choqok::AccountManager::generatePostBackupFileName(account->alias(), timelineName);
    KConfig postsBackup( "choqok/" + fileName, KConfig::NoGlobals, "data" );

    ///Clear previous data:
    QStringList prevList = postsBackup.groupList();
    int c = prevList.count();
    if ( c > 0 ) {
        for ( int i = 0; i < c; ++i ) {
            postsBackup.deleteGroup( prevList[i] );
        }
    }
    QList< Choqok::UI::PostWidget *>::const_iterator it, endIt = timeline.constEnd();
    for ( it = timeline.constBegin(); it != endIt; ++it ) {
        const Choqok::Post *post = ((*it)->currentPost());
        KConfigGroup grp( &postsBackup, post->creationDateTime.toString() );
        grp.writeEntry( "creationDateTime", post->creationDateTime );
        grp.writeEntry( "postId", post->postId.toString() );
        grp.writeEntry( "title", post->title );
        grp.writeEntry( "source", post->source );
        grp.writeEntry( "link", post->link );
        grp.writeEntry( "text", post->content );
	grp.writeEntry( "type", post->type );
	grp.writeEntry( "replyToUserId", post->replyToUserId.toString() );
        grp.writeEntry( "inReplyToPostId", post->replyToPostId.toString() );
        grp.writeEntry( "authorId", post->author.userId.toString() );
        grp.writeEntry( "authorRealName", post->author.realName );
	grp.writeEntry( "profileImageUrl", post->author.profileImageUrl );
        //grp.writeEntry( "isRead" , post->isRead );
    }
    postsBackup.sync();
	if(Choqok::Application::isShuttingDown())
		emit readyForUnload();
}

void FacebookMicroBlog::updateTimelines(Choqok::Account * theAccount)
{
  kDebug();
    FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount);
    if(!acc){
        kError()<<"FacebookMicroBlog::updateTimelines: acc is not an FacebookAccount";
        return;
    }
    
    PostsListJob * job = new PostsListJob(acc->accessToken());
    mJobsAccount.insert(job, acc);
    connect( job, SIGNAL(result(KJob*)), this, SLOT(slotTimeLineLoaded(KJob*)) );
    job->start();
    
}

void FacebookMicroBlog::slotTimeLineLoaded(KJob *job)
{
  kDebug();
    FacebookAccount* acc = mJobsAccount.take((FacebookJob *)job);
    if ( job->error() ) {
        kDebug() << "Job Error: " << job->errorString();
        emit error( acc, CommunicationError, i18n("Timeline update failed, %1", job->errorString()), Low );
     
    }else {
	const QString h = "Home";
        emit timelineDataReceived( acc, h, toChoqokPost(((PostsListJob *)job)->posts() ));
    }
}
void FacebookMicroBlog::aboutToUnload()
{
    emit saveTimelines();
}

QString FacebookMicroBlog::profileUrl (Choqok::Account* acc, const QString& userId) const
{
	return "http://www.facebook.com/profile.php?id=" + userId;
}

QString FacebookMicroBlog::postUrl(Choqok::Account*, const QString& username, const QString& postId) const
{
    QStringList list = postId.split("_");	
    return QString ( "http://www.facebook.com/%1/posts/%2" ).arg ( list.at(0) ).arg ( list.at(1) );
}

QList<Choqok::Post *> FacebookMicroBlog::toChoqokPost(PostInfoList mPosts) const
{
/*
  QList<Choqok::Post *> list;
  PostInfoPtr p;
  foreach(p, mPosts)
  {
     PostInfo * postInfo = p.data();
     Choqok::Post*  post = new Choqok::Post();
     post->postId = postInfo->id();
     post->replyToPostId = postInfo->id();
     kDebug() << "Post Id - " << postInfo->id();	
     kDebug() << "Post Read Status  - " << post->isRead;
     post->source = postInfo->appName();
     post->creationDateTime = postInfo->createdTime().dateTime();
    
     post->replyToUserId = postInfo->appId();
     kDebug() << "Post Type - " << postInfo->type();
     post->type = postInfo->type();
     if( postInfo->type() != "status")
     {
	post->link = postInfo->link();
	kDebug() << "Post Link - " << postInfo->link();
	kDebug() << "Post Title - " << postInfo->name();
	kDebug() << "Post Description - " << postInfo->description();
    post->title = postInfo->name();	
	post->content = postInfo->message() + " | " /*+ postInfo->description()*;	
     }
     else
    post->content = postInfo->message();
    kDebug() << "Post Message - " << postInfo->message();
    Choqok::User * user = new Choqok::User();
    user->userId = postInfo->senderId();
    user->realName = postInfo->senderName();
    user->profileImageUrl = "https://graph.facebook.com/" + postInfo->senderId() + "/picture";
    post->author = *user;	

    	
    list.append(post);
  }
  */
  
  QList<Choqok::Post*> list;
  PostInfoPtr p;
  
  foreach ( p, mPosts)
  {
	  PostInfo * postInfo = p.data();
	  FacebookPost * post = new FacebookPost ();
	  post->postId = postInfo->id();
	  post->author = toChoqokUser(postInfo->from());
	  post->author.profileImageUrl = postInfo->pictureUrl();
	  post->content = postInfo->message();
	  post->link = postInfo->link();
	  post->title = postInfo->name();
	  post->caption = postInfo->caption();
	  post->description = postInfo->description();
	  post->iconUrl = postInfo->icon();
	  post->properties = postInfo->properties();
	  post->type = postInfo->type();
	  post->source = postInfo->source();
	  post->likes = postInfo->likes();
	  post->story = postInfo->story();
	  post->comments = postInfo->comments();
	  post->application = postInfo->application();
	  post->creationDateTime = postInfo->createdTime().dateTime();
	  post->updateDateTime = postInfo->updatedTime().dateTime();
  
      post->content = prepareStatus(post);
      
	  list.append(post);
  }
  
  return list;
}

Choqok::User FacebookMicroBlog::toChoqokUser(UserInfoPtr userInfo) const
{
	Choqok::User * user = new Choqok::User();
	
	user->userId = userInfo->id();
	user->userName = userInfo->username().isNull() ? userInfo->id() : userInfo->username() ;
	user->realName = userInfo->name();
	
	return *user;
}

QString FacebookMicroBlog::prepareStatus(const FacebookPost * post) const
{
	QString content = post->content; 
	QString title = post->title; 
	QString caption = post->caption; 
	QString description = post->description; 
	QString story = post->story;
	
	//content = content.isNull() ? "" : Choqok::UI::PostWidget::prepareStatus(content) ;
	//title = title.isNull() ? "" : Choqok::UI::PostWidget::prepareStatus(title) ;
	//caption = caption.isNull() ? "" : Choqok::UI::PostWidget::prepareStatus(caption) ;
	//description = description.isNull() ? "" : Choqok::UI::PostWidget::prepareStatus(description) ;
	//story = story.isNull() ? "" : Choqok::UI::PostWidget::prepareStatus(story) ;

	QString status = post->content + " <br/> <a href = \"" + post->link + " \"> <h2>" + post->title + "</h2> <br/> <h3>" + post->caption + "</h3> </a><br/> " + post->description + " <br/> ( Post Type - "  + post->type + " ) <br/> "  + post->story;
	
	return status;
}
#include "facebookmicroblog.moc"
