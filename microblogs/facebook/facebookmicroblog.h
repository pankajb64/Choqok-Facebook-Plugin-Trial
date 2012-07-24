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

#ifndef FACEBOOKMICROBLOG_H
#define FACEBOOKMICROBLOG_H
#include "microblog.h" 
#include <kfacebook/facebookjobs.h>
#include <kfacebook/postinfo.h>
#include <kfacebook/userinfo.h>
#include "facebookpost.h"


using namespace KFacebook;

class FacebookAccount;
//class KJob;
class FacebookMicroBlog : public Choqok::MicroBlog
{
  Q_OBJECT
  
  public :
    FacebookMicroBlog(QObject * parent,/*const char *name,*/ const QVariantList& args);
    virtual ~FacebookMicroBlog();
    
    
    virtual ChoqokEditAccountWidget* createEditAccountWidget(Choqok::Account* account, QWidget* parent);
    virtual Choqok::UI::PostWidget* createPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent);
    virtual Choqok::UI::ComposerWidget * createComposerWidget( Choqok::Account *account, QWidget *parent );
    virtual void createPost(Choqok::Account* theAccount, Choqok::Post* post);
    virtual void abortCreatePost(Choqok::Account* theAccount, Choqok::Post* post = 0);
    virtual void fetchPost(Choqok::Account* theAccount, Choqok::Post* post);
    virtual void removePost(Choqok::Account* theAccount, Choqok::Post* post);
    virtual void saveTimeline(Choqok::Account* account, const QString& timelineName, const QList< Choqok::UI::PostWidget* >& timeline);
    virtual QList< Choqok::Post* > loadTimeline(Choqok::Account* account, const QString& timelineName);
    virtual Choqok::Account* createNewAccount(const QString& alias);
    virtual void updateTimelines(Choqok::Account* theAccount);
    virtual Choqok::TimelineInfo* timelineInfo(const QString& timelineName);
    virtual void aboutToUnload();
    virtual QList <Choqok::Post * > toChoqokPost(PostInfoList mPosts) const;
    virtual Choqok::User toChoqokUser(UserInfoPtr userInfo) const;
    virtual QString profileUrl(Choqok::Account* account, const QString& username) const;
    virtual QString postUrl(Choqok::Account* account, const QString& username, const QString& postId) const;
    virtual QString facebookUrl(Choqok::Account* account, const QString& username) const;
    virtual uint postCharLimit() const;
    void createPostWithAttachment(Choqok::Account *theAccount, Choqok::Post *post, const QString &mediumToAttach = QString());
    //virtual QString prepareStatus(const FacebookPost * post) const;

  /*Q_SIGNALS:
    virtual void timelineDataReceived(Choqok::Account *theAccount, const QString &timelineName, QList<Choqok::Post*> data );*/
		
protected  slots :
  void slotCreatePost(KJob* job);
  void slotTimeLineLoaded(KJob *job);
  private:
     QMap<KJob*, FacebookAccount*> mJobsAccount;
     QMap<KJob*, Choqok::Post*> mJobsPost;
    
};   
#endif
