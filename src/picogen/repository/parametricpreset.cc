//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copyright (C) 2011  Sebastian Mach (*1983)
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

#include "parametricpreset.hh"

#include <QDir>
#include <QFile>

namespace picogen { namespace repository {

ParametricPreset::ParametricPreset(Collection package, QString path)
        : Fragment (package, path)
{
        reload();
}

ParametricPreset::ParametricPreset() {
}

void ParametricPreset::reload() {
        Fragment::reload();
        preset_ = read ("preset");
}


void ParametricPreset::save() const {
        Fragment::save();
        write (preset_, "preset");
}

QString ParametricPreset::preset() const {
        return preset_;
}

/*void ParametricPreset::setName(QString s) {
        name_ = s;
}*/

void ParametricPreset::setPreset(QString s) {
        preset_ = s;
}

} }
