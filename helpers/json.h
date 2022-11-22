#pragma once 

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "helpers.h"

namespace json {

    class Value;

    std::ostream & operator << (std::ostream &, Value const &);

    /** The undefined value placeholder. 
     
        Does not contain any useful information apart from the optinal comment, exists for unified creation of values via constructors. 
     */
    class Undefined {
    public:
        Undefined() = default;
        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }
    private:

        friend inline std::ostream & operator << (std::ostream & s, Undefined const & json) {
            UNUSED(json);
            s << "undefined";
            return s;
        }

        std::string comment_;
    }; // json::Undefined

    /** The Null value placeholder. 

        Does not contain any useful information apart from the optinal comment, exists for unified creation of values via constructors. 
     */
    class Null {
    public:
        Null() = default;
        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }
    private:
        friend inline std::ostream & operator << (std::ostream & s, Null const & json) {
            UNUSED(json);
            s << "Null";
            return s;
        }

        std::string comment_;
    }; // json::Null

    /** Boolean JSON value. 
     */
    class Bool {
    public:
        explicit Bool(bool value):value_{value} {}

        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }

        operator bool () const { return value_; } 

        bool operator == (Bool const & other) const { return value_ == other.value_; }
        bool operator != (Bool const & other) const { return value_ == other.value_; }

    private:
        friend inline std::ostream & operator << (std::ostream & s, Bool const & json) {
            s << (json.value_ ? "true" : "false");
            return s;
        }

        bool value_;
        std::string comment_;

    }; // json::Boolean

    /** Integer JSON value. 
     
        Contrary to JSON specification numbers are stored as either double, or boolean values. 
     */
    class Int {
    public:
        explicit Int(int value):value_{value} {}

        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }

        operator int () const { return value_; }

        bool operator == (Int const & other) const { return value_ == other.value_; }
        bool operator != (Int const & other) const { return value_ == other.value_; }

    private:
        friend inline std::ostream & operator << (std::ostream & s, Int const & json) {
            s << json.value_;
            return s;
        }

        int value_;
        std::string comment_;

    }; // json::Integer

    /** Double JSON value. 
     
        Contrary to JSON specification numbers are stored as either double, or boolean values. 
     */
    class Double {
    public:
        explicit Double(double value):value_{value} {}

        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }

        operator double () const { return value_; }

        bool operator == (Double const & other) const { return value_ == other.value_; }
        bool operator != (Double const & other) const { return value_ == other.value_; }

    private:

        friend inline std::ostream & operator << (std::ostream & s, Double const & json) {
            s << json.value_;
            return s;
        }

        double value_;
        std::string comment_;

    }; // json::Double

    /** String JSON value. 
     */
    class String {
    public:
        explicit String(std::string_view value):value_{value} {}
        explicit String(char const * value):value_{value} {}
        explicit String(std::string && value):value_{std::move(value)} {}

        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }

        size_t size() const { return value_.size(); }
        char const * c_str() const { return value_.c_str(); }

        bool operator == (String const & other) const { return value_ == other.value_; }
        bool operator != (String const & other) const { return value_ == other.value_; }

    private:
        friend inline std::ostream & operator << (std::ostream & s, String const & json) {
            // TODO quote the string if necessary
            s << '"' << json.value_ << '"';
            return s;
        }

        std::string value_;
        std::string comment_;

    }; // json::String

    /** JSON Array. 
     */
    class Array {
    public:

        ~Array();

        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }

        Value const & operator [] (size_t i) const { return *elements_[i]; }
        Value & operator [] (size_t i) { return *elements_[i]; }

        void add(Value const & value);
        void add(Value && value);

        bool operator == (Array const & other) const {
            if (elements_.size() != other.elements_.size())
                return false;
            for (size_t i = 0, e = elements_.size(); i < e; ++i)
                if (elements_[i] != other.elements_[i])
                    return false;
            return true;
        }

        bool operator != (Array const & other) const { 
            if (elements_.size() != other.elements_.size())
                return true;
            for (size_t i = 0, e = elements_.size(); i < e; ++i)
                if (elements_[i] != other.elements_[i])
                    return true;
            return false;
        }

    private:

        friend inline std::ostream & operator << (std::ostream & s, Array const & json) {
            s << "[";
            auto i = json.elements_.begin(), e = json.elements_.end();
            while (i != e) {
                s << **i;
                if (++i != e)
                    s << ", ";
            }
            s << "]";
            return s;
        }

        std::vector<Value*> elements_; // have to use ptrs (incomplete type)
        std::string comment_;

    }; // json::Array

    /** JSON Struct.
     */
    class Struct {
    public:

        ~Struct();

        std::string const & comment() const { return comment_; }
        void setComment(std::string_view comment) { comment_ = comment; }

        Value const & operator [] (size_t i) const { return *(elements_[i].second); }
        Value & operator [] (size_t i) { return *(elements_[i].second); }


        Value const & operator [] (std::string const & i) const;
        Value & operator [] (std::string const & i);

        void set(std::string const & name, Value const & value);
        void set(std::string const & name, Value && value);

        bool operator == (Struct const & other) const {
            if (elements_.size() != other.elements_.size())
                return false;
            for (size_t i = 0, e = elements_.size(); i < e; ++i)
                if (elements_[i] != other.elements_[i])
                    return false;
            return true;
        }

        bool operator != (Struct const & other) const { 
            if (elements_.size() != other.elements_.size())
                return true;
            for (size_t i = 0, e = elements_.size(); i < e; ++i)
                if (elements_[i] != other.elements_[i])
                    return true;
            return false;
        }

    private:

        friend inline std::ostream & operator << (std::ostream & s, Struct const & json) {
            s << "{";
            auto i = json.elements_.begin(), e = json.elements_.end();
            while (i != e) {
                s << '"' << i->first << "\" : " << *(i->second);
                if (++i != e)
                    s << ", ";
            }
            s << "}";
            return s;
        }

        std::vector<std::pair<std::string, Value*>> elements_; // have to use ptrs (incomplete type)
        std::unordered_map<std::string, size_t> elementsByName_; 
        std::string comment_;
    }; // json::Struct

    /** A Generic JSON value class. 
     
        Provides a tagged union of all JSON values that can be easily manipulated providing some basic conversions as well. 
     */
    class Value {
    public:
        enum class Kind {
            Undefined, 
            Null, 
            Bool, 
            Int,
            Double,
            String,
            Array,
            Struct
        }; // json::Value::Kind

        Value(): kind_{Kind::Undefined}, valueUndefined_{} {}

        Value(Undefined value): kind_{Kind::Undefined}, valueUndefined_{} { UNUSED(value); }
        Value(Null value): kind_{Kind::Null}, valueNull_{} { UNUSED(value); }

        Value(bool value): kind_{Kind::Bool}, valueBool_{value} {}
        Value(Bool value): kind_{Kind::Bool}, valueBool_{value} {}

        Value(int value): kind_{Kind::Int}, valueInt_{value} {}
        Value(Int value): kind_{Kind::Int}, valueInt_{value} {}

        Value(double value): kind_{Kind::Double}, valueDouble_{value} {}
        Value(Double value): kind_{Kind::Double}, valueDouble_{value} {}

        Value(std::string_view value): kind_{Kind::String}, valueString_{value} {}
        Value(std::string && value): kind_{Kind::String}, valueString_{std::move(value)} {}
        Value(char const * value): kind_{Kind::String}, valueString_{value} {}
        Value(String const & value): kind_{Kind::String}, valueString_{value} {}
        Value(String && value): kind_{Kind::String}, valueString_{std::move(value)} {}

        Value(Array const & value): kind_{Kind::Array}, valueArray_{value} {}
        Value(Array && value): kind_{Kind::Array}, valueArray_{std::move(value)} {}

        Value(Struct const & value): kind_{Kind::Struct}, valueStruct_{value} {}
        Value(Struct && value): kind_{Kind::Struct}, valueStruct_{std::move(value)} {}

        Value(Value const & from):
            kind_{from.kind_},
            valueUndefined_{} {
            switch (kind_) {
                case Kind::Undefined:
                    break;
                case Kind::Null:
                    new (&valueNull_) Null{};
                    break;
                case Kind::Bool:
                    new (&valueBool_) Bool{from.valueBool_};
                    break;
                case Kind::Int:
                    new (&valueInt_) Int{from.valueInt_};
                    break;
                case Kind::Double:
                    new (&valueDouble_) Double{from.valueDouble_};
                    break;
                case Kind::String:
                    new (&valueString_) String{from.valueString_};
                    break;
                case Kind::Array:
                    new (&valueArray_) Array{from.valueArray_};
                    break;
                case Kind::Struct:
                    new (&valueStruct_) Struct{from.valueStruct_};
            }
        }

        Value(Value && from):
            kind_{from.kind_},
            valueUndefined_{} {
            switch (kind_) {
                case Kind::Undefined:
                    break;
                case Kind::Null:
                    new (&valueNull_) Null{};
                    break;
                case Kind::Bool:
                    new (&valueBool_) Bool{std::move(from.valueBool_)};
                    break;
                case Kind::Int:
                    new (&valueInt_) Int{std::move(from.valueInt_)};
                    break;
                case Kind::Double:
                    new (&valueDouble_) Double{std::move(from.valueDouble_)};
                    break;
                case Kind::String:
                    new (&valueString_) String{std::move(from.valueString_)};
                    break;
                case Kind::Array:
                    new (&valueArray_) Array{std::move(from.valueArray_)};
                    break;
                case Kind::Struct:
                    new (&valueStruct_) Struct{std::move(from.valueStruct_)};
            }
        }

        ~Value() {
            detach();
        }

        std::string const & comment() const {
            switch (kind_) {
                case Kind::Undefined:
                    return valueUndefined_.comment();
                case Kind::Null:
                    return valueNull_.comment();
                case Kind::Bool:
                    return valueBool_.comment();
                case Kind::Int:
                    return valueInt_.comment();
                case Kind::Double:
                    return valueDouble_.comment();
                case Kind::String:
                    return valueString_.comment();
                case Kind::Array:
                    return valueArray_.comment();
                case Kind::Struct:
                    return valueStruct_.comment();
            }
        }

        void setComment(std::string_view comment) {
            switch (kind_) {
                case Kind::Undefined:
                    return valueUndefined_.setComment(comment);
                case Kind::Null:
                    return valueNull_.setComment(comment);
                case Kind::Bool:
                    return valueBool_.setComment(comment);
                case Kind::Int:
                    return valueInt_.setComment(comment);
                case Kind::Double:
                    return valueDouble_.setComment(comment);
                case Kind::String:
                    return valueString_.setComment(comment);
                case Kind::Array:
                    return valueArray_.setComment(comment);
                case Kind::Struct:
                    return valueStruct_.setComment(comment);
            }
        }

        template<typename T> T const & as() const;
        template<typename T> T & as();

        Value & operator = (Value const & other) {
            detach();
            kind_ = other.kind_;
            switch (kind_) {
                case Kind::Undefined:
                    new (&valueUndefined_) Undefined{};
                    break;
                case Kind::Null:
                    new (&valueNull_) Null{};
                    break;
                case Kind::Bool:
                    new (&valueBool_) Bool(other.valueBool_);
                    break;
                case Kind::Int:
                    new (&valueInt_) Int{other.valueInt_};
                    break;
                case Kind::Double:
                    new (&valueDouble_) Double(other.valueDouble_);
                    break;
                case Kind::String:
                    new (&valueString_) String(other.valueString_);
                    break;
                case Kind::Array:
                    new (&valueArray_) Array(other.valueArray_);
                    break;
                case Kind::Struct:
                    new (&valueStruct_) Struct{other.valueStruct_};
                    break;
            }
            return *this;
        }

        Value & operator = (Value const && other) {
            detach();
            kind_ = other.kind_;
            switch (kind_) {
                case Kind::Undefined:
                    new (&valueUndefined_) Undefined{};
                    break;
                case Kind::Null:
                    new (&valueNull_) Null{};
                    break;
                case Kind::Bool:
                    new (&valueBool_) Bool{std::move(other.valueBool_)};
                    break;
                case Kind::Int:
                    new (&valueInt_) Int{std::move(other.valueInt_)};
                    break;
                case Kind::Double:
                    new (&valueDouble_) Double{std::move(other.valueDouble_)};
                    break;
                case Kind::String:
                    new (&valueString_) String{std::move(other.valueString_)};
                    break;
                case Kind::Array:
                    new (&valueArray_) Array{std::move(other.valueArray_)};
                    break;
                case Kind::Struct:
                    new (&valueStruct_) Struct{std::move(other.valueStruct_)};
            }
            return *this;
        }

        bool operator == (Value const & other) const {
            if (kind_ != other.kind_)
                return false;
            switch (kind_) {
                case Kind::Undefined:
                case Kind::Null:
                    return true;
                case Kind::Bool:
                    return valueBool_ == other.valueBool_;
                case Kind::Int:
                    return valueInt_ == other.valueInt_;
                case Kind::Double:
                    return valueDouble_ == other.valueDouble_;
                case Kind::String:
                    return valueString_ == other.valueString_;
                case Kind::Array:
                    return valueArray_ == other.valueArray_;
                case Kind::Struct:
                    return valueStruct_ == other.valueStruct_;
            }
        }

        friend bool operator == (Undefined const & a, Value const & b) { return b == a; }
        friend bool operator == (Null const & a, Value const & b) { return b == a; }
        friend bool operator == (Int const & a, Value const & b) { return b == a; }
        friend bool operator == (Double const & a, Value const & b) { return b == a; }
        friend bool operator == (String const & a, Value const & b) { return b == a; }
        friend bool operator == (Array const & a, Value const & b) { return b == a; }
        friend bool operator == (Struct const & a, Value const & b) { return b == a; }

    private:

        friend inline std::ostream & operator << (std::ostream & s, Value const & json) {
            switch (json.kind_) {
                case Kind::Undefined:
                    s << json.valueUndefined_;
                    break;
                case Kind::Null:
                    s << json.valueNull_;
                    break;
                case Kind::Bool:
                    s << json.valueBool_;
                    break;
                case Kind::Int:
                    s << json.valueInt_;
                    break;
                case Kind::Double:
                    s << json.valueDouble_;
                    break;
                case Kind::String:
                    s << json.valueString_;
                    break;
                case Kind::Array:
                    s << json.valueArray_;
                    break;
                case Kind::Struct:
                    s << json.valueStruct_;
                    break;
            }
            return s;
        }

        void detach() {
            switch (kind_) {
                case Kind::String:
                    valueString_.~String();
                    break;
                case Kind::Array:
                    valueArray_.~Array();
                    break;
                case Kind::Struct:
                    valueStruct_.~Struct();
                    break;
                default:
                    break; // trivial destructors
            }
        }

        Kind kind_;
        union {
            Undefined valueUndefined_;
            Null valueNull_;
            Bool valueBool_;
            Int valueInt_;
            Double valueDouble_;
            String valueString_;
            Array valueArray_;
            Struct valueStruct_;
        };

        friend Value parse(std::istream &);

        class Parser;

    }; // json::Value

    /** The undefined singleton that is returned by any unsupported operation. 
     */
    inline const Value undefined = Value{Undefined{}};

    template<> 
    inline Bool const & Value::as() const {
        if (kind_ != Kind::Bool)
            throw "Expected bool but found";
        return valueBool_;
    }

    template<> 
    inline Bool & Value::as() {
        if (kind_ != Kind::Bool)
            throw "Expected bool but found";
        return valueBool_;
    }

    template<> 
    inline Int const & Value::as() const {
        if (kind_ != Kind::Int)
            throw "Expected bool but found";
        return valueInt_;
    }

    template<> 
    inline Int & Value::as() {
        if (kind_ != Kind::Int)
            throw "Expected bool but found";
        return valueInt_;
    }

    inline Array::~Array() {
        for (auto e : elements_) 
            delete e;
    }

    inline void Array::add(Value const & value) {
        elements_.push_back(new Value{value});
    }

    inline void Array::add(Value && value) {
        elements_.push_back(new Value{std::move(value)});
    }

    inline Struct::~Struct() {
        for (auto e : elements_) 
            delete e.second;
    }


    inline Value const & Struct::operator [] (std::string const & i) const {
        auto it = elementsByName_.find(i);
        if (it == elementsByName_.end())
            return json::undefined;
        else
            return *elements_[it->second].second;
    }

    inline Value & Struct::operator [] (std::string const & i) {
        auto it = elementsByName_.find(i);
        if (it == elementsByName_.end()) {
            it = elementsByName_.insert(std::make_pair(i, elementsByName_.size())).first;
            elements_.push_back(std::make_pair(i, new Value{Undefined{}}));
        }
        return *elements_[it->second].second;
    }

    inline void Struct::set(std::string const & name, Value const & value) { (*this)[name] = value; }
    inline void Struct::set(std::string const & name, Value && value) { (*this)[name] = std::move(value); }


    /** Parses the given stream and returns the JSON object. 
     */
    inline Value parse(std::istream & s) {
        return parse(s);
    }

    /** Parses the given string and returns the JSON object. 
     */
    inline Value parse(char const * str) {
        std::stringstream s{str};
        return parse(s);
    }

    /** A rather simle and permissive JSON parser. 
     
        Aside from the proper JSON it also supports comments, trailing commas, literal names and so on. 
        */
    class Value::Parser {
    public:
        
        class Token {
            friend class Parser;
        public:
            enum class Kind {
                Undefined, 
                Null,
                Bool,
                Int,
                Double,
                String, 
                Comment, 
                Identifier,
                Colon,
                Comma,
                SquareOpen,
                SquareClose,
                CurlyOpen,
                CurlyClose,
            }; // json::Value::Parser::Token::Kind

            Token(size_t l, size_t c, Kind kind):
                line{l}, col{c}, kind{kind}, valueBool_{false} {
            }

            Token(size_t l, size_t c, Kind kind, bool value):
                line{l}, col{c}, kind{kind}, valueBool_{value} {
            }

            Token(size_t l, size_t c, Kind kind, int value):
                line{l}, col{c}, kind{kind}, valueInt_{value} {
            }

            Token(size_t l, size_t c, Kind kind, double value):
                line{l}, col{c}, kind{kind}, valueDouble_{value} {
            }

            Token(size_t l, size_t c, Kind kind, std::string && value):
                line{l}, col{c}, kind{kind}, valueString_{std::move(value)} {
            }

            ~Token() {
                detach();
            }

            Token & operator = (Token const && from) {
                detach();
                line = from.line;
                col = from.col;
                kind = from.kind;
                switch (kind) {
                    case Kind::Identifier:
                    case Kind::String:
                    case Kind::Comment:
                        new (&valueString_) std::string{std::move(from.valueString_)};
                        break;
                    case Kind::Bool:
                        valueBool_ = from.valueBool_;
                        break;
                    case Kind::Double:
                        valueDouble_ = from.valueDouble_;
                        break;
                    default:
                        valueInt_ = from.valueInt_;
                }
                return *this;
            }

            size_t line;
            size_t col;

            Kind kind;

        private:

            void detach() {
                using namespace std;
                switch (kind) {
                    case Kind::Identifier:
                    case Kind::String:
                    case Kind::Comment:
                        valueString_.~string();
                        break;
                    default: // no need to detach anyting in this case
                        break;
                }
            }

            union {
                bool valueBool_;
                int valueInt_;
                double valueDouble_;
                std::string valueString_;
            }; 
            
        }; // json::Value::Parser::Token

        Parser(std::istream & s):
            s_{s} {
        }

        Value parse() {
            return parse(next());
        }

    private:

        Value parse(Token const & t) {
            switch (t.kind) {
                case Token::Kind::Comment:
                    return parseWithComment(t.valueString_);
                case Token::Kind::Undefined:
                    return Undefined{};
                case Token::Kind::Null:
                    return Null{};
                case Token::Kind::Bool:
                    return Value{t.valueBool_};
                case Token::Kind::Int:
                    return Value{t.valueInt_};
                case Token::Kind::Double:
                    return Value{t.valueDouble_};
                case Token::Kind::String:
                    return Value{t.valueString_};
                // '[' [ value ] { ',' value } [ ',' ] ']'
                case Token::Kind::SquareOpen: {
                    Array i{};
                    Token t = next();
                    if (t.kind != Token::Kind::SquareClose) {
                        i.add(parse(t));
                        t = next();
                        while (t.kind == Token::Kind::Comma) {
                            t = next();
                            if (t.kind == Token::Kind::SquareClose)
                                break;
                            i.add(parse(t));
                        }
                        if (t.kind != Token::Kind::SquareClose) 
                            throw "error";
                    }
                    return i;
                }
                // '{' [ string = value ]}
                case Token::Kind::CurlyOpen: {
                    Struct i{};

                    // TODO
                    return i;
                }


            }
        }

        Value parseWithComment(std::string comment) {
            Value result = parse();
            result.setComment(comment);
            return result;
        }

        /** Returns the next token in the input stream. 
         
            " => string
            0-9 => number 
            a-zA-Z_ => identifier
            / => comment

            */
        Token next() {
            while (true) {
                auto line = l_;
                auto col = c_;
                auto c = nextChar();
                switch (c) {
                    case ':':
                        return Token{l_, c_, Token::Kind::Colon};
                    case ',':
                        return Token{l_, c_, Token::Kind::Comma};
                    case '[':
                        return Token{l_, c_, Token::Kind::SquareOpen};
                    case ']':
                        return Token{l_, c_, Token::Kind::SquareClose};
                    case '{':
                        return Token{l_, c_, Token::Kind::CurlyOpen};
                    case '}':
                        return Token{l_, c_, Token::Kind::CurlyClose};
                    case '/':
                        return Token{l_, c_, Token::Kind::Comment, nextComment(l_, c_)};
                    case '"':
                    case '\'':
                        return Token{l_, c_, Token::Kind::String, nextString(l_, c_, c)};
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        // whitespace, just read next
                        break;
                    default:
                        if (isDigit(c))
                            return nextNumber(l_, c_, c);
                        else if (isIdentifierStart(c))
                            return nextIdentifier(l_, c_, c);
                        else
                            error("Valid JSON character");
                }
            }
        }

        std::string nextComment(size_t l, size_t c) {
            std::string result{};
            switch (nextChar()) {
                case '/': // single line comment
                    while (! eof()) {
                        char c = nextChar();
                        if (c == '\n')
                            break;
                        result += c;
                    }
                    break;
                case '*': // multi-line comment 
                    while (true) {
                        if (eof())
                            error(l, c, "Unterminated multi-line comment");
                        char c = nextChar();
                        if (c == '*') {
                            char c2 = nextChar();
                            if (c2 == '/')
                                break;
                            result += c;
                            result += c2;
                        } else {
                            result += c;
                        }
                    }
                    break;
                default:
                    error("// or /* comment");
            }
            return result;
        }

        std::string nextString(size_t l, size_t c, char delimiter) {
            std::string result{};
            while (true) {
                if (eof())
                    error(l, c, "unterminated string literal");
                char c = nextChar();
                if (c == '\\') { 
                    char c = nextChar();
                    switch (c) {
                        case '"':
                        case '\'':
                        case '\\':
                            result += c;
                            break;
                        case 't':
                            result += '\t';
                            break;
                        case 'n':
                            result += '\n';
                            break;
                        case 'r':
                            result += '\r';
                            break;
                        case '\n':
                            break;
                        default:
                            error("valid string escape sequence");
                    }
                } else {
                    result += c;
                }
            }
            return result;
        }

        /** Parses an identifier. 
         */
        Token nextIdentifier(size_t l, size_t c, char start) {
            std::string result{start};
            while (! eof() && isIdentifier(peekChar()))
                result += nextChar();
            if (result == "null")
                return Token{l, c, Token::Kind::Null};
            else if (result == "undefined")
                return Token{l, c, Token::Kind::Undefined};
            else if (result == "true")
                return Token{l, c, Token::Kind::Bool, true};
            else if (result == "false")
                return Token{l, c, Token::Kind::Bool, false};
            else
                return Token{l, c, Token::Kind::Identifier, std::move(result)};
        }

        Token nextNumber(size_t l, size_t c, char start) {
            int result = (start - '0');
            while (isDigit(peekChar()))
                result = result * 10 + (nextChar() - '0');
            if (peekChar() == '.') {
                nextChar();
                throw "";
            } else {
                return Token{l, c, Token::Kind::Int, result};
            }
        }

        char nextChar() {
            char result = s_.get();
            if (result == '\r') {
                ++l_;
                c_ = 1;
            } else {
                ++c_;
            }
            return result;
        }

        char peekChar() {
            return s_.peek();
        }

        bool eof() {
            return s_.eof();
        }

        void error(char const * expected) {
            // TODO
        }
        void error(size_t l, size_t c, char const * msg) {
            // TODO
        }

        bool isDigit(char c) { return c > '0' && c <= '9'; }
        bool isIdentifierStart(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
        bool isIdentifier(char c) { return isIdentifierStart(c) || isDigit(c); } 

        // location information for better errors
        size_t l_ = 1;
        size_t c_ = 1;

        std::istream & s_;

    }; // json::Value::Parser



    // TODO serialize



} // namespace json

#if (defined TESTS)
#include "tests.h"

TEST(json, Undefined) {
    auto x = json::Undefined{};
    EXPECT(x.comment().empty());
    x.setComment("foo");
    EXPECT_EQ(x.comment(), "foo");
    EXPECT_EQ(STR(x), "undefined");
    EXPECT_EQ(x, json::undefined);
}

TEST(json, Null) {
    auto x = json::Null{};
    EXPECT(x.comment().empty());
    x.setComment("foo");
    EXPECT_EQ(x.comment(), "foo");
    EXPECT_EQ(STR(x), "Null");
}

TEST(json, Bool) {
    auto x = json::Bool{true};
    EXPECT(x.comment().empty());
    x.setComment("foo");
    EXPECT_EQ(x.comment(), "foo");
    EXPECT_EQ(STR(x), "true");
    auto y = json::Bool{false};
    EXPECT_EQ(STR(y), "false");
}

TEST(json, Int) {
    auto x = json::Int{0};
    EXPECT(x.comment().empty());
    x.setComment("foo");
    EXPECT_EQ(x.comment(), "foo");
    EXPECT_EQ(STR(x), "0");
    x = json::Int{-56};
    EXPECT_EQ(STR(x), "-56");
}

TEST(json, Double) {
    auto x = json::Double{-0.1};
    EXPECT(x.comment().empty());
    x.setComment("foo");
    EXPECT_EQ(x.comment(), "foo");
    EXPECT_EQ(STR(x), "-0.1");
    x = json::Double{56.5};
    EXPECT_EQ(STR(x), "56.5");
}

TEST(json, String) {
    auto x = json::String{"foobar"};
    EXPECT(x.comment().empty());
    x.setComment("foo");
    EXPECT_EQ(x.comment(), "foo");
    EXPECT_EQ(STR(x), "\"foobar\"");
}

TEST(json, Array) {
    auto x = json::Array{};
    x.add(4);
    x.add(5.6);
    x.add(true);
    x.add(false);
    x.add("foo");
    x.add(json::Null{});
    x.add(json::Undefined{});
    EXPECT_EQ(STR(x), "[4, 5.6, true, false, \"foo\", Null, undefined]");
}

TEST(json, Struct) {
    auto x = json::Struct{};
    x.set("foo", "bar");
    x["bar"] = true;
    EXPECT_EQ(STR(x), "{\"foo\" : \"bar\", \"bar\" : true}");
    EXPECT_EQ(x["zaza"], json::undefined);
    EXPECT_EQ(STR(x), "{\"foo\" : \"bar\", \"bar\" : true, \"zaza\" : undefined}");
}

TEST(json, parseNull) {
    //json::Value v = json::parse("Null");
    //EXPECT_EQ(STR(v), STR(json::Null{}));
}


#endif
