#include "sortfilterproxymodel.h"

#include "sortfilterproxymodel.h"
#include <QtDebug>
#include <QtQml>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent), m_complete(false)
{
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SIGNAL(countChanged()));
}

int SortFilterProxyModel::count() const
{
    return rowCount();
}

QObject *SortFilterProxyModel::source() const
{
    return sourceModel();
}

void SortFilterProxyModel::setSource(QObject *source)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(source));
}

QByteArray SortFilterProxyModel::sortRole() const
{
    return m_sortRole;
}

void SortFilterProxyModel::setSortRole(const QByteArray &role)
{
    if (m_sortRole != role) {
        m_sortRole = role;
        if (m_complete)
            QSortFilterProxyModel::setSortRole(roleKey(role));
    }
}

void SortFilterProxyModel::setSortOrder(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}

QByteArray SortFilterProxyModel::filterRole() const
{
    return m_filterRole;
}

void SortFilterProxyModel::setFilterRole(const QByteArray &role)
{
    if (m_filterRole != role) {
        m_filterRole = role;
        if (m_complete)
            QSortFilterProxyModel::setFilterRole(roleKey(role));
    }
}

QString SortFilterProxyModel::filterString() const
{
    return filterRegExp().pattern();
}

void SortFilterProxyModel::setFilterString(const QString &filter)
{
    setFilterRegExp(QRegExp(filter, filterCaseSensitivity(), static_cast<QRegExp::PatternSyntax>(filterSyntax())));
}

SortFilterProxyModel::FilterSyntax SortFilterProxyModel::filterSyntax() const
{
    return static_cast<FilterSyntax>(filterRegExp().patternSyntax());
}

void SortFilterProxyModel::setFilterSyntax(SortFilterProxyModel::FilterSyntax syntax)
{
    setFilterRegExp(QRegExp(filterString(), filterCaseSensitivity(), static_cast<QRegExp::PatternSyntax>(syntax)));
}

QJSValue SortFilterProxyModel::get(int idx) const
{
    QJSEngine *engine = qmlEngine(this);
    QJSValue value = engine->newObject();
    if (idx >= 0 && idx < count()) {
        QHash<int, QByteArray> roles = roleNames();
        QHashIterator<int, QByteArray> it(roles);
        while (it.hasNext()) {
            it.next();
            value.setProperty(QString::fromUtf8(it.value()), data(index(idx, 0), it.key()).toString());
        }
    }
    return value;
}

void SortFilterProxyModel::classBegin()
{
}

void SortFilterProxyModel::componentComplete()
{
    m_complete = true;
    if (!m_sortRole.isEmpty())
        QSortFilterProxyModel::setSortRole(roleKey(m_sortRole));
    if (!m_filterRole.isEmpty())
        QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
}

int SortFilterProxyModel::roleKey(const QByteArray &role) const
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> it(roles);
    while (it.hasNext()) {
        it.next();
        if (it.value() == role)
            return it.key();
    }
    return -1;
}

QHash<int, QByteArray> SortFilterProxyModel::roleNames() const
{
    if (QAbstractItemModel *source = sourceModel())
        return source->roleNames();
    return QHash<int, QByteArray>();
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QRegExp rx = filterRegExp();
    if (rx.isEmpty())
        return true;
    QAbstractItemModel *model = sourceModel();
    if (filterRole().isEmpty()) {
        QHash<int, QByteArray> roles = roleNames();
        QHashIterator<int, QByteArray> it(roles);
        while (it.hasNext()) {
            it.next();
            QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
            QString key = model->data(sourceIndex, it.key()).toString();
            if (key.contains(rx))
                return true;
        }
        return false;
    }
    QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
    if (!sourceIndex.isValid())
        return true;
    QString key = model->data(sourceIndex, roleKey(filterRole())).toString();
    return key.contains(rx);
}
