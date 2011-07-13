#ifndef INTERSECTION_H_20110708
#define INTERSECTION_H_20110708

#include "real.h"
#include <cassert>

namespace picogen { namespace cracker {

class Intersection {
public:
        Intersection() = delete;
        explicit Intersection (real distance) : distance_(distance)
        {
                assert(distance>=0);
        }

        real distance() const { return distance_; }
private:
        real distance_;
};

inline bool nearer (const Intersection &lhs, const Intersection &rhs) {
        return lhs.distance() < rhs.distance();
}

} }

#endif // INTERSECTION_H_20110708
