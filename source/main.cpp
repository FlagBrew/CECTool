#include <3ds.h>
#include <stdio.h>
#include <fstream>
#include "BoxInfo.hpp"
#include "Box.hpp"
#include "STDirectory.hpp"
#include "base64.hpp"
#include "io.hpp"
extern "C"
{
#include "cecdu.h"
}

int __stacksize__ = 64 * 1024;

struct boxList
{
    u8 magic[2];
    u8 padding[2];
    u32 version;
    u32 numBoxes;
    char boxId[12][16];
};

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
    Result res;
    boxList list;
    size_t sizeWritten = sizeof(struct boxList);
    CECDU_OpenRawFile(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_CHECK);
    CECDU_ReadRawFile(&list, &sizeWritten);
    for (int i = 0; i < list.numBoxes; i++)
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
        CECDU_OpenAndWrite(boxId, folderPaths[i], CEC_CREATE | CEC_WRITE, &data, 0);
    }
    for (size_t i = 0; i < 7; i++)
    {
        FILE* in = fopen(templateFiles[i].c_str(), "r");
        fseek(in, 0, SEEK_END);
        size_t size = ftell(in);
        fseek(in, 0, SEEK_SET);
        u8* data = new u8[size];
        fread(data, 1, size, in);
        fclose(in);
        CECDU_OpenAndWrite(boxId, filePaths[i], CEC_WRITE | CEC_CREATE, data, size);
        delete[] data;
    }
    Box box(boxId, true);
    for (size_t j = 0; j < dir.count() && box.getMessages().size() < box.getInfo().maxMessages(); j++)
    {
        FILE* in = fopen((templateDirs[2] + '/' + dir.item(j)).c_str(), "r");
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

    CECDU_OpenAndWrite(boxId, CEC_PATH_MBOX_LIST, CEC_WRITE | CEC_CHECK, &list, sizeof(boxList));
}

void importBoxes(bool del)
{
    Result res;
    boxList list;
    size_t sizeWritten = sizeof(struct boxList);
    CECDU_OpenRawFile(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_CHECK);
    CECDU_ReadRawFile(&list, &sizeWritten);
    STDirectory dir("/3ds/CECTool");
    std::vector<std::string> boxes;
    for (size_t i = 0; i < dir.count(); i++)
    {
        if (dir.folder(i))
        {
            if (io::folderExists("/3ds/CECTool/" + dir.item(i) + "/InBox___"))
            {
                if (dir.item(i).find_first_not_of("1234567890abcdef") == std::string::npos)
                {
                    boxes.push_back(dir.item(i));
                }
            }
        }
    }
    std::vector<int> availableBoxes;
    for (size_t i = 0; i < list.numBoxes; i++)
    {
        if (std::find(boxes.begin(), boxes.end(), list.boxId[i]) != boxes.end())
        {
            availableBoxes.push_back(i);
        }
    }
    if (availableBoxes.size() > 12)
    {
        printf("Too many boxes!\nI'm not implementing box choices\nin this limited beta.\nWait for a UI, please\n");
        return;
    }
    else
    {
        u32 currentId;
        std::string boxId;
        for (size_t i = 0; i < availableBoxes.size(); i++)
        {
            boxId = list.boxId[availableBoxes[i]];
            currentId = std::stoul(list.boxId[availableBoxes[i]], nullptr, 16);
            dir = STDirectory("/3ds/CECTool/" + boxId + "/InBox___");
            FILE* in;
            u8* data;
            Box box(currentId, false);
            for (size_t j = 0; j < dir.good() && dir.count() && box.getMessages().size() < box.getInfo().maxMessages(); j++)
            {
                in = fopen(("/3ds/CECTool/" + boxId + "/InBox___/" + dir.item(j)).c_str(), "r");
                fseek(in, 0, SEEK_END);
                size_t messageSize = ftell(in);
                fseek(in, 0, SEEK_SET);
                data = new u8[messageSize];
                fread(data, 1, messageSize, in);
                fclose(in);

                Message message(data);
                delete[] data;
                box.addMessage(message);
                if (del)
                {
                    remove(("/3ds/CECTool/" + boxId + "/InBox___/" + dir.item(j)).c_str());
                }
            }

            dir = STDirectory("/3ds/CECTool/" + boxId + "/OutBox__");
            for (size_t j = 0; dir.good() && j < dir.count() && box.getMessages().size() < box.getInfo().maxMessages(); j++)
            {
                in = fopen(("/3ds/CECTool/" + boxId + "/OutBox__/" + dir.item(j)).c_str(), "r");
                fseek(in, 0, SEEK_END);
                size_t messageSize = ftell(in);
                fseek(in, 0, SEEK_SET);
                data = new u8[messageSize];
                fread(data, 1, messageSize, in);
                fclose(in);

                Message message(data);
                message.getInfo().updateTimes();
                delete[] data;
                box.addMessage(message);
                if (del)
                {
                    remove(("/3ds/CECTool/" + boxId + "/OutBox__/" + dir.item(j)).c_str());
                }
            }

            if (del)
            {
                remove(("/3ds/CECTool/" + boxId).c_str());
            }
            box.saveInfo();
        }
    }
}

void dumpBoxes()
{
    Result res;
    boxList list;
    size_t sizeWritten = sizeof(struct boxList);
    CECDU_OpenAndRead(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_CHECK, (u8*)&list, &sizeWritten);
    mkdir("/3ds/CECTool", 777);
    mkdir("/3ds/CECTool/template", 777);
    FILE* out;
    for (size_t i = 0; i < list.numBoxes; i++)
    {
        const std::string boxId = list.boxId[i];
        std::string path = "/3ds/CECTool/" + boxId;
        std::string templatePath = "/3ds/CECTool/template/" + boxId;
        mkdir(path.c_str(), 777);
        mkdir(templatePath.c_str(), 777);
        path += "/InBox___";
        templatePath += "/InBox___";
        std::string filePath;
        mkdir(path.c_str(), 777);
        mkdir(templatePath.c_str(), 777);
        u32 id = strtoul(boxId.c_str(), nullptr, 16);
        Box* box = new Box(id, false);
        for (auto message : box->getMessages())
        {
            size_t size = message.messageSize();
            cecMessageId messageId = message.messageID();
            u8* messageData = new u8[size];
            std::string outPath = path + '/' + base64_encode((char*)&messageId, 8);
            if (R_FAILED(res = CECDU_ReadMessage(id, false, messageId, messageData, &size)))
            {
                delete[] messageData;
                printf("Message Read: %s", outPath.c_str());
                continue;
            }
            out = fopen(outPath.c_str(), "w");
            fwrite(messageData, 1, size, out);
            fclose(out);
            delete[] messageData;
        }
        BoxInfo copy = box->getInfo();
        copy.clearMessages();
        filePath = templatePath + "/BoxInfo_____";
        out = fopen(filePath.c_str(), "w");
        fwrite(copy.data().data(), 1, copy.data().size(), out);
        fclose(out);

        path = "/3ds/CECTool/" + boxId + "/OutBox__";
        templatePath = "/3ds/CECTool/template/" + boxId + "/OutBox__";
        mkdir(path.c_str(), 777);
        mkdir(templatePath.c_str(), 777);
        delete box;
        box = new Box(id, true);
        for (auto message : box->getMessages())
        {
            size_t size = message.messageSize();
            cecMessageId messageId = message.messageID();
            u8* messageData = new u8[size];
            std::string outPath = path + '/' + base64_encode((char*)&messageId, 8);
            if (R_FAILED(res = CECDU_ReadMessage(id, true, messageId, messageData, &size)))
            {
                delete[] messageData;
                printf("Message Read: %s", outPath.c_str());
                continue;
            }
            out = fopen(outPath.c_str(), "w");
            fwrite(messageData, 1, size, out);
            fclose(out);
            delete[] messageData;
        }
        copy = box->getInfo();
        copy.clearMessages();
        filePath = templatePath + "/BoxInfo_____";
        out = fopen(filePath.c_str(), "w");
        fwrite(copy.data().data(), 1, copy.data().size(), out);
        fclose(out);
        if (R_FAILED(res = CECDU_OpenRawFile(id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK)))
        {
            printf("OBIndex Read: %X", res);
        }
        else
        {
            u8 data[8] = {0};
            size_t bufferSize = 8;
            CECDU_ReadRawFile(data, &bufferSize);
            *(u32*)(data + 4) = 0;
            filePath = templatePath + "/OBIndex_____";
            out = fopen(filePath.c_str(), "w");
            fwrite(data, 1, 8, out);
            fclose(out);
        }
        
        templatePath = "/3ds/CECTool/template/" + boxId;
        u8* read = new u8[96];
        sizeWritten = 96;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CEC_PATH_MBOX_INFO, CEC_READ | CEC_CHECK, read, &sizeWritten)))
        {
            filePath = templatePath + "/MBoxInfo____";
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, sizeWritten, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            delete[] read;
            printf("MBoxInfo Read: %X", id);
            continue;
        }
        read = new u8[4608];
        sizeWritten = 4608;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CECMESSAGE_BOX_ICON, CEC_READ | CEC_CHECK, read, &sizeWritten)))
        {
            filePath = templatePath + "/MBoxData.001";
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, sizeWritten, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            delete[] read;
            printf("MBoxData.001 Read: %X", id);
            continue;
        }
        // This is fucking hacky but it works
        if (R_SUCCEEDED(res = CECDU_OpenRawFile(id, CECMESSAGE_BOX_TITLE, CEC_READ | CEC_CHECK)))
        {
            filePath = templatePath + "/MBoxData.010";
            char16_t read[256];
            sizeWritten = 0;
            size_t oldSizeWritten = 0;
            do
            {
                sizeWritten = sizeWritten + 2;
                if (oldSizeWritten == sizeWritten)
                {
                    sizeWritten -= 2;
                    break;
                }
                oldSizeWritten = sizeWritten;
                CECDU_ReadRawFile(read, &sizeWritten);
                CECDU_OpenRawFile(id, CECMESSAGE_BOX_TITLE, CEC_READ | CEC_CHECK); // Reopen file
            }
            while (read[sizeWritten / 2 - 1] != u'\0');
            CECDU_ReadRawFile(read, &sizeWritten);
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, sizeWritten, out);
            fclose(out);
        }
        else
        {
            printf("MBoxData.010 Read: %X", id);
            continue;
        }
        read = new u8[8];
        sizeWritten = 8;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CecDataPathType(100 + 50), CEC_READ | CEC_CHECK, read, &sizeWritten)))
        {
            filePath = templatePath + "/MBoxData.050";
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, sizeWritten, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            delete[] read;
            printf("MBoxData.050 Read: %X", id);
            continue;
        }
    }
}

void waitForInput()
{
    while (aptMainLoop() && !hidKeysDown()) hidScanInput();
    hidScanInput();
}

/*
 A testing function for CECDU_SetData
 Findings:
    0: opens OBIndex and OutBox BoxInfo
    1-3: Invalid argument errors
        - Perhaps change buffer size?
    4: deletes the CEC directory
    5: commits changes?
    6: archive not mounted error
    7+: does nothing; returns 0
*/
// int main()
// {
//     u8 dummy[10000];
//     gfxInitDefault();
//     hidInit();
//     hidScanInput();
//     sdmcInit();
//     consoleInit(GFX_TOP, nullptr);
//     cecduInit();
//     Result res;
//     for (u32 i = 0; i < 0x10000; i++)
//     {
//         waitForInput();
//         if (R_SUCCEEDED(res = CECDU_SetData(0x0010bf00, i, dummy, 10000))) printf("%i", i);
//         else printf("%X", res);
//     }
//     cecduExit();
//     return 0;
// }

int main()
{
    Result res;
    gfxInitDefault();
    hidInit();
    hidScanInput();
    sdmcInit();
    consoleInit(GFX_TOP, nullptr);
    if (R_FAILED(res = cecduInit())) printf("Init: %X\n", res);
    bool showMenu = true;
    u32 down = hidKeysDown();
    while (aptMainLoop() && !(down & KEY_START))
    {
        if (showMenu)
        {
            consoleClear();
            printf("What do?\nA: dump all boxes\nB: import all boxes\nY: create SSB box\n");// Possible: \nX: import and delete all SD boxes
            printf("Press START to exit\n");
            showMenu = false;
        }
        down = hidKeysDown();
        hidScanInput();
        if (down & KEY_A)
        {
            printf("Dumping...\n");
            dumpBoxes();
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
        else if (down & KEY_B)
        {
            printf("Importing...\n");
            importBoxes(false);
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
        else if (down & KEY_Y)
        {
            printf("Creating...\n");
            createBox("000b8b00");
            printf("Done!\n");
            waitForInput();
            showMenu = true;
        }
        // Not right now
        // else if (down & KEY_X)
        // {
        //     printf("Importing...\n");
        //     importBoxes(true);
        //     printf("Done!\n");
        //     waitForInput();
        //     showMenu = true;
        // }
    }
    cecduExit();
    return 0;
}