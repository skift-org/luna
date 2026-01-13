module;

#include <karm-core/macros.h>

export module Luna:expr;

import :base;
import :objects;
import :ops;

namespace Luna {

export struct EqExpr : Base {
    // <expr> == <expr>

    Value _lhs;
    Value _rhs;

    EqExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opEq(lhs, rhs);
    }
};

export struct NEqExpr : Base {
    // <expr> != <expr>

    Value _lhs;
    Value _rhs;

    NEqExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opNot(try$(opEq(lhs, rhs)));
    }
};

export struct LtExpr : Base {
    // <expr> < <expr>

    Value _lhs;
    Value _rhs;

    LtExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        auto order = try$(opCmp(lhs, rhs));
        return Ok(order == Symbols::LESS);
    }
};

export struct LtEqExpr : Base {
    // <expr> <= <expr>

    Value _lhs;
    Value _rhs;

    LtEqExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        auto order = try$(opCmp(lhs, rhs));
        return Ok(order == Symbols::LESS or order == Symbols::EQUIVALENT);
    }
};

export struct GtExpr : Base {
    // <expr> > <expr>

    Value _lhs;
    Value _rhs;

    GtExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        auto order = try$(opCmp(lhs, rhs));
        return Ok(order == Symbols::GREATER);
    }
};

export struct GtEqExpr : Base {
    // <expr> >= <expr>

    Value _lhs;
    Value _rhs;

    GtEqExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        auto order = try$(opCmp(lhs, rhs));
        return Ok(order == Symbols::GREATER or order == Symbols::EQUIVALENT);
    }
};

export struct AndExpr : Base {
    // <expr> and <expr>
    Value _lhs;
    Value _rhs;

    AndExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opAnd(lhs, rhs);
    }
};

export struct OrExpr : Base {
    // <expr> or <expr>

    Value _lhs;
    Value _rhs;

    OrExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opOr(lhs, rhs);
    }
};

export struct NotExpr : Base {
    // not <expr>

    Value _expr;

    NotExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto expr = try$(opEval(_expr, env));
        return opNot(expr);
    }
};

export struct NegExpr : Base {
    // -<expr>

    Value _expr;

    NegExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto expr = try$(opEval(_expr, env));
        return opNeg(expr);
    }
};

export struct AddExpr : Base {
    // <expr> + <expr>

    Value _lhs;
    Value _rhs;

    AddExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opAdd(lhs, rhs);
    }
};

export struct SubExpr : Base {
    // <expr> - <expr>

    Value _lhs;
    Value _rhs;

    SubExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opSub(lhs, rhs);
    }
};

export struct MulExpr : Base {
    // <expr> * <expr>

    Value _lhs;
    Value _rhs;

    MulExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opMul(lhs, rhs);
    }
};

export struct DivExpr : Base {
    // <expr> / <expr>

    Value _lhs;
    Value _rhs;

    DivExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opDiv(lhs, rhs);
    }
};

export struct ModExpr : Base {
    // <expr> % <expr>

    Value _lhs;
    Value _rhs;

    ModExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opMod(lhs, rhs);
    }
};

export struct BinNotExpr : Base {
    // ~<expr>

    Value _expr;

    BinNotExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto expr = try$(opEval(_expr, env));
        return opBinNot(expr);
    }
};

export struct BinAndExpr : Base {
    // <expr> & <expr>

    Value _lhs;
    Value _rhs;

    BinAndExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opBinAnd(lhs, rhs);
    }
};

export struct BinOrExpr : Base {
    // <expr> | <expr>

    Value _lhs;
    Value _rhs;

    BinOrExpr(Value lhs, Value rhs)
        : _lhs(lhs), _rhs(rhs) {}

    CompletionOr<Value> eval(Object env) override {
        auto lhs = try$(opEval(_lhs, env));
        auto rhs = try$(opEval(_rhs, env));
        return opBinAnd(lhs, rhs);
    }
};

export struct EnvExpr : Base {
    EnvExpr() {}

    CompletionOr<Value> eval(Object env) override {
        return Ok(env);
    }
};

export struct SetExpr : Base {
    // <expr>[<expr>] = <expr>
    // <expr>.<expr> = <expr>

    Value _target;
    Value _key;
    Value _value;

    SetExpr(Value target, Value key, Value value)
        : _target(target), _key(key), _value(value) {}

    CompletionOr<Value> eval(Object env) override {
        auto target = try$(opEval(_target, env));
        auto key = try$(opEval(_key, env));
        auto value = try$(opEval(_value, env));
        return opSet(target, key, value);
    }
};

export struct SetEnvExpr : Base {
    // <expr> = <expr>

    Value _key;
    Value _value;

    SetEnvExpr(Value key, Value value)
        : _key(key), _value(value) {}

    CompletionOr<Value> eval(Object env) override {
        auto key = try$(opEval(_key, env));
        auto value = try$(opEval(_value, env));
        return opSet(env, key, value);
    }
};

export struct DeclExpr : Base {
    // var <expr> = <expr>

    Value _key;
    Value _value;

    DeclExpr(Value key, Value value)
        : _key(key), _value(value) {}

    CompletionOr<Value> eval(Object env) override {
        auto value = try$(opEval(_value, env));
        return opDecl(env, _key, value);
    }
};

export struct GetExpr : Base {
    // <expr>.<expr>
    // <expr>

    Value _target;
    Value _key;

    GetExpr(Value target, Value key)
        : _target(target), _key(key) {}

    CompletionOr<Value> eval(Object env) override {
        auto target = try$(opEval(_target, env));
        auto key = try$(opEval(_key, env));
        return opGet(target, key);
    }
};

export struct IsExpr : Base {
    // <expr> is <type>
    Value _expr;
    Value _type;

    IsExpr(Value expr, Value type)
        : _expr(expr), _type(type) {}

    CompletionOr<Value> eval(Object env) override {
        auto type = try$(opEval(_type, env));
        auto expr = try$(opEval(_expr, env));

        return Ok(is(_expr, try$(asSymbol(type))));
    }
};

export struct AsExpr : Base {
    // <expr> as <type>
    Value _expr;
    Value _type;

    AsExpr(Value expr, Value type)
        : _expr(expr), _type(type) {}

    CompletionOr<Value> eval(Object env) override {
        auto expr = try$(opEval(_expr, env));
        return as(_expr, try$(asSymbol(_type)));
    }
};

export struct TypeOfExpr : Base {
    // <expr> as <type>
    Value _expr;

    TypeOfExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        return Ok(typeOf(try$(opEval(_expr, env))));
    }
};

export struct QuoteExpr : Base {
    // '<value>

    Value _value;

    QuoteExpr(Value value)
        : _value(value) {}

    CompletionOr<Value> eval(Object) override {
        return Ok(_value);
    }
};

export struct NopExpr : Base {
    NopExpr() {}

    CompletionOr<Value> eval(Object) override {
        return Ok(NONE);
    }
};

export struct ReturnExpr : Base {
    // return <expr>

    Value _expr;

    ReturnExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto value = try$(opEval(_expr, env));
        return Completion::return_(value);
    }
};

export struct ContinueExpr : Base {
    // continue <expr>

    Value _expr;

    ContinueExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto value = try$(opEval(_expr, env));
        return Completion::continue_(value);
    }
};

export struct BreakExpr : Base {
    // break <expr>

    Value _expr;

    BreakExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto value = try$(opEval(_expr, env));
        return Completion::break_(value);
    }
};

export struct ThrowExpr : Base {
    // throw <expr>

    Value _expr;

    ThrowExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto value = try$(opEval(_expr, env));
        return Completion::exception(value);
    }
};

export struct BlockExpr : Base {
    // { <exprs>;... }

    Vec<Value> _exprs;

    BlockExpr(Vec<Value> exprs = {})
        : _exprs(exprs) {}

    CompletionOr<Value> eval(Object env) override {
        for (usize i : range(_exprs.len())) {
            auto value = try$(opEval(_exprs[i], env));
            if (i == _exprs.len() - 1)
                return Ok(value);
        }
        unreachable();
    }
};

export struct ScopeExpr : Base {
    Value _expr;

    ScopeExpr(Value expr)
        : _expr(expr) {}

    CompletionOr<Value> eval(Object env) override {
        auto inner = try$(Environment::create(env));
        return opEval(_expr, inner);
    }
};

export struct TableExpr : Base {
    // { <value> : <expr> , ...}
    Vec<Tuple<Value, Value>> _exprs;

    TableExpr(Vec<Tuple<Value, Value>> exprs = {})
        : _exprs(exprs) {}

    CompletionOr<Value> eval(Object env) override {
        auto table = try$(Table::create());

        for (auto& [key, vexpr] : _exprs) {
            auto value = try$(opEval(vexpr, env));
            try$(opSet(table, key, value));
        }

        return Ok(table);
    }
};

export struct ListExpr : Base {
    // [ <expr>, ... ]

    Vec<Value> _exprs;

    ListExpr(Vec<Value> exprs = {}) : _exprs(exprs) {}

    CompletionOr<Value> eval(Object env) override {
        Vec<Value> result = {};
        for (auto& expr : _exprs) {
            result.pushBack(try$(opEval(expr, env)));
        }
        return List::create(result);
    }
};

export struct IfExpr : Base {
    // if (<expr>) <expr>
    // if (<expr>) <expr> else <expr>
    Value _cond;
    Value _then;
    Value _else;

    IfExpr(Value cond, Value then, Value else_)
        : _cond(cond),
          _then(then),
          _else(else_) {}

    CompletionOr<Value> eval(Object env) override {
        auto cond = try$(opEval(_cond, env));
        auto branch = (try$(asBoolean(cond)) ? _then : _else);
        return opEval(branch, env);
    }
};

export struct WhileExpr : Base {
    // while (<expr>) <expr>

    Value _cond;
    Value _body;

    WhileExpr(Value cond, Value body)
        : _cond(cond),
          _body(body) {}

    CompletionOr<Value> eval(Object env) override {
        Value res = NONE;
        while (true) {
            auto cond = try$(opEval(_cond, env));
            auto thruth = try$(asBoolean(cond));
            if (not thruth)
                return Ok(res);

            auto evalResult = opEval(_body, env);
            if (evalResult) {
                res = try$(evalResult);
                continue;
            }
            auto completion = evalResult.none();
            if (completion.type == Completion::EXCEPTION)
                return completion;
            if (completion.type == Completion::CONTINUE)
                continue;
            return Ok(completion.value);
        }
    }
};

export struct TryExpr : Base {
    // try <expr> catch (<ident>) <expr>

    Value _try;
    Value _errIdent;
    Value _catch;

    TryExpr(Value try_, Value errIdent, Value catch_)
        : _try(try_), _errIdent(errIdent), _catch(catch_) {}

    CompletionOr<Value> eval(Object env) override {
        auto result = opEval(_try, env);
        if (result) {
            return result;
        }
        auto completion = result.none();
        if (completion.type == Completion::EXCEPTION) {
            auto catchEnv = try$(Environment::create(env));
            try$(opDecl(catchEnv, _errIdent, completion.value));
            return opEval(_catch, catchEnv);
        }
        return Ok(completion.value);
    }
};

export struct ParamExpr {
    // <ident>...
    // <ident>: <expr>...

    Value key;
    Opt<Value> value;
};

export struct FuncExpr : Base {
    // fn (param...) <expr>

    Vec<ParamExpr> _sig;
    Value _code;

    FuncExpr(Vec<ParamExpr> sig, Value code)
        : _sig(sig), _code(code) {}

    CompletionOr<Value> eval(Object env) override {
        Vec<Param> sig;
        for (auto& s : _sig) {
            Param p{s.key};
            p.required = true;
            if (s.value) {
                p.value = try$(opEval(s.value.unwrap(), env));
                p.required = false;
            }
            sig.pushBack(p);
        }
        return Func::create(env, sig, _code);
    }
};

export struct ArgExpr {
    // <expr>...
    // <ident>: <expr>...

    Opt<Value> key;
    Value expr;
};

export struct CallExpr : Base {
    // <expr>(args...)

    Value _func;
    Vec<ArgExpr> _args;

    CallExpr(Value func, Vec<ArgExpr> args)
        : _func(func), _args(args) {}

    CompletionOr<Value> eval(Object env) override {
        auto func = try$(opEval(_func, env));
        auto params = try$(Table::create());

        Integer index = 0;
        for (auto& arg : _args) {
            auto value = try$(opEval(arg.expr, env));
            if (arg.key) {
                try$(opSet(params, arg.key.unwrap(), value));
            } else {
                try$(opSet(params, index, value));
                index++;
            }
        }

        return opCall(func, params);
    }
};

} // namespace Luna
