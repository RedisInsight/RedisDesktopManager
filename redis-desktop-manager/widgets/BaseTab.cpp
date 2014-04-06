#include "BaseTab.h"

void BaseTab::close()
{
    delete this;
}

bool BaseTab::shouldBeReplaced()
{
    return false;
}
