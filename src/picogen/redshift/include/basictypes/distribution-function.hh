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

#ifndef DISTRIBUTIONFUNCTION_HH_INCLUDED_20100821
#define DISTRIBUTIONFUNCTION_HH_INCLUDED_20100821

#include "../real.hh"

namespace picogen { namespace redshift {


class DistributionFunction {
public:
        class Probability {
        public:
                explicit Probability (real_t value)
                : val(value<0 ? 0 : value>1 ? 1 : value)
                {}

                real_t value() const {
                        return val;
                }
        private:
                real_t val;
        };

        typedef Probability return_type;

        virtual
         Probability operator ()
           (real_t const &u, real_t const &v)
         const = 0;
};

} }


#endif // DISTRIBUTIONFUNCTION_HH_INCLUDED_20100821
