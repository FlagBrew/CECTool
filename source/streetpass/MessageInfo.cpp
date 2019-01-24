#include <cstring>

#include "streetpass/MessageInfo.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

MessageInfo::MessageInfo(const std::vector<u8>& buffer) : messageHeader(){
    std::memcpy(&messageHeader, buffer.data(), sizeof(CecMessageHeader));
}

std::vector<u8> MessageInfo::data() const {
    std::vector<u8> ret(sizeof(CecMessageHeader));
    std::memcpy(ret.data(), &messageHeader, sizeof(CecMessageHeader));
    return ret;
}

CecMessageId MessageInfo::messageID() const {
    CecMessageId id;
    std::memcpy(id.data, messageHeader.messageID.data(), messageHeader.messageID.size());
    return id;
}

u32 MessageInfo::messageSize() const {
    return messageHeader.messageSize;
}

Timestamp MessageInfo::createdTime() const {
    return messageHeader.created;
}

Timestamp MessageInfo::receivedTime() const {
    return messageHeader.received;
}

Timestamp MessageInfo::sentTime() const {
    return messageHeader.sent;
}

void MessageInfo::createdTime(const Timestamp& timestamp) {
    messageHeader.created = timestamp;
}

void MessageInfo::receivedTime(const Timestamp& timestamp) {
    messageHeader.received = timestamp;
}

void MessageInfo::sentTime(const Timestamp& timestamp) {
    messageHeader.sent = timestamp;
}

void MessageInfo::updateTimes()
{
    time_t curTime = time(nullptr);
    tm* currentTime = gmtime(&curTime);
    Timestamp current;
    current.year = currentTime->tm_year + 1900;
    current.month = currentTime->tm_mon + 1;
    current.day = currentTime->tm_mday;
    current.weekDay = currentTime->tm_wday;
    current.hour = currentTime->tm_hour;
    current.minute = currentTime->tm_min;
    current.second = currentTime->tm_sec;
    current.millisecond = osGetTime() % 1000;

    sentTime(current);
    current.second += 10;
    if (current.second >= 60)
    {
        current.minute++;
        current.second %= 60;
        if (current.minute >= 60)
        {
            current.hour++;
            current.minute %= 60;
            if (current.hour >= 24)
            {
                current.day++;
                current.weekDay = (current.weekDay + 1) % 7;
                current.hour %= 24;
                switch (current.month)
                {
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                    case 12:
                        if (current.day > 31)
                        {
                            current.month++;
                            current.day %= 31;
                            if (current.month == 13)
                            {
                                current.year++;
                                current.month %= 12;
                            }
                        }
                        break;
                    case 4:
                    case 6:
                    case 9:
                    case 11:
                        if (current.day > 30)
                        {
                            current.month++;
                            current.day %= 30;
                        }
                        break;
                    case 2:
                        if (current.year % 4 == 0 && current.day > 29)
                        {
                            current.month++;
                            current.day %= 29;
                        }
                        else if (current.day > 28)
                        {
                            current.month++;
                            current.day %= 28;
                        }
                        break;
                }
            }
        }
    }
    receivedTime(current);
}

} // namespace Streetpass
