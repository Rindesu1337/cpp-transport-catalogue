#pragma once

#include "json.h"


namespace json {
    using namespace std::literals;

class DictValueContext;
class DictItemContext;
class ArrayItemContext;

class Builder {
public: 
    Builder() = default;

    DictValueContext Key(const std::string& key) ;

    Builder& Value(const Node::Value& value, bool = false) ;

    DictItemContext StartDict();

    ArrayItemContext StartArray();

    Builder& EndDict();

    Builder& EndArray();

    Node Build();

private:
    Node PushNode(const Node::Value& value);

    Node root_;
    std::vector<Node*> nodes_stack_;
    bool is_empty_ = true;
};  

class BaseContext {
public:
    BaseContext(Builder& builder)
    : builder_(builder){}

    DictValueContext Key(const std::string& key) ;

    DictItemContext StartDict();

    ArrayItemContext StartArray();

    Builder& EndDict();

    Builder& EndArray();

protected:
    Builder& builder_;
};

class DictItemContext : public BaseContext {
public:
    using BaseContext::BaseContext;
    
    DictValueContext Key(const std::string& key);

    Builder& EndDict();

    Builder& EndArray() = delete;

    DictItemContext StartDict() = delete;

    ArrayItemContext StartArray() = delete;

};

class DictValueContext : public BaseContext {
public:
    using BaseContext::BaseContext;
    
    DictItemContext StartDict();

    ArrayItemContext StartArray();

    DictItemContext Value(const Node::Value& value);

    DictValueContext Key(const std::string& key) = delete;

    Builder& EndDict() = delete;

    Builder& EndArray() = delete;
};

class ArrayItemContext : public BaseContext {
public:
    using BaseContext::BaseContext;
    
    DictItemContext StartDict();

    ArrayItemContext StartArray();

    Builder& EndArray();

    ArrayItemContext Value(const Node::Value& value);

    DictValueContext Key(const std::string& key) = delete;

    Builder& EndDict() = delete;
};


}