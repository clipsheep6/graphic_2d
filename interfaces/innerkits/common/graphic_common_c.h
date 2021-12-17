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

#ifndef INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_C_H
#define INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_C_H

// GraphicStandard Error, number just find fast, it may change
const int LOWERROR_MAX = 1000;
const int LOWERROR_FAILURE = 999;
const int LOWERROR_INVALID = 998;
enum GSError {
    GSERROR_OK                    = 0,

    // 400 BAD_REQUEST
    GSERROR_INVALID_ARGUMENTS     = 40001000,

    // 403 FORBIDDEN
    GSERROR_NO_PERMISSION         = 40301000,

    // 404 NOT_FOUND
    GSERROR_CONNOT_CONNECT_SAMGR  = 40401000,
    GSERROR_CONNOT_CONNECT_SERVER = 40402000,
    GSERROR_CONNOT_CONNECT_WESTON = 40403000,

    // 406 NOT_ACCEPTABLE
    GSERROR_NO_BUFFER             = 40601000,
    GSERROR_NO_ENTRY              = 40602000,
    GSERROR_OUT_OF_RANGE          = 40603000,

    // 412 PRECONDITION_FAILED
    GSERROR_INVALID_OPERATING     = 41201000,
    GSERROR_NO_CONSUMER           = 41202000,
    GSERROR_NOT_INIT              = 41203000,
    GSERROR_TYPE_ERROR            = 41204000,
    GSERROR_DESTROYED_OBJECT      = 41205000,

    // 500 INTERNAL ERROR
    GSERROR_API_FAILED            = 50001000,
    GSERROR_INTERNEL              = 50002000,
    GSERROR_NO_MEM                = 50003000,
    GSERROR_PROXY_NOT_INCLUDE     = 50004000,
    GSERROR_SERVER_ERROR          = 50005000,
    GSERROR_ANIMATION_RUNNING     = 50006000,

    // 501 NOT_IMPLEMENTED
    GSERROR_NOT_IMPLEMENT         = 50101000,
    GSERROR_NOT_SUPPORT           = 50102000,

    // 504 GATEWAY ERROR
    GSERROR_BINDER                = 50401000,

    WM_OK = GSERROR_OK,
    WM_ERROR_SAMGR = GSERROR_CONNOT_CONNECT_SAMGR,
    WM_ERROR_WMS_NOT_FOUND = GSERROR_CONNOT_CONNECT_SERVER,
    WM_ERROR_NOT_INIT = GSERROR_NOT_INIT,
    WM_ERROR_API_FAILED = GSERROR_API_FAILED,
    WM_ERROR_NEW = GSERROR_NO_MEM,
    WM_ERROR_INNER = GSERROR_INTERNEL,
    WM_ERROR_NULLPTR = GSERROR_INVALID_ARGUMENTS,
    WM_ERROR_INVALID_PARAM = GSERROR_INVALID_ARGUMENTS,
    WM_ERROR_CONNOT_CONNECT_WESTON = GSERROR_CONNOT_CONNECT_WESTON,
    WM_ERROR_SERVER = GSERROR_SERVER_ERROR,
    WM_ERROR_NOT_SUPPORT = GSERROR_NOT_SUPPORT,
    WM_ERROR_DESTROYED_OBJECT = GSERROR_DESTROYED_OBJECT,

    SURFACE_ERROR_OK = GSERROR_OK,
    SURFACE_ERROR_ERROR = GSERROR_INTERNEL,
    SURFACE_ERROR_BINDER_ERROR = GSERROR_BINDER,
    SURFACE_ERROR_NULLPTR = GSERROR_INVALID_ARGUMENTS,
    SURFACE_ERROR_NO_ENTRY = GSERROR_NO_ENTRY,
    SURFACE_ERROR_INVALID_OPERATING = GSERROR_INVALID_OPERATING,
    SURFACE_ERROR_NO_BUFFER = GSERROR_NO_BUFFER,
    SURFACE_ERROR_INVALID_PARAM = GSERROR_INVALID_ARGUMENTS,
    SURFACE_ERROR_INIT = GSERROR_INTERNEL,
    SURFACE_ERROR_NOMEM = GSERROR_NO_MEM,
    SURFACE_ERROR_API_FAILED = GSERROR_API_FAILED,
    SURFACE_ERROR_NOT_SUPPORT = GSERROR_NOT_SUPPORT,
    SURFACE_ERROR_OUT_OF_RANGE = GSERROR_OUT_OF_RANGE,
    SURFACE_ERROR_TYPE_ERROR = GSERROR_TYPE_ERROR,
    SURFACE_ERROR_NO_CONSUMER = GSERROR_NO_CONSUMER,

    VSYNC_ERROR_OK = GSERROR_OK,
    VSYNC_ERROR_API_FAILED = GSERROR_API_FAILED,
    VSYNC_ERROR_INVALID_OPERATING = GSERROR_INVALID_OPERATING,
    VSYNC_ERROR_NULLPTR = GSERROR_INVALID_ARGUMENTS,
    VSYNC_ERROR_BINDER_ERROR = GSERROR_BINDER,
    VSYNC_ERROR_SAMGR = GSERROR_CONNOT_CONNECT_SAMGR,
    VSYNC_ERROR_SERVICE_NOT_FOUND = GSERROR_SERVER_ERROR,
    VSYNC_ERROR_PROXY_NOT_INCLUDE = GSERROR_PROXY_NOT_INCLUDE,
    VSYNC_ERROR_INNER = GSERROR_INTERNEL,
    VSYNC_ERROR_INVALID_ARGUMENTS = GSERROR_INVALID_ARGUMENTS,
};

#endif // INTERFACES_INNERKITS_COMMON_GRAPHIC_COMMON_H
