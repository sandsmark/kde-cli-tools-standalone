/* vi: ts=8 sts=4 sw=4
 *
 * $Id$
 *
 * This file is part of the KDE project, module kdesu.
 * Copyright (C) 1999,2000 Geert Jansen <g.t.jansen@stud.tue.nl>
 */

#include <time.h>
#include <assert.h>

#include <qcstring.h>
#include <qmap.h>
#include <qvaluestack.h>
#include <kdebug.h>

#include "repo.h"


Repository::Repository()
{
    head_time = (unsigned) -1;
}


Repository::~Repository()
{}


void Repository::add(const QCString &key, Data_entry &data)
{
    RepoIterator it = repo.find(key);
    if (it != repo.end())
        remove(key);
    if (data.timeout == 0)
        data.timeout = (unsigned) -1;
    else
        data.timeout += time(0L);
    head_time = QMIN(head_time, data.timeout);
    repo.insert(key, data);
}

int Repository::remove(const QCString &key)
{
    if( key.isEmpty() )
        return -1;

     RepoIterator it = repo.find(key);
     if (it == repo.end())
        return -1;
     it.data().value.fill('x');
     it.data().group.fill('x');
     repo.remove(it);
     return 0;
}

int Repository::removeSpecialKey(const QCString &key)
{
    bool found = false;
    if ( !key.isEmpty() )
    {
        RepoIterator it;
        QCString var, group;
        for (it=repo.begin(); it!=repo.end(); it++)
        {
            group = it.data().group;
            if (  key.find( group ) == 0 &&
                  key.length() > group.length() &&
                  it.key().find( key ) >= 0 )
            {
	        kdDebug(1205) << "Removed key: " << it.key() << endl;
                repo.remove( it );
                if(!found) found = true;
            }
        }
    }
    return (found ? 0:-1);
}

int Repository::removeGroup(const QCString &group)
{
    bool found = false;
    if ( !group.isEmpty() )
    {
        RepoIterator it;
        for (it=repo.begin(); it!=repo.end(); it++)
        {
            if (it.data().group == group)
            {
                kdDebug(1205) << "Removed key: " << it.key() << endl;	       
                repo.remove(it);
                if(!found) found = true;
            }
        }
    }
    return (found ? 0:-1);
}

int Repository::hasGroup(const QCString &group) const
{
    if ( !group.isEmpty() )
    {
        RepoCIterator it;
        for (it=repo.begin(); it!=repo.end(); it++)
        {
            if (it.data().group == group)
                return 0;
        }
    }
    return -1;
}

QCString Repository::findKeys(const QCString &group, const char *sep ) const
{
    QCString list="";
    if( !group.isEmpty() )
    {
        int pos;
        QCString key;
        RepoCIterator it;
        for (it=repo.begin(); it!=repo.end(); it++)
        {
            kdDebug(1205) << "Group key: " << it.data().group << endl;
            if (it.data().group == group)
            {
                key = it.key().copy();
                pos = key.findRev(sep);
                key.truncate( pos );
                key.remove(0, 2);
                kdDebug(1205) << "Matching group key found: " << key << endl;
                if (list.isEmpty())
                {
                    list = key;
                }
                else
                {
                    // Add the same key sequence only once please :)
                    if( !list.contains(key) )
                    {
                        list += '\007';
                        list.append(key);
                    }
                }
            }
        }
    }
    return list;
}

QCString Repository::find(const QCString &key) const
{
    if( key.isEmpty() )
        return 0;

    RepoCIterator it = repo.find(key);
    if (it == repo.end())
        return 0;
    return it.data().value;
}


int Repository::expire()
{
    unsigned current = time(0L);
    if (current < head_time)
	return 0;

    unsigned t;
    QValueStack<QCString> keys;
    head_time = (unsigned) -1;
    RepoIterator it;
    for (it=repo.begin(); it!=repo.end(); it++)
    {
	t = it.data().timeout;
	if (t <= current)
	    keys.push(it.key());
	else
	    head_time = QMIN(head_time, t);
    }

    int n = keys.count();
    while (!keys.isEmpty())
	remove(keys.pop());
    return n;
}

