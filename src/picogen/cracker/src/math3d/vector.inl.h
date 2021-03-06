#ifndef VECTOR_INL_H_20110715
#define VECTOR_INL_H_20110715

#include "vector.h"
#include "normal.h"
#include "direction.h"
#include "math3d.h" // to enforce existence of mixed_dot
#include <cassert>

namespace picogen { namespace cracker {
        inline Vector::operator Direction () const {
                return Direction(x_, y_, z_);
        }

        inline Vector::operator Normal() const {
                return Normal(x_,y_,z_);
        }


        inline Vector operator+ (Vector lhs, Vector const &rhs) {
                return lhs += rhs;
        }

        inline Vector operator- (Vector lhs, Vector const &rhs) {
                return lhs -= rhs;
        }

        inline real dot (Vector const &lhs, Vector const &rhs) {
                return mixed_dot (lhs, rhs);
        }

        inline Vector operator* (Vector lhs, real rhs) {
                return lhs *= rhs;
        }
        inline Vector operator* (real lhs, Vector rhs) {
                return rhs *= lhs;
        }

        inline Vector operator/ (Vector lhs, real rhs) {
                assert (rhs != 0);
                return lhs /= rhs;
        }
} }
#endif // VECTOR_INL_H_20110715
