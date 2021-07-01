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
// PMR Web service
//==============================================================================

#pragma once

//==============================================================================

#include "pmrexposure.h"
#include "pmrsupportglobal.h"
#include "pmrworkspace.h"

//==============================================================================

#include <QHash>

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

class PmrWebServiceManager;

//==============================================================================

static const auto RequestMimeType    = QStringLiteral("application/vnd.physiome.pmr2.json.0");
static const auto CollectionMimeType = QStringLiteral("application/vnd.physiome.pmr2.json.1");

//==============================================================================

class PMRSUPPORT_EXPORT PmrWebService : public QObject
{
    Q_OBJECT

public:
    explicit PmrWebService(const QString &pPmrUrl, QObject *pParent);
    explicit PmrWebService(QObject *pParent);

    bool isAuthenticated() const;
    void authenticate(bool pAuthenticate = true);

    void requestExposures() const;

    PmrWorkspace * workspace(const QString &pUrl) const;

    void requestNewWorkspace(const QString &pName, const QString &pDescription,
                             const QString &pPath) const;
    void requestWorkspaces() const;

    void requestWorkspaceClone(PmrWorkspace *pWorkspace,
                               const QString &pPath);
    void requestWorkspaceSynchronize(PmrWorkspace *pWorkspace, bool pPush);

    void update(const QString &pPmrUrl);

    QString siteName() const;

private:
    enum class Action {
        None,
        CloneExposureWorkspace,
        RequestExposureFiles
    };

    QString mPmrUrl;

    PmrWebServiceManager *mPmrWebServiceManager;

    QMap<QString, PmrExposure *> mUrlExposures;
    QHash<PmrExposure *, int> mFileExposuresLeftCount;

    void requestWorkspaceInformation(const QString &pUrl,
                                     const QString &pPath,
                                     PmrExposure *pExposure = nullptr);
    void requestWorkspaceCredentials(PmrWorkspace *pWorkspace);

    void requestExposureFileInformation(const QString &pUrl,
                                        PmrExposure *pExposure);

    void emitInformation(const QString &pMessage);

signals:
    void busy(bool pBusy);
    void authenticated(bool pAuthenticated);

    void information(const QString &pMessage);
    void warning(const QString &pMessage);
    void error(const QString &pMessage);

    void authenticationCancelled();

    void workspaces(const PmrWorkspaces &pWorkspaces);

    void workspaceCreated(const QString &pUrl);
    void workspaceCloned(PmrWorkspace *pWorkspace);
    void workspaceSynchronized(PmrWorkspace *pWorkspace);

    void exposures(const PmrExposures &pExposures);
    void exposureFiles(const QString &pUrl, const QStringList &pExposureFiles);

public slots:
    void forbidden(const QString &pUrl);

    void requestExposureFiles(const QString &pUrl);
    void requestExposureWorkspaceClone(const QString &pUrl);

private slots:
    void exposuresResponse(const QJsonDocument &pJsonDocument);

    void workspaceResponse(const QJsonDocument &pJsonDocument);

    void newWorkspaceResponse(const QString &pUrl);
    void workspacesResponse(const QJsonDocument &pJsonDocument);

    void workspaceInformationResponse(const QJsonDocument &pJsonDocument);

    void workspaceErrored();
    void workspaceCloneFinished(PmrWorkspace *pWorkspace);
    void workspaceSynchronizeFinished(PmrWorkspace *pWorkspace);

    void workspaceCredentialsResponse(const QJsonDocument &pJsonDocument);

    void exposureInformationResponse(const QJsonDocument &pJsonDocument);
    void exposureFileInformationResponse(const QJsonDocument &pJsonDocument);
};

//==============================================================================

} // namespace PMRSupport
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
