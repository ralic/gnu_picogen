#include "mono-sun.h"

namespace picogen { namespace cracker {

MonoSun::MonoSun (const Color &color,
                  const Direction &dir)
: color_ (color), direction_(dir)
{
}

Ray MonoSun::deterministicShadowRay_(Point const &from) const {
        return Ray(from, -direction_);
}

Color MonoSun::radiance_() const {
        return color_;
}

} }

