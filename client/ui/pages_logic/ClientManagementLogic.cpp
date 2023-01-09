#include "ClientManagementLogic.h"

#include "core/servercontroller.h"
#include "ui/pages_logic/ClientInfoLogic.h"
#include "ui/models/clientManagementModel.h"
#include "ui/uilogic.h"

ClientManagementLogic::ClientManagementLogic(UiLogic *logic, QObject *parent):
    PageLogicBase(logic, parent)
{

}

void ClientManagementLogic::onUpdatePage()
{
    DockerContainer selectedContainer = m_settings->defaultContainer(uiLogic()->selectedServerIndex);
    QString selectedContainerName = ContainerProps::containerHumanNames().value(selectedContainer);
    set_labelCurrentVpnProtocolText(tr("Service: ") + selectedContainerName);

    QJsonObject clients;

    auto protocols = ContainerProps::protocolsForContainer(selectedContainer);
    if (!protocols.empty()) {
        m_currentMainProtocol = protocols.front();

        ErrorCode e = m_serverController->getClientsList(m_settings->serverCredentials(uiLogic()->selectedServerIndex),
                                                          selectedContainer, m_currentMainProtocol, clients);
    }
    QVector<ClientManagementModel::ClientInfo> clientsArray;
    for (auto &clientId : clients.keys()) {
        ClientManagementModel::ClientInfo clientInfo;
        clientInfo.certId = clientId;
        clientInfo.name = clients[clientId].toObject()["name"].toString();
        clientInfo.certData = clients[clientId].toObject()["certificate"].toString();
        clientsArray.push_back(clientInfo);
    }
    qobject_cast<ClientManagementModel*>(uiLogic()->clientManagementModel())->setContent(clientsArray);
}

void ClientManagementLogic::onClientItemClicked(int index)
{
    uiLogic()->pageLogic<ClientInfoLogic>()->setCurrentClientId(index);
    emit uiLogic()->goToClientInfoPage(m_currentMainProtocol);
}
