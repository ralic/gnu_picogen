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

#include "../../include/rendertargets/sdlrendertarget.hh"
#include <stdexcept>
#include <sstream>

namespace picogen { namespace redshift {


struct SdlRenderTarget::SdlRenderTargetLock : redshift::RenderTargetLock {

        redshift::SdlRenderTarget const & display;
        const real_t linearOutputScale;
        const bool convertToSrgb;

        SdlRenderTargetLock (
                redshift::SdlRenderTarget const & display_,
                real_t linearOutputScale,
                bool convertToSrgb
        )
        : display (display_)
        , linearOutputScale(linearOutputScale)
        , convertToSrgb(convertToSrgb)
        {
                if (SDL_MUSTLOCK (display.display) &&
                        SDL_LockSurface (display.display)<0) {
                        std::stringstream ss;
                        ss << "Failed to lock display in SdlRenderTarget::"
                           << "SdlRenderTargetLock()";
                        throw std::runtime_error (ss.str());
                }
                SDL_WM_SetCaption ("redshift-sdl.", "redshift-sdl.");
        }

        ~SdlRenderTargetLock () {
                if (SDL_MUSTLOCK (display.display))
                        SDL_UnlockSurface (display.display);
        }

        void setPixel (int x, int y, redshift::Color const &color) {
                if (static_config::debug) {
                        if (x<0 || x>=display.getWidth()
                             || y<0 || y>=display.getHeight()
                        ) {
                                std::stringstream ss;
                                ss << "Invalid coordinates passed to "
                                   << "ColorRenderTargetLock: "
                                   << '(' << x << ',' << y << ')'
                                   << ", but resolution is "
                                   << display.getWidth() << "x"
                                   << display.getHeight();
                                throw std::out_of_range (ss.str());
                        }
                }

                Uint32 &bufp = static_cast<Uint32*>(display.display->pixels)
                                          [y * (display.display->pitch/4) + x];


                //Rgb const rgb = saturate (color,0,1).toRgb(); // TODO: strange, saturate yields NaNs?
                color::RGB const rgb = color.toRGB();

                if (convertToSrgb) {
                        const color::SRGB srgb = rgb.toSRGB();
                        const int r_ = (int)(255.f * srgb.R * linearOutputScale);
                        const int g_ = (int)(255.f * srgb.G * linearOutputScale);
                        const int b_ = (int)(255.f * srgb.B * linearOutputScale);
                        const int r = r_<0?0:r_>255?255:r_;
                        const int g = g_<0?0:g_>255?255:g_;
                        const int b = b_<0?0:b_>255?255:b_;
                        bufp = SDL_MapRGB(display.display->format,r,g,b);
                } else {
                        const int r_ = (int)(255.f * rgb.R * linearOutputScale);
                        const int g_ = (int)(255.f * rgb.G * linearOutputScale);
                        const int b_ = (int)(255.f * rgb.B * linearOutputScale);
                        const int r = r_<0?0:r_>255?255:r_;
                        const int g = g_<0?0:g_>255?255:g_;
                        const int b = b_<0?0:b_>255?255:b_;
                        bufp = SDL_MapRGB(display.display->format,r,g,b);
                }
        }

        Color getPixel (int x, int y) const {
                // TODO STUB
                if (static_config::debug) {
                        if (x<0 || x>=display.getWidth()
                             || y<0 || y>=display.getHeight()
                        ) {
                                std::stringstream ss;
                                ss << "Invalid coordinates passed to "
                                   << "ColorRenderTargetLock: "
                                   << '(' << x << ',' << y << ')'
                                   << ", but resolution is "
                                   << display.getWidth() << "x"
                                   << display.getHeight();
                                throw std::out_of_range (ss.str());
                        }
                }
                return Color();
        }
};



SdlRenderTarget::SdlRenderTarget (
        int width_, int height_,
        std::string const &outputFile,
        real_t linearOutputScale, bool convertToSrgb
)
: width(width_), height(height_)
, outputFile (outputFile)
, linearOutputScale(linearOutputScale)
, convertToSrgb(convertToSrgb)
{
        using namespace std;

        // Create a new window.
        display = SDL_SetVideoMode (width, height, 32,
                                        SDL_HWSURFACE | SDL_DOUBLEBUF);
        if (0 == display) {
                stringstream ss;
                ss << "Unable to set video-mode to "
                   << width << "x" << height << "x" << 32
                   << ": " << SDL_GetError();
                throw runtime_error (ss.str());
        }
}



SdlRenderTarget::~SdlRenderTarget() {
        if (outputFile.length())
        if (0 != SDL_SaveBMP (display, outputFile.c_str())) {
                std::cout << "Error while writing output file. Trying \"tmp.bmp\"."
                        << std::endl;
                SDL_SaveBMP (display, "tmp.bmp");
        }
}



shared_ptr<RenderTargetLock> SdlRenderTarget::lock () {
        return shared_ptr<RenderTargetLock> (new SdlRenderTargetLock (
                *this, linearOutputScale, convertToSrgb
        ));
}



shared_ptr<RenderTargetLock const> SdlRenderTarget::lock () const {
        return shared_ptr<RenderTargetLock const> (new SdlRenderTargetLock (
                *this, linearOutputScale, convertToSrgb
        ));
}



void SdlRenderTarget::flip() {
        SDL_Flip (display);
}



int SdlRenderTarget::getWidth() const {
        return width;
}



int SdlRenderTarget::getHeight() const {
        return height;
}

} }
