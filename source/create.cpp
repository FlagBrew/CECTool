#include <3ds.h>
#include <fstream>
#include "import.hpp"
#include "common/io.hpp"
#include "common/STDirectory.hpp"
#include "common/util.hpp"
#include "streetpass/BoxInfo.hpp"

#include "streetpass/MBoxList.hpp"
#include "create.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

using Streetpass::Box;
using Streetpass::MBoxListHeader;
using Streetpass::Message;

CreateBox::CreateBox(const std::string& id) : id(id), boxId(std::stoul(id, nullptr, 16)), mboxList() {

}

bool CreateBox::CheckMBoxList() {
    Result res = CECDU_OpenAndRead(sizeof(MBoxListHeader), boxId, CEC_PATH_MBOX_LIST, CEC_READ | CEC_CHECK, &mboxList, nullptr);
    if (R_FAILED(res)) {
        // shouldn't cause failure, mboxlist will be created by cecd if needed
        //printf("MBoxList OpenAndRead failed\n");
        //return false;
    } else {
        for (u32 i = 0; i < mboxList.numBoxes; i++)
        {
            if (id == mboxList.boxIds[i])
            {
                printf("Box already exists, idiot!\n");
                return false;
            }
        }
        if (mboxList.numBoxes == 12)
        {
            printf("Too many boxes already exist!\n");
            return false;
        }
    }
    return true;
}

bool CreateBox::CheckTemplateFiles() {
    const std::string templateDirs[] = {
        "/3ds/CECTool/template/" + id,
        "/3ds/CECTool/template/" + id + "/InBox___",
        "/3ds/CECTool/template/" + id + "/OutBox__"
    };

    const std::string templateFiles[] = {
        "/3ds/CECTool/template/" + id + "/MBoxInfo____",
        "/3ds/CECTool/template/" + id + "/MBoxData.001",
        "/3ds/CECTool/template/" + id + "/MBoxData.010",
        "/3ds/CECTool/template/" + id + "/MBoxData.050",
        "/3ds/CECTool/template/" + id + "/InBox___/BoxInfo_____",
        "/3ds/CECTool/template/" + id + "/OutBox__/BoxInfo_____",
        "/3ds/CECTool/template/" + id + "/OutBox__/OBIndex_____"
    };

    std::string missingFiles = "";
    for (auto file : templateDirs)
    {
        if (!io::folderExists(file))
        {
            missingFiles += '\t' + file + '\n';
        }
    }

    for (auto file : templateFiles)
    {
        if (!io::fileExists(file))
        {
            missingFiles += '\t' + file + '\n';
        }
    }

    if (!missingFiles.empty())
    {
        printf("Required template files missing!\n%s\n", missingFiles.c_str());
        return false;
    }
    return true;
}

bool CreateBox::CheckOutboxMessages() {
    STDirectory dir("/3ds/CECTool/" + id + "/OutBox__");
    if (!dir.good() || dir.count() < 1)
    {
        printf("No outbox messages. Continue?\nA: Yes\nB: No\n");
        hidScanInput();
        while (aptMainLoop() && !(hidKeysDown() & KEY_A) && !(hidKeysDown() & KEY_B))
        {
            hidScanInput();
            if (hidKeysDown() & KEY_B)
            {
                hidScanInput();
                return false;
            }
        }
        hidScanInput();
    }
    return true;
}

bool CreateBox::CreateFile(const std::string& templateFile, const CecDataPathType cecPath) {
    FILE* in = fopen(templateFile.c_str(), "r");
    fseek(in, 0, SEEK_END);
    const u32 filesize = ftell(in);
    fseek(in, 0, SEEK_SET);
    std::vector<u8> buffer(filesize);
    fread(buffer.data(), 1, filesize, in);
    fclose(in);

    Result res = CECDU_OpenAndWrite(filesize, boxId, cecPath, CEC_WRITE | CEC_CREATE | CEC_CHECK,
                                    buffer.data());
    if (R_FAILED(res)) {
        printf("CreateFile from template failed\n");
        return false;
    }
    return true;
}

bool CreateBox::CreateInbox() {
    Result res = CECDU_Open(boxId, CEC_PATH_INBOX_DIR, CEC_CREATE, nullptr);
    if (R_FAILED(res)) {
        printf("Create Inbox Directory failed\n");
        return false;
    }

    bool ret = CreateFile("/3ds/CECTool/template/" + id + "/InBox___/BoxInfo_____", CEC_PATH_INBOX_INFO);
    if (!ret) {
        printf("/InBox___/BoxInfo_____ failed to create\n");
        return false;
    }
    return true;
}

bool CreateBox::CreateOutbox() {
    Result res = CECDU_Open(boxId, CEC_PATH_OUTBOX_DIR, CEC_CREATE, nullptr);
    if (R_FAILED(res)) {
        printf("Create Outbox Directory failed\n");
        return false;
    }

    bool ret = CreateFile("/3ds/CECTool/template/" + id + "/OutBox__/BoxInfo_____", CEC_PATH_OUTBOX_INFO);
    if (!ret) {
        printf("/OutBox__/BoxInfo_____ failed to create\n");
        return false;
    }

    // Creates obindex header only, 8 bytes.
    ret = CreateFile("/3ds/CECTool/template/" + id + "/OutBox__/OBIndex_____", CEC_PATH_OUTBOX_INDEX);
    if (!ret) {
        printf("/OutBox__/OBIndex_____ failed to create\n");
        return false;
    }

    return true;
}

bool CreateBox::CreateMessage(const std::string& templateFile) {
    FILE* in = fopen(templateFile.c_str(), "r");
    fseek(in, 0, SEEK_END);
    const u32 filesize = ftell(in);
    fseek(in, 0, SEEK_SET);
    std::vector<u8> buffer(filesize);
    fread(buffer.data(), 1, filesize, in);
    fclose(in);

    std::unique_ptr<Message> message = std::make_unique<Message>(buffer);
    Result res = box->addMessage(*message);
    if(R_FAILED(res)) {
        printf("box->addMessage failed\n");
        return false;
    }
    return true;
}

bool CreateBox::CreateOutboxMessages() {
    box = std::make_unique<Box>(boxId, true);
    STDirectory dir("/3ds/CECTool/" + id + "/OutBox__");
    const std::string outboxDir = "/3ds/CECTool/" + id + "/OutBox__/";
    
    for (size_t j = 0; j < dir.count() && box->getMessages().size() < box->getInfo().maxMessages(); j++)
    {
        CreateMessage((outboxDir + dir.item(j)));
    }

    box->saveInfo();
    return true;
}

bool CreateBox::CreateMBoxData() {
    Result res = CECDU_Open(boxId, CEC_PATH_MBOX_DIR, CEC_CREATE, nullptr);
    if (R_FAILED(res)) {
        printf("Create MBox Directory failed\n");
        return false;
    }

    bool ret = CreateFile("/3ds/CECTool/template/" + id + "/MBoxInfo____", CEC_PATH_MBOX_INFO);
    if(!ret) {
        printf("MBoxInfo____ failed to create\n");
        return false;
    }
    ret = CreateFile("/3ds/CECTool/template/" + id + "/MBoxData.001", CECMESSAGE_BOX_ICON);
    if(!ret) {
        printf("MBoxData.001 failed to create\n");
        return false;
    }
    ret = CreateFile("/3ds/CECTool/template/" + id + "/MBoxData.010", CECMESSAGE_BOX_TITLE);
    if(!ret) {
        printf("MBoxData.010 failed to create\n");
        return false;
    }
    ret = CreateFile("/3ds/CECTool/template/" + id + "/MBoxData.050", CecDataPathType(150));
    if(!ret) {
        printf("MBoxData.050 failed to create\n");
        return false;
    }
    return true;
}

bool CreateBox::UpdateMBoxList() {
    std::copy(id.begin(), id.end(), mboxList.boxIds[mboxList.numBoxes++]);
    Result res = CECDU_OpenAndWrite(sizeof(MBoxListHeader), boxId, CEC_PATH_MBOX_LIST,
                                    CEC_WRITE | CEC_CREATE | CEC_CHECK, &mboxList);
    if (R_FAILED(res)) {
        printf("Update MBoxList failed\n");
        return false;
    }
    return true;
}

void createBox(const std::string& id)
{
    if (id.size() != 8 || id.find_first_not_of("1234567890abcdefABCDEF") != std::string::npos)
    {
        printf("Wrong ID format! What did you do?!\n");
        printf("Input: %s\n", id.c_str());
        return;
    }

    std::unique_ptr<CreateBox> create = std::make_unique<CreateBox>(id);
    if (!create->CheckMBoxList()) return;
    if (!create->CheckTemplateFiles()) return;
    if (!create->CheckOutboxMessages()) return;
    if (!create->CreateMBoxData()) return;
    if (!create->CreateInbox()) return;
    if (!create->CreateOutbox()) return;
    if (!create->CreateOutboxMessages()) return;
    if (!create->UpdateMBoxList()) return;
}

