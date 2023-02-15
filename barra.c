
#include <winsock2.h>
#include <ws2tcpip.h>

#include "process.h"
#include <io.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <libcurl-simple-httpsConfig.h>

//#include "include/libmodbus/modbus.h"

#include "include/curl/x64 Debug/include/curl/curl.h"
#include "include/curl/x64 Release/include/curl/curl.h"

#include <windows.h>
#include <stdio.h>

int load() {
    char symbols[] = { '-', '\\', '|', '/' };
    int i;
    while (1) {
        for (i = 0; i < 4; i++) {
            printf("\rLoading %c", symbols[i]);
            fflush(stdout);
            Sleep(100);
        }
    }
    return 0;
}