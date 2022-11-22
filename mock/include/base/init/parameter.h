/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

/**
 * @addtogroup parameter
 * @{
 *
 * @brief Provides functions for obtaining system parameters.
 *
 * This module can obtain device information such as device type and manufacturer.\n
 *
 * @since 6.0
 * @version 6.0
 */

/**
 * @file parameter.h
 *
 * @brief Declares functions for obtaining system parameters.
 *
 * You can use the provided functions to obtain device information such as device type and manufacturer.\n
 * @since 6.0
 * @version 6.0
 */

#ifndef MOCK_PARAMETER_API_H
#define MOCK_PARAMETER_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Obtains a system parameter matching the specified <b>key</b>.
 *
 * If no system parameter is found, the <b>def</b> parameter will be returned.\n
 *
 * @param key Indicates the key for the system parameter to query.
 * The value can contain lowercase letters, digits, underscores (_), and dots (.).
 * Its length cannot exceed 32 bytes (including the end-of-text character in the string).
 * @param def Indicates the default value to return when no query result is found.
 * This parameter is specified by the caller.
 * @param value Indicates the data buffer that stores the query result.
 * This parameter is applied for and released by the caller and can be used as an output parameter.
 * @param len Indicates the length of the data in the buffer.
 * @return Returns the number of bytes of the system parameter if the operation is successful;
 * returns <b>-9</b> if a parameter is incorrect; returns <b>-1</b> in other scenarios.
 * @since 6.0
 * @version 6.0
 */
int GetParameter(const char *key, const char *def, char *value, unsigned int len)
{
    return 0;
}

/**
 * @brief Sets or updates a system parameter.
 *
 * You can use this function to set a system parameter that matches <b>key</b> as <b>value</b>.\n
 *
 * @param key Indicates the key for the parameter to set or update.
 * The value can contain lowercase letters, digits, underscores (_), and dots (.).
 * Its length cannot exceed 32 bytes (including the end-of-text character in the string).
 * @param value Indicates the system parameter value.
 * Its length cannot exceed 128 bytes (including the end-of-text character in the string).
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-9</b> if a parameter is incorrect; returns <b>-1</b> in other scenarios.
 * @since 6.0
 * @version 6.0
 */
int SetParameter(const char *key, const char *value)
{
    return 0;
}

/**
 * @brief Wait for a system parameter with specified value.
 *
 * You can use this function to wait a system parameter that matches <b>key</b> as <b>value</b>.\n
 *
 * @param key Indicates the key for the parameter to wait.
 * The value can contain lowercase letters, digits, underscores (_), and dots (.).
 * Its length cannot exceed 96 bytes (including the end-of-text character in the string).
 * @param value Indicates the system parameter value.
 * Its length cannot exceed 96 bytes (including the end-of-text character in the string).
 * value can use "*" to do arbitrary match.
 * @param timeout Indicates the timeout value, in seconds.
 * <=0 means wait for ever.
 * >0 means wait for specified seconds
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-10</b> if timeout; returns <b>-1</b> in other scenarios.
 * @since 1.1
 * @version 1.1
 */
int WaitParameter(const char *key, const char *value, int timeout)
{
    return 0;
}

/**
 * @brief Watch for system parameter values.
 *
 * You can use this function to watch system parameter values.\n
 *
 * @param keyprefix Indicates the key prefix for the parameter to be watched.
 * If keyprefix is not a full name, "A.B." for example, it means to watch for all parameter started with "A.B.".
 * @param callback Indicates value change callback.
 * If callback is NULL, it means to cancel the watch.
 * @return Returns <b>0</b> if the operation is successful;
 * returns <b>-1</b> in other scenarios.
 * @since 1.1
 * @version 1.1
 */
typedef void (*ParameterChgPtr)(const char *key, const char *value, void *context);
int WatchParameter(const char *keyprefix, ParameterChgPtr callback, void *context)
{
    return 0;
}

/**
 * @brief Obtains the device type.
 *
 * The device type can have a maximum length of 32 characters.\n
 *
 * @return Returns the device type if a result is found, for example, returns <b>wifiiot</b>
 * if the application is running on a Wi-Fi connected IoT device; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0 */
const char *GetDeviceType(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device manufacturer.
 *
 * The device manufacturer can have a maximum length of 32 characters.\n
 *
 * @return Returns the device manufacturer if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetManufacture(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device brand.
 *
 * The device brand can have a maximum length of 32 characters.\n
 *
 * @return Returns the device brand if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBrand(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device marketing name.
 *
 * The device marketing name can have a maximum length of 32 characters.\n
 *
 * @return Returns the device marketing name if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetMarketName(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device series name.
 *
 * The device series name can have a maximum length of 32 characters.\n
 *
 * @return Returns the device series name if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetProductSeries(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device authentication model.
 *
 * The device authentication model can have a maximum length of 32 characters.\n
 *
 * @return Returns the device authentication model if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetProductModel(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device software model.
 *
 * The device software model can have a maximum length of 32 characters.\n
 *
 * @return Returns the device software model if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0 */
const char *GetSoftwareModel(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device hardware model.
 *
 * The device hardware model can have a maximum length of 32 characters.\n
 *
 * @return Returns the device hardware model if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetHardwareModel(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device hardware profile.
 *
 * The device hardware profile is a string in JSON format and has a maximum length of 1000 characters.\n
 *
 * @return Returns the device hardware profile if a result is found; returns <b>NULL</b> otherwise.
 * The JSON field in the device hardware profile is defined as follows:
 * Description | key
 * -----------|-----------
 * Audio playback | aout
 * Display | display
 * Camera | camera
 * 2D acceleration capability | DMA_2D
 * Random-access memory | RAM
 * Read-only memory | ROM
 * Graphics processing unit | GPU
 * Neural-network processing unit | NPU
 * Radio | radio
 * Bluetooth | bluetooth
 * Wi-Fi | WIFI
 * USB | usbhost
 * @since 6.0
 * @version 6.0
 */
const char *GetHardwareProfile(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device serial number (SN).
 *
 * The device SN can have a maximum length of 64 characters.\n
 *
 * @return Returns the device SN if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetSerial(void)
{
    return nullptr;
}

/**
 * @brief Obtains the operating system (OS) name.
 *
 * The device OS name can have a maximum length of 32 characters.\n
 *
 * @return Returns the device OS name if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetOSFullName(void)
{
    return nullptr;
}

/**
 * @brief Obtains the software version visible to users.
 *
 * The software version visible to users can have a maximum length of 64 characters.\n
 *
 * @return Returns the software version visible to users if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetDisplayVersion(void)
{
    return nullptr;
}

/**
 * @brief Obtains the bootloader version of this device.
 *
 * The bootloader version can have a maximum length of 64 characters.\n
 *
 * @return Returns the bootloader version if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBootloaderVersion(void)
{
    return nullptr;
}

/**
 * @brief Obtains the security patch tag.
 *
 * @return Returns the security patch tag if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetSecurityPatchTag(void)
{
    return nullptr;
}

/**
 * @brief Obtains the list of application binary interfaces (ABIs) supported on this device.
 *
 * The interfaces in the ABI list are separated by commas (,).
 * This function is available only for an OS with an ecosystem accommodating native applications.\n
 *
 * @return Returns the ABI list if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetAbiList(void)
{
    return nullptr;
}

/**
 * @brief Obtains the SDK API version that matches the current system software.
 *
 * In general, the SDK API version is an integer. This function is only available for an OS with an ecosystem.\n
 *
 * @return Returns the SDK API version if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
int GetSdkApiVersion(void)
{
    return 0;
}

/**
 * @brief Obtains the first SDK API version of the system software.
 *
 * In general, the first SDK API version is an integer. This function is only available for an OS with an ecosystem.\n
 *
 * @return Returns the first SDK API version if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
int GetFirstApiVersion(void)
{
    return 0;
}

/**
 * @brief Obtains the incremental version.
 *
 * The incremental version can be used as the unique software version when the device model is the same.\n
 *
 * @return Returns the incremental version if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetIncrementalVersion(void)
{
    return nullptr;
}

/**
 * @brief Obtains the version ID.
 *
 * The version ID can have a maximum length of 127 characters. It is the unique identifier of a device.\n
 *
 * @return Returns the version ID if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetVersionId(void)
{
    return nullptr;
}

/**
 * @brief Obtains the build type.
 *
 * Different build types with the same baseline codes will be returned,
 * for example, <b>debug/release</b> and <b>log/nolog</b>.
 * Multiple build types can be separated by semicolons (;).\n
 *
 * @return Returns the build type if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBuildType(void)
{
    return nullptr;
}

/**
 * @brief Obtains the build account user name.
 *
 * The build account user name can have a maximum length of 32 characters.\n
 *
 * @return Returns the build account user name if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBuildUser(void)
{
    return nullptr; 
}

/**
 * @brief Obtains the build host name.
 *
 * The build host name can have a maximum length of 32 characters.\n
 *
 * @return Returns the build host name if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBuildHost(void)
{
    return nullptr;
}

/**
 * @brief Obtains the version build time.
 *
 * The version build time is represented by the number of milliseconds elapsed since 1970-01-01 00:00:00 GMT.\n
 *
 * @return Returns the version build time if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBuildTime(void)
{
    return nullptr;
}

/**
 * @brief Obtains the buildroot hash value of this version.
 *
 * The buildroot hash value is represented by the root hash value in the software version hash tree.\n
 *
 * @return Returns the buildroot hash value if a result is found; returns <b>NULL</b> otherwise.
 * @since 6.0
 * @version 6.0
 */
const char *GetBuildRootHash(void)
{
    return nullptr;
}

/**
 * @brief Obtains the OS release type.
 *
 * The OS release category can be Release, Beta, Canary.
 * The specific release type may be Release, Beta1, or others alike.\n
 *
 * @return Returns the OS Release Type.
 * @since 6.0
 * @version 6.0
 */
const char *GetOsReleaseType(void)
{
    return nullptr;
}

/**
 * @brief Obtains the device udid.
 *
 * The OS release category can be Release, Beta, Canary.
 * The specific release type may be Release, Beta1, or others alike.\n
 *
 * @return Return 0 if a result is found; return fail label otherwise.
 * @since 7.0
 * @version 7.0
 */
int GetDevUdid(char *udid, int size)
{
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // PARAMETER_API_H
/** @} */
