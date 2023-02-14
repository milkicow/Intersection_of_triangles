#include "triangle.hpp"

namespace Geo3D
{
std::ostream& operator<<(std::ostream& out, const Triangle& tr) {
    out << tr.v0_.x_ << " " << tr.v0_.y_ << " " << tr.v0_.z_ << " " << tr.v1_.x_ << " " << tr.v1_.y_ << " " << tr.v1_.z_ << " " << tr.v2_.x_ << " " << tr.v2_.y_ << " " << tr.v2_.z_ << "\n";
    return out;
}

}
