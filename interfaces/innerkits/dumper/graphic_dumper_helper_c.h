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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_C_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_C_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*OnDumpFuncPtr)(void);
typedef void(*OnConfigChangeFuncPtr)(const char *, const char *);

int SendInfo(const char* tag, const char *fmt, ...);
int AddConfigChangeListener(const char* tag, OnConfigChangeFuncPtr func);
int RemoveConfigChangeListener(const int listenerId);
int AddDumpListener(const char* tag, OnDumpFuncPtr func);
int RemoveDumpListener(const int listenerId);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_C_H
