//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2009  Sebastian Mach (*1983)
// * mail: phresnel/at/gmail/dot/com
// * http://phresnel.org
// * http://picogen.org
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "ignore_strict_aliasing" // because of boost::optional
#include "../../include/shapes/closedsphere.hh"
#include "../../include/tuple.hh"
#include "../../include/algorithm.hh"
#include "../../include/basictypes/differentialgeometry.hh"


namespace picogen { namespace redshift { namespace shape {



ClosedSphere::ClosedSphere (Point const & center, real_t radius)
: sphereData(center, radius) {
}



BoundingBox ClosedSphere::boundingBox () const {
        const Point min (
                sphereData.center.x-sphereData.radius,
                sphereData.center.y-sphereData.radius,
                sphereData.center.z-sphereData.radius
        );
        const Point max (
                sphereData.center.x+sphereData.radius,
                sphereData.center.y+sphereData.radius,
                sphereData.center.z+sphereData.radius
        );
        return BoundingBox(min, max);
}



bool ClosedSphere::doesIntersect (Ray const &ray) const {
        if (kallisto::intersect<true> (ray, sphereData))
                return true;
        else
                return false;
}




optional<DifferentialGeometry> ClosedSphere::intersect(Ray const &ray) const {
        const optional<real_t> d = kallisto::intersect<true> (ray, sphereData);
        if (!d) {
                return false;
        }

        const Point  poi = ray (*d);
        const Normal normal =
                     normalize (vector_cast<Normal> (poi - sphereData.center));

        const tuple<Vector,Vector,Vector> cs = coordinateSystem (normal);

        return DifferentialGeometry (
                poi,
                normal,
                get<0>(cs), get<2>(cs),
                Vector(),Vector());
}



} } }
