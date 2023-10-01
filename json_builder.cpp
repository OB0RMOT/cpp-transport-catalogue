#include "json_builder.h"

using namespace std::literals;

namespace json {

Builder::Builder()
{
    nodes_stack_.push_back(&root_);
}
Builder::DictKeyContext Builder::Key(const std::string& key)
{
    //auto top_node = nodes_stack_.back();
    //if(!top_node->IsDict()) { throw std::logic_error("Key is called out of the dictionary"s); }
    key_ = key;
    return *this;
}
[[maybe_unused]] Node* Builder::AddObject(const Node& node)
{
    auto top_node = nodes_stack_.back();
    Node* node_ptr = nullptr;
    if (top_node->IsNull())
    {
        *top_node = node;
    }
    else if (top_node->IsArray())
    {
        auto& arr = std::get<Array>(top_node->GetValue());
        arr.push_back(node);
        node_ptr = &arr.back();
    }
    else if (top_node->IsDict())
    {
        auto& dict = std::get<Dict>(top_node->GetValue());
        auto [pos, _] = dict.emplace(key_, node);
        node_ptr = &(pos->second);
    }
    else
    {
        throw std::logic_error("Unknown error"s);
    }
    return node_ptr;
}
Builder& Builder::Value(const Node::Value& value)
{
    Node node;
    node.GetValue() = value;
    AddObject(node);
    return *this;
}
Builder::DictItemContext Builder::StartDict()
{
    Node node;
    node.GetValue() = Dict();
    Node* node_ptr = AddObject(node);
    if(node_ptr) { nodes_stack_.push_back(node_ptr); }
    return *this;
}
Builder::ArrayItemContext Builder::StartArray()
{
    Node node;
    node.GetValue() = Array();
    Node* node_ptr = AddObject(node);
    if(node_ptr) { nodes_stack_.push_back(node_ptr); }
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