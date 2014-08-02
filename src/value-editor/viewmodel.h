#pragma once

#include <QString>

class ViewModel
{
public:
    ViewModel();

    void removeKey();
    void renameKey(const QString& newName);

    void removeValueItem(int itemIndex);
    void updateValueItemComponent(int itemIndex, int partIndex, const QString& newValue);



};
