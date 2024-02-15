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

#include "rs_profiler_telemetry.h"

#include <dirent.h>
#include <format>
#include <fstream>

#include "cpu_collector.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static void RemoveLineBreak(std::string& content)
{
    std::string flagOne = "\r";
    std::string flagTwo = "\n";
    std::string::size_type ret = content.find(flagOne);
    while (ret != content.npos) {
        content.replace(ret, 1, "");
        ret = content.find(flagOne);
    }
    ret = content.find(flagTwo);
    while (ret != content.npos) {
        content.replace(ret, 1, "");
        ret = content.find(flagTwo);
    }
}

static bool LoadFile(const std::string& filePath, std::string& content)
{
    char realPath[PATH_MAX] = { 0x00 };
    if (!realpath(filePath.c_str(), realPath)) {
        return false;
    }
    std::ifstream file(realPath);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, std::ios::end);
    file.tellg();

    content.clear();
    file.seekg(0, std::ios::beg);
    copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(content));
    // remove '' \n\r
    RemoveLineBreak(content);
    return true;
}

static std::string ExtractNumber(const std::string& str)
{
    int cntInt = 0;
    const int shift = 10;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            cntInt *= shift;
            cntInt += str[i] - '0';
        }
    }
    return std::to_string(cntInt);
}

static inline std::string IncludePathDelimiter(const std::string& path)
{
    if (path.rfind('/') != path.size() - 1) {
        return path + "/";
    }

    return path;
}

static void IterateDirFiles(const std::string& path, std::vector<std::string>& files)
{
    std::string pathStringWithDelimiter;
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return;
    }

    while (true) {
        struct dirent* ptr = readdir(dir);
        if (!ptr) {
            break;
        }

        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        } 
        if (ptr->d_type == DT_DIR) {
            pathStringWithDelimiter = IncludePathDelimiter(path) + std::string(ptr->d_name);
            IterateDirFiles(pathStringWithDelimiter, files);
        } else {
            files.push_back(IncludePathDelimiter(path) + std::string(ptr->d_name));
        }
    }
    closedir(dir);
}

static void StrSplit(const std::string& content, const std::string& sp, std::vector<std::string>& out)
{
    size_t index = 0;
    while (index != std::string::npos) {
        size_t tEnd = content.find_first_of(sp, index);
        std::string tmp = content.substr(index, tEnd - index);
        if (!tmp.empty() && tmp != " ") {
            out.push_back(tmp);
        }
        if (tEnd == std::string::npos) {
            break;
        }
        index = tEnd + 1;
    }
}

static std::string GetMetric(const std::string& name)
{
    std::string metric("0");
    LoadFile(name, metric);
    return metric;
}

static float GetMetricFloat(const std::string& name)
{
    return std::stof(GetMetric(name));
}

static std::string GetCPUTemperatureMetric()
{
    std::vector<std::string> directories;
    IterateDirFiles("/sys/class/thermal", directories);

    std::string type;
    for (const std::string& directory : directories) {
        LoadFile(directory + "/type", type);
        if (type.find("soc_thermal") != std::string::npos) {
            return directory + "/temp";
        }
    }

    return "";
}

static void GetCPUTemperature(CPUInfo& cpu)
{
    static const std::string TEMPERATURE_METRIC = GetCPUTemperatureMetric();
    cpu.temperature = GetMetricFloat(TEMPERATURE_METRIC) * Utils::milli;
}

static void GetCPUCurrent(CPUInfo& cpu)
{
    cpu.current = GetMetricFloat("/sys/class/power_supply/Battery/current_now") * Utils::micro;
}

static void GetCPUVoltage(CPUInfo& cpu)
{
    cpu.voltage = GetMetricFloat("/sys/class/power_supply/Battery/voltage_now") * Utils::micro;
}

static void GetCPUMemory(CPUInfo& cpu)
{
    std::ifstream memoryFile("/proc/meminfo", std::ios::in);
    if (!memoryFile) {
        return;
    }

    struct {
        const char* name;
        uint64_t& value;
    } properties[] = {
        { "MemTotal", cpu.ramTotal },
        { "MemFree", cpu.ramFree },
    };

    const size_t propertyCount = sizeof(properties) / sizeof(properties[0]);
    size_t found = 0U;
    std::string line;
    while (getline(memoryFile, line, '\n') && (found < propertyCount)) {
        if (line.find(properties[found].name) != std::string::npos) {
            properties[found].value = std::stoull(ExtractNumber(line));
            found++;
        }
    }
}

static void GetCPUCores(CPUInfo& cpu)
{
    auto collector = OHOS::HiviewDFX::UCollectUtil::CpuCollector::Create();
    auto freqResult = collector->CollectCpuFrequency();
    auto& cpuFreq = freqResult.data;
    auto usageResult = collector->CollectSysCpuUsage(true);
    auto& cpuUsage = usageResult.data;

    cpu.cores = std::min(CPUInfo::maxCoreCount, static_cast<uint32_t>(cpuFreq.size()));

    for (uint32_t i = 0; i < cpu.cores; i++) { // Frequency
        const uint32_t frequencyCpuId = std::min(static_cast<uint32_t>(cpuFreq[i].cpuId), cpu.cores - 1u);
        cpu.coreFrequency[frequencyCpuId] = cpuFreq[i].curFreq * Utils::micro;
    }

    for (auto& cpuInfo : cpuUsage.cpuInfos) { // Load
        const uint32_t loadCpuId = std::min(static_cast<uint32_t>(std::atoi(cpuInfo.cpuId.data())), cpu.cores - 1u);
        cpu.coreLoad[loadCpuId] = cpuInfo.userUsage + cpuInfo.niceUsage + cpuInfo.systemUsage + cpuInfo.idleUsage +
                                  cpuInfo.ioWaitUsage + cpuInfo.irqUsage + cpuInfo.softIrqUsage;
    }
}

static void GetGPUFrequency(GPUInfo& gpu)
{
    static const std::string PATHS[] = {
        "/sys/class/devfreq/fde60000.gpu/cur_freq",
        "/sys/class/devfreq/gpufreq/cur_freq",
    };
    std::string frequency;
    for (const auto& path : PATHS) {
        if (LoadFile(path, frequency)) {
            break;
        }
    }

    gpu.frequency = std::stof(frequency) * Utils::nano;
}

static void GetGPULoad(GPUInfo& gpu)
{
    static const std::string PATHS[] = {
        "/sys/class/devfreq/gpufreq/gpu_scene_aware/utilisation",
        "/sys/class/devfreq/fde60000.gpu/load",
    };
    std::string load;
    for (const auto& path : PATHS) {
        if (LoadFile(path, load)) {
            break;
        }
    }

    std::vector<std::string> splits;
    StrSplit(load, "@", splits);

    gpu.load = std::stof(splits.empty() ? load : splits[0]);
}

const DeviceInfo& RSTelemetry::GetDeviceInfo()
{
    static DeviceInfo info;

    // cpu
    GetCPUTemperature(info.cpu);
    GetCPUCurrent(info.cpu);
    GetCPUVoltage(info.cpu);
    GetCPUMemory(info.cpu);
    GetCPUCores(info.cpu);

    // gpu
    GetGPUFrequency(info.gpu);
    GetGPULoad(info.gpu);

    return info;
}

static std::string TemperatureToString(float temperature)
{
    return std::to_string(temperature) + " °C";
}

static std::string CurrentToString(float current)
{
    return std::to_string(current) + " mA";
}

static std::string VoltageToString(float voltage)
{
    return std::to_string(voltage) + " V";
}

static std::string MemoryToString(uint64_t memory)
{
    return std::to_string(memory * Utils::micro) + " GB";
}

static std::string FrequencyLoadToString(float frequency, float load)
{
    return std::to_string(frequency) + " GHz (" + std::to_string(load) + " %)";
}

std::string RSTelemetry::GetDeviceInfoString()
{
    const DeviceInfo info = GetDeviceInfo();

    std::string string;
    for (size_t i = 0; i < info.cpu.cores; i++) {
        string += +"\nCPU" + std::to_string(i) + ": " +
                  FrequencyLoadToString(info.cpu.coreFrequency[i], info.cpu.coreLoad[i]);
    }

    string += "\nTemperature: " + TemperatureToString(info.cpu.temperature) +
              "\nCurrent: " + CurrentToString(info.cpu.current) + "\nVoltage: " + VoltageToString(info.cpu.voltage) +
              "\nRAM Total: " + MemoryToString(info.cpu.ramTotal) + "\nRAM Free: " + MemoryToString(info.cpu.ramFree) +
              "\nGPU: " + FrequencyLoadToString(info.gpu.frequency, info.gpu.load);

    return string;
}
} // namespace OHOS::Rosen