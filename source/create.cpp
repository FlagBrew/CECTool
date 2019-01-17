#include <3ds.h>
#include <fstream>
#include "import.hpp"
#include "common/io.hpp"
#include "common/STDirectory.hpp"
#include "common/util.hpp"
#include "streetpass/BoxInfo.hpp"
#include "streetpass/Box.hpp"
#include "streetpass/MBoxList.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

using Streetpass::Box;
using Streetpass::boxList;
using Streetpass::Message;

void createBox(std::string id)
{
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
    const cecDataPath filePaths[] = {
        CEC_PATH_MBOX_INFO,
        CECMESSAGE_BOX_ICON,
        CECMESSAGE_BOX_TITLE,
        CecDataPathType(150),
        CEC_PATH_INBOX_INFO,
        CEC_PATH_OUTBOX_INFO,
        CEC_PATH_OUTBOX_INDEX
    };
    const cecDataPath folderPaths[] = {
        CEC_PATH_MBOX_DIR,
        CEC_PATH_INBOX_DIR,
        CEC_PATH_OUTBOX_DIR
    };
    if (id.size() != 8 || id.find_first_not_of("1234567890abcdefABCDEF") != std::string::npos)
    {
        printf("Wrong ID format! What did you do?!\n");
        printf("Input: %s\n", id.c_str());
        return;
    }
    //Result res; currently unused
    boxList list;
    u32 size = sizeof(boxList);
    // What happens if mboxlist doesn't exist?
    CECDU_Open(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_CHECK, nullptr);
    CECDU_Read(size, &list, nullptr);
    for (u32 i = 0; i < list.numBoxes; i++)
    {
        if (id == list.boxId[i])
        {
            printf("Box already exists, idiot!\n");
            return;
        }
    }
    if (list.numBoxes == 12)
    {
        printf("Too many boxes already exist!\n");
        return;
    }
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
        printf("Required template files missing!\n%s", missingFiles.c_str());
        return;
    }
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
                return;
            }
        }
        hidScanInput();
    }
    std::copy(id.begin(), id.end(), list.boxId[list.numBoxes++]);
    u32 boxId = std::stoul(id, nullptr, 16);
    for (size_t i = 0; i < 3; i++)
    {
        u8 data;
        CECDU_OpenAndWrite(0, boxId, folderPaths[i], CEC_CREATE | CEC_WRITE, &data);
    }
    u32 filesize = 0;
    for (size_t i = 0; i < 7; i++)
    {
        FILE* in = fopen(templateFiles[i].c_str(), "r");
        fseek(in, 0, SEEK_END);
        filesize = ftell(in);
        fseek(in, 0, SEEK_SET);
        u8* data = new u8[filesize];
        fread(data, 1, filesize, in);
        fclose(in);
        CECDU_OpenAndWrite(filesize, boxId, filePaths[i], CEC_WRITE | CEC_CREATE, data);
        delete[] data;
    }
    Box box(boxId, true);
    std::string outboxDir = "/3ds/CECTool/" + id + "/OutBox__";
    for (size_t j = 0; j < dir.count() && box.getMessages().size() < box.getInfo().maxMessages(); j++)
    {
        FILE* in = fopen((outboxDir + '/' + dir.item(j)).c_str(), "r");
        fseek(in, 0, SEEK_END);
        size_t messageSize = ftell(in);
        fseek(in, 0, SEEK_SET);
        u8* data = new u8[messageSize];
        fread(data, 1, messageSize, in);
        fclose(in);

        Message message(data);
        delete[] data;
        box.addMessage(message);
    }
    box.saveInfo();

    CECDU_OpenAndWrite(sizeof(boxList), boxId, CEC_PATH_MBOX_LIST, CEC_WRITE | CEC_CHECK, &list);
}
