#ifndef GRID_TERRAIN_H_20110715
#define GRID_TERRAIN_H_20110715

#include "real.h"
#include <vector>

namespace picogen { namespace cracker {

class PotentialIntersection;
class Ray;

class GridTerrain {
public:
        GridTerrain ();
        PotentialIntersection operator() (Ray const &ray) const;
private:
        unsigned int heightfieldWidth_, heightfieldDepth_;
        std::vector<real> heightfield_;
};

} }

#endif // GRID_TERRAIN_H_20110715