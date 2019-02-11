#pragma once

#include <3ds/types.h>
#include <string>
#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class OBIndex {
public:
    explicit OBIndex();
    explicit OBIndex(const std::vector<u8>& buffer);
    ~OBIndex();

    bool AddMessageId(const CecMessageId& messageId);
    bool DeleteMessageId(const CecMessageId& messageId);
    bool DeleteAllMessageIds();

    std::vector<u8> data() const;
    std::vector<CecMessageId> MessageIds() const;

    u32 FileSize() const;
    u32 NumberOfMessages() const;

private:
    CecOBIndexHeader obIndexHeader;
    std::vector<CecMessageId> messageIds;

    const std::string filename = "OBIndex_____";
};

} // namespace Streetpass
