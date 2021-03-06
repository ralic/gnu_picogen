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


#ifndef STASH_DEF_HH_20100902
#define STASH_DEF_HH_20100902

#include <ctime>
#include <vector>
#include "../stash.hh"

namespace picogen { namespace cosyscene {
template <typename T>
template <typename Arch>
inline void StashObject<T>::serialize (Arch &arch) {
        using actuarius::pack;
        arch & pack("value", value_);
        arch & pack("description", description_);
}

template <typename T>
template <typename Arch>
inline void Stash<T>::serialize (Arch &arch) {
        using actuarius::pack;
        arch & pack (&StashObject<T>::time_, objects);
}

} }

#endif // STASH_DEF_HH
