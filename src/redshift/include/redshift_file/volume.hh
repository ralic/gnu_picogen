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

#ifndef VOLUME_HH_20101013
#define VOLUME_HH_20101013

namespace redshift_file {
        struct Volume {
                enum Type {
                        homogeneous,
                        exponential
                };
                static const actuarius::Enum<Type> Typenames;
                Type type;

                shared_ptr<redshift::VolumeRegion> toVolume () const {
                        switch (type) {
                        case homogeneous:
                                return shared_ptr<redshift::VolumeRegion> (
                                   new redshift::volume::Homogeneous(
                                        sigma_a.toColor(redshift::IlluminantSpectrum),
                                        sigma_s.toColor(redshift::IlluminantSpectrum),
                                        Lve.toColor(redshift::IlluminantSpectrum),
                                        hg
                                ));
                        case exponential:
                                return shared_ptr<redshift::VolumeRegion> (
                                   new redshift::volume::Exponential(
                                        sigma_a.toColor(redshift::IlluminantSpectrum),
                                        sigma_s.toColor(redshift::IlluminantSpectrum),
                                        Lve.toColor(redshift::IlluminantSpectrum),
                                        hg,
                                        baseFactor,
                                        exponentFactor,
                                        redshift::Point(min.x, min.y, min.z),
                                        redshift::Vector(up.x, up.y, up.z),
                                        epsilon
                                ));
                        };
                        return shared_ptr<redshift::VolumeRegion>();
                }

                Color sigma_a, sigma_s, Lve;
                double hg;

                Normal up;
                Point min;
                real_t baseFactor, exponentFactor;
                double epsilon; // minimum supported density, required to build bounding volumes

                Volume ()
                : sigma_a(0,0,0)
                , sigma_s(0,0,0)
                , Lve(0,0,0)
                , hg(0)
                , up(0,1,0)
                , min(0,0,0)
                , baseFactor(1.f)
                , exponentFactor(0.05)
                , epsilon(0.05)
                {}

                // Serialization.
                template<typename Arch>
                void serialize (Arch &arch);
        };
}

#endif // VOLUME_HH_20101013
