#include "segment.hpp"

namespace Geo3D 
{
std::ostream& operator<<(std::ostream& out, const Segment& segment) {
    return out << segment.v0_ << segment.v1_;
}
}