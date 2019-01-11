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

typedef enum CecStateAbbreviated
{
    CEC_STATE_ABBREV_IDLE = 1,
    CEC_STATE_ABBREV_INACTIVE,
    CEC_STATE_ABBREV_SCANNING,
    CEC_STATE_ABBREV_WLREADY_PLUS, // CEC_STATE_WIRELESS_READY and some unknown bool
    CEC_STATE_ABBREV_OTHER
} cecStateAbbreviated;

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
 * @brief Opens a file and writes to it.
 * @param programID 
 * @param path
 * @param fileOpen
 * @param data
 * @param size
 **/
Result CECDU_OpenAndWrite(u32 programID, cecDataPath path, u32 flag, void* data, size_t size);

/**
 * 
 **/
Result CECDU_OpenRawFile(u32 programID, cecDataPath path, u32 flag);

/**
 * @param bufferSize Set to bytes read on success
 **/
Result CECDU_ReadRawFile(void* buffer, size_t *bufferSize);

/**
 * @param messageID Should always be 8 bytes long
 * @param bufferSize Set to bytes read on success
 **/
Result CECDU_ReadMessage(u32 programID, bool outBox, cecMessageId messageID, void* buffer, size_t* bufferSize);

/**
 * Not implemented because I have no clue
 **/
// Result CECDU_ReadMessageWithHMAC(u32 programID, bool outBox, cecMessageId messageID, void* buffer, size_t* bufferSize, void* hmac);

/**
 * 
 **/
Result CECDU_WriteRawFile(void* buffer, size_t bufferSize);

/**
 * 
 **/
Result CECDU_WriteMessage(u32 programID, bool outBox, cecMessageId messageID, void* buffer, size_t bufferSize);

/**
 * Not implemented because I have no clue
 **/
// Result CECDU_WriteMessageWithHMAC(u32 programID, bool outBox, cecMessageId messageID, void* buffer, size_t bufferSize, void* hmac);

/**
 * @param messageID 8-byte value
 **/
Result CECDU_Delete(u32 programID, cecDataPath path, bool outBox, cecMessageId messageID);

/**
 * 
 **/
Result CECDU_RunCommand(cecCommand command);

/**
 * 
 **/
Result CECDU_OpenAndRead(u32 programID, cecDataPath path, u32 flag, void* buffer, size_t* bufferSize);