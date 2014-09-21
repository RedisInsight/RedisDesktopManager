#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <QStringList>
#include <functional>
#include "exception.h"

namespace Console {
    class Operations;
}

namespace ConnectionsTree {

    class Operations
    {
        ADD_EXCEPTION
    public:

        /**
         * List of databases with keys counters
         * @emit databesesLoaded
         **/
        typedef QHash <QString, int> DatabaseList;
        virtual void getDatabases(std::function<void(DatabaseList)>) = 0;

        /**
         * @brief getDatabaseKeys
         * @param dbIndex
         */
        typedef QStringList RawKeysList;
        virtual void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)>) = 0;

        /**
         * Cancel all operations & close connection
         * @brief disconnect
         */
        virtual void disconnect() = 0;

        /**
         * @brief getNamespaceSeparator
         * @return
         */
        virtual QString getNamespaceSeparator() = 0;

        virtual void openKeyTab(/* TBD */) = 0;
        virtual void openNewKeyTab(/* TBD */) = 0;

//        QSharedPointer<ValueEditor::Tab> tab(new ValueEditor::Tab());

//        tabs.addTab(tab.staticCast<BaseTab>());

        //void MainWin::OnKeyOpenInNewTab()
        //{
        //    QStandardItem * item = ui.serversTreeView->getSelectedItem();

        //    if (item == nullptr || item->type() != RedisKeyItem::TYPE)
        //        return;

        //    ui.tabWidget->openKeyTab((RedisKeyItem *)item, true);
        //}

        virtual void openConsoleTab() = 0;

        virtual ~Operations() {}

    };
}


#endif // OPERATIONS_H
