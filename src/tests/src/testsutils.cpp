/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://gnu.org/licenses>.

*******************************************************************************/

//==============================================================================
// Some useful tests-related functions
//==============================================================================

#include "testsutils.h"

//==============================================================================

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QProcess>

//==============================================================================

namespace OpenCOR {

//==============================================================================

QString targetPlatformDir()
{
#ifdef QT_DEBUG
#if defined(Q_OS_WIN)
    return "windows/debug";
#elif defined(Q_OS_LINUX)
    return "linux/debug";
#else
    return "macos/debug";
#endif
#else
#if defined(Q_OS_WIN)
    return "windows/release";
#elif defined(Q_OS_LINUX)
    return "linux/release";
#else
    return "macos/release";
#endif
#endif
}

//==============================================================================

QString dirOrFileName(const QString &pDirOrFileName)
{
    // Format and return the given directory or file name, so that it can be
    // used on all our supported platforms

    static const QStringList SourceDirs = OpenCOR::fileContents(":/source_directory");
    static const QString SourceDir = SourceDirs.first();

    return QDir::toNativeSeparators(SourceDir+"/"+QString(pDirOrFileName));
}

//==============================================================================

QString dirName(const QString &pDirName)
{
    // Format and return the given file name, so that it can be used on all our
    // supported platforms

    return dirOrFileName(pDirName);
}

//==============================================================================

QString fileName(const QString &pFileName)
{
    // Format and return the given file name, so that it can be used on all our
    // supported platforms

    return dirOrFileName(pFileName);
}

//==============================================================================

QByteArray rawFileContents(const QString &pFileName)
{
    // Read and return the contents of the given file

    QByteArray res;
    QFile file(pFileName);

    if (file.open(QIODevice::ReadOnly)) {
        res = file.readAll();

        file.close();
    }

    return res;
}

//==============================================================================

QString textFileContents(const QString &pFileName)
{
    // Read and return the contents of the given file

    QString res;
    QFile file(pFileName);

    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        // Note: QIODevice::Text ensures that end-of-line terminators are
        //       converted to "\n", which is exactly what we want...

        res = file.readAll();

        file.close();
    }

    return res;
}

//==============================================================================

QStringList fileContents(const QString &pFileName)
{
    // Read and return the contents of the given file

    return textFileContents(pFileName).split('\n');
}

//==============================================================================

QStringList fileContents(const QString &pFileName, const QString &pArg)
{
    // Read and return the contents of the given file

    return textFileContents(pFileName).arg(pArg).split('\n');
}

//==============================================================================

QString fileSha1(const QString &pFileName)
{
    // Read and return the contents of the given file

    return QCryptographicHash::hash(rawFileContents(pFileName),
                                    QCryptographicHash::Sha1).toHex();
}

//==============================================================================

int runCli(const QStringList &pArguments, QStringList &pOutput)
{
    // Go to the directory where our tests are located
    // Note: see main()...

    static const QStringList BuildDirs = OpenCOR::fileContents(":/build_directory");
    static const QString BuildDir = BuildDirs.first();

#ifdef Q_OS_WIN
    QString origPath = QDir::currentPath();

    QDir::setCurrent(BuildDir+"/bin");
#endif

    // Execute the CLI version of OpenCOR (passing to it the given arguments)
    // and return the output it has generated, if any

#if defined(Q_OS_WIN)
    QString program = "OpenCOR.com";
#elif defined(Q_OS_LINUX)
    QString program = BuildDir+"/bin/OpenCOR";
#else
    QString program = BuildDir+"/OpenCOR.app/Contents/MacOS/OpenCOR";
#endif

    QProcess process;

    process.setProcessChannelMode(QProcess::MergedChannels);

    process.start(program, QStringList() << pArguments);
    process.waitForFinished(-1);

    QString output = process.readAll();

    // Clean up our output by:
    //  - Replacing escaped backslashes with a non-escaped one;
    //  - Making paths relative rather than absolute;
    //  - Replacing backslashes with forward slashes; and
    //  - Removing all occurrences of the CR character.
    // Note: the idea is to be able to compare our output independent of where
    //       the test was run from and of which operating system it was run
    //       on...

    pOutput = output.replace("\\\\", "\\")
                    .remove(dirName())
                    .replace("\\", "/")
                    .remove('\r')
                    .split('\n');

    // Go back to our original directory

#ifdef Q_OS_WIN
    QDir::setCurrent(origPath);
#endif

    return process.exitCode();
}

//==============================================================================

} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
