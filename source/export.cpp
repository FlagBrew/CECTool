#include <3ds.h>
#include <fstream>
#include <sys/stat.h>
#include "common/base64.hpp"
#include "export.hpp"
#include "streetpass/BoxInfo.hpp"
#include "streetpass/Box.hpp"
#include "streetpass/MBoxList.hpp"

extern "C"
{
#include "3ds/services/cecdu.h"
}

using Streetpass::boxList;

void dumpBoxes()
{
    Result res;
    boxList list;
    u32 boxListSize = sizeof(boxList);
    CECDU_OpenAndRead(boxListSize, 0x0, CEC_PATH_MBOX_LIST, CEC_READ | CEC_CHECK, (u8*)&list, nullptr);
    mkdir("/3ds/CECTool", 777);
    mkdir("/3ds/CECTool/template", 777);
    FILE* out;
    for (u32 i = 0; i < list.numBoxes; i++)
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
            u32 messageSize = message.messageSize();
            cecMessageId messageId = message.messageID();
            u8* messageData = new u8[messageSize];
            std::string outPath = path + '/' + base64_encode((char*)&messageId, 8);
            if (R_FAILED(res = CECDU_ReadMessage(id, false, 8, messageSize, messageId.data, messageData, nullptr)))
            {
                delete[] messageData;
                printf("Message Read: %s", outPath.c_str());
                continue;
            }
            out = fopen(outPath.c_str(), "w");
            fwrite(messageData, 1, messageSize, out);
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
            u32 messageSize = message.messageSize();
            cecMessageId messageId = message.messageID();
            u8* messageData = new u8[messageSize];
            std::string outPath = path + '/' + base64_encode((char*)&messageId, 8);
            if (R_FAILED(res = CECDU_ReadMessage(id, true, 8, messageSize, messageId.data, messageData, nullptr)))
            {
                delete[] messageData;
                printf("Message Read: %s", outPath.c_str());
                continue;
            }
            out = fopen(outPath.c_str(), "w");
            fwrite(messageData, 1, messageSize, out);
            fclose(out);
            delete[] messageData;
        }
        copy = box->getInfo();
        copy.clearMessages();
        filePath = templatePath + "/BoxInfo_____";
        out = fopen(filePath.c_str(), "w");
        fwrite(copy.data().data(), 1, copy.data().size(), out);
        fclose(out);
        if (R_FAILED(res = CECDU_Open(id, CEC_PATH_OUTBOX_INDEX, CEC_READ | CEC_CHECK, nullptr)))
        {
            printf("OBIndex Read: %X", res);
        }
        else
        {
            u32 bufferSize = 8;
            u8 data[bufferSize] = {0};
            CECDU_Read(bufferSize, &data, nullptr);
            *(u32*)(data + 4) = 0;
            filePath = templatePath + "/OBIndex_____";
            out = fopen(filePath.c_str(), "w");
            fwrite(data, 1, 8, out);
            fclose(out);
        }
        
        templatePath = "/3ds/CECTool/template/" + boxId;
        u32 bufferSize = 96;
        u8* read = new u8[bufferSize];
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(bufferSize, id, CEC_PATH_MBOX_INFO, CEC_READ | CEC_CHECK, read, nullptr)))
        {
            filePath = templatePath + "/MBoxInfo____";
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, bufferSize, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            delete[] read;
            printf("MBoxInfo Read: %X", id);
            continue;
        }
        bufferSize = 4608;
        read = new u8[bufferSize];
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(bufferSize, id, CECMESSAGE_BOX_ICON, CEC_READ | CEC_CHECK, read, nullptr)))
        {
            filePath = templatePath + "/MBoxData.001";
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, bufferSize, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            delete[] read;
            printf("MBoxData.001 Read: %X", id);
            continue;
        }

        u32 filesize = 0;
        if (R_SUCCEEDED(res = CECDU_Open(id, CECMESSAGE_BOX_TITLE, CEC_READ, &filesize)))
        {
            filePath = templatePath + "/MBoxData.010";
            read = new u8[filesize];

            CECDU_Read(filesize, read, &filesize);
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, filesize, out);
            fclose(out);
            delete[] read;
        }
        else
        {
            printf("MBoxData.010 Read: %X", id);
            continue;
        }
        bufferSize = 8;
        read = new u8[bufferSize];
        if (R_SUCCEEDED(res = CECDU_OpenAndRead(bufferSize, id, CecDataPathType(100 + 50), CEC_READ | CEC_CHECK, read, nullptr)))
        {
            filePath = templatePath + "/MBoxData.050";
            out = fopen(filePath.c_str(), "w");
            fwrite(read, 1, bufferSize, out);
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
