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

#ifndef QIMAGERENDERTARGET_HH_INCLUDED_20090327
#define QIMAGERENDERTARGET_HH_INCLUDED_20090327

#include "../../redshift/include/setup.hh"
#include "../../redshift/include/rendertargets/rendertargetlock.hh"
#include "../../redshift/include/rendertargets/rendertarget.hh"
#include "../../redshift/include/smart_ptr.hh"

#include <QImage>

namespace redshift {

        DefineFinalizer(QImageRenderTarget);
        class QImageRenderTarget
                : public redshift::RenderTarget
                , DoFinalize(QImageRenderTarget) {
        public:
                QImageRenderTarget (int width_, int height_,
                                    double colorscale, bool toSRGB);
                ~QImageRenderTarget();
                
                shared_ptr<RenderTargetLock> lock ();
                shared_ptr<RenderTargetLock const> lock () const;
                
                void flip();        
                int getWidth() const;        
                int getHeight() const;
                
                operator QImage ();

        private:         
                int width, height;                
                QImage display;
                double colorscale;
                bool toSRGB;
                
                class QImageRenderTargetLock;
                friend class QImageRenderTargetLock;
                
                class QImageRenderTargetConstLock;
                friend class QImageRenderTargetConstLock;

                // disable
                QImageRenderTarget() ;
                QImageRenderTarget(QImageRenderTarget const&) ;
                QImageRenderTarget & operator = (QImageRenderTarget const &) ;                                
        };

} // namespace redshift

#endif // QIMAGERENDERTARGET_HH_INCLUDED_20090327