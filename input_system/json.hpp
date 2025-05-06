#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

//------------------- NODE -----------------------------------
class Node;
using Dict = std::unordered_map<std::string, Node>;
using Array = std::vector<Node>;
using Number = std::variant<int, double>;
class Builder;

class Node final {
public:
using Value = std::variant<std::nullptr_t, bool, int, double, std::string, Array, Dict>;
friend Builder;

    //	//--------------- container ------------------------------

    Node();
    Node(std::nullptr_t ptr);
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(std::string value);

    //	//--------------- check_data -----------------------------

    bool IsInt() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsDict() const;

    //	//--------------- get_data -------------------------------

    const Value& GetValue() const;
    int AsInt() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsDict() const;

    //--------------- compare --------------------------------

    bool operator==(const Node& rhs) const {
        return GetValue() == rhs.GetValue();
    }

    inline bool operator!=(Node other) const {
        return !(*this == other);
    }
private:
    Value data_;
};

//------------------ PRINT_CONTEXT ---------------------------

struct PrintContext {
    PrintContext(std::ostream& o, int in_step = 4, int in = 0)
        :out(o), indent_step(in_step), indent(in)
    {
    }

    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext Indented() const {
        return { out, indent_step, indent_step + indent };
    }
};



//------------------- DOCUMENT -------------------------------

class Document {
public:
    explicit Document(Node&& root);

    const Node& GetRoot() const;

    inline bool operator==(Document other) const {
        return root_ == other.root_;
    }

    inline bool operator!=(Document other) const {
        return !(*this == other);
    }

private:
    Node root_;
};

//------------------ LOAD_VALUE ------------------------------
Document Load(std::istream& input);
void Print(Document node, std::ostream& out);



namespace load {

    Node LNode(std::istream& input);
    nullptr_t LNullptr(std::istream& input);
    Number LNumber(std::istream& input);
    std::string LString(std::istream& input);
    Array LArray(std::istream& input);
    Dict LDict(std::istream& input);

    bool FindNextDivider(std::istream& input, char end_sequence);
}


//------------------- PRINT_VALUE ----------------------------
namespace print {
    using namespace std::literals;

    std::string String(const std::string& val);
    void PrintValue(const Node::Value& val, const PrintContext& ctx);
}
