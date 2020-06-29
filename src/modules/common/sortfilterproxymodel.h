#pragma once

#include <QtCore/qsortfilterproxymodel.h>
#include <QtQml/qqmlparserstatus.h>

class SortFilterProxyModel : public QSortFilterProxyModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QObject *source READ source WRITE setSource)

    Q_PROPERTY(QByteArray sortRole READ sortRole WRITE setSortRole)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)

    Q_PROPERTY(QByteArray filterRole READ filterRole WRITE setFilterRole)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
    Q_PROPERTY(FilterSyntax filterSyntax READ filterSyntax WRITE setFilterSyntax)

    Q_PROPERTY(int filterKeyColumn READ filterKeyColumn WRITE setFilterKeyColumn)

    Q_ENUMS(FilterSyntax)

public:
    explicit SortFilterProxyModel(QObject *parent = 0);

    QObject *source() const;
    void setSource(QObject *source);

    QByteArray sortRole() const;
    Q_INVOKABLE void setSortRole(const QByteArray &role);

    Q_INVOKABLE void setSortOrder(Qt::SortOrder order);

    QByteArray filterRole() const;
    void setFilterRole(const QByteArray &role);

    QString filterString() const;
    void setFilterString(const QString &filter);

    enum FilterSyntax {
        RegExp,
        Wildcard,
        FixedString
    };

    FilterSyntax filterSyntax() const;
    void setFilterSyntax(FilterSyntax syntax);

    void classBegin();
    void componentComplete();

    Q_INVOKABLE int getOriginalRowIndex(int i);

signals:
    void filterStringChanged();

protected:
    int roleKey(const QByteArray &role) const;

private:
    bool m_complete;
    QByteArray m_sortRole;
    QByteArray m_filterRole;
};
