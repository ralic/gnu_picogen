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

#ifndef SDLRENDERTARGET_HH_INCLUDED_20090307
#define SDLRENDERTARGET_HH_INCLUDED_20090307

#include "../rendertargets/rendertarget.hh"
#include "../sealed.hh"
#include <SDL/SDL.h>

namespace picogen { namespace redshift {

        SEALED(SdlRenderTarget);
        class SdlRenderTarget
                : public redshift::RenderTarget
                , MAKE_SEALED(SdlRenderTarget) {
        public:
                SdlRenderTarget (int width_, int height_,
                        std::string const & outputFile,
                        real_t linearOutputScale=1.f,
                        bool convertToSrgb=false
                );
                ~SdlRenderTarget();
                shared_ptr<RenderTargetLock> lock ();
                shared_ptr<RenderTargetLock const> lock () const;
                void flip();
                int getWidth() const;
                int getHeight() const;

        private:
                int width, height;
                SDL_Surface *display;
                std::string outputFile;
                real_t linearOutputScale;
                bool convertToSrgb;

                class SdlRenderTargetLock;
                friend class SdlRenderTargetLock;

                // disable
                SdlRenderTarget() ;
                SdlRenderTarget(SdlRenderTarget const&) ;
                SdlRenderTarget & operator = (SdlRenderTarget const &) ;
        };

} }

#endif // SDLRENDERTARGET_HH_INCLUDED_20090307
