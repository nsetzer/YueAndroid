
#ifndef YUE_CORE_ART_RANDOMARTEXPR_H
#define YUE_CORE_ART_RANDOMARTEXPR_H

#include <vector>

#include "yue/core/util/random.h"

namespace yue {
namespace core {
namespace art {

class Expr {
public:

    Expr() {}
    virtual ~Expr() = default;

    virtual float eval(float x, float y) = 0;
    virtual int size(void) = 0;

    static Expr* create(Random& rnd);
    static Expr* deserialize(std::string code);

};

} // namespace art

} // namespace core
} // namespace yue
#endif
