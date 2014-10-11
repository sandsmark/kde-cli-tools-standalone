/*
 *  Copyright (C) 2002 David Faure   <faure@kde.org>
 *  Copyright (C) 2008 Pino Toscano  <pino@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include <QFile>

#include <qmimetype.h>
#include <kcmdlineargs.h>
#include <QCoreApplication>
#include <kdeversion.h>
#include <klocale.h>
#include <kcomponentdata.h>

#include <stdio.h>
#include <QMimeDatabase>
#include <QMimeType>

int main(int argc, char *argv[])
{
    KCmdLineArgs::init( argc, argv, "kmimetypefinder", 0, ki18n("MimeType Finder"), PROJECT_VERSION , ki18n("Gives the mimetype for a given file"));


    KCmdLineOptions options;

    options.add("c").add("content", ki18n("Use only the file content for determining the mimetype."));
    options.add("f").add("filename-only", ki18n("Whether use the file name only for determining the mimetype. Not used if -c is specified."));
    options.add("+filename", ki18n("The filename to test. '-' to read from stdin."));

    KCmdLineArgs::addCmdLineOptions( options );

    QCoreApplication app(argc, argv); // in case KSycoca needs a QEventLoop
    KComponentData instance("kmimetypefinder");

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if( args->count() < 1 ) {
        printf( "No filename specified\n" );
        return 1;
    }
    const QString fileName = args->arg( 0 );
    QMimeDatabase db;
    QMimeType mime;
    if (fileName == QLatin1String("-")) {
        QFile qstdin;
        qstdin.open(stdin, QIODevice::ReadOnly);
        const QByteArray data = qstdin.readAll();
        mime = db.mimeTypeForData(data);
    } else if (args->isSet("c")) {
        mime = db.mimeTypeForFile(fileName, QMimeDatabase::MatchContent);
    } else if (args->isSet("f")) {
        mime = db.mimeTypeForFile(fileName, QMimeDatabase::MatchExtension);
    } else {
        mime = db.mimeTypeForFile(fileName);
    }
    if ( !mime.isDefault() ) {
        printf("%s\n", mime.name().toLatin1().constData());
    } else {
        return 1; // error
    }

    return 0;
}
