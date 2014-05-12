/*******************************************************************************

Licensed to the OpenCOR team under one or more contributor license agreements.
See the NOTICE.txt file distributed with this work for additional information
regarding copyright ownership. The OpenCOR team licenses this file to you under
the Apache License, Version 2.0 (the "License"); you may not use this file
except in compliance with the License. You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

*******************************************************************************/

//==============================================================================
// Main
//==============================================================================

#include "common.h"
#include "guiutils.h"
#include "mainwindow.h"
#include "splashscreenwindow.h"

//==============================================================================

#include <QDir>
#include <QProcess>
#include <QSettings>

#ifdef Q_OS_WIN
    #include <QWebSettings>
#endif

//==============================================================================

#include <QtSingleApplication>

//==============================================================================

#include <stdlib.h>

//==============================================================================

int main(int pArgC, char *pArgV[])
{
    // On OS X, if the user double clicks on the OpenCOR bundle or enter
    // something like the following at the command line:
    //     open OpenCOR.app
    // then we want to start OpenCOR directly in GUI mode rather than try to run
    // it as a CLI application first. Indeed, if we were to try to do the
    // latter, we would get an error message similar to this one:
    //    LSOpenURLsWithRole() failed with error -10810 for the file [SomePath]/OpenCOR.app.
    // Fortunately, when double clicking on the OpenCOR bundle or opening it,
    // then a special argument in the form of -psn_0_1234567 is passed to it, so
    // we can use that to determine whether we need to force OpenCOR to be run
    // into GUI mode or whether we can try to run it in CLI mode

 #ifdef Q_OS_MAC
    if ((pArgC > 1) && !memcmp(pArgV[1], "-psn_", 5)) {
        #define FORCE_GUI_MODE
    }
#endif

    // Create our application
    // Note: on Linux and OS X, we first try to run the CLI version of OpenCOR
    //       while on Windows, we go straight for the GUI version. Indeed, in
    //       the case of Windows, we have two binaries (.com and .exe that are
    //       for the CLI and GUI versions of OpenCOR, respectively). This means
    //       that when a console window is open, to enter something like:
    //           C:\>OpenCOR
    //       will effectively call OpenCOR.com. From there, should there be no
    //       argument that requires CLI treatment, then the GUI version of
    //       OpenCOR will be launched. This is, unfortunately, the only way to
    //       have OpenCOR to behave as both a CLI and a GUI application on
    //       Windows, hence the [OpenCOR]/windows/main.cpp file which is used to
    //       generate the CLI version of OpenCOR...

#if defined(Q_OS_WIN) || defined(FORCE_GUI_MODE)
    SharedTools::QtSingleApplication *app = new SharedTools::QtSingleApplication(QFileInfo(pArgV[0]).baseName(),
                                                                                 pArgC, pArgV);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    QCoreApplication *cliApp = new QCoreApplication(pArgC, pArgV);
#else
    #error Unsupported platform
#endif

    // Remove all 'global' instances, in case OpenCOR previously crashed or
    // something (and therefore didn't remove all of them before quitting)

    OpenCOR::removeGlobalInstances();

    // Some general initialisations

#if defined(Q_OS_WIN) || defined(FORCE_GUI_MODE)
    OpenCOR::initApplication(app);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    OpenCOR::initApplication(cliApp);
#else
    #error Unsupported platform
#endif

    // Try to run OpenCOR as a CLI application, if possible

#if defined(Q_OS_WIN) || defined(FORCE_GUI_MODE)
    // Do nothing...
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    int res;

    bool runCliApplication = OpenCOR::cliApplication(cliApp, &res);

    delete cliApp;

    if (runCliApplication) {
        // OpenCOR was run as a CLI application, so...

        OpenCOR::removeGlobalInstances();

        return res;
    }

    // At this stage, we tried to run the CLI version of OpenCOR, but in the end
    // we need to run the GUI version, so start over but with the GUI version of
    // OpenCOR this time

    // Make sure that we always use indirect rendering on Linux
    // Note: indeed, depending on which plugins are selected, OpenCOR may need
    //       LLVM. If that's the case, and in case the user's video card uses a
    //       driver that relies on LLVM (e.g. Gallium3D and Mesa 3D), then
    //       there may be a conflict between the version of LLVM used by
    //       OpenCOR and the one used by the video card. One way to address this
    //       issue is by using indirect rendering...

#ifdef Q_OS_LINUX
    setenv("LIBGL_ALWAYS_INDIRECT", "1", 1);
#endif

    // Create our application
    // Note: the CLI version of OpenCOR didn't actually do anything, so no need
    //       to re-remove all 'global' instances...

    SharedTools::QtSingleApplication *app = new SharedTools::QtSingleApplication(QFileInfo(pArgV[0]).baseName(),
                                                                                 pArgC, pArgV);

    // Some general initialisations

    OpenCOR::initApplication(app);
#else
    #error Unsupported platform
#endif

    // Initialise our colours by 'updating' them

    OpenCOR::Core::updateColors();

    // Create and show our splash screen, if we are not in debug mode

#ifndef QT_DEBUG
    OpenCOR::SplashScreenWindow *splashScreen = new OpenCOR::SplashScreenWindow();

    splashScreen->show();

    app->processEvents();
    // Note: this ensures that our splash screen is immediately visible...
#endif

    // Send a message (containing the arguments that were passed to this
    // instance of OpenCOR minus the first one since it corresponds to the full
    // path to our executable, which we are not interested in) to our 'official'
    // instance of OpenCOR, should there be one. If there is no none, then just
    // carry on as normal, otherwise exit since we want only one instance of
    // OpenCOR at any given time

    QStringList appArguments = app->arguments();

    appArguments.removeFirst();

    QString arguments = appArguments.join("|");

    if (app->isRunning()) {
        app->sendMessage(arguments);

        delete app;

        return 0;
    }

    // Specify where to find non-OpenCOR plugins, but only if we are on Windows

#ifdef Q_OS_WIN
    app->addLibraryPath( QDir(app->applicationDirPath()).canonicalPath()
                        +QDir::separator()+QString("..")
                        +QDir::separator()+"plugins");
#endif

    // Create our main window

    OpenCOR::MainWindow *win = new OpenCOR::MainWindow(app);

    // Keep track of our main window (required by QtSingleApplication so that it
    // can do what it's supposed to be doing)

    app->setActivationWindow(win);

    // Handle our arguments

    win->handleArguments(arguments);

    // Show our main window

    win->show();

    // By default, we can and should execute our application

    bool canExecuteAplication = true;

    // Close and delete our splash screen once our main window is visible, if we
    // are not in debug mode

#ifndef QT_DEBUG
    splashScreen->closeAndDeleteAfter(win);

    // Make sure that our main window is in the foreground, unless the user
    // decided to close our main window while we were showing our splash screen
    // Note: indeed, on Linux, to show our splash screen may result in our main
    //       window being shown in the background, so...

    if (!win->shuttingDown())
        win->showSelf();
    else
        canExecuteAplication = false;
#endif

    // Execute our application, if possible

#if defined(Q_OS_WIN) || defined(FORCE_GUI_MODE)
    int res;
#endif

    if (canExecuteAplication)
        res = app->exec();
    else
        res = 0;

    // Keep track of our application file and directory paths (in case we need
    // to restart OpenCOR)

    QString appFilePath = app->applicationFilePath();
    QString appDirPath  = app->applicationDirPath();

    // Delete our main window

    delete win;

    // If we use QtWebKit, and QWebPage in particular, then leak messages will
    // get generated on Windows when leaving OpenCOR. This is because an object
    // cache is shared between all QWebPage instances. So to destroy a QWebPage
    // instance won't clear the cache, hence the leak messages. However, these
    // messages are 'only' warnings, so we can safely live with them. Still, it
    // doesn't look 'good', so we clear the memory caches, thus avoiding those
    // leak messages...
    // Note: the below must absolutely be done after calling app->exec() and
    //       before deleting app...

#ifdef Q_OS_WIN
    QWebSettings::clearMemoryCaches();
#endif

    // Delete our application

    delete app;

    // Remove all 'global' instances that were created and used during this
    // session

    OpenCOR::removeGlobalInstances();

    // We are done with the execution of our application, so now the question is
    // whether we need to restart
    // Note: we do this here rather than 'within' the GUI because once we have
    //       launched a new instance of OpenCOR, we want this instance of
    //       OpenCOR to finish as soon as possible which will be the case here
    //       since all that remains to be done is to return the result of the
    //       execution of our application...

    if (res == OpenCOR::NeedRestart)
        // Restart OpenCOR, but without providing any of the arguments that were
        // originally passed to us since we want to reset everything

        QProcess::startDetached(appFilePath, QStringList(), appDirPath);

    // We are done, so...

    return res;
}

//==============================================================================
// End of file
//==============================================================================
