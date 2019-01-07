#include <3ds.h>
#include <stdio.h>
#include <fstream>
#include "BoxInfo.hpp"
#include "Box.hpp"
#include "STDirectory.hpp"
extern "C"
{
#include "cecdu.h"
}

struct boxList
{
    u8 magic[2];
    u8 padding[2];
    u32 version;
    u32 numBoxes;
    char boxId[12][16];
};

int main()
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

int main3()
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
        for (auto message : box.messageNames())
        {
            printf("    %s\n", message.c_str());
        }
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