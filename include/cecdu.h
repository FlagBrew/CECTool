#pragma once
#include <3ds/types.h>
#include <3ds/result.h>
#include <3ds/services/fs.h>

typedef enum CecDataPathType {
    CEC_PATH_MBOX_LIST = 1,
    CEC_PATH_MBOX_INFO,
    CEC_PATH_INBOX_INFO,
    CEC_PATH_OUTBOX_INFO,
    CEC_PATH_OUTBOX_INDEX,
    CEC_PATH_INBOX_MSG,
    CEC_PATH_OUTBOX_MSG,
    CEC_PATH_ROOT_DIR = 10,
    CEC_PATH_MBOX_DIR,
    CEC_PATH_INBOX_DIR,
    CEC_PATH_OUTBOX_DIR,
    CECMESSAGE_BOX_ICON = 101,
    CECMESSAGE_BOX_TITLE = 110
} cecDataPath;

typedef enum CecCommand
{
    CEC_COMMAND_NONE,
    CEC_COMMAND_START,
    CEC_COMMAND_RESET_START,
    CEC_COMMAND_READYSCAN,
    CEC_COMMAND_READYSCANWAIT,
    CEC_COMMAND_STARTSCAN,
    CEC_COMMAND_RESCAN,
    CEC_COMMAND_NDM_RESUME,
    CEC_COMMAND_NDM_SUSPEND,
    CEC_COMMAND_NDM_SUSPEND_IMMEDIATE,
    CEC_COMMAND_STOPWAIT,
    CEC_COMMAND_STOP,
    CEC_COMMAND_STOP_FORCE,
    CEC_COMMAND_STOP_FORCE_WAIT,
    CEC_COMMAND_RESET_FILTER,
    CEC_COMMAND_DAEMON_STOP,
    CEC_COMMAND_DAEMON_START,
    CEC_COMMAND_EXIT,
    CEC_COMMAND_OVER_BOSS,
    CEC_COMMAND_OVER_BOSS_FORCE,
    CEC_COMMAND_OVER_BOSS_FORCE_WAIT,
    CEC_COMMAND_END
} cecCommand;

enum CecOpenFlag
{
    CEC_READ = BIT(1),
    CEC_WRITE = BIT(2),
    CEC_CREATE = BIT(3),
    CEC_CHECK = BIT(4)
};

typedef struct CecMessageId
{
    u8 data[8];
} cecMessageId;

Result cecduInit();
void cecduExit();

/**
 * CECD::Open service function
 *  Inputs:
 *      0 : Header Code[0x000100C2]
 *      1 : NCCH Program ID
 *      2 : Path type
 *      3 : File open flag
 *      4 : Descriptor for process ID
 *      5 : Placeholder for process ID
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : File size
 **/
Result CECDU_Open(u32 programID, cecDataPath path, u32 flag, u32* size);

/**
 * CECD::Read service function
 *  Inputs:
 *      0 : Header Code[0x00020042]
 *      1 : Buffer size (unused)
 *      2 : Descriptor for mapping a write-only buffer in the target process
 *      3 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Read size
 *      3 : Descriptor for mapping a write-only buffer in the target process
 *      4 : Buffer address
 **/
Result CECDU_Read(u32 bufferSize, void* buffer, u32* readSize);

/**
 * CECD::ReadMessage service function
 *  Inputs:
 *      0 : Header Code[0x00030104]
 *      1 : NCCH Program ID
 *      2 : bool is_outbox
 *      3 : Message ID size (unused, always 8)
 *      4 : Buffer size (unused)
 *      5 : Descriptor for mapping a read-only buffer in the target process
 *      6 : Message ID address
 *      7 : Descriptor for mapping a write-only buffer in the target process
 *      8 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Read size
 *      3 : Descriptor for mapping a read-only buffer in the target process
 *      4 : Message ID address
 *      5 : Descriptor for mapping a write-only buffer in the target process
 *      6 : Buffer address
 **/
Result CECDU_ReadMessage(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* messageID, void* buffer, u32* readSize);

/**
 * CECD::ReadMessageWithHMAC service function
 *  Inputs:
 *      0 : Header Code[0x00040106]
 *      1 : NCCH Program ID
 *      2 : bool is_outbox
 *      3 : Message ID size(unused, always 8)
 *      4 : Buffer size(unused)
 *      5 : Descriptor for mapping a read-only buffer in the target process
 *      6 : Message ID address
 *      7 : Descriptor for mapping a read-only buffer in the target process
 *      8 : HMAC key address
 *      9 : Descriptor for mapping a write-only buffer in the target process
 *     10 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Read size
 *      3 : Descriptor for mapping a read-only buffer in the target process
 *      4 : Message ID address
 *      5 : Descriptor for mapping a read-only buffer in the target process
 *      6 : HMAC key address
 *      7 : Descriptor for mapping a write-only buffer in the target process
 *      8 : Buffer address
 **/
Result CECDU_ReadMessageWithHMAC(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* messageID, void* hmacKey, void* buffer, u32* readSize);

/**
 * CECD::Write service function
 *  Inputs:
 *      0 : Header Code[0x00050042]
 *      1 : Buffer size(unused)
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Buffer address
 **/
Result CECDU_Write(u32 bufferSize, void* buffer);

/**
 * CECD::WriteMessage service function
 *  Inputs:
 *      0 : Header Code[0x00060104]
 *      1 : NCCH Program ID
 *      2 : bool is_outbox
 *      3 : Message ID size(unused, always 8)
 *      4 : Buffer size(unused)
 *      5 : Descriptor for mapping a read-only buffer in the target process
 *      6 : Buffer address
 *      7 : Descriptor for mapping a read/write buffer in the target process
 *      8 : Message ID address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Buffer address
 *      4 : Descriptor for mapping a read/write buffer in the target process
 *      5 : Message ID address
 **/
Result CECDU_WriteMessage(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* buffer, void* messageID);

/**
 * CECD::WriteMessageWithHMAC service function
 *  Inputs:
 *      0 : Header Code[0x00070106]
 *      1 : NCCH Program ID
 *      2 : bool is_outbox
 *      3 : Message ID size(unused, always 8)
 *      4 : Buffer size(unused)
 *      5 : Descriptor for mapping a read-only buffer in the target process
 *      6 : Buffer address
 *      7 : Descriptor for mapping a read-only buffer in the target process
 *      8 : HMAC key address
 *      9 : Descriptor for mapping a read/write buffer in the target process
 *     10 : Message ID address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Buffer address
 *      4 : Descriptor for mapping a read-only buffer in the target process
 *      5 : HMAC key address
 *      6 : Descriptor for mapping a read/write buffer in the target process
 *      7 : Message ID address
 **/
Result CECDU_WriteMessageWithHMAC(u32 programID, bool outBox, u32 idSize, u32 bufferSize, void* buffer, void* hmac, void* messageID);

/**
 * CECD::Delete service function
 *  Inputs:
 *      0 : Header Code[0x00080102]
 *      1 : NCCH Program ID
 *      2 : Path type
 *      3 : bool is_outbox
 *      4 : Message ID size (unused)
 *      5 : Descriptor for mapping a read-only buffer in the target process
 *      6 : Message ID address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Message ID address
 **/
Result CECDU_Delete(u32 programID, cecDataPath path, bool outBox, u32 idSize, void* messageID);

/**
 * CECD::SetData service function
 *  Inputs:
 *      0 : Header Code[0x000900C2]
 *      1 : NCCH Program ID
 *      2 : Buffer Size
 *      3 : Option
 *      4 : Descriptor for mapping a read-only buffer in the target process
 *      5 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Buffer address
 **/
Result CECDU_SetData(u32 programID, u32 bufferSize, u32 option, void* buffer);

/**
 * CECD::ReadData service function
 *  Inputs:
 *      0 : Header Code[0x000A00C4]
 *      1 : Destination buffer size (unused)
 *      2 : Info type
 *      3 : Param buffer size (unused)
 *      4 : Descriptor for mapping a read-only buffer in the target process
 *      5 : Param buffer address
 *      6 : Descriptor for mapping a write-only buffer in the target process
 *      7 : Destination buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Param buffer address
 *      4 : Descriptor for mapping a write-only buffer in the target process
 *      5 : Destination buffer address
 **/
Result CECDU_ReadData(u32 destBufferSize, u32 infoType, u32 paramBufferSize, void* paramBuffer, void* destBuffer);

/**
 * CECD::Start service function
 *  Inputs:
 *      0 : Header Code[0x000B0040]
 *      1 : Command
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 **/
Result CECDU_Start(cecCommand command);

/**
 * CECD::Stop service function
 *  Inputs:
 *      0 : Header Code[0x000C0040]
 *      1 : Command
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 **/
Result CECDU_Stop(cecCommand command);

/**
 * CECD::GetCecInfoBuffer service function
 *  Inputs:
 *      0 : Header Code[0x000D0082]
 *      1 : unknown
 *      2 : unknown, buffer size?
 *      3 : Descriptor for mapping a write-only buffer in the target process
 *      4 : Destination buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *    2-3 : MappedBuffer
 **/
Result CECDU_GetCecInfoBuffer(u32 unknown, u32 unknown2, void* buffer);

/**
 * GetCecdState service function
 *  Inputs:
 *      0: Header Code[0x000E0000]
 *  Outputs:
 *      1: ResultCode
 *      2: CecdState
 **/
Result CECDU_GetCecdState(u32* state);

/**
 * GetCecInfoEventHandle service function
 *  Inputs:
 *      0: Header Code[0x000F0000]
 *  Outputs:
 *      1: ResultCode
 *      3: Event Handle
 **/
Result CECDU_GetCecInfoEventHandle(Handle* event);

/**
 * GetChangeStateEventHandle service function
 *  Inputs:
 *      0: Header Code[0x00100000]
 *  Outputs:
 *      1: ResultCode
 *      3: Event Handle
 **/
Result CECDU_GetChangeStateEventHandle(Handle* event);

/**
 * CECD::OpenAndWrite service function
 *  Inputs:
 *      0 : Header Code[0x00110104]
 *      1 : Buffer size (unused)
 *      2 : NCCH Program ID
 *      3 : Path type
 *      4 : File open flag
 *      5 : Descriptor for process ID
 *      6 : Placeholder for process ID
 *      7 : Descriptor for mapping a read-only buffer in the target process
 *      8 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Descriptor for mapping a read-only buffer in the target process
 *      3 : Buffer address
 **/
Result CECDU_OpenAndWrite(u32 bufferSize, u32 programID, cecDataPath path, u32 flag, void* buffer);

/**
 * CECD::OpenAndRead service function
 *  Inputs:
 *      0 : Header Code[0x00120104]
 *      1 : Buffer size (unused)
 *      2 : NCCH Program ID
 *      3 : Path type
 *      4 : File open flag
 *      5 : Descriptor for process ID
 *      6 : Placeholder for process ID
 *      7 : Descriptor for mapping a write-only buffer in the target process
 *      8 : Buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : Toal bytes read
 *      3 : Descriptor for mapping a write-only buffer in the target process
 *      4 : Buffer address
 **/
Result CECDU_OpenAndRead(u32 bufferSize, u32 programID, cecDataPath path, u32 flag, void* buffer, u32* readSize);

/**
 * CECD::GetEventLog service function
 *  Inputs:
 *      0 : Header Code[0x001E0082]
 *      1 : unknown
 *      2 : unknown
 *      3 : buffer descriptor
 *      4 : buffer address
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : unknown
 **/
Result CECDU_GetEventLog(u32 unknown, u32 unknown2, void* buffer, u32* unknownRet);

/**
 * CECD::GetEventLogStart service function
 *  Inputs:
 *      0 : Header Code[0x001F0000]
 *  Outputs:
 *      1 : Result of function, 0 on success, otherwise error code
 *      2 : unknown
 **/
Result CECDU_GetEventLogStart(u32* unknownRet);
