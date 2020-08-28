#pragma once

#include "treeitem.h"

namespace ConnectionsTree {

class Model;

class SortableTreeItem : public TreeItem {

public:
  SortableTreeItem(Model& model)
    : TreeItem(model), m_row(0) {}

  int row() const override { return m_row; }

  void setRow(int r) { m_row = r; }

  virtual void unload() = 0;

protected:
  int m_row;
};

}
