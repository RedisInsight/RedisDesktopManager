#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QString>
#include <QHash>
#include <QSharedPointer>
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
        typedef QHash <QString, int> DatabaseList;
        virtual void getDatabases(std::function<void(DatabaseList)>) = 0;

        /**
         * @brief getDatabaseKeys
         * @param dbIndex
         */
        typedef QStringList RawKeysList;
        virtual void getDatabaseKeys(uint dbIndex, std::function<void(const RawKeysList&)>);

        /**
         * @brief getConsoleOperations
         * @return QSharedPointer<ConsoleOperations>
         */
        virtual QSharedPointer<ConsoleOperations> getConsoleOperations();

        /**
         * Cancel all operations & close connection
         * @brief disconnect
         */
        virtual void disconnect() = 0;

    };
}


#endif // OPERATIONS_H
