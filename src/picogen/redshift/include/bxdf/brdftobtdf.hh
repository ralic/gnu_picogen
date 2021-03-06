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

#ifndef BRDFTOBTDF_HH_INCLUDED_20100825
#define BRDFTOBTDF_HH_INCLUDED_20100825

#include "../basictypes/bsdf.hh"

namespace picogen { namespace redshift { namespace bsdf {
        SEALED(BrdfToBtdf);
        class BrdfToBtdf : public Bxdf, MAKE_SEALED(BrdfToBtdf) {
        public:
                BrdfToBtdf (shared_ptr<const Bxdf> brdf);

                BsdfSample sample_f (
                        const Vector &in, Random &
                ) const ;

                Color f (const Vector &out, const Vector &in, Random &) const ;
                real_t pdf (const Vector &out, const Vector &in) const;

        private:
                shared_ptr<const Bxdf> brdf;
        };
} } }

#endif // BRDFTOBTDF_HH_INCLUDED_20100825
