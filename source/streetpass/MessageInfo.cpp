#include "streetpass/MessageInfo.hpp"

extern "C" {
#include "3ds/services/cecdu.h"
}

namespace Streetpass {

cecMessageId MessageInfo::messageID() const
{
    cecMessageId id;
    std::copy(info.messageID, info.messageID + 8, id.data);
    return id;
}

static Timestamp currentStamp()
{
    time_t current = time(nullptr);
    tm* currentTime = gmtime(&current);
    Timestamp ret;
    ret.year = currentTime->tm_year + 1900;
    ret.month = currentTime->tm_mon + 1;
    ret.day = currentTime->tm_mday;
    ret.weekDay = currentTime->tm_wday;
    ret.hour = currentTime->tm_hour;
    ret.minute = currentTime->tm_min;
    ret.second = currentTime->tm_sec;
    ret.millisecond = osGetTime() % 1000;
    return ret;
}

void MessageInfo::updateTimes()
{
    Timestamp current = currentStamp();
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
