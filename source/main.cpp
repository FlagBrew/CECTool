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

void importBoxes(bool del)
{
    Result res;
    boxList list;
    size_t sizeWritten = sizeof(struct boxList);
    CECDU_OpenRawFile(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_SKIP_CHECKS);
    CECDU_ReadRawFile(&list, &sizeWritten);
    STDirectory dir("/3ds/CECTool");
    std::vector<std::string> boxes;
    for (size_t i = 0; i < dir.count(); i++)
    {
        if (dir.folder(i))
        {
            if (io::exists("/3ds/CECTool/" + dir.item(i) + "/InBox___/BoxInfo_____") && io::exists("/3ds/CECTool/" + dir.item(i) + "/OutBox__/BoxInfo_____"))
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
        for (int i = 0; i < availableBoxes.size(); i++)
        {
            boxId = list.boxId[availableBoxes[i]];
            currentId = std::stoul(list.boxId[availableBoxes[i]], nullptr, 16);
            dir = STDirectory("/3ds/CECTool/" + boxId + "/InBox___");
            FILE* in = fopen(("/3ds/CECTool/" + boxId + "/InBox___/BoxInfo_____").c_str(), "r");
            u8* data = new u8[0x20];
            fread(data, 1, 0x20, in);
            fclose(in);
            BoxInfo info(data, false);
            Box box(currentId, false);
            delete[] data;
            u8 boxDataMod = 0;
            for (size_t j = 0; j < std::min(dir.count() - 1, (size_t) info.maxMessages()); j++)
            {
                if (dir.item(j + boxDataMod) == "BoxInfo_____")
                {
                    boxDataMod++;
                    j--;
                    continue;
                }
                in = fopen(("/3ds/CECTool/" + boxId + "/InBox___/" + dir.item(j + boxDataMod)).c_str(), "r");
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
                    remove(("/3ds/CECTool/" + boxId + "/InBox___/" + dir.item(j + boxDataMod)).c_str());
                }
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
    CECDU_OpenRawFile(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_SKIP_CHECKS);
    CECDU_ReadRawFile(&list, &sizeWritten);
    mkdir("/3ds/CECTool", 777);
    FILE* out;
    for (size_t i = 0; i < list.numBoxes; i++)
    {
        std::string path = "/3ds/CECTool/" + std::string(list.boxId[i]);
        mkdir(path.c_str(), 777);
        path += "/InBox___";
        mkdir(path.c_str(), 777);
        u32 id = strtoul(list.boxId[i], nullptr, 16);
        Box box(id);
        for (auto message : box.getMessages())
        {
            size_t size = message.messageSize();
            cecMessageId messageId = message.messageID();
            char messageString[9];
            std::copy(messageId.data, messageId.data + 8, messageString);
            messageString[8] = '\0';
            u8 messageData[size];
            std::string outPath = path + '/' + base64_encode(messageString, 8);
            if (R_FAILED(res = CECDU_ReadMessage(id, false, messageId, messageData, &size)))
            {
                printf("Message Read: %s", outPath.c_str());
                continue;
            }
            out = fopen(outPath.c_str(), "w");
            fwrite(messageData, 1, size, out);
            fclose(out);
        }
        BoxInfo copy = box.getInfo();
        copy.clearMessages();
        out = fopen((path + "/BoxInfo_____").c_str(), "w");
        fwrite(copy.data().data(), 1, 0x20, out);
        fclose(out);

        path = "/3ds/CECTool/" + std::string(list.boxId[i]) + "/OutBox__";
        mkdir(path.c_str(), 777);
        box = Box(id, true);
        for (auto message : box.getMessages())
        {
            size_t size = message.messageSize();
            cecMessageId messageId = message.messageID();
            char messageString[9];
            std::copy(messageId.data, messageId.data + 8, messageString);
            messageString[8] = '\0';
            u8 messageData[size];
            std::string outPath = path + '/' + base64_encode(messageString, 8);
            if (R_FAILED(res = CECDU_ReadMessage(id, true, messageId, messageData, &size)))
            {
                printf("Message Read: %s", outPath.c_str());
                continue;
            }
            out = fopen(outPath.c_str(), "w");
            fwrite(messageData, 1, size, out);
            fclose(out);
        }
        copy = box.getInfo();
        copy.clearMessages();
        out = fopen((path + "/BoxInfo_____").c_str(), "w");
        fwrite(copy.data().data(), 1, 0x20, out);
        fclose(out);
        if (R_FAILED(res = CECDU_OpenRawFile(id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_SKIP_CHECKS)))
        {
            printf("OBIndex Read: %X", res);
        }
        else
        {
            u8 data[16] = {0};
            size_t bufferSize = 16;
            CECDU_ReadRawFile(data, &bufferSize);
            out = fopen((path + "/OBIndex_____").c_str(), "w");
            fwrite(data, 1, 16, out);
            fclose(out);
        }
        
        path = "/3ds/CECTool/" + std::string(list.boxId[i]);
        u8* read = new u8[96];
        sizeWritten = 96;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CEC_PATH_MBOX_INFO, CEC_READ | CEC_SKIP_CHECKS, read, &sizeWritten)))
        {
            out = fopen((path + "/MBoxInfo____").c_str(), "w");
            fwrite(read, 1, sizeWritten, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            printf("MBoxInfo Read: %X", id);
            continue;
        }
        read = new u8[4608];
        sizeWritten = 4608;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CECMESSAGE_BOX_ICON, CEC_READ | CEC_SKIP_CHECKS, read, &sizeWritten)))
        {
            out = fopen((path + "/MBoxData.001").c_str(), "w");
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
        read = new u8[256];
        sizeWritten = 256;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CECMESSAGE_BOX_TITLE, CEC_READ | CEC_SKIP_CHECKS, read, &sizeWritten)))
        {
            out = fopen((path + "/MBoxData.010").c_str(), "w");
            fwrite(read, 1, sizeWritten, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            delete[] read;
            printf("MBoxData.010 Read: %X", id);
            continue;
        }
        read = new u8[8];
        sizeWritten = 8;
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(id, CecDataPathType(100 + 50), CEC_READ | CEC_SKIP_CHECKS, read, &sizeWritten)))
        {
            out = fopen((path + "/MBoxData.050").c_str(), "w");
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
}

int main()
{
    Result res;
    gfxInitDefault();
    hidInit();
    hidScanInput();
    consoleInit(GFX_TOP, nullptr);
    if (R_FAILED(res = cecduInit())) printf("Init: %X\n", res);
    bool showMenu = true;
    u32 down = hidKeysDown();
    while (aptMainLoop() && !(down & KEY_START))
    {
        if (showMenu)
        {
            consoleClear();
            printf("What do?\nA: dump all boxes\nB: import all boxes\n");// Possible: \nX: import and delete all SD boxes // Wishlist: \nY: dump outboxes
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
        // Not right now
        // else if (down & KEY_X)
        // {
        //     printf("Importing...\n");
        //     importBoxes(true);
        //     printf("Done!\n");
        //     waitForInput();
        //     showMenu = true;
        // }
        // Wishlist
        // else if (down & KEY_Y)
        // {
        //     printf("Dumping...");
        //     dumpOutboxes();
        //     printf("Done!");
        // }
    }
    cecduExit();
    return 0;
}

int main4() // Deletes all messages from Kirby
{
    Result res;
    gfxInitDefault();
    hidInit();
    consoleInit(GFX_TOP, nullptr);
    if (R_FAILED(res = cecduInit())) printf("Init: %X", res);
    Box kirby(0x0010bf00);
    if (R_FAILED(res = kirby.clearMessages())) printf("Clearing messages: %X", res);
    return 0;
}

int main3() // Reads all boxes and their message IDs
{
    Result res;
    gfxInitDefault();
    hidInit();
    consoleInit(GFX_TOP, nullptr);
    if (R_FAILED(res = cecduInit())) printf("Init: %X", res);
    boxList list;
    size_t sizeWritten = sizeof(struct boxList);
    CECDU_OpenRawFile(0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_SKIP_CHECKS);
    CECDU_ReadRawFile(&list, &sizeWritten);
    for (size_t i = 0; i < list.numBoxes; i++)
    {
        u32 id = strtoul(list.boxId[i], NULL, 16);
        Box box(id);
        printf("Box %u: %s %X\n", i, list.boxId[i], strtoul(list.boxId[i], NULL, 16));
        // for (auto message : box.messageNames())
        // {
        //     printf("    %s\n", message.c_str());
        // }
        while (aptMainLoop() && !(hidKeysDown() & KEY_A)) hidScanInput();
        hidScanInput();
    }
    while (aptMainLoop() && !(hidKeysDown() & KEY_START)) hidScanInput();
    cecduExit();
    hidExit();
    gfxExit();
    return 0;
}

int main2()
{
    gfxInitDefault();
    hidInit();
    consoleInit(GFX_TOP, nullptr);
    printf("Here goes nothing");
    Result res = cecduInit();
    if (R_FAILED(res)) printf("Init\n%X", res);
    // CECDU_WriteMessage(0x0010bf00, false, id, (void*) "What happens now?", 18);
    // u8 dataRead[30];
    // std::fill_n(dataRead, 30, '\0');
    size_t bufferSize = 30;
    // CECDU_ReadMessage(0x0010bf00, false, id, (void*) dataRead, &bufferSize);
    // dataRead[29] = '\0';
    // printf("\n%i bytes read", bufferSize);
    // printf("\nData: %s", dataRead);
    u8* infoRead = new u8[32];
    bufferSize = 32;
    std::fill_n(infoRead, 32, '\0');
    res = CECDU_OpenAndRead(0x0010bf00, CEC_PATH_INBOX_INFO, CEC_READ | CEC_WRITE | CEC_SKIP_CHECKS, infoRead, &bufferSize);
    if (R_FAILED(res)) printf("Read info 1\n%X", res);
    BoxInfo info(infoRead, false);
    if (info.currentMessages() > 0)
    {
        delete[] infoRead;
        infoRead = new u8[bufferSize = info.fileSize()];
        res = CECDU_OpenAndRead(0x0010bf00, CEC_PATH_INBOX_INFO, CEC_READ | CEC_WRITE | CEC_SKIP_CHECKS, infoRead, &bufferSize);
        if (R_FAILED(res)) printf("Read info 2\n%X", res);
        info = BoxInfo(infoRead);
    }
    std::ifstream in("/data.cec", std::ios::ate | std::ios::in);
    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);
    delete[] infoRead;
    infoRead = new u8[size];
    in.read((char*)infoRead, size);
    in.close();
    Message m(infoRead);
    info.addMessage(m);
    cecMessageId id;
    MessageInfo mInfo = m.getInfo();
    // std::copy(mInfo.messageID(), mInfo.messageID() + 8, id.data);
    res = CECDU_WriteMessage(0x0010bf00, false, id, infoRead, size);
    if (R_FAILED(res)) printf("Write Message\n%X", res);
    std::vector<u8> data = info.data();
    res = CECDU_Delete(0x0010bf00, CEC_PATH_INBOX_INFO, false, id);
    if (R_FAILED(res)) printf("Delete Info\n%X", res);
    res = CECDU_OpenAndWrite(0x0010bf00, CEC_PATH_INBOX_INFO, CEC_WRITE | CEC_SKIP_CHECKS, data.data(), data.size());
    if (R_FAILED(res)) printf("Write Info\n%X", res);
    Box kirby(0x0010bf00);
    //printf("%s", kirby.messageNames().c_str());
    while (aptMainLoop() && !(hidKeysDown() & KEY_START)) hidScanInput();
    hidExit();
    cecduExit();
    gfxExit();
    return 0;
}