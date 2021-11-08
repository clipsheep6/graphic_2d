/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "usb_dev_test.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define OPTION_EDN (-1)

static void ShowUsage()
{
    printf("Usage options:\n");
    printf("-1 : acm_test\n");
    printf("-2 : prop_test\n");
    printf("-3 : acm_speed_read\n");
    printf("-4 : acm_speed_write\n");
    printf("-h : show usage\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("argv too flew\n");
        return -1;
    }
    const char **arg = (const char **)&argv[1];
    if (strcmp(arg[0], "-1") == 0) {
        acm_test(argc - 1, arg);
    } else if (strcmp(arg[0], "-2") == 0) {
        prop_test(argc - 1, arg);
    } else if (strcmp(arg[0], "-3") == 0) {
        acm_speed_read(argc - 1, arg);
    } else if (strcmp(arg[0], "-4") == 0) {
        acm_speed_write(argc - 1, arg);
    } else if (strcmp(arg[0], "-h") == 0 || \
        strcmp(arg[0], "?") == 0) {
        ShowUsage();
    } else {
        printf("unkown cmd\n");
    }
    return 0;
}
