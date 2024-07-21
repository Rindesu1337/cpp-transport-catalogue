#include "json_builder.h"

namespace json {

    Node Builder::PushNode(const Node::Value& value) {
        auto lam = [&] (auto& value_) {
            Node elem(value_);
            return elem;
        };
        return std::visit(lam, value);
    }

    Builder& Builder::Value(const Node::Value& value, bool is_new) {
        if (is_empty_) {
            is_empty_ = false;
            root_.GetValue() = value;
            if (is_new) {
                nodes_stack_.push_back(&root_);
            }

            return *this;
        }

        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsNull()){
                nodes_stack_.back()->GetValue() = value;
                if (!is_new) {
                    nodes_stack_.pop_back();
                }

                return *this;
            }

            if (nodes_stack_.back()->IsArray()){
                auto& arr = const_cast<Array&>(nodes_stack_.back()->AsArray());
                arr.push_back(PushNode(value));

                if (is_new) {
                    nodes_stack_.push_back(&arr.back());
                }

                return *this;
            }
        }

        throw std::logic_error("no Value()"s);
    }

    DictItemContext Builder::StartDict() {
        Value(Dict{}, true);
        return *this;
    }

    DictValueContext Builder::Key(const std::string& key) {
        if (!nodes_stack_.empty() and !is_empty_ and nodes_stack_.back()->IsMap() ) {
            nodes_stack_.push_back(&std::get<Dict>(nodes_stack_.back()->GetValue())[key]);
            return *this;
        }

        throw std::logic_error("not found StartDict()"s);
    }
 
    ArrayItemContext Builder::StartArray() {
        Value(Array{}, true);
        return *this;
    }

    Builder& Builder::EndDict() {
        if (!is_empty_ and !nodes_stack_.empty() and nodes_stack_.back()->IsMap()) {
            nodes_stack_.pop_back(); 
            return *this;
        }
        
        throw std::logic_error("not found dict"s);
    }

    Builder& Builder::EndArray() {
        if (!is_empty_ and !nodes_stack_.empty() and nodes_stack_.back()->IsArray()) {
            nodes_stack_.pop_back();
            return *this;
        }
        
        throw std::logic_error("not found array"s);
    }

    Node Builder::Build() {
        if (!is_empty_ and nodes_stack_.empty()) {
            return root_;
        }
        throw std::logic_error("not found value"s);
    }

    DictValueContext BaseContext::Key(const std::string& key) {
        return builder_.Key(key);
    }

    /* Builder& BaseContext::Value(const Node::Value& value) {
        return builder_.Value(value);
    } */

    DictItemContext BaseContext::StartDict() {
        return builder_.StartDict();
    }

    ArrayItemContext BaseContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& BaseContext::EndDict() {
        return builder_.EndDict();
    }

    Builder& BaseContext::EndArray() {
        return builder_.EndArray();
    }

    /* Node BaseContext::Build() {
        return builder_.Build();
    } */

    DictItemContext DictValueContext::StartDict() {
        return builder_.StartDict();
    }

    ArrayItemContext DictValueContext::StartArray() {
        return builder_.StartArray();
    }

    DictItemContext DictValueContext::Value(const Node::Value& value) {
        return builder_.Value(value);
    }

    DictValueContext DictItemContext::Key(const std::string& key) {
        return builder_.Key(key);
    }

    Builder& DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    ArrayItemContext ArrayItemContext::Value(const Node::Value& value) {
        return builder_.Value(value);
    }

    Builder& ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

    ArrayItemContext ArrayItemContext::StartArray(){
        return builder_.StartArray();
    }

    DictItemContext ArrayItemContext::StartDict() {
        return builder_.StartDict();
    }

}