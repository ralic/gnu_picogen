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

#ifndef STOPWATCH_HH_INCLUDED_20100125
#define STOPWATCH_HH_INCLUDED_20100125

#include <omp.h>
namespace redshift {
        class StopWatch {
                double begin, end;
                bool stopped;
        public:
                StopWatch ()
                : begin (omp_get_wtime())
                , stopped(false)
                {}

                void restart () {
                        stopped = false;
                        begin = omp_get_wtime();
                }

                double stop () {
                        stopped = true;
                        end = omp_get_wtime();
                        return end-begin;
                }

                double operator () () const {
                        if (stopped) {
                                return end - begin;
                        } else {
                                return omp_get_wtime() - begin;
                        }
                }

        };
}

#endif // STOPWATCH_HH_INCLUDED_20100125