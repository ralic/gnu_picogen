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

#ifndef CREATEREDSHIFTSCENECLOSURE_HH
#define CREATEREDSHIFTSCENECLOSURE_HH

#include "redshift/include/smart_ptr.hh"
namespace picogen { namespace redshift_file {
        class Scene;
} }


namespace picogen { namespace qt4_gui {
class CreateRedshiftSceneClosure {
public:
        typedef redshift::shared_ptr<CreateRedshiftSceneClosure> Ptr;

public:
        virtual redshift::shared_ptr< ::picogen::redshift_file::Scene>
                        createPreviewScene   () const = 0;
        virtual redshift::shared_ptr< ::picogen::redshift_file::Scene>
                        createProductionScene() const = 0;
};

} }

#endif // CREATEREDSHIFTSCENECLOSURE_HH
