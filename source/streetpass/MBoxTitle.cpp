#include "streetpass/MBoxTitle.hpp"

namespace Streetpass {

MBoxTitle::MBoxTitle() : titleData() {

}

MBoxTitle::MBoxTitle(const std::vector<u8>& buffer) : titleData(buffer) {
    
}

std::vector<u8> MBoxTitle::data() const {
    return titleData;
}

u32 MBoxTitle::size() const {
    return titleData.size();
}

} // namespace Streetpass