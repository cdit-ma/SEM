
#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <string>
#include <sstream>
#include <stack>

class JsonWriter {
public:
    std::string ToString();

    void Begin();
    void End();

    void AddPair(std::string key, std::string val);

    void BeginKeyedObject(std::string key);
    void BeginObject();
    void EndObject();

    void BeginArray(std::string key);
    void EndArray();
 private:
    void BeginObject_();    
    enum class WriterState{UNINITIALIZED, START_LIST, CONTINUE_LIST, FINISHED, ERROR};
    enum class JsonValueType{STRING, OBJECT, ARRAY};

    WriterState state_ = WriterState::UNINITIALIZED;
    std::stringstream json_text_;
    unsigned int current_indent_ = 0;

    std::stack<JsonValueType> json_stack_;

    void ChangeState(WriterState newState);
    void JsonNewline();

    std::string Tab(int i = 1);
    std::string JsonPair(std::string key, std::string val);
    std::string DblQuoteWrap(std::string val);

    unsigned int GetIndentLevel();
};

#endif //JSONWRITER_H