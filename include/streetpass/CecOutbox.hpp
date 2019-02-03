#pragma once

#include <memory>
#include <vector>

#include "streetpass/Box.hpp"
#include "streetpass/BoxInfo.hpp"
#include "streetpass/Message.hpp"
#include "streetpass/OBIndex.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class CecOutbox : public virtual Box {
public:
    explicit CecOutbox(u32 id);
    CecOutbox(u32 id, std::unique_ptr<BoxInfo> outboxInfo, std::unique_ptr<OBIndex> outboxIndex);
    ~CecOutbox() override;

    Result AddMessage(const Message& message) override;
    Result DeleteMessage(const CecMessageId& messageId) override;
    Result DeleteAllMessages() override;

    OBIndex& Index();
    const OBIndex& Index() const;

    BoxInfo& Info() override;
    const BoxInfo& Info() const override;

    std::vector<Message> Messages() override;
    const std::vector<Message> Messages() const override;

private:
    u32 boxId;
    std::unique_ptr<BoxInfo> boxInfo;
    std::unique_ptr<OBIndex> obIndex;

    std::vector<Message> messages;
};

} // namespace Streetpass
