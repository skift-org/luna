module;

#include <karm-core/macros.h>

export module Luna:base;

import Karm.Core;

using namespace Karm;

namespace Luna {

export struct Base;

// MARK: Primitives ------------------------------------------------------------

using Object = Rc<Base>;

export using Boolean = bool;

export using Integer = i64;

export using Number = f64;

export struct Value : Union<None, Boolean, Integer, Number, Symbol, String, Object> {
    using Union::Union;
};

namespace Symbols {

Symbol THIS = "this"_sym;
Symbol FROM = "from"_sym;
Symbol PARENT = "parent"_sym;
Symbol RHS = "rhs"_sym;

Symbol LESS = "less"_sym;
Symbol EQUIVALENT = "equivalent"_sym;
Symbol GREATER = "greater"_sym;
Symbol UNORDERED = "unordered"_sym;

Symbol NONE = "none"_sym;
Symbol BOOLEAN = "boolean"_sym;
Symbol INTEGER = "integer"_sym;
Symbol NUMBER = "number"_sym;
Symbol SYMBOL = "symbol"_sym;
Symbol STRING = "string"_sym;
Symbol OBJECT = "object"_sym;

} // namespace Symbols

// MARK: Exception -------------------------------------------------------------

struct Completion {
    enum struct Type {
        RETURN,
        CONTINUE,
        BREAK,
        EXCEPTION,
    };

    using enum Type;

    Type type;
    Value value;

    static Completion return_(Value value) {
        return Completion(Completion::RETURN, value);
    }

    static Completion continue_(Value value) {
        return Completion(Completion::CONTINUE, value);
    }

    static Completion break_(Value value) {
        return Completion(Completion::BREAK, value);
    }

    static Completion exception(Str msg) {
        return exception(Value{String{msg}});
    }

    static Completion exception(Value value) {
        return Completion(Completion::EXCEPTION, value);
    }

    operator Error() const {
        return Error::other("exception thrown");
    }
};

template <typename T = None>
using CompletionOr = Res<T, Completion>;

// MARK: Prototype -------------------------------------------------------------

struct Base {
    virtual ~Base() {}

    virtual CompletionOr<Value> get([[maybe_unused]] Value key) {
        return Completion::exception("not indexable");
    }

    virtual CompletionOr<> set([[maybe_unused]] Value key, [[maybe_unused]] Value value) {
        return Completion::exception("not indexable");
    }

    virtual CompletionOr<> decl([[maybe_unused]] Value key, [[maybe_unused]] Value value) {
        return Completion::exception("not indexable");
    }

    virtual CompletionOr<Boolean> has([[maybe_unused]] Value key) {
        return Completion::exception("not indexable");
    }

    virtual CompletionOr<Boolean> eq([[maybe_unused]] Value rhs) const {
        return Completion::exception("not equatable");
    }

    virtual CompletionOr<Symbol> cmp([[maybe_unused]] Value rhs) const {
        return Completion::exception("not comparable");
    }

    virtual CompletionOr<Value> eval([[maybe_unused]] Object env) {
        return Completion::exception("not evaluable");
    }

    virtual CompletionOr<Value> call([[maybe_unused]] Object params) {
        return Completion::exception("not callable");
    }

    virtual CompletionOr<Value> string() {
        return Ok<String>("{}"s);
    }

    virtual CompletionOr<Boolean> boolean() {
        return Ok(true);
    }

    virtual CompletionOr<Integer> len() const {
        return Completion::exception("can't len");
    }

    bool operator==(Value other) const {
        return eq(other).unwrap();
    }
};

} // namespace Luna
