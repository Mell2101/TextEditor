#include <catch2/catch_all.hpp>

#include <TextEditorCore/Document.h>

//=============================================================================
//=============================================================================

class DocumentListener: public TextEditorCore::IDocumentListener
{
public:
    std::function<void (const size_t)>                  onCreatedCallback = [](const size_t){printf("onCreatedCallback\n");};
    std::function<void (const size_t, std::string&)>    onChangedCallback = [](const size_t index, std::string& data){printf("onChangedCallback\n");};
    std::function<void (const size_t)>                  onModifyErrorCallback = [](const size_t index){printf("onModifyErrorCallback\n");};
    
    std::function<void (const size_t)>                                      onIOStartCallback = [](const size_t){printf("onIOStartCallback\n");};
    std::function<void (const size_t, TextEditorCore::FileIOErrorsEnum)>    onIOErrorCallback = [](const size_t, TextEditorCore::FileIOErrorsEnum){printf("onIOErrorCallback\n");};
    std::function<void (const size_t, const float)>                         onProgressCallback = [](const size_t, const float) {printf("onProgressCallback\n");};
    std::function<void (const size_t)>                                      onPauseCallback = [](const size_t){printf("onPauseCallback\n");};
    std::function<void (const size_t)>                                      onResumeCallback = [](const size_t index){printf("onResumeCallback\n");};
    std::function<void (const size_t)>                                      onStopCallback = [](const size_t index){printf("onStopCallback\n");};
    std::function<void (const size_t, std::string&)>                        onLoadCompleteCallback = [](const size_t, std::string&){printf("onLoadCompleteCallback\n");};
    std::function<void (const size_t)>                                      onSaveCompleteCallback = [](const size_t){printf("onSaveCompleteCallback\n");};
    
    
    virtual void onCreated(const size_t index) override { onCreatedCallback(index); }
    virtual void onChanged(const size_t index, std::string& data) override { onChangedCallback(index, data); }
    virtual void onModifyError(const size_t index) override { onModifyErrorCallback(index); }
    
    virtual void onStartLoading(const size_t index) override { onIOStartCallback(index); };
    virtual void onIOError(const size_t index, TextEditorCore::FileIOErrorsEnum error) override { onIOErrorCallback(index, error); };
    virtual void onProgress(const size_t index, const float percent) override { onProgressCallback(index, percent); };
    virtual void onPause(const size_t index) override { onPauseCallback(index); };
    virtual void onResume(const size_t index) override { onResumeCallback(index); };
    virtual void onStop(const size_t index) override { onStopCallback(index); };
    virtual void onLoadComplete(const size_t index, std::string& dataBuffer) override { onLoadCompleteCallback(index, dataBuffer); };
    virtual void onSaveComplete(const size_t index) override { onSaveCompleteCallback(index); };
    
    virtual ~DocumentListener() = default;
};

//=============================================================================
//=============================================================================

TEST_CASE("Document::setText()--Success", "[Document::setText()--Success]")
{
    TextEditorCore::Document document(0);
    DocumentListener documentListener;
    
    documentListener.onCreatedCallback = [](const size_t){REQUIRE(false);};
    documentListener.onModifyErrorCallback = [](const size_t index){REQUIRE(false);};
    
    documentListener.onChangedCallback = [&](const size_t index, std::string& data)
    {
        REQUIRE(data == "Blah blah");
    };
    
    document.setListener(documentListener);
    document.setText("Blah blah");
}

//=============================================================================

TEST_CASE("Document::modifyText()--Success", "[Document::modifyText()--Success]")
{
    TextEditorCore::Document document(0);
    
    DocumentListener documentListener;
    documentListener.onCreatedCallback = [](const size_t){REQUIRE(false);};
    documentListener.onModifyErrorCallback = [](const size_t index){REQUIRE(false);};
    
    documentListener.onChangedCallback = [&](const size_t index, std::string& data)
    {
        REQUIRE(data == "Blah BLLah");
    };
    
    document.setText("Blah blah");
    document.setListener(documentListener);
    document.modifyText(5, 2, "BLL");
}