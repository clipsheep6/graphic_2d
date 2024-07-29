/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_MACRO_H
#define RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_MACRO_H

#define DEFAULT_CONSTURCT_(T) \
    T()

#define DEFAULT_DECONSTURCT_(T) \
    virtual ~T()

#define DISALLOW_COPY_AND_ASSIGN(T) \
    T(const T&) = delete;       \
    T& operator=(const T&) = delete\

#define DISALLOW_MOVE_AND_ASSIGN(T) \
    T(const T&&) = delete;       \
    T& operator=(const T&&) = delete

#define SINGLETON_DECLARE(T) \
    DEFAULT_CONSTURCT_(T); \
    DEFAULT_DECONSTURCT_(T); \
    DISALLOW_COPY_AND_ASSIGN(T); \
    DISALLOW_MOVE_AND_ASSIGN(T)

#define NO_COPY_DECLARE_DEFAULT(T) \
    DEFAULT_CONSTURCT_(T) = default; \
    DEFAULT_DECONSTURCT_(T) = default; \
    DISALLOW_COPY_AND_ASSIGN(T)

#define SUBTREE_LOG(format, ...) \

#define SUBTREE_TRACE_NAME(name) \
    HITRACE_METER_NAME(HITRACE_TAG_GRAPHIC_AGP, name)

#define SUBTREE_TRACE_NAME_FMT(fmt, ...) \
    HITRACE_METER_FMT(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__)
#endif