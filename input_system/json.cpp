#include "json.hpp"

#include <iterator>
#include <utility>
#include <iostream>

using namespace std;

//------------------- NODE -----------------------------------
//------------------- container ------------------------------

Node::Node()
	:data_(nullptr) 
{
}

Node::Node(nullptr_t ptr)
	:data_(ptr) 
{
}

Node::Node(int value)
	:data_(value) 
{
}

Node::Node(string value) {
	if (value == "null"s) 
    {
		data_ = nullptr;
	}
	else 
    {
		data_ = value;
	}
}

Node::Node(Array array)
	: data_(move(array)) 
{
}

Node::Node(Dict map)
	: data_(move(map))
{
}


//------------------- check_data -----------------------------

bool Node::IsInt() const 
{
	return holds_alternative<int>(data_);
}

bool Node::IsString() const 
{
	return holds_alternative<string>(data_);
}

bool Node::IsNull() const 
{
	return holds_alternative<nullptr_t>(data_);
}

bool Node::IsArray() const 
{
	return holds_alternative<Array>(data_);
}

bool Node::IsDict() const 
{
	return holds_alternative<Dict>(data_);
}


//------------------- get_data -------------------------------

const Node::Value& Node::GetValue() const 
{
	return data_;
}

int Node::AsInt() const 
{
	if (IsInt()) 
    {
		return get<int>(data_);
	}
	else 
    {
		throw logic_error("");
	}
}

const string& Node::AsString() const 
{
	if (IsString()) 
    {
		return get<string>(data_);
	}
	else 
    {
		throw logic_error("");
	}
}

const Array& Node::AsArray() const 
{
	if (IsArray()) 
    {
		return get<Array>(data_);
	}
	else 
    {
		throw logic_error("");
	}
}

const Dict& Node::AsDict() const 
{
	if (IsDict()) 
    {
		return get<Dict>(data_);
	}
	else 
    {
		throw logic_error("");
	}
}



//------------------- DOCUMENT -------------------------------
//------------------- container ------------------------------

Document::Document(Node&& root)
	: root_(move(root)) //
{
}

//------------------ get_data --------------------------------

const Node& Document::GetRoot() const 
{
	return root_;
}

void Print(Document node, ostream& out) 
{
	const PrintContext ctx = PrintContext(out);
	print::PrintValue(node.GetRoot().GetValue(), ctx);
}

//------------------- load_data ------------------------------

nullptr_t load::LNullptr(istream& input) 
{
	auto it = istreambuf_iterator<char>(input);
	auto end = istreambuf_iterator<char>();
	char c;
	c = *it;
	++it;
	string null = "";
	null += c;
	int i = 0;
	while (it != end && i < 3) 
    {
		c = *it;
		null += c;
		++i;
		++it;
	}

	if (null != "null"s) 
    {
		throw ParsingError("Null parsing error");
	}
	else 
    {
		input.putback(c);
		return nullptr;
	}
}

Number load::LNumber(istream& input) 
{
	using namespace literals;

	string parsed_num;

	auto read_char = [&parsed_num, &input] 
    {
		parsed_num += static_cast<char>(input.get());
		if (!input) 
        {
			throw ParsingError("Failed to read number from stream"s);
		}
	};

	auto read_digits = [&input, read_char] 
    {
		if (!isdigit(input.peek())) 
        {
			throw ParsingError("A digit is expected"s);
		}
		while (isdigit(input.peek())) 
        {
			read_char();
		}
	};

	if (input.peek() == '-') 
    {
		read_char();
	}
	else
	{
		read_digits();
	}

    try 
    {
        return stoi(parsed_num);
    }
	catch (...) 
    {
		throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
	}
}

string load::LString(istream& input) 
{
	using namespace literals;

	auto it = istreambuf_iterator<char>(input);
	auto end = istreambuf_iterator<char>();
	string s;
	while (true) 
    {
		if (it == end) 
        {
			throw ParsingError("String parsing error");
		}
		const char ch = *it;

		if (ch == '"') 
        {
			++it;
			break;
		}
		else if (ch == '\\') 
        {
			++it;
			if (it == end) 
            {
				throw ParsingError("String parsing error");
			}
			const char escaped_char = *(it);
			switch (escaped_char) 
            {
			case 'n':
				s.push_back('\n');
				break;
			case 't':
				s.push_back('\t');
				break;
			case 'r':
				s.push_back('\r');
				break;
			case '"':
				s.push_back('"');
				break;
			case '\\':
				s.push_back('\\');
				break;
			default:
				throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
			}
		}
		else if (ch == '\n' || ch == '\r') 
        {
			throw ParsingError("Unexpected end of line"s);
		}
		else 
        {
			s.push_back(ch);
		}
		++it;
	}

	return s;
}

bool load::FindNextDivider(istream& input, char end_sequence) 
{
	auto it = istreambuf_iterator<char>(input);
	auto end = istreambuf_iterator<char>();

	bool found_end = false;
	string occupy = "";
	while (*it != ',') {
		bool miss = false;

		if (*it == '[') 
        {
			while (*it != ']') 
            {
				if (it == end) 
                {
					throw ParsingError("Array parsing error"s);
				}
				occupy += *it;
				++it;
			}
			occupy += *it;
			++it;
			miss = true;
		}

		if (*it == '{') 
        {
			while (*it != '}') 
            {
				if (it == end) 
                {
					throw ParsingError("Dict parsing error"s);
				}
				occupy += *it;
				++it;
			}
			occupy += *it;
			++it;
			miss = true;
		}

		if (*it == end_sequence) 
        {
			found_end = true;
			break;
		}

		if (!miss) 
        {
			occupy += *it;
			++it;
		}
	}

	for (auto occ = occupy.rbegin(); occ != occupy.rend(); ++occ) 
    {
		input.putback(*occ);
	}
	return found_end;
}


Array load::LArray(istream& input) 
{
	vector<Node> result;

	for (char c; input >> c && c != ']';) 
    {
		if (c != ',') 
        {
			input.putback(c);
		}
		result.push_back(load::LNode(input));
	}
	if (!input) 
    {
		throw ParsingError("Array parsing error"s);
	}
	return result;
}

Dict load::LDict(istream& input) 
{
	Dict dict;

	for (char c; input >> c && c != '}';) 
    {
		if (c == '"') 
        {
			string key = load::LString(input);
			if (input >> c && c == ':') {
				if (dict.find(key) != dict.end()) 
                {
					throw ParsingError("Duplicate key '"s + key + "' have been found");
				}
				dict.emplace(move(key), load::LNode(input));
			}
			else 
            {
				throw ParsingError(": is expected but '"s + c + "' has been found"s);
			}
		}
		else if (c != ',') 
        {
			throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
		}
	}
	if (!input) 
    {
		throw ParsingError("Dictionary parsing error"s);
	}
	return dict;
}

Node load::LNode(istream& input) 
{
	char c;
	input >> c;

	switch (c) {

	case '[':
		return Node{ (LArray(input)) };

	case '{':
		return Node{ (LDict(input)) };

	case '"':
		return Node{ (LString(input)) };

	case 'n':
		input.putback(c);
		return Node{ (LNullptr(input)) };

	default:
		input.putback(c);
		auto numb = LNumber(input);
		if (holds_alternative<int>(numb)) 
        {
			return Node{ get<int>(numb) };
		}
		else 
        {
			return Node{ nullptr };
		}
	}
}

Document Load(istream& input) 
{
	return Document{ move(load::LNode(input)) };
}

//------------------ print_data ------------------------------

string print::String(const string& val) 
{
	using namespace literals;

	string s = "";
	for (auto c : val) 
    {
		if (static_cast<int>(c) <= 34 || static_cast<int>(c) == 92) 
        {
			switch (c) {
			case '\n':
				s += "\\n"s;
				break;
			case '\t':
				s += "\t"s;
				break;
			case '\r':
				s += "\\r"s;
				break;
			case '\"':
				s += "\\\""s;
				break;
			case '\\':
				s += "\\\\"s;
				break;
			default:
				s += c;
			}
		}
		else 
        {
			s += c;
		}
	}
	return s;
}

void print::PrintValue(const Node::Value& val, const PrintContext& ctx) 
{
	if (holds_alternative<nullptr_t>(val)) 
    {
		ctx.out << "null"sv;
	}
	else if (holds_alternative<int>(val)) 
    {
		ctx.out << get<int>(val);
	}
	else if (holds_alternative<string>(val)) 
    {
		string res = print::String(get<string>(val));
		ctx.out << "\""sv << res << "\""sv;
	}
	else if (holds_alternative<Array>(val)) 
    {
		bool is_not_first = false;
		ctx.out << "["sv << endl;
		const PrintContext new_pt = ctx.Indented();
		auto vec = get<Array>(val);

		for (const auto& v : vec) 
        {
			if (is_not_first) 
            {
				ctx.out << ","sv << endl;
			}
			new_pt.PrintIndent();
			is_not_first = true;
			visit(
				[&ctx, &new_pt](const Node::Value& v) { PrintValue(v, new_pt); },
				v.GetValue());
		}
		ctx.out << endl;
		ctx.PrintIndent();
		ctx.out << "]"sv;
	}
	else if (holds_alternative<Dict>(val)) 
    {
		bool is_not_first = false;

		ctx.out << "{"sv << endl;
		const PrintContext new_pt = ctx.Indented();

		for (const auto& [key, v] : get<Dict>(val)) 
        {

			if (is_not_first) {
				ctx.out << ","sv << endl;
			}
			new_pt.PrintIndent();
			is_not_first = true;
			ctx.out << "\""sv << key << "\": "sv;
			visit(
				[&ctx, &new_pt](const Node::Value& v) { PrintValue(v, new_pt); },
				v.GetValue());
		}
		ctx.out << endl;
		ctx.PrintIndent();
		ctx.out << "}"sv;
	}
}
