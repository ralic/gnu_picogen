/***************************************************************************
 *            PicoPico.h
 *
 *  May 03 12:20:00 2008
 *  Copyright  2008  Sebastian Mach
 *  seb@greenhybrid.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifndef _PICOPICO_H
#define _PICOPICO_H

#include <picogen/misc/scripting/AST.h>

namespace picogen {

    /// \todo find proper namespace for this
    class PicoPico {

        public:
            typedef enum {
                delete_ast=0,
                return_ast=1
            } Flags;

            PicoPico (const std::string &code, Flags flags=delete_ast, const ExprAST **ast=0); // stub!
            virtual ~PicoPico();

    };

} // namespace picogen{

#endif // #ifndef PICOPICO