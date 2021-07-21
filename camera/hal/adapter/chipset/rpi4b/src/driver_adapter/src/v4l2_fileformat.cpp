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

#include "v4l2_fileformat.h"
#include "securec.h"
#include "v4l2_dev.h"

namespace OHOS::Camera {
HosFileFormat::HosFileFormat() {}
HosFileFormat::~HosFileFormat() {}

RetCode HosFileFormat::V4L2SearchFormat(int fd, std::vector<DeviceFormat>& fmtDesc)
{
    int i, j, k;
    struct v4l2_fmtdesc enumFmtDesc = {};
    struct v4l2_frmsizeenum frmSize = {};
    struct v4l2_frmivalenum  fraMival = {};
    constexpr uint32_t fmtMax = 50;

    for (i = 0; i < fmtMax; ++i) {
        enumFmtDesc.index = i;
        enumFmtDesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_ENUM_FMT, &enumFmtDesc) < 0) {
            break;
        }

        CAMERA_LOGD("[%d]Supported format with description = %s\n\n", i, enumFmtDesc.description);

        for (j = 0; j < fmtMax; ++j) {
            frmSize.index = j;
            frmSize.pixel_format = enumFmtDesc.pixelformat;
            if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmSize) < 0) {
                break;
            }

            if (frmSize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                CAMERA_LOGD("V4L2_FRMSIZE_TYPE_DISCRETE width %d x height %d\n\n",
                    frmSize.discrete.width, frmSize.discrete.height);
            }

            for (k = 0; k < fmtMax; ++k) {
                fraMival.index = k;
                fraMival.pixel_format = frmSize.pixel_format;
                fraMival.width = frmSize.discrete.width;
                fraMival.height = frmSize.discrete.height;
                if (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fraMival) < 0) {
                    break;
                }

                DeviceFormat currentFormat = {};
                currentFormat.fmtdesc.description = std::string((char*)enumFmtDesc.description);
                currentFormat.fmtdesc.pixelformat = enumFmtDesc.pixelformat;
                currentFormat.fmtdesc.width = frmSize.discrete.width;
                currentFormat.fmtdesc.height = frmSize.discrete.height;
                currentFormat.fmtdesc.fps.numerator = fraMival.discrete.numerator;
                currentFormat.fmtdesc.fps.denominator = fraMival.discrete.denominator;

                fmtDesc.push_back(currentFormat);

                CAMERA_LOGD("frame interval: %d, %d\n\n", fraMival.discrete.numerator, fraMival.discrete.denominator);
            }
        }
    }

    if (i == 0) {
        CAMERA_LOGD("no valid supported formats\n");
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode HosFileFormat::V4L2GetFmtDescs(int fd, std::vector<DeviceFormat>& fmtDesc)
{
    RetCode rc = RC_OK;

    std::vector<DeviceFormat>().swap(fmtDesc);

    if (fd < 0) {
        CAMERA_LOGE("V4L2GetFmtDescs fd error\n");
        return RC_ERROR;
    }

    rc = V4L2SearchFormat(fd, fmtDesc);
    if (rc != RC_OK) {
        CAMERA_LOGE("V4L2SearchFormat error\n");
    }

    return rc;
}

RetCode HosFileFormat::V4L2GetCapability(int fd, const std::string& dev_name, std::string& cameraId)
{
    struct v4l2_capability cap = {};

    int rc = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (rc < 0) {
        return RC_ERROR;
    }

    if (!((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) && (cap.capabilities & V4L2_CAP_STREAMING))) {
        return RC_ERROR;
    }

    if (cameraId != std::string((char*)cap.driver)) {
        return RC_ERROR;
    }

    std::lock_guard<std::mutex> l(HosV4L2Dev::deviceFdLock_);
    HosV4L2Dev::deviceMatch.insert(std::make_pair(std::string((char*)cap.driver), dev_name));

    CAMERA_LOGD("v4l2 driver name = %s\n", cap.driver);
    CAMERA_LOGD("v4l2 capabilities = 0x%x\n", cap.capabilities);
    CAMERA_LOGD("v4l2 card: %s\n", cap.card);
    CAMERA_LOGD("v4l2 bus info: %s\n", cap.bus_info);

    return RC_OK;
}

RetCode HosFileFormat::V4L2GetFmt(int fd, DeviceFormat& format)
{
    struct v4l2_format fmt = {};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int rc = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_G_FMT failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    format.fmtdesc.width = fmt.fmt.pix.width;
    format.fmtdesc.height = fmt.fmt.pix.height;
    format.fmtdesc.pixelformat = fmt.fmt.pix.pixelformat;
    format.fmtdesc.sizeimage = fmt.fmt.pix.sizeimage;

    return RC_OK;
}

RetCode HosFileFormat::V4L2SetFmt(int fd, DeviceFormat& format)
{
    struct v4l2_format fmt = {};

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = format.fmtdesc.pixelformat;
    fmt.fmt.pix.width = format.fmtdesc.width;
    fmt.fmt.pix.height = format.fmtdesc.height;

    int rc = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_S_FMT failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode HosFileFormat::V4L2GetCrop(int fd, DeviceFormat& format)
{
    struct v4l2_crop crop = {};

    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int rc = ioctl(fd, VIDIOC_G_CROP, &crop);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_G_CROP failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    format.crop.left = crop.c.left;
    format.crop.top = crop.c.top;
    format.crop.width = crop.c.width;
    format.crop.height = crop.c.height;

    return RC_OK;
}

RetCode HosFileFormat::V4L2SetCrop(int fd, DeviceFormat& format)
{
    struct v4l2_crop crop = {};

    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c.left = format.crop.left;
    crop.c.top = format.crop.top;
    crop.c.width = format.crop.width;
    crop.c.height = format.crop.height;

    int rc = ioctl(fd, VIDIOC_S_CROP, &crop);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_S_CROP failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode HosFileFormat::V4L2GetCropCap(int fd, DeviceFormat& format)
{
    struct v4l2_cropcap cropcap = {};

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int rc = ioctl(fd, VIDIOC_CROPCAP, &cropcap);
    if (rc < 0) {
        CAMERA_LOGE("error: ioctl VIDIOC_CROPCAP failed: %s\n", strerror(errno));
        return RC_ERROR;
    }

    format.cropcap.bounds.height = cropcap.bounds.height;
    format.cropcap.bounds.left = cropcap.bounds.left;
    format.cropcap.bounds.top = cropcap.bounds.top;
    format.cropcap.bounds.width = cropcap.bounds.width;

    format.cropcap.defrect.height = cropcap.defrect.height;
    format.cropcap.defrect.left = cropcap.defrect.left;
    format.cropcap.defrect.top = cropcap.defrect.top;
    format.cropcap.defrect.width = cropcap.defrect.width;

    format.cropcap.pixelaspect.denominator = cropcap.pixelaspect.denominator;
    format.cropcap.pixelaspect.numerator = cropcap.pixelaspect.numerator;

    return RC_OK;
}

int HosFileFormat::V4L2OpenDevice(const std::string& deviceName)
{
    if (deviceName.length() == 0) {
        CAMERA_LOGD("V4L2OpenDevice deviceName length is 0\n");
    }

    int rc = 0;
    char* devName = nullptr;

    devName = realpath(deviceName.c_str(), nullptr);
    if (devName == nullptr) {
        CAMERA_LOGE("V4L2OpenDevice realpath error\n");
        return RCERRORFD;
    }
    rc = open(devName, O_RDWR | O_NONBLOCK, 0);

    CAMERA_LOGD("V4L2OpenDevice %s\n", devName);
    free(devName);

    return rc;
}

void HosFileFormat::V4L2CloseDevice(int fd)
{
    close(fd);
}

void HosFileFormat::V4L2MatchDevice(std::vector<std::string>& cameraIDs)
{
    struct stat st = {};
    char dev_name[16] = {0};
    std::string name = DEVICENAMEX;
    int fd = 0;
    int rc = 0;

    for (auto &it : cameraIDs) {
        for (int i = 0; i < MAXVIDEODEVICE; ++i) {
            sprintf_s(dev_name, sizeof(dev_name), "%s%d", name.c_str(), i);

            if (stat(dev_name, &st) != 0) {
                continue;
            }

            if (!S_ISCHR(st.st_mode)) {
                continue;
            }

            fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
            if (fd == -1) {
                continue;
            }

            rc = V4L2GetCapability(fd, dev_name, it);
            if (rc == RC_ERROR) {
                close(fd);
                continue;
            }

            close(fd);
            break;
        }
    }
}
} // namespace OHOS::Camera
