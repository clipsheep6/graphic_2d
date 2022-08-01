/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_base_render_util.h"

#include <unordered_set>

#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
namespace Detail {
// [PLANNING]: Use GPU to do the gamut conversion instead of these following works.
using PixelTransformFunc = std::function<float(float)>;

typedef std::pair<uint8_t, uint8_t> Offset; // first: offsetSrc; second: offsetDst
#define STUB_PIXEL_FMT_RGBA_16161616 0X7fff0001
#define STUB_PIXEL_FMT_RGBA_1010102 0X7fff0002

inline constexpr float PassThrough(float v)
{
    return v;
}

template<typename T>
static constexpr T Saturate(T v) noexcept
{
    return T(std::min(static_cast<T>(1), std::max(static_cast<T>(0), v)));
}

inline Vector3f ApplyTransForm(const Vector3f& val, const PixelTransformFunc& func)
{
    return Vector3f {func(val.x_), func(val.y_), func(val.z_)};
}

inline float SafePow(float x, float e)
{
    return powf(x < 0.0f ? 0.0f : x, e);
}

inline PixelTransformFunc GenOETF(float gamma)
{
    if (gamma == 1.0f || gamma == 0.0f) {
        return PassThrough;
    }

    return std::bind(SafePow, std::placeholders::_1, 1.0f / gamma);
}

inline PixelTransformFunc GenEOTF(float gamma)
{
    if (gamma == 1.0f) {
        return PassThrough;
    }

    return std::bind(SafePow, std::placeholders::_1, gamma);
}

struct TransferParameters {
    float g = 0.0f;
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    float d = 0.0f;
    float e = 0.0f;
    float f = 0.0f;
};

inline float RcpResponsePq(float x, const TransferParameters& p)
{
    float tmp = powf(x, p.a);
    return std::powf((p.c + p.d * tmp) / (1 + p.e * tmp), p.b);
}

inline float ResponsePq(float x, const TransferParameters& p)
{
    float tmp = powf(x, 1.f / p.b);
    return std::powf(std::max((tmp - p.c), p.f) / (p.d - p.e * tmp), 1.f / p.a);
}


static constexpr float RcpResponse(float x, const TransferParameters& p)
{
    return x >= p.d * p.c ? (std::pow(x, 1.0f / p.g) - p.b) / p.a : x / p.c;
}

inline constexpr float Response(float x, const TransferParameters& p)
{
    return x >= p.d ? std::pow(p.a * x + p.b, p.g) : p.c * x;
}

inline constexpr float RcpFullResponse(float x, const TransferParameters& p)
{
    return x >= p.d * p.c ? (std::pow(x - p.e, 1.0f / p.g) - p.b) / p.a : (x - p.f) / p.c;
}

inline constexpr float FullResponse(float x, const TransferParameters& p)
{
    return x >= p.d ? std::pow(p.a * x + p.b, p.g) + p.e : p.c * x + p.f;
}

inline PixelTransformFunc GenOETF(const TransferParameters& params)
{
    if (params.g < 0) { // HDR
        return std::bind(RcpResponsePq, std::placeholders::_1, params);
    }

    if (params.e == 0.0f && params.f == 0.0f) {
        return std::bind(RcpResponse, std::placeholders::_1, params);
    }

    return std::bind(RcpFullResponse, std::placeholders::_1, params);
}

inline PixelTransformFunc GenEOTF(const TransferParameters& params)
{
    if (params.g < 0) {
        return std::bind(Response, std::placeholders::_1, params);
    }

    if (params.e == 0.0f && params.f == 0.0f) {
        return std::bind(ResponsePq, std::placeholders::_1, params);
    }

    return std::bind(FullResponse, std::placeholders::_1, params);
}

float ACESToneMapping(float color, float adapted_lum)
{
	const float a = 2.51f;
	const float b = 0.03f;
	const float c = 2.43f;
	const float d = 0.59f;
	const float e = 0.14f;

	color *= adapted_lum;
	return (color * (a * color + b)) / (color * (c * color + d) + e);
}

inline PixelTransformFunc GenACESToneMapping(float adapted_lum)
{
    if (adapted_lum <= 0) {
        adapted_lum = 10000;
    }
    return std::bind(ACESToneMapping, std::placeholders::_1, adapted_lum);
}

Matrix3f GenRGBToXYZMatrix(const std::array<Vector2f, 3>& basePoints, const Vector2f& whitePoint)
{
    const Vector2f& R = basePoints[0];
    const Vector2f& G = basePoints[1];
    const Vector2f& B = basePoints[2];

    float RxRy = R.x_ / R.y_;
    float GxGy = G.x_ / G.y_;
    float BxBy = B.x_ / B.y_;
    float WxWy = whitePoint.x_ / whitePoint.y_;

    float oneRxRy = (1 - R.x_) / R.y_;
    float oneGxGy = (1 - G.x_) / G.y_;
    float oneBxBy = (1 - B.x_) / B.y_;
    float oneWxWy = (1 - whitePoint.x_) / whitePoint.y_;

    float BY =
        ((oneWxWy - oneRxRy) * (GxGy - RxRy) - (WxWy - RxRy) * (oneGxGy - oneRxRy)) /
        ((oneBxBy - oneRxRy) * (GxGy - RxRy) - (BxBy - RxRy) * (oneGxGy - oneRxRy));
    float GY = (WxWy - RxRy - BY * (BxBy - RxRy)) / (GxGy - RxRy);
    float RY = 1 - GY - BY;

    float RYRy = RY / R.y_;
    float GYGy = GY / G.y_;
    float BYBy = BY / B.y_;

    return Matrix3f {
        RYRy * R.x_, RY, RYRy * (1 - R.x_ - R.y_),
        GYGy * G.x_, GY, GYGy * (1 - G.x_ - G.y_),
        BYBy * B.x_, BY, BYBy * (1 - B.x_ - B.y_)
    };
}

class SimpleColorSpace {
public:
    // 3 RGB basePoints and 1 whitePoint.
    SimpleColorSpace(
        const std::array<Vector2f, 3>& basePoints,
        const Vector2f& whitePoint,
        float gamma,
        PixelTransformFunc clamper = Saturate<float>
    ) noexcept
        : rgbToXyz_(GenRGBToXYZMatrix(basePoints, whitePoint)),
          xyzToRgb_(rgbToXyz_.Inverse()),
          transEOTF_(GenEOTF(gamma)),
          transOETF_(GenOETF(gamma)),
          clamper_(std::move(clamper)),
          transferParams_({ gamma, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f })
    {
    }

    SimpleColorSpace(
        const std::array<Vector2f, 3>& basePoints,
        const Vector2f& whitePoint,
        const TransferParameters& parameters,
        PixelTransformFunc clamper = Saturate<float>
    ) noexcept
        : rgbToXyz_(GenRGBToXYZMatrix(basePoints, whitePoint)),
          xyzToRgb_(rgbToXyz_.Inverse()),
          transEOTF_(GenEOTF(parameters)),
          transOETF_(GenOETF(parameters)),
          clamper_(std::move(clamper)),
          transferParams_(parameters)
    {
    }

    ~SimpleColorSpace() noexcept = default;

    Vector3f ToneMapping(const Vector3f& color, float adapted_lum = 0) const
    {
        PixelTransformFunc toneMappingFunc = GenACESToneMapping(adapted_lum);
        return ApplyTransForm(ApplyTransForm(color, toneMappingFunc), clamper_);
    }

    Vector3f ToLinear(const Vector3f& val) const
    {
        if (transferParams_.g < 0) { // HDR
            return ToneMapping(ApplyTransForm(val, transEOTF_));
        }
        return ApplyTransForm(val, transEOTF_);
    }

    Vector3f FromLinear(const Vector3f& val) const
    {
        return ApplyTransForm(val, transOETF_);
    }

    Vector3f RGBToXYZ(const Vector3f& rgb) const
    {
        return rgbToXyz_ * ToLinear(rgb);
    }

    Vector3f XYZToRGB(const Vector3f& xyz) const
    {
        return ApplyTransForm(FromLinear(xyzToRgb_ * xyz), clamper_);
    }

private:
    Matrix3f rgbToXyz_;
    Matrix3f xyzToRgb_;
    PixelTransformFunc transEOTF_;
    PixelTransformFunc transOETF_;
    PixelTransformFunc clamper_;
    TransferParameters transferParams_;
};

SimpleColorSpace &GetSRGBColorSpace()
{
    static SimpleColorSpace sRGB {
        {{Vector2f{0.640f, 0.330f}, {0.300f, 0.600f}, {0.150f, 0.060f}}}, // rgb base points.
        {0.3127f, 0.3290f}, // white points.
        {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.04045f, 0.0f, 0.0f}}; // TransferParameters
    return sRGB;
}

SimpleColorSpace &GetAdobeRGBColorSpace()
{
    static SimpleColorSpace adobeRGB {
        {{Vector2f{0.64f, 0.33f}, {0.21f, 0.71f}, {0.15f, 0.06f}}}, // rgb base points.
        {0.3127f, 0.3290f}, // white points.
        2.2f}; // gamma 2.2
    return adobeRGB;
}

SimpleColorSpace &GetDisplayP3ColorSpace()
{
    static SimpleColorSpace displayP3 {
        {{Vector2f{0.680f, 0.320f}, {0.265f, 0.690f}, {0.150f, 0.060f}}}, // rgb base points.
        {0.3127f, 0.3290f}, // white points.
        {2.4f, 1 / 1.055f, 0.055f / 1.055f, 1 / 12.92f, 0.039f, 0.0f, 0.0f}}; // TransferParameters
    return displayP3;
}

SimpleColorSpace &GetDCIP3ColorSpace()
{
    static SimpleColorSpace dciP3 {
        {{Vector2f{0.680f, 0.320f}, {0.265f, 0.690f}, {0.150f, 0.060f}}}, // rgb base points.
        {0.314f, 0.351f}, // white points.
        2.6f}; // gamma 2.6
    return dciP3;
}

bool IsValidMetaData(const std::vector<HDRMetaData> &metaDatas)
{
    uint16_t validFlag = 0;
    for(auto metaData : metaDatas) {
        validFlag ^= 1 << metaData.key;
    }

    uint16_t bitsToCheck = 0xFF;
    // has complete and unique primaries;
    return (validFlag & bitsToCheck) == bitsToCheck;
}

SimpleColorSpace &GetColorSpaceFromMetaData(const std::vector<HDRMetaData> &metaDatas, float targetLum = 0)
{
    std::vector<HDRMetaData> metaDataSorted = metaDatas;
    std::sort(metaDataSorted.begin(), metaDataSorted.end(), [&](const HDRMetaData &a,const HDRMetaData &b)->bool{
        return a.key < b.key;
    });
    static SimpleColorSpace hdrPq {
         // rgb base points.
        {{Vector2f{metaDataSorted[HDRMetadataKey::MATAKEY_RED_PRIMARY_X].value,
                   metaDataSorted[HDRMetadataKey::MATAKEY_RED_PRIMARY_Y].value},
                  {metaDataSorted[HDRMetadataKey::MATAKEY_GREEN_PRIMARY_X].value,
                   metaDataSorted[HDRMetadataKey::MATAKEY_GREEN_PRIMARY_Y].value},
                  {metaDataSorted[HDRMetadataKey::MATAKEY_BLUE_PRIMARY_X].value,
                   metaDataSorted[HDRMetadataKey::MATAKEY_BLUE_PRIMARY_Y].value}}},
        {metaDataSorted[HDRMetadataKey::MATAKEY_WHITE_PRIMARY_X].value,
         metaDataSorted[HDRMetadataKey::MATAKEY_WHITE_PRIMARY_Y].value}, // white points.
        {-2.0f, 0.1593017578125, 78.84375, 0.8359375, 18.8515625, 18.6875, 0.f}}; // PQ TransferParameters
    return hdrPq;
}

SimpleColorSpace &GetHdrPqColorSpace(const std::vector<HDRMetaData> &metaData, float targetLum = 0.f)
{
    if (metaData.size() > 0 && IsValidMetaData(metaData)) {
        return GetColorSpaceFromMetaData(metaData, targetLum);
    } 

    static SimpleColorSpace DefaultHdrPq {
        {{Vector2f{0.708f, 0.292f}, {0.17f, 0.797f}, {0.131f, 0.046f}}}, // BT.2020 rgb base points.
        {0.3127f, 0.3290f}, // BT.2020 white points.
        {-2.0f, 0.1593017578125, 78.84375, 0.8359375, 18.8515625, 18.6875, 0.f}}; // PQ TransferParameters
    return DefaultHdrPq;
}

bool IsSupportedFormatForGamutConversion(int32_t pixelFormat)
{
    static std::unordered_set<PixelFormat> supportedFormats = {
        PixelFormat::PIXEL_FMT_RGBX_8888,
        PixelFormat::PIXEL_FMT_RGBA_8888,
        PixelFormat::PIXEL_FMT_RGB_888,
        PixelFormat::PIXEL_FMT_BGRX_8888,
        PixelFormat::PIXEL_FMT_BGRA_8888
    };

    // Because PixelFormat has no enumeration for RBG_16,
    // we use a temporary stub here for testing
    // The final version should use a PixelFormat::PIXEL_FMT_RGBA_XXXX
    return pixelFormat == STUB_PIXEL_FMT_RGBA_16161616 ||
        supportedFormats.count(static_cast<PixelFormat>(pixelFormat)) > 0;
}

bool IsSupportedColorGamut(ColorGamut colorGamut)
{
    static std::unordered_set<ColorGamut> supportedColorGamuts = {
        ColorGamut::COLOR_GAMUT_SRGB,
        ColorGamut::COLOR_GAMUT_ADOBE_RGB,
        ColorGamut::COLOR_GAMUT_DISPLAY_P3,
        ColorGamut::COLOR_GAMUT_DCI_P3,
        ColorGamut::COLOR_GAMUT_BT2020,
        ColorGamut::COLOR_GAMUT_BT2100_PQ
    };
    return supportedColorGamuts.count(colorGamut) > 0;
}

SimpleColorSpace& GetColorSpaceOfCertainGamut(ColorGamut colorGamut, const std::vector<HDRMetaData> &metaData = {})
{
    switch (colorGamut) {
        case ColorGamut::COLOR_GAMUT_SRGB: {
            return GetSRGBColorSpace();
        }
        case ColorGamut::COLOR_GAMUT_ADOBE_RGB: {
            return GetAdobeRGBColorSpace();
        }
        case ColorGamut::COLOR_GAMUT_DISPLAY_P3:
        case ColorGamut::COLOR_GAMUT_DCI_P3: {
            return GetDisplayP3ColorSpace(); // Currently p3 colorspace is displayP3
        }
        case ColorGamut::COLOR_GAMUT_BT2020:
        case ColorGamut::COLOR_GAMUT_BT2100_PQ: {
            return GetHdrPqColorSpace(metaData);
        }
        default: {
            return GetSRGBColorSpace();
        }
    }
}

float RGBUint8ToFloat(uint8_t val)
{
    return val * 1.0f / 255.0f; // 255.0f is the max value.
}

float RGBUint10ToFloat(uint8_t val)
{
    return val * 1.0f / 1023.0f; // 1023.0f is the max value
}

uint8_t RGBFloatToUint8(float val)
{
    return static_cast<uint8_t>(Saturate(val) * 255 + 0.5f); // 255.0 is the max value, + 0.5f to avoid negative.
}

uint8_t RGBFloatToUint10(float val)
{
    return static_cast<uint8_t>(Saturate(val) * 1023 + 0.5f); // 255.0 is the max value, + 0.5f to avoid negative.
}

uint8_t RGBUintToFloat(uint8_t* dst, uint8_t* src, int32_t pixelFormat, Vector3f &srcColor,
    std::array<uint8_t*, 3> &colorDst)
{
    // Because PixelFormat does not have enumeration for RGBA_16 or RGBA_1010102,
    // we use two special IF statements here to realize the transfer process.
    // They should to be adjusted to the SWITCH process after the enumerations are added.
    if (pixelFormat == STUB_PIXEL_FMT_RGBA_16161616) {
        auto src16 = (const uint16_t*) src;
        // R: src[0], G: src[1], B: src[2]
        srcColor = {RGBUint10ToFloat(src16[0]), RGBUint10ToFloat(src16[1]), RGBUint10ToFloat(src16[2])};
        // R: dst + 0, G: dst + 1, B: dst + 2
        colorDst = {dst + 0, dst + 1, dst + 2};
        // Alpha: linear transfer src[3] to dst[3]
        dst[3] = RGBFloatToUint8(RGBUint10ToFloat(src16[3]));
        return 8; // 8 bytes per pixel and HDR pictures are always redrawn as sRGB
    }
    if (pixelFormat == STUB_PIXEL_FMT_RGBA_1010102) {
        auto src32 = (const uint32_t*) src;
        // R: 0-9 bits, G: 10-19 ts, B: 20-29bits
        srcColor = {RGBUint10ToFloat(*src32 & 0x3FF), RGBUint10ToFloat((*src32 >> 10) & 0x3FF),
             RGBUint10ToFloat((*src32 >> 20) & 0x3FF)};
        // R: dst + 0, G: dst + 1, B: dst + 2
        colorDst = {dst + 0, dst + 1, dst + 2};
        // Alpha: copy src[3] to dst[3]
        dst[3] = (uint8_t)(*src32 >> 30);
        return 4; // 4 bytes per pixel and HDR pictures are always redrawn as sRGB
    }
    switch (static_cast<PixelFormat>(pixelFormat)) {
        case PixelFormat::PIXEL_FMT_RGBX_8888:
        case PixelFormat::PIXEL_FMT_RGBA_8888: {
            // R: src[0], G: src[1], B: src[2]
            srcColor = {RGBUint8ToFloat(src[0]), RGBUint8ToFloat(src[1]), RGBUint8ToFloat(src[2])};
            // R: dst + 0, G: dst + 1, B: dst + 2
            colorDst = {dst + 0, dst + 1, dst + 2};
            // Alpha: copy src[3] to dst[3]
            dst[3] = src[3];
            return 4; // 4 bytes per pixel.
        }
        case PixelFormat::PIXEL_FMT_RGB_888: {
            // R: src[0], G: src[1], B: src[2]
            srcColor = {RGBUint8ToFloat(src[0]), RGBUint8ToFloat(src[1]), RGBUint8ToFloat(src[2])};
            // R: dst + 0, G: dst + 1, B: dst + 2
            colorDst = {dst + 0, dst + 1, dst + 2};
            return 3; // 3 bytes per pixel.
        }
        case PixelFormat::PIXEL_FMT_BGRX_8888:
        case PixelFormat::PIXEL_FMT_BGRA_8888: {
            // R: src[2], G: src[1], B: src[0]
            srcColor = {RGBUint8ToFloat(src[2]), RGBUint8ToFloat(src[1]), RGBUint8ToFloat(src[0])};
            // R: dst + 2, G: dst + 1, B: dst + 0
            colorDst = {dst + 2, dst + 1, dst + 0};
            // Alpha: copy src[3] to dst[3]
            dst[3] = src[3];
            return 4; // 4 bytes per pixel.
        }
        default: {
            RS_LOGE("RGBUintToFloat: unexpected pixelFormat(%d).", pixelFormat);
            return 0;
        }
    }
}

Offset ConvertColorGamut(uint8_t* dst, uint8_t* src, int32_t pixelFormat, SimpleColorSpace& srcColorSpace,
    SimpleColorSpace& dstColorSpace)
{
    Vector3f srcColor;
    std::array<uint8_t*, 3> colorDst; // color dst, 3 bytes (R G B).

    auto len = RGBUintToFloat(dst, src, pixelFormat, srcColor, colorDst);
    Vector3f outColor = dstColorSpace.XYZToRGB(srcColorSpace.RGBToXYZ(srcColor));
    *(colorDst[0]) = RGBFloatToUint8(outColor[0]); // outColor 0 to colorDst[0]
    *(colorDst[1]) = RGBFloatToUint8(outColor[1]); // outColor 1 to colorDst[1]
    *(colorDst[2]) = RGBFloatToUint8(outColor[2]); // outColor 2 to colorDst[2]

    return std::make_pair(len, 4);
}

bool ConvertBufferColorGamut(std::vector<uint8_t>& dstBuf, const sptr<OHOS::SurfaceBuffer>& srcBuf,
    ColorGamut srcGamut, ColorGamut dstGamut, const std::vector<HDRMetaData>& metaDatas)
{
    RS_TRACE_NAME("ConvertBufferColorGamut");
    
    int32_t pixelFormat = srcBuf->GetFormat();
    if (!IsSupportedFormatForGamutConversion(pixelFormat)) {
        RS_LOGE("ConvertBufferColorGamut: the buffer's format is not supported.");
        return false;
    }
    if (!IsSupportedColorGamut(srcGamut) || !IsSupportedColorGamut(dstGamut)) {
        return false;
    }

    uint32_t bufferSize = srcBuf->GetSize();
    dstBuf.resize(bufferSize);

    auto bufferAddr = srcBuf->GetVirAddr();
    uint8_t* srcStart = static_cast<uint8_t*>(bufferAddr);

    uint32_t offsetDst = 0, offsetSrc = 0;
    auto& srcColorSpace = GetColorSpaceOfCertainGamut(srcGamut, metaDatas);
    auto& dstColorSpace = GetColorSpaceOfCertainGamut(dstGamut, metaDatas);
    while (offsetSrc < bufferSize) {
        uint8_t* dst = &dstBuf[offsetDst];
        uint8_t* src = srcStart + offsetSrc;
        Offset len = ConvertColorGamut(dst, src, pixelFormat, srcColorSpace, dstColorSpace);
        if (len.first == 0 || len.second == 0) {
            return false;
        }
        offsetSrc += len.first;
        offsetDst += len.second;
    }
    dstBuf.resize(offsetDst); // dstBuf size might not be as large ad srcBuf in HDR

    return true;
}

SkImageInfo GenerateSkImageInfo(const sptr<OHOS::SurfaceBuffer>& buffer)
{
    SkColorType colorType = (buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
        kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    return SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        colorType, kPremul_SkAlphaType);
}

// YUV to RGBA: Pixel value conversion table
static int Table_fv1[256] = { -180, -179, -177, -176, -174, -173, -172, -170, -169, -167, -166, -165, -163, -162,
    -160, -159, -158, -156, -155, -153, -152, -151, -149, -148, -146, -145, -144, -142, -141, -139,
    -138, -137,  -135, -134, -132, -131, -130, -128, -127, -125, -124, -123, -121, -120, -118,
    -117, -115, -114, -113, -111, -110, -108, -107, -106, -104, -103, -101, -100, -99, -97, -96,
    -94, -93, -92, -90,  -89, -87, -86, -85, -83, -82, -80, -79, -78, -76, -75, -73, -72, -71,
    -69, -68, -66, -65, -64, -62, -61, -59, -58, -57, -55, -54, -52, -51, -50, -48, -47, -45,
    -44, -43, -41, -40, -38, -37,  -36, -34, -33, -31, -30, -29, -27, -26, -24, -23, -22, -20,
    -19, -17, -16, -15, -13, -12, -10, -9, -8, -6, -5, -3, -2, 0, 1, 2, 4, 5, 7, 8, 9, 11, 12,
    14, 15, 16, 18, 19, 21, 22, 23, 25, 26, 28, 29, 30, 32, 33, 35, 36, 37, 39, 40, 42, 43, 44,
    46, 47, 49, 50, 51, 53, 54, 56, 57, 58, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74, 75, 77,
    78, 79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 105, 106, 107,
    109, 110, 112, 113, 114, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 131, 133, 134,
    136, 137, 138, 140, 141, 143, 144, 145, 147, 148,  150, 151, 152, 154, 155, 157, 158, 159, 161,
    162, 164, 165, 166, 168, 169, 171, 172, 173, 175, 176, 178 };
static int Table_fv2[256] = { -92, -91, -91, -90, -89, -88, -88, -87, -86, -86, -85, -84, -83, -83, -82, -81,
    -81, -80, -79, -78, -78, -77, -76, -76, -75, -74, -73, -73, -72, -71, -71, -70, -69, -68, -68, -67, -66,
    -66, -65, -64, -63, -63, -62, -61, -61, -60, -59, -58, -58, -57, -56, -56, -55, -54, -53, -53, -52, -51,
    -51, -50, -49, -48, -48, -47, -46, -46, -45, -44, -43, -43, -42, -41, -41, -40, -39, -38, -38, -37, -36,
    -36, -35, -34, -33, -33, -32, -31, -31, -30, -29, -28, -28, -27, -26, -26, -25, -24, -23, -23, -22, -21,
    -21, -20, -19, -18, -18, -17, -16, -16, -15, -14, -13, -13, -12, -11, -11, -10, -9, -8, -8, -7, -6, -6,
    -5, -4, -3, -3, -2, -1, 0, 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16,
    17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35,
    36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55,
    55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74,
    75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 90 };
static int Table_fu1[256] = { -44, -44, -44, -43, -43, -43, -42, -42, -42, -41, -41, -41, -40, -40, -40, -39, -39,
    -39, -38, -38, -38, -37, -37, -37, -36, -36, -36, -35, -35, -35, -34, -34, -33, -33, -33, -32, -32, -32, -31,
    -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24,
    -24, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16,
    -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8,
    -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, 0, 0, 0, 1, 1, 1, 2, 2,
    2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13,
    14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23,
    23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32,
    32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41,
    41, 42, 42, 42, 43, 43 };
static int Table_fu2[256] = { -227, -226, -224, -222, -220, -219, -217, -215, -213, -212, -210, -208, -206, -204,
    -203, -201, -199, -197, -196, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178, -176, -174, -173,
    -171, -169, -167, -165, -164, -162, -160, -158, -157, -155, -153, -151, -149, -148, -146, -144, -142, -141,
    -139, -137, -135, -134, -132, -130, -128, -126, -125, -123, -121, -119, -118, -116, -114, -112, -110, -109,
    -107, -105, -103, -102, -100, -98, -96, -94, -93, -91, -89, -87, -86, -84, -82, -80, -79, -77, -75, -73,
    -71, -70, -68, -66, -64, -63, -61, -59, -57, -55, -54, -52, -50, -48, -47, -45, -43, -41, -40, -38, -36,
    -34, -32, -31, -29, -27, -25, -24, -22, -20, -18, -16, -15, -13, -11, -9, -8, -6, -4, -2, 0, 1, 3, 5, 7, 8,
    10, 12, 14, 15, 17, 19, 21, 23, 24, 26, 28, 30, 31, 33, 35, 37, 39, 40, 42, 44, 46, 47, 49, 51, 53, 54, 56,
    58, 60, 62, 63, 65, 67, 69, 70, 72, 74, 76, 78, 79, 81, 83, 85, 86, 88, 90, 92, 93, 95, 97, 99, 101, 102,
    104, 106, 108, 109, 111, 113, 115, 117, 118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 141,
    143, 145, 147, 148, 150, 152, 154, 156, 157, 159, 161, 163, 164, 166, 168, 170, 172, 173, 175, 177, 179, 180,
    182, 184, 186, 187, 189, 191, 193, 195, 196, 198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218,
    219, 221, 223, 225 };

bool ConvertYUV420SPToRGBA(std::vector<uint8_t>& rgbaBuf, const sptr<OHOS::SurfaceBuffer>& srcBuf)
{
    if (srcBuf == nullptr || rgbaBuf.empty()) {
        RS_LOGE("RSBaseRenderUtil::ConvertYUV420SPToRGBA invalid params");
        return false;
    }
    int32_t bufferHeight = srcBuf->GetHeight();
    int32_t bufferStride = srcBuf->GetStride();
    int32_t bufferWidth = srcBuf->GetWidth();
    if (bufferStride < 1 || bufferWidth < 1 || bufferHeight < 1) {
        RS_LOGE("RSBaseRenderUtil::ConvertYUV420SPToRGBA invalid buffer size");
        return false;
    }
    uint8_t* rgbaDst = &rgbaBuf[0];
    auto bufferAddr = srcBuf->GetVirAddr();
    uint8_t* src = static_cast<uint8_t*>(bufferAddr);
    if (src == nullptr || rgbaDst == nullptr) {
        RS_LOGE("RSBaseRenderUtil::ConvertYUV420SPToRGBA null buffer ptr");
        return false;
    }
    uint8_t* ybase = src;
    int32_t len = bufferStride * bufferHeight;
#ifdef PADDING_HEIGHT_32
    // temporally only update buffer len for video stream
    if (srcBuf->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
        int32_t paddingBase = 32;
        float yuvSizeFactor = 1.5f; // y:uv = 2:1
        int32_t paddingHeight = ((bufferHeight - 1) / paddingBase + 1) * paddingBase;
        int32_t totalSize = static_cast<int32_t>(srcBuf->GetSize());
        int32_t paddingSize = static_cast<int32_t>(bufferStride * paddingHeight * yuvSizeFactor);
        if (totalSize >= paddingSize) {
            len = bufferStride * paddingHeight;
        }
    }
#endif
    uint8_t* ubase = &src[len];

    int rgb[3] = {0, 0, 0};
    int idx = 0;
    int rdif = 0;
    int invgdif = 0;
    int bdif = 0;
    for (int i = 0; i < bufferHeight; i++) {
        for (int j = 0; j < bufferWidth; j++) {
            int Y = static_cast<int>(ybase[i * bufferStride + j]);
            int U = static_cast<int>(ubase[i / 2 * bufferStride + (j / 2) * 2 + 1]);
            int V = static_cast<int>(ubase[i / 2 * bufferStride + (j / 2) * 2]);
            if (srcBuf->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
                std::swap(U, V);
            }
            rdif = Table_fv1[V];
            invgdif = Table_fu1[U] + Table_fv2[V];
            bdif = Table_fu2[U];

            rgb[0] = Y + rdif;
            rgb[1] = Y - invgdif;
            rgb[2] = Y + bdif; // 2 is index

            for (int k = 0; k < 3; k++) { // 3 is index
                idx = (i * bufferWidth + j) * 4 + k; // 4 is color channel
                if (rgb[k] >=0 && rgb[k] <= 255) { // 255 is upper threshold
                    rgbaDst[idx] = static_cast<uint8_t>(rgb[k]);
                } else {
                    rgbaDst[idx] = (rgb[k] < 0) ? 0 : 255; // 255 is upper threshold
                }
            }
            ++idx;
            rgbaDst[idx] = 255; // 255 is upper threshold
        }
    }
    return true;
}
} // namespace Detail

void RSBaseRenderUtil::DropFrameProcess(RSSurfaceHandler& node)
{
    auto availableBufferCnt = node.GetAvailableBufferCount();
    const auto& surfaceConsumer = node.GetConsumer();
    if (surfaceConsumer == nullptr) {
        RS_LOGE("RsDebug RSBaseRenderUtil::DropFrameProcess (node: %" PRIu64 "): surfaceConsumer is null!",
            node.GetNodeId());
        return;
    }

    int maxDirtyListSize = surfaceConsumer->GetQueueSize() - 1;
    // maxDirtyListSize > 2 means QueueSize >3 too
    if (maxDirtyListSize > 2 && availableBufferCnt >= maxDirtyListSize) {
        RS_TRACE_NAME("DropFrame");
        OHOS::sptr<SurfaceBuffer> cbuffer;
        Rect damage;
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        auto ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGW("RSBaseRenderUtil::DropFrameProcess(node: %" PRIu64 "): AcquireBuffer failed(ret: %d), do nothing ",
                node.GetNodeId(), ret);
            return;
        }

        ret = surfaceConsumer->ReleaseBuffer(cbuffer, SyncFence::INVALID_FENCE);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGW("RSBaseRenderUtil::DropFrameProcess(node: %" PRIu64
                    "): ReleaseBuffer failed(ret: %d), Acquire done ",
                node.GetNodeId(), ret);
        }
        availableBufferCnt = node.ReduceAvailableBuffer();
        RS_LOGD("RsDebug RSBaseRenderUtil::DropFrameProcess (node: %" PRIu64 "), drop one frame", node.GetNodeId());
    }

    return;
}

bool RSBaseRenderUtil::ConsumeAndUpdateBuffer(RSSurfaceHandler& surfaceHandler)
{
    auto availableBufferCnt = surfaceHandler.GetAvailableBufferCount();
    if (availableBufferCnt <= 0) {
        // this node has no new buffer, try use old buffer.
        return true;
    }
    auto& consumer = surfaceHandler.GetConsumer();
    if (consumer == nullptr) {
        return false;
    }

    DropFrameProcess(surfaceHandler);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    auto ret = consumer->AcquireBuffer(buffer, acquireFence, timestamp, damage);
    if (buffer == nullptr || ret != SURFACE_ERROR_OK) {
        RS_LOGE("RsDebug surfaceHandler(id: %" PRIu64 ") AcquireBuffer failed(ret: %d)!",
            surfaceHandler.GetNodeId(), ret);
        return false;
    }

    surfaceHandler.SetBuffer(buffer, acquireFence, damage, timestamp);
    RS_LOGD("RsDebug surfaceHandler(id: %" PRIu64 ") AcquireBuffer success, timestamp = %" PRId64 ".",
        surfaceHandler.GetNodeId(), timestamp);
    availableBufferCnt = surfaceHandler.ReduceAvailableBuffer();
    return true;
}

bool RSBaseRenderUtil::ReleaseBuffer(RSSurfaceHandler& surfaceHandler)
{
    auto& consumer = surfaceHandler.GetConsumer();
    if (consumer == nullptr) {
        return false;
    }

    auto& preBuffer = surfaceHandler.GetPreBuffer();
    if (preBuffer.buffer != nullptr) {
        auto ret = consumer->ReleaseBuffer(preBuffer.buffer, preBuffer.releaseFence);
        if (ret != OHOS::SURFACE_ERROR_OK) {
            RS_LOGE("RsDebug surfaceHandler(id: %" PRIu64 ") ReleaseBuffer failed(ret: %d)!",
                surfaceHandler.GetNodeId(), ret);
            return false;
        }
        // reset prevBuffer if we release it successfully,
        // to avoid releasing the same buffer next frame in some situations.
        preBuffer.Reset();
    }

    return true;
}

bool RSBaseRenderUtil::IsBufferValid(const sptr<SurfaceBuffer>& buffer)
{
    if (!buffer) {
        RS_LOGE("RSBaseRenderUtil: buffer is nullptr");
        return false;
    }
    auto addr = buffer->GetVirAddr();
    if (addr == nullptr) {
        RS_LOGE("RSBaseRenderUtil: buffer has no vir addr");
        return false;
    }
    if (buffer->GetWidth() <= 0 || buffer->GetHeight() <= 0) {
        RS_LOGE("RSBaseRenderUtil: this buffer has negative width or height [%d %d]",
            buffer->GetWidth(), buffer->GetHeight());
        return false;
    }
    return true;
}

bool RSBaseRenderUtil::ConvertBufferToBitmap(sptr<SurfaceBuffer> buffer, ColorGamut dstGamut, SkBitmap& bitmap,
    const std::vector<HDRMetaData>& metaDatas)
{
    if (!IsBufferValid(buffer)) {
        return false;
    }
    bool bitmapCreated = false;
    ColorGamut srcGamut = static_cast<ColorGamut>(buffer->GetSurfaceBufferColorGamut());
    // [PLANNING]: We will not use this tmp newBuffer if we use GPU to do the color conversions.
    // Attention: make sure newBuffer's lifecycle is longer than the moment call drawBitmap
    if (buffer->GetFormat() == PIXEL_FMT_YCRCB_420_SP || buffer->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
        std::vector<uint8_t> newBuffer;
        bitmapCreated = CreateYuvToRGBABitMap(buffer, newBuffer, bitmap);
    } else if (buffer->GetFormat() == STUB_PIXEL_FMT_RGBA_16161616) {
        bitmapCreated = CreateNewColorGamutBitmap(buffer, bitmap, ColorGamut::COLOR_GAMUT_SRGB, dstGamut, metaDatas);
    } else if (srcGamut != dstGamut) {
        RS_LOGD("RSBaseRenderUtil::ConvertBufferToBitmap: need to convert color gamut.");
        bitmapCreated = CreateNewColorGamutBitmap(buffer, bitmap, srcGamut, dstGamut);
    } else {
        bitmapCreated = CreateBitmap(buffer, bitmap, buffer->GetVirAddr());
    }
    return bitmapCreated;
}

bool RSBaseRenderUtil::CreateYuvToRGBABitMap(sptr<OHOS::SurfaceBuffer> buffer, std::vector<uint8_t>& newBuffer,
    SkBitmap& bitmap)
{
    newBuffer.resize(buffer->GetWidth() * buffer->GetHeight() * 4, 0); // 4 is color channel
    if (!Detail::ConvertYUV420SPToRGBA(newBuffer, buffer)) {
        return false;
    }

    SkColorType colorType = kRGBA_8888_SkColorType;
    SkImageInfo imageInfo = SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(),
        colorType, kPremul_SkAlphaType);
    SkPixmap pixmap(imageInfo, newBuffer.data(), buffer->GetWidth() * 4); // 4 is color channel
    return bitmap.installPixels(pixmap);
}

bool RSBaseRenderUtil::CreateBitmap(sptr<OHOS::SurfaceBuffer> buffer, SkBitmap& bitmap, const void* addr)
{
    SkImageInfo imageInfo = Detail::GenerateSkImageInfo(buffer);
    SkPixmap pixmap(imageInfo, buffer->GetVirAddr(), buffer->GetStride());
    return bitmap.installPixels(pixmap);
}

bool RSBaseRenderUtil::CreateNewColorGamutBitmap(sptr<OHOS::SurfaceBuffer> buffer, SkBitmap& bitmap,
    ColorGamut srcGamut, ColorGamut dstGamut, const std::vector<HDRMetaData>& metaDatas)
{
    std::vector<uint8_t> newGamutBuffer;
    bool convertRes = Detail::ConvertBufferColorGamut(newGamutBuffer, buffer, srcGamut, dstGamut, metaDatas);
    if (convertRes) {
        RS_LOGD("CreateNewColorGamutBitmap: convert color gamut succeed, use new buffer to create bitmap.");
        return CreateBitmap(buffer, bitmap, newGamutBuffer.data());
    } else {
        RS_LOGD("CreateNewColorGamutBitmap: convert color gamut failed, use old buffer to create bitmap.");
        return CreateBitmap(buffer, bitmap, buffer->GetVirAddr());
    }
}

#ifdef RS_ENABLE_EGLIMAGE
bool RSBaseRenderUtil::ConvertBufferToEglImage(sptr<SurfaceBuffer> buffer,
    std::shared_ptr<RSEglImageManager> eglImageManager, GrContext* grContext, sptr<SyncFence> acquireFence,
    sk_sp<SkImage>& image)
{
    if (!IsBufferValid(buffer) || eglImageManager == nullptr || grContext == nullptr) {
        RS_LOGE("RSBaseRenderUtil::ConvertBufferToEglImage invalid param!");
        return false;
    }
    auto eglTextureId = eglImageManager->MapEglImageFromSurfaceBuffer(buffer, acquireFence);
    if (eglTextureId == 0) {
        RS_LOGE("RSBaseRenderUtil::ConvertBufferToEglImage MapEglImageFromSurfaceBuffer return invalid texture ID");
        return false;
    }
    SkColorType colorType = (buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
        kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, eglTextureId, GL_RGBA8 };
    GrBackendTexture backendTexture(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
        GrMipMapped::kNo, grExternalTextureInfo);
    image = SkImage::MakeFromTexture(grContext, backendTexture,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
    return true;
}
#endif

std::unique_ptr<RSTransactionData> RSBaseRenderUtil::ParseTransactionData(MessageParcel& parcel)
{
    RS_TRACE_NAME("UnMarsh RSTransactionData: data size:" + std::to_string(parcel.GetDataSize()));
    auto transactionData = parcel.ReadParcelable<RSTransactionData>();
    std::unique_ptr<RSTransactionData> transData(transactionData);
    return transData;
}
} // namespace Rosen
} // namespace OHOS
