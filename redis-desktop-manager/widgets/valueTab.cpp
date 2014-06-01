#include "valueTab.h"
#include "HashKeyModel.h"
#include "RedisKeyItem.h"
#include "ListKeyModel.h"
#include "SortedSetKeyModel.h"
#include "ValueTabView.h"
#include "command.h"
#include "response.h"
#include <QMessageBox>

ValueTab::ValueTab(RedisKeyItem * key)    
    : key(key), ui(nullptr), isInitialized(false),
      tabMustBeDestroyed(false), operationInProgress(true)
{    
    setObjectName("valueTab");

    if (key == nullptr
            || key->getDbItem() == nullptr
            || key->getConnection() == nullptr) {
        return;
    }

    ui = QSharedPointer<ValueTabView>(new ValueTabView(key, key->text(), this));

    connect((QObject *)key->getDbItem(), SIGNAL(destroyed(QObject *)), this, SLOT(OnClose()));

    RedisClient::Command typeCmd = key->getTypeCommand();
    typeCmd.setOwner(this);
    typeCmd.setCallBackName("keyTypeLoaded");

    key->getConnection()->runCommand(typeCmd);        

    /** Connect View SIGNALS to Controller SLOTS **/
    connect(ui->renameKey, SIGNAL(clicked()), this, SLOT(renameKey()));
    connect(ui->deleteKey, SIGNAL(clicked()), this, SLOT(deleteKey()));
    connect(ui.data(), SIGNAL(saveChangedValue(const QString&, const QModelIndex *)),
        this, SLOT(updateValue(const QString&, const QModelIndex *)));

    connect(this, SIGNAL(error(const QString&)), this, SLOT(errorOccurred(const QString&)));    

    isInitialized = true;
}

void ValueTab::close()
{
    tabMustBeDestroyed = true;

    if (!operationInProgress)
        destroy();
}

void ValueTab::OnClose()
{
    close();
}

void ValueTab::destroy()
{
    delete this;
}

bool ValueTab::isOperationsAborted()
{
    operationInProgress = false;

    return tabMustBeDestroyed;
}

bool ValueTab::shouldBeReplaced()
{
    return !operationInProgress;
}

void ValueTab::keyTypeLoaded(RedisClient::Response type)
{    
    if (isOperationsAborted())
        return destroy();

    QString t = type.getValue().toString();
    ui->keyTypeLabelValue->setText(
        ui->keyTypeLabelValue->text()  + t.toUpper()
        );

    keyModel = QSharedPointer<KeyModel>(key->getKeyModel(t));

    if (keyModel.isNull()) {
        emit error("Can not load key value. Key was removed or redis-server went away.");        
        return;
    }

    connect(keyModel.data(), SIGNAL(valueLoaded()), this, SLOT(valueLoaded()));
    connect(keyModel.data(), SIGNAL(keyRenameError(const QString&)), this, SIGNAL(error(const QString&)));
    connect(keyModel.data(), SIGNAL(keyRenamed()), this, SLOT(keyRenamed()));
    connect(keyModel.data(), SIGNAL(keyDeleteError(const QString&)), this, SIGNAL(error(const QString&)));
    connect(keyModel.data(), SIGNAL(keyDeleted()), this, SLOT(keyDeleted()));
    connect(keyModel.data(), SIGNAL(valueUpdateError(const QString&)), this, SIGNAL(error(const QString&)));
    connect(keyModel.data(), SIGNAL(valueUpdated()), this, SLOT(valueUpdated()));
    connect(keyModel.data(), SIGNAL(ttlLoaded(RedisClient::Response)), this, SLOT(ttlLoaded(RedisClient::Response)));

    operationInProgress = true;
    keyModel->loadValue();
    keyModel->loadTTL();
}

void ValueTab::valueLoaded()
{
    if (isOperationsAborted())
        return destroy();

    ui->initKeyValue(keyModel.data());

    setObjectName("valueTabReady");
}

void ValueTab::ttlLoaded(RedisClient::Response r)
{
    if (isOperationsAborted())
        return destroy();

    if (r.isErrorMessage())
        return;

    ui->setKeyExpire(r.getValue().toInt());
}

void ValueTab::renameKey()
{
    operationInProgress = true;
    ui->showLoader();
    ui->renameKey->setEnabled(false);
    keyModel->renameKey(ui->keyName->text());    
}

void ValueTab::keyRenamed()
{
    if (isOperationsAborted())
        return destroy();

    key->setText(ui->keyName->text());
    ui->renameKey->setEnabled(true);
    ui->hideLoader();
}

void ValueTab::deleteKey()
{
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "Confirm action", "Do you want delete this key?",
        QMessageBox::Yes|QMessageBox::No);

    if (reply != QMessageBox::Yes) 
        return;

    operationInProgress = true;
    ui->showLoader();
    keyModel->deleteKey();    
}

void ValueTab::keyDeleted()
{
    if (isOperationsAborted())
        return destroy();

    key->remove();
    ui->hideLoader();
    emit keyDeleted(this, key);    
}

void ValueTab::updateValue(const QString& value, const QModelIndex *cellIndex)
{
    operationInProgress = true;
    ui->showLoader();
    keyModel->updateValue(value, cellIndex);
}

void ValueTab::valueUpdated()
{
    if (isOperationsAborted())
        return destroy();

    ui->hideLoader();
}

ValueTab::~ValueTab()
{
    this->disconnect();
    keyModel->disconnect();    
}

void ValueTab::errorOccurred(const QString& message)
{
    ui->hideLoader();
    QMessageBox::warning(this, "Error occurred", message);
}

