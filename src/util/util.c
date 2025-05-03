//
// Created by dongvin on 25. 5. 3.
//
#include <string.h>
#include <time.h>

#include "../util/util.h"

void get_current_utc_time_string(char* buffer, size_t buffer_size)
{
    time_t now = time(NULL);
    struct tm utc_time;

#ifdef _WIN32
    gmtime_s(&utc_time, &now);
#else
    struct tm* tmp = gmtime(&now);
    if (tmp)
    {
        utc_time = *tmp;
    } else
    {
        memset(&utc_time, 0, sizeof(struct tm));
    }
#endif

    strftime(buffer, buffer_size, "UTC%Y_%m_%dT%H_%M_%S", &utc_time);
}