#ifndef SPHERE_H_20110711
#define SPHERE_H_20110711

#include "potentialintersection.h"
#include "ray.h"
#include "real.h"
#include "primitives/primitive.h"
#include "math3d.h"

namespace picogen { namespace cracker {

class Sphere : public Primitive {
public:
        Sphere() = delete;
        Sphere (Point center, real radius) :
                center_(center), radius_(radius)
        {}

        PotentialIntersection operator() (Ray const &ray) const {
                return intersect(ray);
        }

private:
        PotentialIntersection intersect (Ray const &ray) const {
                /*if (ray.direction().x()<0) {
                        const Intersection isect(std::fabs(100*ray.direction().x()));
                        return isect;
                }*/
                const Vector    &dst = ray.origin() - center_;

                const real         B = mixed_dot (dst, ray.direction()),
                                   C = dot(dst,dst) - radius_*radius_,
                                   D = B*B - C,
                                   E = -B - sqrt (D);

                if ((D>=0) & (E>=0)) {
                        return Intersection (E);
                }


                return PotentialIntersection();
        }

        Point center_;
        real radius_;
};

} }

#endif // SPHERE_H_20110711