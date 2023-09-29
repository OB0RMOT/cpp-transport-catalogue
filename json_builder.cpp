#include "json_builder.h"

using namespace std::literals;

namespace json {

Builder::Builder()
{
    nodes_stack_.push_back(&root_);
}
Builder::DictKeyContext Builder::Key(const std::string& key)
{
    auto top_node = nodes_stack_.back();
    if(!top_node->IsDict()) { throw std::logic_error("Key is called out of the dictionary"s); }
    else if(key_) { throw std::logic_error("Previous Key was called but not used"s); }
    key_ = key;
    return *this;
}
Builder& Builder::Value(const Node::Value& value)
{
    auto top_node = nodes_stack_.back();
    if (top_node->IsNull())
    {
        top_node->GetValue() = value;
    }
    else if (top_node->IsArray())
    {
        auto& arr = std::get<Array>(top_node->GetValue());
        arr.push_back(Node{});
        arr.back().GetValue() = value;
    }
    else if (top_node->IsDict())
    {
        if (!key_) { throw std::logic_error("No key to add a value to the dictionary"s); }
        auto& dict = std::get<Dict>(top_node->GetValue());
        auto [pos, _] = dict.emplace(key_.value(), Node{});
        pos->second.GetValue() = value;
        key_ = std::nullopt;
    }
    else
    {
        throw std::logic_error("Unknown error"s);
    }
    return *this;
}
Builder::DictItemContext Builder::StartDict()
{
    auto top_node = nodes_stack_.back();
    if (top_node->IsNull())
    {
        top_node->GetValue() = Dict();
    }
    else if (top_node->IsArray())
    {
        auto& arr = std::get<Array>(top_node->GetValue());
        arr.push_back(Dict());
        nodes_stack_.push_back(&arr.back());
    }
    else if (top_node->IsDict())
    {
        if (!key_) { throw std::logic_error("No key to add a new dictionary to the dictionary"s); }
        auto& dict = std::get<Dict>(root_.GetValue());
        auto [pos, _] = dict.emplace(key_.value(), Node(Dict()));
        nodes_stack_.push_back(&(pos->second));
        key_ = std::nullopt;
    }
    else
    {
        throw std::logic_error("Unknown error"s);
    }
    return *this;
}
Builder::ArrayItemContext Builder::StartArray()
{
    auto top_node = nodes_stack_.back();
    if (top_node->IsNull())
    {
        top_node->GetValue() = Array();
    }
    else if (top_node->IsArray())
    {
        auto& arr = std::get<Array>(top_node->GetValue());
        arr.push_back(Array());
        nodes_stack_.push_back(&arr.back());
    }
    else if (top_node->IsDict())
    {
        if (!key_) { throw std::logic_error("No key to add a new dictionary to the dictionary"s); }
        auto& dict = std::get<Dict>(root_.GetValue());
        auto [pos, _] = dict.emplace(key_.value(), Node(Array()));
        nodes_stack_.push_back(&(pos->second));
        key_ = std::nullopt;
    }
    else
    {
        throw std::logic_error("Unknown error"s);
    }
    return *this;
}
Builder& Builder::EndDict()
{
    auto top_node = nodes_stack_.back();
    if (top_node->IsDict()) { nodes_stack_.pop_back(); }
    else { throw std::logic_error("Last incomplete call Start is not a StartDict"s); }
    return *this;
}
Builder& Builder::EndArray()
{
    auto top_node = nodes_stack_.back();
    if (top_node->IsArray()) { nodes_stack_.pop_back(); }
    else { throw std::logic_error("Last incomplete call Start is not a StartArray"s); }
    return *this;
}
json::Node Builder::Build()
{
    if (nodes_stack_.size() > 1 && (nodes_stack_.back()->IsDict() || nodes_stack_.back()->IsArray()))
    { throw std::logic_error("End was not called for every Start"s); }
    else if (root_.IsNull()) { throw std::logic_error("Object is not defined"s); }
    else if (key_) { throw std::logic_error("Key was called but not used"s); }
    return root_;
}
    
Builder::DictKeyContext::DictKeyContext(Builder& builder)
    : builder_(builder)
{}

Builder::DictItemContext Builder::DictKeyContext::Value(Node::Value value) {
    return DictItemContext(builder_.Value(value));
}

Builder::ArrayItemContext Builder::DictKeyContext::StartArray() {
    return builder_.StartArray();
}

Builder::DictItemContext Builder::DictKeyContext::StartDict() {
    return builder_.StartDict();
}

Builder::DictItemContext::DictItemContext(Builder& builder)
    : builder_(builder)
{}

Builder::DictKeyContext Builder::DictItemContext::Key(std::string key) {
    return builder_.Key(key);
}

Builder& Builder::DictItemContext::EndDict() {
    return builder_.EndDict();
}

Builder::ArrayItemContext::ArrayItemContext(Builder& builder)
    : builder_(builder)
{}

Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node::Value value) {
    return ArrayItemContext(builder_.Value(value));
}

Builder::DictItemContext Builder::ArrayItemContext::StartDict() {
    return builder_.StartDict();
}

Builder::ArrayItemContext Builder::ArrayItemContext::StartArray() {
    return builder_.StartArray();
}

Builder& Builder::ArrayItemContext::EndArray() {
    return builder_.EndArray();
}
    
}