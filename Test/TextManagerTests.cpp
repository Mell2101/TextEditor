#include <TextManager.h>
#include "TestCommons.h"

namespace TextManagerTest
{

void defaultContructorTest()
{
    TextEditorCore::TextManager textManager;
    const std::string& text = textManager.getTextData();
    ASSERT(text.empty());
}

}