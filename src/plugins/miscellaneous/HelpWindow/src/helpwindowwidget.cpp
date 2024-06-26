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
// Help window widget
//==============================================================================

#include "corecliutils.h"
#include "helpwindowwidget.h"

//==============================================================================

#include <QAction>
#include <QDesktopServices>
#include <QDir>
#include <QHelpEngine>
#include <QIODevice>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include <QWebPage>
#include <QWebView>

//==============================================================================

namespace OpenCOR {
namespace HelpWindow {

//==============================================================================

HelpWindowNetworkReply::HelpWindowNetworkReply(const QByteArray &pData) :
    mData(pData)
{
    // Set a few things for the network reply

    setOpenMode(QIODevice::ReadOnly);

    // Let ourselves know immediately that data is available for reading

    QTimer::singleShot(0, this, &HelpWindowNetworkReply::readyRead);
    QTimer::singleShot(0, this, &HelpWindowNetworkReply::finished);
}

//==============================================================================

void HelpWindowNetworkReply::abort()
{
    // Do nothing on purpose...
}

//==============================================================================

qint64 HelpWindowNetworkReply::bytesAvailable() const
{
    // Return the size of the data which is available for reading

    return mData.length()-mOffset+QNetworkReply::bytesAvailable();
}

//==============================================================================

qint64 HelpWindowNetworkReply::readData(char *pData, qint64 pMaxlen)
{
    // Make sure that we still have some data to read

    qint64 dataLength = qint64(mData.length());

    if (mOffset >= dataLength) {
        pData = nullptr;

        return -1;
    }

    // Determine the length of the data to read, read it and return how much of
    // it we read

    qint64 res = qMin(dataLength-mOffset, pMaxlen);

    memcpy(pData, mData.constData()+mOffset, size_t(res));

    mOffset += res;

    return res;
}

//==============================================================================

HelpWindowNetworkAccessManager::HelpWindowNetworkAccessManager(QHelpEngine *pHelpEngine,
                                                               QObject *pParent) :
    QNetworkAccessManager(pParent),
    mHelpEngine(pHelpEngine)
{
    // Retrieve the error message template

    Core::readFile(":/HelpWindow/error.html", mErrorMessageTemplate);
}

//==============================================================================

static const char *HelpRoot = "qthelp://opencor/doc/html/user/";

//==============================================================================

QNetworkReply * HelpWindowNetworkAccessManager::createRequest(Operation pOperation,
                                                              const QNetworkRequest &pRequest,
                                                              QIODevice *pOutgoingData)
{
    Q_UNUSED(pOperation)
    Q_UNUSED(pOutgoingData)

    // Retrieve and return, if possible, the requested document

    QUrl url = pRequest.url();
    QByteArray data = mHelpEngine->fileData(url);

    return new HelpWindowNetworkReply(data.isEmpty()?
                                          mErrorMessageTemplate.arg(tr("Error"),
                                                                    tr("The following help file could not be found:")+" <strong>"+url.toString().remove(HelpRoot)+"</strong>.",
                                                                    tr(R"(Please <a href="https://opencor.ws/contactUs.html">contact us</a> about this error.)"),
                                                                    Core::copyright()).toUtf8():
                                          data);
}

//==============================================================================

HelpWindowWidget::HelpWindowWidget(QWidget *pParent) :
    WebViewerWidget::WebViewerWidget(pParent)
{
    // Extract the help files

    QString applicationBaseFileName =  QDir::tempPath()+"/"
                                      +QFileInfo(qApp->applicationFilePath()).baseName();

    mQchFileName = applicationBaseFileName+".qch";
    mQhcFileName = applicationBaseFileName+".qhc";

    Core::writeResourceToFile(mQchFileName, ":HelpWindow_qchFile");
    Core::writeResourceToFile(mQhcFileName, ":HelpWindow_qhcFile");

    // Set up our help engine

    mHelpEngine = new QHelpEngine(mQhcFileName);

    // Use our own help network access manager classes

    webView()->page()->setNetworkAccessManager(new HelpWindowNetworkAccessManager(mHelpEngine, this));

    // Set and go to our home page
    // Note: we call setUrl() rather than goToHomePage() because we want and
    //       need our home page to be loaded straight away (otherwise if our
    //       home page is wrong and OpenCOR is in a non-English locale then our
    //       contents will be empty upon starting OpenCOR)...

    setHomePage(QString("%1index.html").arg(HelpRoot));

    webView()->setUrl(homePage());
}

//==============================================================================

HelpWindowWidget::~HelpWindowWidget()
{
    // Delete some internal objects

    delete mHelpEngine;

    // Delete the help files

    QFile::remove(mQchFileName);
    QFile::remove(mQhcFileName);
}

//==============================================================================

void HelpWindowWidget::retranslateUi()
{
    // Retranslate the current document, but only if it is an error document
    // since a valid document is hard-coded and therefore cannot be retranslated

    if (!mHelpEngine->findFile(webView()->url()).isValid()) {
        webView()->load(webView()->url());
    }
}

//==============================================================================

void HelpWindowWidget::loadSettings(QSettings &pSettings)
{
    // Default handling of the event

    WebViewerWidget::WebViewerWidget::loadSettings(pSettings);
}

//==============================================================================

void HelpWindowWidget::saveSettings(QSettings &pSettings) const
{
    // Default handling of the event

    WebViewerWidget::WebViewerWidget::saveSettings(pSettings);
}

//==============================================================================

bool HelpWindowWidget::isUrlSchemeSupported(const QString &pUrlScheme)
{
    // We only support URLs that refer to an OpenCOR document (qthelp://...)

    static const QString QtHelp = "qthelp";

    return pUrlScheme == QtHelp;
}

//==============================================================================

} // namespace HelpWindow
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
