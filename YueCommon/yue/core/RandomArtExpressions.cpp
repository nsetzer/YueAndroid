

#include <vector>
#include <math.h>   // cos, sin, pow
#include <random>
#include <iostream>

#include "yue/core/RandomArtExpressions.h"

#define PI 3.14159265

namespace yue {
namespace core {

namespace {

using namespace art;

Expr* newExpression(Random& rnd, int depth, float probability, float decay);
Expr* constructExpression(ExprKind kind);

class TerminalX : public Expr {

public:
    TerminalX()
        : Expr()
    {}
    virtual float eval(float x, float y) {
        return x;
    }
    virtual int size(void) {
        return 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        return {ExprKind::X,};
    }
    virtual size_t deserialize_(std::vector<ExprKind>, size_t index) {
        return 0;
    }

};

class TerminalY : public Expr {

public:
    TerminalY()
        : Expr()
    {}
    virtual float eval(float x, float y) {
        return y;
    }
    virtual int size(void) {
        return 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        return {ExprKind::Y,};
    }
    virtual size_t deserialize_(std::vector<ExprKind>, size_t index) {
        return 0;
    }
};

class TerminalAverage : public Expr {

public:
    TerminalAverage()
        : Expr()
    {}
    virtual float eval(float x, float y) {
        return (x+y)/2;
    }
    virtual int size(void) {
        return 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        return {ExprKind::Average,};
    }
    virtual size_t deserialize_(std::vector<ExprKind>, size_t index) {
        return 0;
    }
};

class ExprCos : public Expr {

    Expr* arg;
public:
    ExprCos() : Expr() {}
    ExprCos(Random& rnd, int depth, float probability, float decay)
        : Expr()
    {
        arg = newExpression(rnd, depth, probability, decay);
    }

    virtual ~ExprCos() {
        delete arg;
    }

    virtual float eval(float x, float y) {
        return cos(PI * arg->eval(x,y));
    }

    virtual int size(void) {
        return arg->size() + 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        std::vector<ExprKind> v = {ExprKind::Cos,};
        std::vector<ExprKind> v2 = arg->serialize_();
        v.insert(v.end(),v2.begin(),v2.end());
        return v;
    }
    virtual size_t deserialize_(std::vector<ExprKind> repr, size_t index) {
        if (index >= repr.size())
            throw std::runtime_error("deserialization error");
        arg = constructExpression(repr[index]);
        return arg->deserialize_(repr,index+1) + 1;
    }
};

class ExprSin : public Expr {

    Expr* arg;
public:
    ExprSin() : Expr() {}
    ExprSin(Random& rnd, int depth, float probability, float decay)
        : Expr()
    {
        arg = newExpression(rnd, depth, probability, decay);
    }

    virtual ~ExprSin() {
        delete arg;
    }

    virtual float eval(float x, float y) {
        return sin(PI * arg->eval(x,y));
    }

    virtual int size(void) {
        return arg->size() + 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        std::vector<ExprKind> v = {ExprKind::Sin,};
        std::vector<ExprKind> v2 = arg->serialize_();
        v.insert(v.end(),v2.begin(),v2.end());
        return v;
    }
    virtual size_t deserialize_(std::vector<ExprKind> repr, size_t index) {
        if (index >= repr.size())
            throw std::runtime_error("deserialization error");
        arg = constructExpression(repr[index]);
        return arg->deserialize_(repr,index+1) + 1;
    }
};

class ExprTanh : public Expr {

    Expr* arg;
public:
    ExprTanh() : Expr() {}
    ExprTanh(Random& rnd, int depth, float probability, float decay)
        : Expr()
    {
        arg = newExpression(rnd, depth, probability, decay);
    }

    virtual ~ExprTanh() {
        delete arg;
    }

    virtual float eval(float x, float y) {
        return tanh(PI * arg->eval(x,y));
    }

    virtual int size(void) {
        return arg->size() + 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        std::vector<ExprKind> v = {ExprKind::Tanh,};
        std::vector<ExprKind> v2 = arg->serialize_();
        v.insert(v.end(),v2.begin(),v2.end());
        return v;
    }
    virtual size_t deserialize_(std::vector<ExprKind> repr, size_t index) {
        if (index >= repr.size())
            throw std::runtime_error("deserialization error");
        arg = constructExpression(repr[index]);
        return arg->deserialize_(repr,index+1) + 1;
    }
};

class ExprInverse : public Expr {

    Expr* arg;
public:
    ExprInverse() : Expr() {}
    ExprInverse(Random& rnd, int depth, float probability, float decay)
        : Expr()
    {
        arg = newExpression(rnd, depth, probability, decay);
    }

    virtual ~ExprInverse() {
        delete arg;
    }

    virtual float eval(float x, float y) {
        return 1.0 - arg->eval(x,y);
    }

    virtual int size(void) {
        return arg->size() + 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        std::vector<ExprKind> v = {ExprKind::Inverse,};
        std::vector<ExprKind> v2 = arg->serialize_();
        v.insert(v.end(),v2.begin(),v2.end());
        return v;
    }
    virtual size_t deserialize_(std::vector<ExprKind> repr, size_t index) {
        if (index >= repr.size())
            throw std::runtime_error("deserialization error");
        arg = constructExpression(repr[index]);
        return arg->deserialize_(repr,index+1) + 1;
    }
};

class ExprSquared : public Expr {

    Expr* arg;
public:
    ExprSquared() : Expr() {}
    ExprSquared(Random& rnd, int depth, float probability, float decay)
        : Expr()
    {
        arg = newExpression(rnd, depth, probability, decay);
    }

    virtual ~ExprSquared() {
        delete arg;
    }

    virtual float eval(float x, float y) {
        float v = arg->eval(x,y);
        return v*v;
    }

    virtual int size(void) {
        return arg->size() + 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        std::vector<ExprKind> v = {ExprKind::Squared,};
        std::vector<ExprKind> v2 = arg->serialize_();
        v.insert(v.end(),v2.begin(),v2.end());
        return v;
    }
    virtual size_t deserialize_(std::vector<ExprKind> repr, size_t index) {
        if (index >= repr.size())
            throw std::runtime_error("deserialization error");
        arg = constructExpression(repr[index]);
        return arg->deserialize_(repr,index+1) + 1;
    }
};

class ExprTimes : public Expr {

    Expr* arg1;
    Expr* arg2;
public:
    ExprTimes() : Expr() {}
    ExprTimes(Random& rnd, int depth, float probability, float decay)
        : Expr()
    {
        arg1 = newExpression(rnd, depth, probability, decay);
        arg2 = newExpression(rnd, depth, probability, decay);
    }

    virtual ~ExprTimes() {
        delete arg1;
        delete arg2;
    }

    virtual float eval(float x, float y) {
        return arg1->eval(x,y) * arg2->eval(x,y);
    }

    virtual int size(void) {
        return arg1->size() + arg2->size() + 1;
    }
    virtual std::vector<ExprKind> serialize_() {
        std::vector<ExprKind> v = {ExprKind::Times,};
        std::vector<ExprKind> v2 = arg1->serialize_();
        std::vector<ExprKind> v3 = arg2->serialize_();
        v.insert(v.end(),v2.begin(),v2.end());
        v.insert(v.end(),v3.begin(),v3.end());
        return v;
    }
    virtual size_t deserialize_(std::vector<ExprKind> repr, size_t index) {

        if (index >= repr.size())
            throw std::runtime_error("deserialization error1");
        arg1 = constructExpression(repr[index]);
        size_t consumed1 = arg1->deserialize_(repr,index+1);

        if ( (index+1+consumed1) >= repr.size())
            throw std::runtime_error("deserialization error2");
        arg2 = constructExpression(repr[index+1+consumed1]);
        size_t consumed2 = arg2->deserialize_(repr,index+2+consumed1);
        return 2 + consumed1 + consumed2;
    }
};


Expr* newExpression(Random& rnd, int depth, float probability, float decay)
{
    //float p = probability * pow(decay,depth);
    // exponential decay
    // mean lifetime: 1 / lambda
    // lambda: decay / N
    // setting decay to 1/N will put the expect depth close to N
    float p;
    if (depth<0) // don't terminate for small values
        p = 1.0;
    else if (depth > 16)
        p = 0.0;
    else
        p = probability * exp(-decay*static_cast<float>(depth));


    if (rnd.rand() >= p) {
        switch(rnd.randInt(0,2)) {
            case 0: return new TerminalX();
            case 1: return new TerminalY();
            case 2: return new TerminalAverage();
            default:
                throw std::runtime_error("illegal i value");
        }
    } else {
        p = probability*decay;
        int max = 3;
        //if (depth < 3)
        //    max--;
        switch(rnd.randInt(0,max)) {
            case 0: return new ExprCos(rnd, depth+1, probability, decay);
            case 1: return new ExprSin(rnd, depth+1, probability, decay);
            case 2: return new ExprTanh(rnd, depth+1, probability, decay);
            case 3: return new ExprTimes(rnd, depth+1, probability, decay);
            case 4: return new ExprInverse(rnd, depth+1, probability, decay);
            case 5: return new ExprSquared(rnd, depth+1, probability, decay);
            default:
                throw std::runtime_error("illegal i value");
        }
    }

}

Expr* constructExpression(ExprKind kind)
{
    switch (kind) {
        case ExprKind::X:
            return new TerminalX();
        case ExprKind::Y:
            return new TerminalY();
        case ExprKind::Average:
            return new TerminalAverage();
        case ExprKind::Cos:
            return new ExprCos();
        case ExprKind::Sin:
            return new ExprSin();
        case ExprKind::Tanh:
            return new ExprTanh();
        case ExprKind::Inverse:
            return new ExprInverse();
        case ExprKind::Squared:
            return new ExprSquared();
        case ExprKind::Times:
            return new ExprTimes();
        default:
        {
            std::cout << "kind:" << static_cast<int>(kind) << std::endl;
            throw std::runtime_error("illegal kind value");
        }
    }
}

} // anonymous

namespace art {


std::string Expr::serialize() {
    std::vector<ExprKind> v = serialize_();
    std::string s;
    s.resize(v.size());
    size_t i=0;
    for (ExprKind kind : v) {
        char c = static_cast<char>(kind);
        // convert to hex
        if (c<10) {
            c += '0';
        } else {
            c += 'A' - 10;
        }
        s[i++]=c;
    }
    return s;
}

Expr* Expr::create(Random& rnd) {
    return newExpression(rnd,0,0.99F,1.0F/20.0F);
}

Expr* Expr::deserialize(std::string code) {
    // translate back to a vector of kinds
    std::vector<ExprKind> v;
    for (char c : code) {
        // convert to hex
        if (c<'A') {
            c -= '0';
        } else {
            c -= 'A' - 10; // TODO I THINK THERE IS A BUG HERE
        }
        v.push_back(static_cast<ExprKind>(c));
    }
    Expr* expr = constructExpression(v[0]);
    expr->deserialize_(v,1);
    return expr;
}

/*
int main() {
    Random rnd;

    Expr* expr = Expr::create(rnd);
    float f = expr->eval(0,0);
    std::cout << expr->size() << " " << f <<std::endl;
    std::string s = expr->serialize();
    std::cout <<"s1 " << s << std::endl;
    try {
        Expr* expr2 = Expr::deserialize(s);
        std::cout <<"s2 " << expr2->serialize() << std::endl;
    } catch (std::runtime_error& e) {
        std::cout << "error" << e.what() << std::endl;
    }

    return 0;
}
*/

} // namespace art

} // namespace core
} // namespace yue
