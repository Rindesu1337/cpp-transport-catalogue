#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);
// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
Node LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
                // Поток закончился до того, как встретили закрывающую кавычку?
                throw ParsingError("String parsing error");
                }
            const char ch = *it;
            if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
            } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
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
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
            } else if (ch == '\n' || ch == '\r') {
                // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node(s);
}

string LoadTokens (istream& input) {
    string result ;
    
    while (isalpha(input.peek())) {
        result.push_back(static_cast<char>(input.get()));
    }

    return result;
}

Node LoadBool(istream& input) {
    string str = LoadTokens(input);

    if (str == "true"s) {
        return Node(true);
    } else if (str == "false"s) {
        return Node(false);
    } else {
        throw ParsingError("not current bool"s);
    }
}

Node LoadNull(istream& input) {
    string str = LoadTokens(input);

    if (str == "null"s) {
        return Node();
    } else {
        throw ParsingError("not current null"s);
    }
}

Node LoadArray(istream& input) {
    Array result;

    for (char c; input >> c and c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    if (!input) {
        throw ParsingError("no array"s);
    }

    return Node(move(result));
}

Node LoadDict(istream& input) {
    Dict result;

    for (char c; input >> c and c != '}';) {
        if (c == '"') {
            string key = LoadString(input).AsString();

            if (input >> c and c == ':') {
                if (result.find(key) != result.end()) {
                    throw ParsingError("we got a key"s);
                }

                result.insert({move(key), LoadNode(input)});
            } else {
                throw ParsingError("dont find :"s);
            }
        } else if (c != ',') {
            throw ParsingError("dont find ,");
        }
    }

    if (!input) {
        throw ParsingError("no dict"s);
    }

    return Node(move(result));
} 

Node LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node(std::stoi(parsed_num));
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node(std::stod(parsed_num));
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadNode(istream& input) {
    char c;

    if (!(input >> c)){
        throw ParsingError("Bad input"s);
    }

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == 't' or c == 'f'){
        input.putback(c);
        return LoadBool(input);
    } else if (c == 'n') {
        input.putback(c);
        return LoadNull(input);
    } else {
        input.putback(c);
        return LoadNumber(input);
    }
}

}  // namespace


Node::Node(std::nullptr_t) : Node() {}
Node::Node(bool value) : value_(value) {}
Node::Node(int value) : value_(value) {}
Node::Node(double value) : value_(value) {}
Node::Node(std::string value) : value_(move(value)) {}
Node::Node(Dict dic) : value_(move(dic)) {}
Node::Node(Array arr) : value_(move(arr)) {}

bool Node::IsInt() const{
    return std::holds_alternative<int>(value_);
}
bool Node::IsDouble() const{
    return std::holds_alternative<int>(value_) or std::holds_alternative<double>(value_);
}
bool Node::IsPureDouble() const{
    return std::holds_alternative<double>(value_);
}
bool Node::IsBool() const{
    return std::holds_alternative<bool>(value_);
}
bool Node::IsString() const{
    return std::holds_alternative<std::string>(value_);
}
bool Node::IsNull() const{
    return std::holds_alternative<std::nullptr_t>(value_);
}
bool Node::IsArray() const{
    return std::holds_alternative<Array>(value_);
}
bool Node::IsMap() const {
    return std::holds_alternative<Dict>(value_);
}

int Node::AsInt() const{
    if (!IsInt()) {
        throw std::logic_error("Not a type"s);
    }

    return std::get<int>(value_);
}
bool Node::AsBool() const{
    if (!IsBool()) {
        throw std::logic_error("Not a type"s);
    }

    return std::get<bool>(value_);
}
double Node::AsDouble() const{
    if (!IsDouble()) {
        throw std::logic_error("Not a type"s);
    }

    return IsPureDouble() ? std::get<double>(value_) : AsInt();
}
const std::string& Node::AsString() const{
    if (!IsString()) {
        throw std::logic_error("Not a type"s);
    }

    return std::get<string>(value_);
}
const Array& Node::AsArray() const{
    if (!IsArray()) {
        throw std::logic_error("Not a type"s);
    }

    return std::get<Array>(value_);
}
const Dict& Node::AsMap() const{
    if (!IsMap()) {
        throw std::logic_error("Not a type"s);
    }

    return std::get<Dict>(value_);
}

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.GetValue() == rhs.GetValue();
}

bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}
bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

/* // Шаблон, подходящий для вывода double и int
template <typename Value>
void PrintValue(const Value& value, std::ostream& out) {
    out << value;
} */

// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};

void PrintNode(const Node& node, const PrintContext& ctx);

void PrintStr(const string& str, ostream& out) {
    out << '"' ;

    for (auto& ch : str) {
        if (ch == '\r') {
            out << "\\r";
        } else if (ch == '\n') {
            out << "\\n";
        } else if (ch == '\t'){
            out << "\\t";
        } else if (ch == '"') {
            out << "\\\"";
        } else if (ch == '\\') {
            out << "\\\\";
        } else {
            out << ch;
        }
    }

    out << '"' ;
}

template <typename Value>
void PrintValue(const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}

template<>
void PrintValue<string> (const string& str, const PrintContext& ctx) {
    PrintStr(str, ctx.out);
}

template<>
void PrintValue<Array> (const Array& arr, const PrintContext& ctx) {
    ctx.out << "[\n"sv;

    auto inner_ctx = ctx.Indented();
    bool is_first = true;
    for (const auto& elem : arr) {
        if (is_first) {
            is_first = false;
        } else {
            ctx.out << ",\n"sv;
        }

        inner_ctx.PrintIndent();
        PrintNode(elem,inner_ctx);
    }

    ctx.out.put('\n');
    ctx.PrintIndent();
    ctx.out.put(']');
}

template<>
void PrintValue<Dict> (const Dict& dic, const PrintContext& ctx) {
    ctx.out << "{\n"sv;

    bool is_first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto& [key, val] : dic) {
        if (is_first) {
            is_first = false;
        } else {
            ctx.out << ",\n"sv;
        }

        inner_ctx.PrintIndent();
        PrintStr(key,ctx.out);
        ctx.out << ": "sv;
        PrintNode(val,inner_ctx);
    }

    ctx.out.put('\n');
    ctx.PrintIndent();
    ctx.out.put('}');
}


void PrintValue(const bool& b, const PrintContext& ctx) {
    ctx.out << boolalpha << b;
}

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, const PrintContext& ctx) {
    ctx.out << "null"sv;
}
// Другие перегрузки функции PrintValue пишутся аналогично

void PrintNode(const Node& node, const PrintContext& ctx) {
    std::visit(
        [&ctx](const auto& value){ PrintValue(value, ctx); },
        node.GetValue());
}

void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), PrintContext{output});
}

}  // namespace json