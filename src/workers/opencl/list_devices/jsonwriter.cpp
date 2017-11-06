
#include "jsonwriter.h"
#include <iostream>


void JsonWriter::ChangeState(const WriterState new_state) {
    bool transition_okay = false;
    switch (state_) {
        case WriterState::UNINITIALIZED:{
            switch(new_state){
            case WriterState::START_LIST:
            case WriterState::ERROR:
                transition_okay = true;
                break;
            default:
                break;
            }
            break;
        }

        case WriterState::START_LIST:{
            switch(new_state) {
                case WriterState::START_LIST:
                case WriterState::CONTINUE_LIST:
                //case WriterState::FINISHED:
                case WriterState::ERROR:
                    transition_okay = true;
                    break;
                default:
                    break;
            }
            break;
        }

        case WriterState::CONTINUE_LIST:{
            switch(new_state) {
                case WriterState::START_LIST:
                case WriterState::CONTINUE_LIST:
                case WriterState::FINISHED:
                case WriterState::ERROR:
                    transition_okay = true;
                    break;
                default:
                    break;
            }
            break;
        }

        case WriterState::FINISHED:{
            switch(new_state) {
                case WriterState::ERROR:
                    transition_okay = true;
                    break;
                default:
                    break;
            }
            break;
        }

        case WriterState::ERROR:{
            std::cerr << "JsonWriter attempting to transition while in error state" << std::endl;
            return;
        }

        default: {
            std::cerr << "JsonWriter attempting to transition from an unknown state" << std::endl;
            break;
        }
    }

    if(transition_okay){
        //Transition(new_state);
        state_ = new_state;
    }else{
        std::cerr << "Illegal state change in JsonWriter" << std::endl;
    }
}

void JsonWriter::Begin() {
    BeginObject();
}

void JsonWriter::End() {
    EndObject();
    ChangeState(WriterState::FINISHED);
}

std::string JsonWriter::ToString() {
    if (state_ != WriterState::FINISHED) {
        std::cerr << "Warning: requested string for unfinished JSON" << std::endl;
    }
    return json_text_.str();
}

void JsonWriter::AddPair(std::string key, std::string val) {
    JsonNewline();
    
    json_text_ << Tab(GetIndentLevel()) << JsonPair(key, val);
}

void JsonWriter::BeginKeyedObject(std::string key){
    JsonNewline();
    json_text_ << Tab(GetIndentLevel()) << '"' << key << '"' << ':';
    BeginObject_();
}

void JsonWriter::BeginObject(){
    JsonNewline();
    json_text_ << Tab(GetIndentLevel());
    BeginObject_();
}

void JsonWriter::BeginObject_(){
    ChangeState(WriterState::START_LIST);
    json_text_ << "{";
    json_stack_.push(JsonValueType::OBJECT);        
    //current_indent++;
    ChangeState(WriterState::START_LIST);
}

void JsonWriter::EndObject() {
    json_text_ << std::endl;

    if (json_stack_.top() != JsonValueType::OBJECT) {
        ChangeState(WriterState::ERROR);
        std::cerr << "Attempting to end an object in the middle of an array!" << std::endl;
    }
    json_stack_.pop();
    ChangeState(WriterState::CONTINUE_LIST);
    //current_indent--;

    json_text_ << Tab(GetIndentLevel()) << '}';
}

void JsonWriter::BeginArray(std::string key) {
    JsonNewline();
    ChangeState(WriterState::START_LIST);
    

    json_text_ << Tab(GetIndentLevel()) << '"' << key << '"' << ": [";

    json_stack_.push(JsonValueType::ARRAY);
    //current_indent++;
    ChangeState(WriterState::START_LIST);
}

void JsonWriter::EndArray() {
    json_text_ << std::endl;

    if (json_stack_.top() != JsonValueType::ARRAY) {
        ChangeState(WriterState::ERROR);
        std::cerr << "Attempting to end an array in the middle of an object!" << std::endl;
    }

    json_stack_.pop();
    ChangeState(WriterState::CONTINUE_LIST);
    //current_indent--;

    json_text_ << Tab(GetIndentLevel()) << ']';
}


void JsonWriter::JsonNewline() {
    bool needs_nl = true;
    switch(state_){
        case WriterState::UNINITIALIZED:
            ChangeState(WriterState::START_LIST);
            needs_nl = false;
        case WriterState::START_LIST:
            ChangeState(WriterState::CONTINUE_LIST);
            break;
        default:
            json_text_ << ',';
    }
    
    if(needs_nl){
        json_text_ << std::endl;
    }
}

std::string JsonWriter::DblQuoteWrap(std::string val){
    std::string quote("\"");
    return quote + val + quote;
}

std::string JsonWriter::Tab(int i){
    return std::string(i, '\t');
}

std::string JsonWriter::JsonPair(std::string key, std::string val){
    return DblQuoteWrap(key) + ": " + DblQuoteWrap(val);
}

unsigned int JsonWriter::GetIndentLevel() {
    return json_stack_.size();
}