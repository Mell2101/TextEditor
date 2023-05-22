// #include <TextManager.h>
// #include <catch2/catch_all.hpp>

// TEST_CASE("TextManager::TextManager()", "[TextManager::TextManager()]")
// {
//     TextEditorCore::TextManager textManager;
//     const std::string& text = textManager.getTextData();
//     REQUIRE(text.empty());
// }

// TEST_CASE("TextManager::TextManager(str)", "[TextManager::TextManager(str)]")
// {
//     TextEditorCore::TextManager textManager("blah blah!");
//     const std::string& text = textManager.getTextData();
//     REQUIRE(text == "blah blah!");
// }

// TEST_CASE("TextManager::clean()", "[TextManager::clean()]")
// {
//     {
//         TextEditorCore::TextManager textManager("blah blah!");
//         textManager.clean();
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text.empty());
//     }

//     {
//         TextEditorCore::TextManager textManager;
//         textManager.clean();
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text.empty());
//     }
// }

// TEST_CASE("TextManager::setTextData()", "[TextManager::setTextData()]")
// {
//     {
//         TextEditorCore::TextManager textManager("blah blah!");
//         textManager.setTextData("new text!");
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text == "new text");
//     }

//     {
//         TextEditorCore::TextManager textManager;
//         textManager.setTextData("new text!");
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text == "new text!");
//     }

//     {
//         TextEditorCore::TextManager textManager;
//         textManager.setTextData(std::string("aboba"));
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text == "aboba");
//     }
    
//     {
//         TextEditorCore::TextManager textManager("blah blah!");
//         textManager.setTextData(std::string());
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text.empty());
//     }

//     {
//         TextEditorCore::TextManager textManager;
//         textManager.setTextData(std::string());
//         const std::string& text = textManager.getTextData();
//         REQUIRE(text.empty());
//     }
// }