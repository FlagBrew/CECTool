#pragma once

#include <3ds/types.h>
#include <memory>
#include <string>

#include "streetpass/Box.hpp"
#include "streetpass/MBoxList.hpp"

class CreateBox {
public:
    explicit CreateBox(const std::string& id);
    ~CreateBox() = default;

    bool CheckMBoxList();
    bool CheckTemplateFiles();
    bool CheckOutboxMessages();

    bool CreateFile(const std::string& templateFile, const CecDataPathType cecPath);
    bool CreateMessage(const std::string& templateFile);

    bool CreateInbox();
    bool CreateOutbox();
    bool CreateOutboxMessages();

    bool CreateMBoxData();

    bool UpdateMBoxList();

private:
    std::string id;
    u32 boxId;

    Streetpass::MBoxListHeader mboxList;
    std::unique_ptr<Streetpass::Box> box;
};

void createBox(const std::string& id);
