#include "3ds/services/cecdu.h"
#include <3ds/synchronization.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/ipc.h>

static Handle cecduHandle;
static int cecduRefCount;

Result cecduInit()
{
    Result res;
	if (AtomicPostIncrement(&cecduRefCount)) return 0;
	res = srvGetServiceHandle(&cecduHandle, "cecd:u");
	if (R_FAILED(res)) AtomicDecrement(&cecduRefCount);
    return res;
}

void cecduExit()
{
	if (AtomicDecrement(&cecduRefCount)) return;
	svcCloseHandle(cecduHandle);
}

Result CECDU_Open(u32 programID, cecDataPath path, u32 flag, u32* size)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x1, 3, 2);
    cmdbuf[1] = programID;
    cmdbuf[2] = path;
    cmdbuf[3] = flag;
    cmdbuf[4] = IPC_Desc_CurProcessHandle(); // do placeholder
    cmdbuf[5] = 0; // placeholder

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(size)*size = cmdbuf[2];
    }
    return res;
}

Result CECDU_Read(u32 bufferSize, void* buffer, u32* readSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x2, 1, 2);
    cmdbuf[1] = bufferSize;
    cmdbuf[2] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[3] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return res;
}

Result CECDU_ReadMessage(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* messageID, void* buffer, u32* readSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x3, 4, 4);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = idSize;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(idSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)messageID;
    cmdbuf[7] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[8] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return res;
}

Result CECDU_ReadMessageWithHMAC(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* messageID, void* hmacKey, void* buffer, u32* readSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x4, 4, 6);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = idSize;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(idSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)messageID;
    cmdbuf[7] = IPC_Desc_Buffer(0x20, IPC_BUFFER_R); // 32 byte key size
    cmdbuf[8] = (u32)hmacKey;
    cmdbuf[9] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[10] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return res;
}

Result CECDU_Write(u32 bufferSize, void* buffer)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x5, 1, 2);
    cmdbuf[1] = bufferSize;
    cmdbuf[2] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[3] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_WriteMessage(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* buffer, void* messageID)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x6, 4, 4);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = 8;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)buffer;
    cmdbuf[7] = IPC_Desc_Buffer(8, IPC_BUFFER_RW);
    cmdbuf[8] = (u32)messageID;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_WriteMessageWithHMAC(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* buffer, void* hmacKey, void* messageID)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x7, 4, 6);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = idSize;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(idSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)buffer;
    cmdbuf[7] = IPC_Desc_Buffer(0x20, IPC_BUFFER_R); // 32 byte key size
    cmdbuf[8] = (u32)hmacKey;
    cmdbuf[9] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_RW);
    cmdbuf[10] = (u32)messageID;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_Delete(u32 programID, cecDataPath path, bool outBox, u32 idSize, void* messageID)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x8, 4, 2);
    cmdbuf[1] = programID;
    cmdbuf[2] = path;
    cmdbuf[3] = outBox;
    cmdbuf[4] = idSize;
    cmdbuf[5] = IPC_Desc_Buffer(idSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)messageID;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_SetData(u32 programID, u32 bufferSize, u32 option, void* buffer)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x9, 3, 2);
    cmdbuf[1] = programID;
    cmdbuf[2] = bufferSize;
    cmdbuf[3] = option;
    cmdbuf[4] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[5] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_ReadData(u32 destBufferSize, u32 infoType, u32 paramBufferSize, void* paramBuffer, void* destBuffer)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xA, 3, 4);
    cmdbuf[1] = destBufferSize;
    cmdbuf[2] = infoType;
    cmdbuf[3] = paramBufferSize;
    cmdbuf[4] = IPC_Desc_Buffer(paramBufferSize, IPC_BUFFER_R);
    cmdbuf[5] = (u32)paramBuffer;
    cmdbuf[6] = IPC_Desc_Buffer(destBufferSize, IPC_BUFFER_W);
    cmdbuf[7] = (u32)destBuffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_Start(cecCommand command)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xB, 1, 0);
    cmdbuf[1] = command;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_Stop(cecCommand command)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xC, 1, 0);
    cmdbuf[1] = command;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_GetCecInfoBuffer(u32 unknown, u32 unknown2, void* buffer)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xD, 2, 2);
    cmdbuf[1] = unknown;
    cmdbuf[2] = unknown;
    cmdbuf[3] = IPC_Desc_Buffer(unknown, IPC_BUFFER_W);
    cmdbuf[4] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_GetCecdState(u32* state)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xE, 0, 0);

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(state)*state = cmdbuf[2];
    }
    return res;
}

Result CECDU_GetCecInfoEventHandle(Handle* event)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xF, 0, 0);

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(event)*event = cmdbuf[3];
    }
    return res;
}

Result CECDU_GetChangeStateEventHandle(Handle* event)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x10, 0, 0);

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(event)*event = cmdbuf[3];
    }
    return res;
}

Result CECDU_OpenAndWrite(u32 bufferSize, u32 programID, cecDataPath path, u32 flag, void* buffer)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x11, 4, 4);
    cmdbuf[1] = bufferSize;
    cmdbuf[2] = programID;
    cmdbuf[3] = path;
    cmdbuf[4] = flag;
    cmdbuf[5] = IPC_Desc_CurProcessHandle(); // do placeholder
    cmdbuf[6] = 0; // placeholder
    cmdbuf[7] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[8] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_OpenAndRead(u32 bufferSize, u32 programID, cecDataPath path, u32 flag, void* buffer, u32* readSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x12, 4, 4);
    cmdbuf[1] = bufferSize;
    cmdbuf[2] = programID;
    cmdbuf[3] = path;
    cmdbuf[4] = flag;
    cmdbuf[5] = IPC_Desc_CurProcessHandle();
    cmdbuf[6] = 0;
    cmdbuf[7] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[8] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return res;
}

// Not sure if unknown1 or unknown2 is the buffer size
Result CECDU_GetEventLog(u32 unknown, u32 unknown2, void* buffer, u32* unknownRet)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x1E, 2, 2);
    cmdbuf[1] = unknown;
    cmdbuf[2] = unknown2;
    cmdbuf[3] = IPC_Desc_Buffer(unknown, IPC_BUFFER_RW);
    cmdbuf[4] = (u32)buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(unknownRet)*unknownRet = cmdbuf[2];
    }
    return res;
}

Result CECDU_GetEventLogStart(u32* unknownRet)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x1F, 0, 0);

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;
    res = (Result)cmdbuf[1];

    if(R_SUCCEEDED(res))
    {
        if(unknownRet)*unknownRet = cmdbuf[2];
    }
    return res;
}
