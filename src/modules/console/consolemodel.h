#pragma once
#include "exception.h"
#include "common/tabviewmodel.h"

namespace Console {

    class Model : public TabModel
    {        
        Q_OBJECT
        ADD_EXCEPTION
    public:       
        Model(QSharedPointer<RedisClient::Connection> connection);

        Q_INVOKABLE void init();

        QString getName() const override;

    public slots:
        void executeCommand(const QString&);

    signals:
        void changePrompt(const QString &text, bool showPrompt);
        void addOutput(const QString &text, QString resultType);

    private:        
        int m_current_db;

    private:
        void updatePrompt(bool showPrompt);
    };
}
