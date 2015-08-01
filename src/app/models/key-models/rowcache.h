#pragma once
#include <QPair>
#include <QList>
#include <QHash>


template < typename T > class MappedCache
{
public:
    typedef unsigned long int RowIndex;
    typedef QPair<RowIndex, RowIndex> CacheRange;

public:

    MappedCache()
        : m_size(0)
    {

    }

    void addLoadedRange(const CacheRange& range, const QList<T>& dataForRange)
    {
        m_mapping[range] = dataForRange;
        m_size += dataForRange.size();
    }

    bool isRowLoaded(RowIndex index)
    {
        CacheRange i = findTargetRange(index);
        return i != CacheRange({-1, -1});
    }

    T getRow(RowIndex index)
    {
        if (!isRowLoaded(index))
            return T();

        CacheRange i = findTargetRange(index);

        RowIndex mappedIndex = i.first - index;

        return m_mapping.value(i).at(mappedIndex);
    }

    T operator [](RowIndex index) {
        return getRow(index);
    }

    void replace(RowIndex i, T row) {
        if (!isRowLoaded(i)) {
            return std::
        }



    }

    unsigned long long size() const
    {
        return m_size;
    }

    void clear()
    {
        m_mapping.clear();
        m_size = 0;
    }

private:
    CacheRange findTargetRange(RowIndex index)
    {
        for (auto i = m_mapping.constBegin();
             i != m_mapping.constEnd(); ++i)
        {
            if (i.key().first <= index && index <= i.key().second) {
                return i.key();
            }
        }
        return {-1, -1};
    }

private:
    QHash<CacheRange, QList<T>> m_mapping;
    unsigned long long m_size;
};

