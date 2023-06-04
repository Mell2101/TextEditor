#pragma once

#include <ITextEditorListener.h>

namespace TextEditorCore
{
    
class TextEditor
{
public:
    TextEditor() = delete;
    TextEditor(const TextEditor& other) = delete;
    TextEditor(TextEditor&& other) = delete;
    TextEditor& operator=(const TextEditor& other) = delete;
    TextEditor(TextEditor&& other) = delete;
    TextEditor& operator=(TextEditor&& other) = delete;

    TextEditor(ITextEditorListener* listener);
    ~TextEditor();

    void loadFile(const std::string& fileName);
    void saveFile(const std::string& fileName);

    void modifyText(size_t pos, size_t length, const std::string& data);
};

} // namespace TextEditorCore
