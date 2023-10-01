#pragma once

#include <optional>

#include "json.h"

namespace json {

class Builder {
public:
    class DictKeyContext;
    class ArrayItemContext;
    class DictItemContext;
    
    Builder();
    DictKeyContext Key(const std::string& key);
    [[maybe_unused]] Node* AddObject(const Node& node);
    Builder& Value(const Node::Value& value);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
    json::Node Build();

private:
    Node root_{ nullptr };
    std::vector<Node*> nodes_stack_;
    std::string key_;
};
    
class Builder::DictKeyContext {
public:
    DictKeyContext(Builder& builder);

    DictItemContext Value(Node::Value value);
    ArrayItemContext StartArray();
    DictItemContext StartDict();

private:
    Builder& builder_;
};
    
class Builder::DictItemContext {
public:
    DictItemContext(Builder& builder);

    DictKeyContext Key(std::string key);
    Builder& EndDict();

private:
    Builder& builder_;
};

class Builder::ArrayItemContext {
public:
    ArrayItemContext(Builder& builder);

    ArrayItemContext Value(Node::Value value);
    DictItemContext StartDict();
    Builder& EndArray();
    ArrayItemContext StartArray();

private:
    Builder& builder_;
};

}