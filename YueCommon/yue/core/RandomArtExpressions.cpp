

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
};

// this introduces a slant from bottom left to top right
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
};

// this introduces a slant from top left to bottom right
class TerminalAverage2 : public Expr {

public:
    TerminalAverage2()
        : Expr()
    {}
    virtual float eval(float x, float y) {
        return (x-y)/2;
    }
    virtual int size(void) {
        return 1;
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
        switch(rnd.randInt(0,3)) {
            case 0: return new TerminalX();
            case 1: return new TerminalY();
            case 2: return new TerminalAverage();
            case 3: return new TerminalAverage2();

            default:
                throw std::runtime_error("illegal i value");
        }
    } else {
        switch(rnd.randInt(0,3)) {

            case 0: return new ExprTimes(rnd, depth+1, probability, decay);
            case 1: return new ExprCos(rnd, depth+1, probability, decay);
            case 2: return new ExprSin(rnd, depth+1, probability, decay);
            case 3: return new ExprTanh(rnd, depth+1, probability, decay);
            // these actually reduce complexity in the image.
            //case 4: return new ExprSquared(rnd, depth+1, probability, decay);
            //case 5: return new ExprInverse(rnd, depth+1, probability, decay);

            default:
                throw std::runtime_error("illegal i value");
        }
    }

}

} // anonymous

namespace art {

Expr* Expr::create(Random& rnd) {
    return newExpression(rnd,0,0.99F,1.0F/20.0F);
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
