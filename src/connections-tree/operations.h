#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QString>
#include <QHash>
#include <functional>

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
         * Cancel all operations & close connection
         * @brief disconnect
         */
        virtual void disconnect() = 0;

    };
}


#endif // OPERATIONS_H
