#pragma once

#include <vector>

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class BoxInfo {
public:
    explicit BoxInfo();
    explicit BoxInfo(const std::vector<u8>& buffer);
    ~BoxInfo();

    bool AddMessageHeader(const CecMessageHeader& messageHeader);
    bool DeleteMessageHeader(const CecMessageId& messageId);
    bool DeleteAllMessageHeaders();

    u32 BoxSize() const;
    u32 FileSize() const;
    u32 MaxBatchSize() const;
    u32 MaxBoxSize() const;
    u32 MaxMessages() const;
    u32 MaxMessageSize() const;
    u32 NumberOfMessages() const;

    std::vector<u8> data() const;

    CecBoxInfoHeader& Header();
    const CecBoxInfoHeader& Header() const;

    std::vector<CecMessageHeader> MessageHeaders();
    const std::vector<CecMessageHeader> MessageHeaders() const;

private:
    CecBoxInfoHeader boxInfoHeader;
    std::vector<CecMessageHeader> messageHeaders;
};

} // namespace Streetpass
