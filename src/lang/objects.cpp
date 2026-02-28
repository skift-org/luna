module;

#include <karm/macros>

export module Luna:objects;

import :base;
import :ops;

namespace Luna {

export struct Table : Base {
    Map<Value, Value> _fields;

    Table(Map<Value, Value> fields = {})
        : _fields(std::move(fields)) {}

    static CompletionOr<Reference> create(Map<Value, Value> fields = {}) {
        return Ok(makeRc<Table>(fields));
    }

    CompletionOr<Value> get(Value key) override {
        return _fields.lookup(key)
            .okOr(Completion::exception("key not found"));
    }

    CompletionOr<> set(Value key, Value value) override {
        _fields.put(key, value);
        return Ok();
    }

    CompletionOr<> decl(Value key, Value value) override {
        return set(key, value);
    }

    CompletionOr<Boolean> has(Value key) override {
        return Ok(_fields.contains(key));
    }

    CompletionOr<Boolean> eq(Value rhs) const override {
        if (not isObject(rhs))
            return Ok(false);
        auto obj = asObject(rhs).unwrap();

        if (not try$(opEq(try$(len()), try$(opLen(rhs)))))
            return Ok(false);

        for (auto const& [k, v] : _fields.iterItems()) {
            if (not try$(opHas(rhs, k)))
                return Ok(false);

            if (not try$(opEq(v, try$(opGet(rhs, k)))))
                return Ok(false);
        }
        return Ok(true);
    }

    CompletionOr<Value> string() override {
        StringBuilder sb;
        sb.append("{"s);
        bool first = true;
        for (auto const& [k, v] : _fields.iterItems()) {
            if (not first)
                sb.append(", "s);
            first = false;

            sb.append(try$(asString(k)));
            sb.append(":"s);
            sb.append(try$(asString(v)));
        }
        sb.append("}"s);
        return Ok(sb.take());
    }

    CompletionOr<Boolean> boolean() override {
        return Ok(_fields.len() != 0);
    }

    CompletionOr<Integer> len() const override {
        return Ok(_fields.len());
    }

    u64 hash() const override {
        return Karm::hash(_fields);
    }
};

export struct List : Base {
    Vec<Value> _items;

    List(Vec<Value> items = {})
        : _items(std::move(items)) {}

    static CompletionOr<Reference> create(Vec<Value> items = {}) {
        return Ok(makeRc<List>(items));
    }

    CompletionOr<Value> get(Value key) override {
        auto index = try$(asIndex(key));
        if (0 <= index and index < (Integer)_items.len())
            return Ok(_items[index]);
        return Completion::exception("index out of bound");
    }

    CompletionOr<> set(Value key, Value value) override {
        auto index = try$(asIndex(key));
        if (0 <= index and index < (Integer)_items.len()) {
            _items[index] = value;
            return Ok();
        }
        return Completion::exception("index out of bound");
    }

    CompletionOr<> decl(Value key, Value value) override {
        return set(key, value);
    }

    CompletionOr<Boolean> has(Value key) override {
        if (not isIndex(key))
            return Ok(false);
        auto index = try$(asIndex(key));
        return Ok(0 <= index and index < (Integer)_items.len());
    }

    CompletionOr<Boolean> eq(Value rhs) const override {
        if (not isObject(rhs))
            return Ok(false);
        auto obj = asObject(rhs).unwrap();

        if (not try$(opEq(try$(len()), try$(opLen(rhs)))))
            return Ok(false);

        Integer index = 0;
        for (auto& v : iter(_items)) {
            if (not try$(opHas(rhs, index)))
                return Ok(false);

            if (not try$(opEq(v, try$(opGet(rhs, index)))))
                return Ok(false);
            index++;
        }
        return Ok(true);
    }

    CompletionOr<Value> string() override {
        StringBuilder sb;
        sb.append("["s);
        bool first = true;
        for (auto& v : iter(_items)) {
            if (not first)
                sb.append(", "s);
            first = false;
            sb.append(try$(asString(v)));
        }
        sb.append("]"s);
        return Ok(sb.take());
    }

    CompletionOr<Boolean> boolean() override {
        return Ok(_items.len() != 0);
    }

    CompletionOr<Integer> len() const override {
        return Ok(_items.len());
    }

    u64 hash() const override {
        return Karm::hash(_items);
    }
};

export struct Environment : Base {
    Value _parent;
    Reference _decls = makeRc<Table>();

    Environment(Value parent) : _parent(parent) {}

    static CompletionOr<Reference> create(Value parent) {
        return Ok(makeRc<Environment>(parent));
    }

    CompletionOr<Value> get(Value key) override {
        if (try$(opHas(_decls, key)))
            return opGet(_decls, key);

        if (try$(asBoolean(_parent)))
            return opGet(_parent, key);

        return Completion::exception("not defined");
    }

    CompletionOr<> set(Value key, Value value) override {
        if (try$(opHas(_decls, key)))
            return opSet(_decls, key, value);

        if (try$(asBoolean(_parent))) {
            if (try$(opHas(_parent, key)))
                return opSet(_parent, key, value);
        }

        return opSet(_decls, key, value);
    }

    CompletionOr<> decl(Value key, Value value) override {
        return opSet(_decls, key, value);
    }

    CompletionOr<Boolean> has(Value key) override {
        if (try$(opHas(_decls, key)))
            return Ok(true);

        if (try$(asBoolean(_parent)))
            return opHas(_parent, key);

        return Ok(false);
    }
};

struct Param {
    Value key;
    Value value = NONE;
    bool required = false;
};

using Native = Func<CompletionOr<Value>(Reference params)>;

using Code = Union<
    Value,
    Native>;

export struct Func : Base {
    Reference _env;
    Vec<Param> _sig;
    Code _code;

    Func(Reference env, Vec<Param> sig, Code code)
        : _env(env), _sig(sig), _code(std::move(code)) {}

    static CompletionOr<Reference> create(Reference env, Vec<Param> sig, Code code) {
        return Ok(makeRc<Func>(env, sig, std::move(code)));
    }

    CompletionOr<Value> call(Reference params) override {
        auto locals = try$(Environment::create(_env));

        Integer index = 0;
        for (auto& s : _sig) {
            if (try$(opHas(params, s.key))) {
                try$(opDecl(locals, s.key, try$(opGet(params, s.key))));
            } else if (try$(opHas(params, index))) {
                try$(opDecl(locals, s.key, try$(opGet(params, index))));
                index++;
            } else if (not s.required) {
                try$(opDecl(locals, s.key, s.value));
            } else {
                return Completion::exception("missing parameter");
            }
        }

        return _code.visit(Visitor{
            [&](Value expr) {
                return opEval(expr, locals);
            },
            [&](Native& native) {
                return native(locals);
            },
        });
    }
};

} // namespace Luna
