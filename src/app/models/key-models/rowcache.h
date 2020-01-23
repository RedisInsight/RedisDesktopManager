#pragma once
#include <QHash>
#include <QList>
#include <QPair>
#include <exception>

typedef qlonglong RowIndex;

class CacheRange : public QPair<RowIndex, RowIndex> {
 public:
  CacheRange(const RowIndex& f = -1, const RowIndex& s = -1)
      : QPair<RowIndex, RowIndex>(f, s) {}

  bool isEmpty() const { return first == -1 && second == -1; }
};

template <typename T>
class MappedCache {
 public:
  MappedCache() : m_valid(false) {}

  bool isValid() const { return m_valid; }

  void addLoadedRange(const CacheRange& range, const QList<T>& dataForRange) {
    if (!isValid()) clear();

    m_mapping[range] = dataForRange;
  }

  bool isRowLoaded(RowIndex index) {
    CacheRange i = findTargetRange(index);
    return !i.isEmpty();
  }

  T getRow(RowIndex index) {
    if (!isRowLoaded(index)) return T();

    CacheRange i = findTargetRange(index);
    return m_mapping[i].at(index - i.first);
  }

  T operator[](RowIndex index) { return getRow(index); }

  void replace(RowIndex index, T row) {
    if (!isRowLoaded(index)) {
      throw std::out_of_range("Invalid row");
    }
    CacheRange i = findTargetRange(index);
    return m_mapping[i].replace(index - i.first, row);
  }

  void removeAt(RowIndex index) {
    if (!isRowLoaded(index)) {
      throw std::out_of_range("Invalid row");
    }
    CacheRange i = findTargetRange(index);

    m_mapping[i].removeAt(index - i.first);
    CacheRange newKey{i.first, i.second - 1};
    replaceRangeInMapping(newKey, i);
    m_valid = false;
  }

  void push_back(const T& row) {
    CacheRange newKey{0, 1};

    if (m_mapping.size() > 0) {
      newKey.first += m_mapping.lastKey().first;
      newKey.second += m_mapping.lastKey().second;

      m_mapping.last().push_back(row);
      replaceRangeInMapping(newKey);
    } else {
      m_mapping.insert(newKey, QList<T>{row});
    }
  }

  unsigned long long size() const {
    unsigned long long cacheSize = 0;
    for (auto cachePage : m_mapping) {
      cacheSize += cachePage.size();
    }
    return cacheSize;
  }

  void clear() {
    m_mapping.clear();
    m_valid = true;
  }

 private:
  CacheRange findTargetRange(RowIndex index) {
    for (auto i = m_mapping.constBegin(); i != m_mapping.constEnd(); ++i) {
      if (i.key().first <= index && index <= i.key().second) {
        return i.key();
      }
    }
    return CacheRange();
  }

  void replaceRangeInMapping(const CacheRange& newRange,
                             const CacheRange& current = CacheRange()) {
    CacheRange replaceKey = current.isEmpty() ? m_mapping.lastKey() : current;

    m_mapping[newRange] = m_mapping[replaceKey];
    m_mapping.remove(replaceKey);
  }

 private:
  QMap<CacheRange, QList<T>> m_mapping;
  bool m_valid;
};
