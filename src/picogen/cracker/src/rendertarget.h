#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <cstddef>
#include <vector>
#include "pixel.h"

namespace picogen { namespace cracker {

class RenderTargetRow;
class ConstRenderTargetRow;

class RenderTarget
{
public:
        RenderTarget(unsigned int width,
                     unsigned int height);

        RenderTargetRow row (size_t y);
        ConstRenderTargetRow row (size_t y) const;
        unsigned int width () const;
        unsigned int height() const;

private:
        unsigned int width_, height_;
        std::vector<Pixel> pixels_;
};

} }

#endif // RENDERTARGET_H
