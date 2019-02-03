#include "streetpass/MBoxIcon.hpp"

namespace Streetpass {

MBoxIcon::MBoxIcon() : iconData() {

}

MBoxIcon::MBoxIcon(const std::vector<u8>& buffer) : iconData(buffer) {
    
}

std::vector<u8> MBoxIcon::data() const {
    return iconData;
}

u32 MBoxIcon::size() const {
    return iconData.size();
}

} // namespace Streetpass