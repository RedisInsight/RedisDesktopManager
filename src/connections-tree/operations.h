#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QString>
#include <QHash>
#include <QSharedPointer>
#include <QStringList>
#include <functional>

namespace ConnectionsTree {

    class Operations
    {
    public:

        class ConsoleOperations {
        public:

        };

        /**
         * List of databases with keys counters
         * @emit databesesLoaded
         **/
        typedef QMap <QString, int> DatabaseList;
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
        virtual QSharedPointer<ConsoleOperations> getConsoleOperations() = 0;

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
