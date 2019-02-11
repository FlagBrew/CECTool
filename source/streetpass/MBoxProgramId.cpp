#include "streetpass/MBoxProgramId.hpp"

namespace Streetpass {

MBoxProgramId::MBoxProgramId() : programIdData() {

}

MBoxProgramId::MBoxProgramId(const std::vector<u8>& buffer) : programIdData(buffer) {
    
}

std::vector<u8> MBoxProgramId::data() const {
    return programIdData;
}

u32 MBoxProgramId::size() const {
    return programIdData.size();
}

} // namespace Streetpass