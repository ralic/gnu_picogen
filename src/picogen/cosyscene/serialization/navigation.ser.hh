//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2010  Sebastian Mach (*1983)
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


#ifndef NAVIGATION_SERIALIZATION_HH_20101015
#define NAVIGATION_SERIALIZATION_HH_20101015

#include "../navigation.hh"
#include "cosyscene/serialization/stash.ser.hh"
#include "cosyscene/serialization/point3d.ser.hh"


namespace picogen { namespace cosyscene {

template<typename Arch>
inline void YawPitchRoll::serialize (Arch &arch) {
        using actuarius::pack;
        arch & pack("position", position);
        arch & pack("yaw", yaw);
        arch & pack("pitch", pitch);
        arch & pack("roll", roll);
}

template<typename Arch>
inline void Navigation::serialize (Arch &arch) {
        using actuarius::pack;
        if (Arch::deserialize || !stash_.empty())
                arch & pack("stash", stash_);

        arch & pack("type", kind_, Typenames);

        switch (kind_) {
        case YawPitchRoll: arch & pack ("parameters", yawPitchRoll_); break;
        case None: break;
        }
}

} }

#endif // NAVIGATION_SERIALIZATION_HH_20101015
