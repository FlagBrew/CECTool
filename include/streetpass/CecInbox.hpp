#pragma once

#include <memory>
#include <vector>

#include "streetpass/Box.hpp"
#include "streetpass/BoxInfo.hpp"
#include "streetpass/Message.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

class CecInbox : public virtual Box {
public:
    explicit CecInbox(u32 id, std::unique_ptr<BoxInfo> inboxInfo);
    explicit CecInbox(u32 id);
    ~CecInbox() override;

    Result AddMessage(const Message& message) override;
    Result DeleteMessage(const CecMessageId& messageId) override;
    Result DeleteAllMessages() override;

    BoxInfo& Info() override;
    const BoxInfo& Info() const override;

    std::vector<Message> Messages() override;
    const std::vector<Message> Messages() const override;

private:
    u32 boxId;
    std::unique_ptr<BoxInfo> boxInfo;
    std::vector<Message> messages;
};

} // namespace Streetpass
