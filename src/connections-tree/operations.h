#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <QStringList>
#include <functional>

namespace Console {
    class Operations;
}

namespace ConnectionsTree {

    class Operations
    {
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
         * @brief getConsoleOperations
         * @return QSharedPointer<ConsoleOperations>
         */
        virtual QSharedPointer<Console::Operations> getConsoleOperations() = 0;

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


        virtual ~Operations() {}

    };
}


#endif // OPERATIONS_H
