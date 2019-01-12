#include "cecdu.h"
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

Result CECDU_OpenAndWrite(u32 programId, cecDataPath path, u32 flag, void* data, size_t size)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x11, 4, 4);
    cmdbuf[1] = size;
    cmdbuf[2] = programId;
    cmdbuf[3] = path;
    cmdbuf[4] = flag;
    cmdbuf[5] = IPC_Desc_CurProcessHandle(); // do placeholder
    cmdbuf[6] = 0; // placeholder
    cmdbuf[7] = IPC_Desc_Buffer(size, IPC_BUFFER_R) | (size << 4);
    cmdbuf[8] = data;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_OpenRawFile(u32 programID, cecDataPath path, u32 flag)
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

    return (Result)cmdbuf[1];
}

Result CECDU_ReadRawFile(void* buffer, size_t* bufferSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x2, 1, 2);
    cmdbuf[1] = *bufferSize;
    cmdbuf[2] = IPC_Desc_Buffer(*bufferSize, IPC_BUFFER_W);
    cmdbuf[3] = buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    *bufferSize = cmdbuf[2];
    return (Result)cmdbuf[1];
}

Result CECDU_ReadMessage(u32 programID, bool outBox, cecMessageId messageID, void* buffer, size_t* readSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x3, 4, 4);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = 8;
    cmdbuf[4] = *readSize;
    cmdbuf[5] = IPC_Desc_Buffer(8, IPC_BUFFER_R);
    cmdbuf[6] = messageID.data;
    cmdbuf[7] = IPC_Desc_Buffer(*readSize, IPC_BUFFER_W);
    cmdbuf[8] = buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    *readSize = cmdbuf[2];
    return (Result)cmdbuf[1];
}

Result CECDU_WriteRawFile(void* buffer, size_t bufferSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x5, 1, 2);
    cmdbuf[1] = bufferSize;
    cmdbuf[2] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[3] = buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return (Result)cmdbuf[1];
}

Result CECDU_WriteMessage(u32 programID, bool outBox, cecMessageId messageID, void* buffer, size_t bufferSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x6, 4, 4);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = 8;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[6] = buffer;
    cmdbuf[7] = IPC_Desc_Buffer(8, IPC_BUFFER_RW);
    cmdbuf[8] = messageID.data;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return cmdbuf[1];
}

Result CECDU_Delete(u32 programID, cecDataPath path, bool outBox, cecMessageId* messageID)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x8, 4, 2);
    cmdbuf[1] = programID;
    cmdbuf[2] = path;
    cmdbuf[3] = outBox;
    cmdbuf[4] = messageID ? 8 : 0;
    cmdbuf[5] = messageID ? IPC_Desc_Buffer(8, IPC_BUFFER_R) : IPC_Desc_Buffer(0, IPC_BUFFER_R);
    cmdbuf[6] = messageID;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return cmdbuf[1];
}

Result CECDU_RunCommand(cecCommand command)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0xB, 1, 0);
    cmdbuf[1] = command;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return cmdbuf[1];
}

Result CECDU_OpenAndRead(u32 programID, cecDataPath path, u32 flag, void* buffer, size_t* bufferSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x12, 4, 4);
    cmdbuf[1] = *bufferSize;
    cmdbuf[2] = programID;
    cmdbuf[3] = path;
    cmdbuf[4] = flag;
    cmdbuf[5] = IPC_Desc_CurProcessHandle();
    cmdbuf[6] = 0;
    cmdbuf[7] = IPC_Desc_Buffer(*bufferSize, IPC_BUFFER_W);
    cmdbuf[8] = buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    *bufferSize = cmdbuf[2];
    return cmdbuf[1];
}

Result CECDU_SetData(u32 programID, u32 option, void* buffer, size_t bufferSize)
{
    Result res;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x9, 3, 2);
    cmdbuf[1] = programID;
    cmdbuf[2] = bufferSize;
    cmdbuf[3] = option;
    cmdbuf[4] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_R);
    cmdbuf[5] = buffer;

    if (R_FAILED(res = svcSendSyncRequest(cecduHandle))) return res;

    return cmdbuf[1];
}