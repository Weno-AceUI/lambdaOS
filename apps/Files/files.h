#pragma once
#include "../../ui/graphics/GraphicsContext.h"
#include <string>

namespace lambdaOS {
namespace apps {

    class Files {
        public:
        Files();
        ~Files();

        void draw(GraphicsContext& ctx, int x, int y, int width, int height);
    }
}
}