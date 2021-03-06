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

#ifndef CAMERA_HH_INCLUDED_20090310
#define CAMERA_HH_INCLUDED_20090310

#include "../geometry.hh"
#include "../sealed.hh"
#include "../tuple.hh"

namespace picogen { namespace redshift {
        class RenderTarget;
        class Sample;
} }

namespace picogen { namespace redshift {
        class Camera {
        public:
                virtual bool hasCommonCenter () const = 0;
                virtual Point getCommonCenter () const = 0;

                virtual tuple<real_t,RayDifferential>
                                generateRay (Sample const &) const = 0;

                Camera() : transform() {}
                Camera(Transform const &xform) : transform(xform) {}
                virtual ~Camera () {}

                virtual void setTransform (Transform const &t) {
                        transform = t;
                }
        protected:
                Transform transform;
        };
} }

#endif // CAMERA_HH_INCLUDED_20090310
