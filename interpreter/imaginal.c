#include "imaginal.h"
#include "syscalls/syscalls.h"
#include "ir/manual_gen.h"
#include "math/math.h"

enum { imaginal_log_error, imaginal_log_trace, imaginal_log_debug } imaginal_log_level;

#define imaginal_debug(LEVEL, ...)\
    if (imaginal_log_##LEVEL <= imaginal_log_level)\
        print(__VA_ARGS__);

buffer imaginal_buf;

void imaginal_print(codegen exp){
    if (!imaginal_buf.buffer) imaginal_buf = buffer_create(0x1000, buffer_can_grow);
    buffer_wipe(&imaginal_buf);
    codegen_debug_print(exp, 0);
    print("%v",slice_from_buffer(&imaginal_buf));
}

bool is_true(codegen exp){
    switch (exp.type) {
        case sem_rule_sexp: {
            s_exp_code *scode = exp.ptr;
            return is_true(scode->car);
        }
        case sem_rule_atom: {
            atom_code *lcode = exp.ptr;
            return lcode->type == car_true;
        }
        default: return false;
    }
}

static inline codegen is_atom_s_exp(codegen exp){
    return is_atom(exp) ? make_true_atom() : nil_exp;
}

static inline bool is_nil(codegen m){
    return (!m.ptr || !m.type);
}

typedef enum { imaginal_add, imaginal_sub, imaginal_mul, imaginal_div } imaginal_math;

codegen imaginal_builtin_mathf(codegen exp, imaginal_math op){
    print("FMATH");
    s_exp_code *code = exp.ptr;
    if (!code) { print("[MATH error] Add null ptr"); return nil_exp; }
    atom_code *car_val = code->car.ptr;
    if (!code->car.ptr) { print("[MATH error] null car"); return nil_exp; }
    double a = 0;
    double b = 0;
    if (car_val->type == car_int) a = (double)car_val->integer;
    else if (car_val->type != car_flo) { print("[MATH error] non-numeric lh"); imaginal_print(code->car); return nil_exp; }
    else a = car_val->floating;
    s_exp_code *cdr = code->cdr.ptr;
    if (cdr){
        atom_code *cdr_val = (code->cdr.type == sem_rule_atom) ? code->cdr.ptr : ((s_exp_code*)code->cdr.ptr)->car.ptr;
        if (cdr_val->type == car_int) b = (double)cdr_val->integer;
        else if (cdr_val->type != car_flo) { print("[MATH error] non-numeric rh"); imaginal_print(code->cdr); return nil_exp; }
        else b = cdr_val->floating;
    }
    print("%f %f",a,b);
    switch (op) {
        case imaginal_add: imaginal_debug(trace,"[ADD trace] %f + %f = %f",a,b,a+b); return make_flo_atom(a+b);
        case imaginal_sub: imaginal_debug(trace,"[SUB trace] %f - %f = %f",a,b,a-b); return make_flo_atom(a-b);
        case imaginal_mul: imaginal_debug(trace,"[MUL trace] %f * %f = %f",a,b,a*b); return make_flo_atom(a*b);
        case imaginal_div: {
            if (b == 0){
                print("[DIV error] divide by 0");
                return nil_exp;
            }
            imaginal_debug(trace,"[DIV trace] %f/%f = %f",a,b,a/b);
            return make_flo_atom(a/b);
        } 
    }
}

codegen imaginal_builtin_math(codegen exp, imaginal_math op){
    s_exp_code *code = exp.ptr;
    if (!code) { print("[MATH error] Add null ptr"); return nil_exp; }
    atom_code *car_val = code->car.ptr;
    if (!code->car.ptr) { print("[MATH error] null car"); return nil_exp; }
    i64 a = 0;
    i64 b = 0;
    if (car_val->type == car_flo) return imaginal_builtin_mathf(exp, op);
    else if (car_val->type != car_int) { print("[MATH error] non-numeric lh"); imaginal_print(code->car); return nil_exp; }
    else a = car_val->integer;
    s_exp_code *cdr = code->cdr.ptr;
    if (cdr){
        atom_code *cdr_val = (code->cdr.type == sem_rule_atom) ? code->cdr.ptr : ((s_exp_code*)code->cdr.ptr)->car.ptr;
        if (cdr_val->type == car_flo) return imaginal_builtin_mathf(exp, op);
        if (cdr_val->type != car_int) { print("[MATH error] non-numeric rh"); imaginal_print(code->cdr); return nil_exp; }
        else b = cdr_val->integer;
    }
    switch (op) {
        case imaginal_add: imaginal_debug(trace, "[ADD trace] %i + %i = %i",a,b,a+b); return make_int_atom(a+b);
        case imaginal_sub: imaginal_debug(trace, "[SUB trace] %i - %i = %i",a,b,a-b); return make_int_atom(a-b);
        case imaginal_mul: imaginal_debug(trace, "[MUL trace] %i * %i = %i",a,b,a*b); return make_int_atom(a*b);
        case imaginal_div: {
            if (b == 0){
                print("[DIV error] divide by 0");
                return nil_exp;
            }
            imaginal_debug(trace,"[DIV trace] %i/%i = %i",a,b,a/b);
            return make_int_atom(a/b);
        } 
    }
}

bool equality_atom(codegen a, codegen b){
    if (a.type != b.type || a.type != sem_rule_atom) return false;
    atom_code *acode = a.ptr;
    atom_code *bcode = b.ptr;
    if (acode->type != bcode->type) return false;
    switch (acode->type) {
        case car_none:
            return true;
        case car_identifier:
        case car_string:
            return slices_equal(acode->val, bcode->val, true);
        case car_flo:
            return absd(acode->floating - bcode->floating) < 0.001f;
        case car_int:
            return acode->integer == bcode->integer;
        case car_true:
            return true;
      break;
    }
}

static inline codegen equality_atom_exp(codegen a, codegen b){
    return equality_atom(a, b) ? make_true_atom() : nil_exp;
}

static inline codegen equality(codegen args){
    s_exp_code *s = args.ptr;
    codegen a = s->car;
    codegen b = ((s_exp_code*)s->cdr.ptr)->car;
    return equality_atom_exp(a, b);
}

codegen pairlis(codegen x, codegen y, codegen env){
    if (is_nil(x)) return env;
    return cons(
        cons(car(x), car(y)),
        pairlis(cdr(x), cdr(y), env)
    );
}

codegen letvars(codegen x, codegen env){
    if (is_nil(x)) return env;
    return cons(
        cons(car(car(x)), eval(car(cdr(car(x))),&env)),
        letvars(cdr(x), env)
    );
}

// evlis[m;a] = [null[m] → NIL;
//              T → cons[eval[car[m];a];evlis[cdr[m];a]]]

codegen eval_list(codegen l, bool last, int depth, codegen *env){
    imaginal_debug(trace,"[S_EXP trace] transform");
    if (is_nil(l)) return nil_exp;
    s_exp_code *code = l.ptr;
    codegen local = *env;
    codegen n = s_exp_code_init();
    s_exp_code *ncode = n.ptr;
    ncode->car = eval(code->car, &local);
    imaginal_debug(trace,"[S_EXP trace] next");
    if (code->cdr.ptr) ncode->cdr = eval_list(code->cdr, last, depth+1, &local);
    else if (last || depth == 0) {
        codegen c = ncode->car;
        release(ncode);
        return c;
    }
    imaginal_debug(trace,"[S_EXP trace] done");
    return n;
}

codegen evlis(codegen l, codegen *env){
    return eval_list(l, false, 0, env);
}

// apply[fn;x;a] =
//      [atom[fn] → [eq[fn;CAR] → caar[x];
//                  eq[fn;CDR] → cdar[x];
//                  eq[fn;CONS] → cons[car[x];cadr[x]];
//                  eq[fn;ATOM] → atom[car[x]];
//                  eq[fn;EQ] → eq[car[x];cadr[x]];
//                  T → apply[eval[fn;a];x;a]];
//      eq[car[fn];LAMBDA] → eval[caddr[fn]; pairlis[cadr[fn];x;a]];
//      eq[car[fn];LABEL] → apply[caddr[fn];x;cons[cons[cadr[fn];caddr[fn]];a]]]

codegen imaginal_debugger_options(string_slice s, codegen a, codegen *env){
    if (slice_lit_match(s, "halt", true)){
        halt(0);
    }
    if (slice_lit_match(s, "printenv", true)){
        print("Environment:");
        imaginal_print(*env);
        print("============");
    }
    if (slice_lit_match(s, "print",true)){
        imaginal_print(a);
        return a;
    }
    if (slice_lit_match(s, "trace",true)){
        string_slice lvl = car_get_string(car(a));
        print("LEVEL |%v|",lvl);
        if (slice_lit_match(lvl, "error", true)){
            imaginal_log_level = imaginal_log_error;
            print("Set log level >>>> %i",imaginal_log_level);
        }
        if (slice_lit_match(lvl, "trace", true)){
            imaginal_log_level = imaginal_log_trace;
            print("Set log level >>>> %i",imaginal_log_level);
        }
        if (slice_lit_match(lvl, "debug", true)){
            imaginal_log_level = imaginal_log_debug;
            print("Set log level >>>> %i",imaginal_log_level);
        }
    }
    return nil_exp;
}

extern codegen (*imaginal_fallback_fncall)(codegen fn_exp, codegen a, codegen *env);

codegen apply(codegen fn_exp, codegen a, codegen *env){
    imaginal_debug(trace,"[APPLY] begin");
    if (!fn_exp.ptr) { print("[APPLY error] Apply null ptr"); return (codegen){}; }
    if (is_atom(fn_exp)){
        string_slice s = car_id(fn_exp);
        imaginal_debug(trace,"[APPLY trace] Atomic expression %v",s);
        if (!s.length) { print("[APPLY error] Wrong expression type"); return (codegen){}; }
        if (slice_lit_match(s, "car", true)){
            print(">>CAR of:");
            imaginal_print(a);
            print("IS:");
            imaginal_print(car(a));
            return car(a);
        }
        else if (slice_lit_match(s, "cdr", true)){
            return cdr(a);
        }
        else if (slice_lit_match(s, "cons", true)){
            return cons(car(a), car(cdr(a)));
        }
        else if (slice_lit_match(s, "atom", true)){
            s_exp_code *arg = a.ptr;
            if (!arg) return make_true_atom();
            if (is_atom(a)) return make_true_atom();
            if (arg->cdr.ptr && arg->cdr.type) return nil_exp;
            return is_atom_s_exp(arg->car);
        }
        else if (slice_lit_match(s, "eq", true)){
            return equality(a);
        } else if (slice_lit_match(s, "add", true)){
            return imaginal_builtin_math(a,imaginal_add);
        } else if (slice_lit_match(s, "sub", true)){
            return imaginal_builtin_math(a,imaginal_sub);
        } else if (slice_lit_match(s, "mul", true)){
            return imaginal_builtin_math(a,imaginal_mul);
        } else if (slice_lit_match(s, "div", true)){
            return imaginal_builtin_math(a,imaginal_div);
        } else {
            codegen deb = imaginal_debugger_options(s,a,env);
            if (deb.ptr) return deb;
            codegen exp = eval(fn_exp, env);
            if (is_nil(exp)){
                if (imaginal_fallback_fncall){
                    return imaginal_fallback_fncall(fn_exp, a, env);
                }
                print("[APPLY error]: function not found %v",s);
                return nil_exp;
            }
            return apply(exp, a, env);
        }
    } else {
        s_exp_code *fn = fn_exp.ptr;
        codegen c = car(fn_exp);
        if (!is_atom(c)) { print("[APPLY error] no atomic expression in sub-expression"); return (codegen){}; }
        string_slice s = car_id(c);
        imaginal_debug(trace,"[APPLY trace] Non-atomic expression %v",s);
        if (!s.length) return (codegen){};
        if (slice_lit_match(s, "lambda", true)){
            codegen local = pairlis(car(cdr(fn_exp)),a,*env);
            return eval_list(cdr(cdr(fn_exp)), true, 0, &local);
        }
        if (slice_lit_match(s, "label", true)){
            codegen local = cons(
                cons(car(cdr(fn_exp)), car(cdr(cdr(fn_exp)))),
                *env
            );
            return apply(car(cdr(cdr(fn_exp))),a,&local);
        }
        print("[APPLY error] unknown expression %v in apply",s);
        return nil_exp;
    }
}

// evcon[c;a] = [eval[caar[c];a] → eval[cadar[c];a];
//              T → evcon[cdr[c];a]]

codegen evcon(codegen c, codegen env){
    if (is_true(eval(car(car(c)), &env))){
        return eval(car(cdr(car(c))), &env);
    }
    codegen tail = cdr(c);
    if (!tail.ptr) return nil_exp;
    return evcon(cdr(c), env);
}

codegen copy_val(codegen a){
    switch (a.type) {
        case sem_rule_sexp:
        {
            codegen s = s_exp_code_init();
            s_exp_code *os = a.ptr;
            s_exp_code *ns = s.ptr;
            ns->car = copy_val(os->car);
            ns->cdr = copy_val(os->cdr);
            return s;
        }
        case sem_rule_atom:
        {
            codegen atom = atom_code_init();
            atom_code *new = atom.ptr;
            atom_code *old = a.ptr;
            new->integer = old->integer;
            new->floating = old->floating;
            new->type = old->type;
            new->val = old->val;
            return atom;
        }
    }
    return nil_exp;
}

// assoc[x;a] = [equal[caar[a];x]→car[a]; T → assoc[x;cdr[a]]]

codegen assoc(codegen x, codegen a){
    imaginal_debug(trace,"[EVAL trace] assoc");
    if (is_nil(a)) {
        if (imaginal_log_level >= imaginal_log_trace){
            print("[ASSOC error] Symbol not found:");
            imaginal_print(x);
        }
        return nil_exp;
    }
    if (equality_atom(car(car(a)), x)){
        return copy_val(cdr(car(a)));
    }
    return assoc(x, cdr(a));
}

// eval[e;a] = 
    // [atom[e] → cdr[assoc[e;a]];
    //  atom[car[e]] →
    //          [eq[car[e];QUOTE] → cadr[e];
    //          eq[car[e];COND] → evcon[cdr[e];a];
    //          T → apply[car[e];evlis[cdr[e];a];a]];
    //  T → apply[car[e];evlis[cdr[e];a];a]]

codegen eval(codegen exp, codegen *env){
    imaginal_debug(trace,"[EVAL] begin");
    if (!exp.ptr) { print("[EVAL error] null ptr"); return nil_exp; }
    if (is_atom(exp)){
        atom_code *code = exp.ptr;
        imaginal_debug(trace,"[EVAL trace] Atomic expression");
        if (code->type == car_identifier){
            return assoc(exp, *env);
        }
        return exp;
    }
    s_exp_code *code = exp.ptr;
    if (is_atom(code->car)){
        string_slice s = car_id(code->car);
        imaginal_debug(trace,"[EVAL trace] Atomic car expression %v",s);
        if (slice_lit_match(s, "cond", true)){
            return evcon(code->cdr, *env);
        }
        else if (slice_lit_match(s, "quote", true)){
            return car(cdr(exp));
        } 
        else if (slice_lit_match(s, "list", true)){
            imaginal_debug(trace,"[EVAL trace] list");
            return make_list(evlis(code->cdr, env));
        }
        else if (slice_lit_match(s, "var", true) || slice_lit_match(s, "const", true)){
            codegen name = car(cdr(exp));
            if (slice_lit_match(s, "const", true) && assoc(name, *env).ptr) return nil_exp;
            *env = cons(
                    cons(name, car(cdr(cdr(exp)))), 
                *env);
            return nil_exp;
        }
        else if (slice_lit_match(s, "fun", true)/* || slice_lit_match(s, "macro", true)*/){
            codegen next = cdr(exp);
            codegen name = car(next);
            next = cdr(next);
            codegen lambda = make_lambda(next);
            *env = cons(
                    cons(name, lambda), 
                *env);
            return nil_exp;
        } else if (slice_lit_match(s, "let", true)){
            codegen local = letvars(car(cdr(exp)), *env);
            codegen body = cdr(cdr(exp));
            return eval_list(body, true, 0, &local);
        } else {
            return apply(code->car, evlis(code->cdr, env), env);
        }
    } 
    else {
        imaginal_debug(trace,"[EVAL trace] non-atomic expression");
        return apply(code->car, evlis(code->cdr, env), env);
    }
}