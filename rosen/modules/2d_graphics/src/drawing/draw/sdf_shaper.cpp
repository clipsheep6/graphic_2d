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

#include "draw/sdf_shaper.h"
#include "utils/log.h"
#include <iostream>
#include <cstdio>
#include <string>

namespace OHOS {
namespace Rosen {
namespace Drawing {
const int LEFT_NODE = 0;
const int ADD_STR_LEN = 100;

SDFShape::SDFShape() noexcept
{}

void SDFShape::addCircle()
{
    LOGD("Drawing SDF Circle module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdCircle(p%d,  para%d);", shapeCount_, transCount_, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addEllipse()
{
    LOGD("Drawing SDF Ellipse module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_ELLIPSE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdEllipse(vec2 p, vec2 ab)
{
    p = abs(p); if (p.x > p.y) {p=p.yx; ab=ab.yx;}
    float l = ab.y*ab.y - ab.x*ab.x;
    float m = ab.x*p.x/l;      float m2 = m*m;
    float n = ab.y*p.y/l;      float n2 = n*n;
    float c = (m2 + n2 - 1.0) / 3.0; float c3 = c * c * c;
    float q = c3 + m2*n2*2.0;
    float d = c3 + m2*n2;
    float g = m + m*n2;
    float co;
    if (d < 0.0) {
        float h = acos(q/c3)/3.0;
        float s = cos(h);
        float t = sin(h)*sqrt(3.0);
        float rx = sqrt(-c*(s + t + 2.0) + m2);
        float ry = sqrt(-c*(s - t + 2.0) + m2);
        co = (ry+sign(l)*rx+abs(g)/(rx*ry)- m)/2.0;
    } else {
        float h = 2.0*m*n*sqrt(d);
        float s = sign(q+h)*pow(abs(q+h), 1.0/3.0);
        float u = sign(q-h)*pow(abs(q-h), 1.0/3.0);
        float rx = -s - u - c*4.0 + 2.0*m2;
        float ry = (s - u)*sqrt(3.0);
        float rm = sqrt(rx*rx + ry*ry);
        co = (ry/sqrt(rm-rx) + 2.0*g/rm-m)/2.0;
    }
    vec2 r = ab * vec2(co, sqrt(1.0-co*co));
    return length(r-p) * sign(p.y-r.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_ELLIPSE)] = true;
    }
    for (int i = 0; i < 2; i++) { // 2 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdEllipse(p%d,  vec2(para%d, para%d));",
                    shapeCount_, transCount_, paraCount_-1, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addArc()
{
    LOGD("Drawing SDF ARC module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_ARC)] == false) {
        functionShader_ = functionShader_ + R"(
float sdArc(vec2 p, float r, float a)
{
    vec2 sc = vec2(sin(a), cos(a));
    p.x = abs(p.x);
    return ((sc.y*p.x>sc.x*p.y) ? length(p-sc*r) :  abs(length(p)-r));
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_ARC)] = true;
    }
    for (int i = 0; i < 2; i++) { // 2 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdArc(p%d, para%d, para%d);",
                    shapeCount_, transCount_, paraCount_-1, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addBox()
{
    LOGD("Drawing SDF BOX module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_BOX)] == false) {
        functionShader_ = functionShader_ + R"(
float sdBox(vec2 p, in vec2 b)
{
    vec2 d = abs(p)-b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_BOX)] = true;
    }
    for (int i = 0; i < 2; i++) { // 2 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdBox(p%d, vec2(para%d, para%d));",
                    shapeCount_, transCount_, paraCount_-1, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addEquilateralTriangle()
{
    LOGD("Drawing SDF EquilateralTriangle module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_EQUILATERAL_TRIANGLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdEquilateralTriangle(vec2 p, float r)
{
    const float k = sqrt(3.0);
    p.x = abs(p.x) - r;
    p.y = p.y + r/k;
    if (p.x+k*p.y>0.0) p = vec2(p.x-k*p.y, -k*p.x-p.y)/2.0;
    p.x -= clamp(p.x, -2.0*r, 0.0);
    return -length(p)*sign(p.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_EQUILATERAL_TRIANGLE)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdEquilateralTriangle(p%d,  para%d);",
                    shapeCount_, transCount_, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addRhombus()
{
    LOGD("Drawing SDF Rhombus module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_RHOMBUS)] == false) {
        functionShader_ = functionShader_ + R"(
float ndot(in vec2 a, in vec2 b)
{
    return a.x*b.x - a.y*b.y;
}
float sdRhombus(vec2 p, vec2 b)
{
    p = abs(p);
    float h = clamp(ndot(b-2.0*p, b)/dot(b, b), -1.0, 1.0);
    float d = length(p-0.5*b*vec2(1.0-h, 1.0+h));
    return d * sign(p.x*b.y + p.y*b.x - b.x*b.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_RHOMBUS)] = true;
    }
    for (int i = 0; i < 2; i++) { // 2 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdRhombus(p%d, vec2(para%d, para%d));",
                    shapeCount_, transCount_, paraCount_-1, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addParallelogram()
{
    LOGD("Drawing SDF Parallelogram module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_PARALLELOGRAM)] == false) {
        functionShader_ = functionShader_ + R"(
float sdParallelogram(vec2 p, float wi, float he, float sk)
{
    vec2 e = vec2(sk, he);
    p = (p.y < 0.0) ? -p : p;
    vec2  w = p - e; w.x -= clamp(w.x, -wi, wi);
    vec2  d = vec2(dot(w, w), -w.y);
    float s = p.x*e.y - p.y*e.x;
    p = (s < 0.0) ? -p : p;
    vec2  v = p - vec2(wi, 0); v -= e*clamp(dot(v, e)/dot(e, e), -1.0, 1.0);
    d = min(d, vec2(dot(v, v), wi*he-abs(s)));
    return sqrt(d.x)*sign(-d.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_PARALLELOGRAM)] = true;
    }
    for (int i = 0; i < 3; i++) { // 3 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdParallelogram(p%d, para%d, para%d, para%d);",
                    shapeCount_, transCount_, paraCount_-2, paraCount_-1, paraCount_); //2 is para offset
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addPentagon()
{
    LOGD("Drawing SDF Pentagon module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_PENTAGON)] == false) {
        functionShader_ = functionShader_ + R"(
float sdPentagon(vec2 p, float r)
{
    const vec3 k = vec3(0.809016994, 0.587785252, 0.726542528);
    p.x = abs(p.x);
    p -= 2.0*min(dot(vec2(-k.x, k.y), p), 0.0)*vec2(-k.x, k.y);
    p -= 2.0*min(dot(vec2(k.x, k.y), p), 0.0)*vec2(k.x, k.y);
    p -= vec2(clamp(p.x, -r*k.z, r*k.z), r);
    return length(p)*sign(p.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_PENTAGON)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdPentagon(p%d, para%d);", shapeCount_, transCount_, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addHexagon()
{
    LOGD("Drawing SDF Hexagon module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_HEXAGON)] == false) {
        functionShader_ = functionShader_ + R"(
float sdHexagon(in vec2 p, in float r)
{
    const vec3 k = vec3(-0.866025404, 0.5, 0.577350269);
    p = abs(p);
    p -= 2.0*min(dot(k.xy, p), 0.0)*k.xy;
    p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
    return length(p)*sign(p.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_HEXAGON)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdHexagon(p%d, para%d);", shapeCount_, transCount_, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addOctogon()
{
    LOGD("Drawing SDF Octogon module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_OCTOGON)] == false) {
        functionShader_ = functionShader_ + R"(
float sdOctogon(in vec2 p, in float r)
{
    const vec3 k = vec3(-0.9238795325, 0.3826834323, 0.4142135623);
    p = abs(p);
    p -= 2.0*min(dot(vec2(k.x, k.y), p), 0.0)*vec2(k.x, k.y);
    p -= 2.0*min(dot(vec2(-k.x, k.y), p), 0.0)*vec2(-k.x, k.y);
    p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
    return length(p)*sign(p.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_OCTOGON)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdOctogon(p%d, para%d);", shapeCount_, transCount_, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addSegment()
{
    LOGD("Drawing SDF segment module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] == false) {
        functionShader_ = functionShader_ + R"(
float sdSegment(in vec2 p, in vec2 a, in vec2 b)
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp(dot(pa, ba)/dot(ba, ba), 0.0, 1.0);
    return length(pa - ba*h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] = true;
    }
    
    for (int i = 0; i < 4; i++) { // 4 is para size
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdSegment(p%d, vec2(para%d, para%d), vec2(para%d, para%d));",
                    shapeCount_, transCount_, (paraCount_-3), //3 is para offset.
                    (paraCount_-2), (paraCount_-1), paraCount_);  //2 is para offset.
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opRound()
{
    LOGD("Drawing SDF opround module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_ROUND)] == false) {
        functionShader_ = functionShader_ + R"(
float opRound(float d, float r)
{
    return d - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_ROUND)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opRound(d%d, para%d);", shapeCount_, shapeCount_, paraCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opOnion()
{
    LOGD("Drawing SDF oponion module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_ONION)] == false) {
        functionShader_ = functionShader_ + R"(
float opOnion(float d, float r)
{
    return abs(d) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_ONION)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opOnion(d%d, para%d);", shapeCount_, shapeCount_, paraCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opUnion()
{
    LOGD("Drawing SDF opunion module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_UNION)] == false) {
        functionShader_ = functionShader_ + R"(
float opUnion(float d1, float d2)
{
    return min(d1, d2);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_UNION)] = true;
    }
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opUnion(d%d, d%d);", shapeCount_, shapeCount_-1, shapeCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opSubtraction()
{
    LOGD("Drawing SDF opSubstraction module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_SUBTRACTION)] == false) {
        functionShader_ = functionShader_ + R"(
float opSubtraction(float d1, float d2)
{
    return max(d1, -d2);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_SUBTRACTION)] = true;
    }
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opSubtraction(d%d, d%d);", shapeCount_, shapeCount_-1, shapeCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opIntersection()
{
    LOGD("Drawing SDF opIntersection module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_INTERSECTION)] == false) {
        functionShader_ = functionShader_ + R"(
float opIntersection(float d1, float d2)
{
    return max(d1, d2);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_INTERSECTION)] = true;
    }
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opIntersection(d%d, d%d);", shapeCount_, shapeCount_-1, shapeCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opXor()
{
    LOGD("Drawing SDF opXor module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_XOR)] == false) {
        functionShader_ = functionShader_ + R"(
float opXor(float d1, float d2)
{
    return max(min(d1, d2), -max(d1, d2));
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_XOR)] = true;
    }
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opXor(d%d, d%d);", shapeCount_, shapeCount_-1, shapeCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opSmoothUnion()
{
    LOGD("Drawing SDF opSmoothUnion module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_UNION)] == false) {
        functionShader_ = functionShader_ + R"(
float opSmoothUnion(float d1, float d2, float k)
{
    float h = clamp(0.5 + 0.5*(d2-d1)/k, 0.0, 1.0);
    return mix(d2, d1, h) - k*h*(1.0-h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_UNION)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opSmoothUnion(d%d, d%d, para%d);",
                    shapeCount_, shapeCount_-1, shapeCount_, paraCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opSmoothSubstraction()
{
    LOGD("Drawing SDF opSmoothSubstraction module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_SUBSTRACTION)] == false) {
        functionShader_ = functionShader_ + R"(
float opSmoothSubtraction(float d1, float d2, float k)
{
    float h = clamp(0.5 - 0.5*(d2+d1)/k, 0.0, 1.0);
    return mix(d2, -d1, h) + k*h*(1.0-h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_SUBSTRACTION)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opSmoothSubtraction(d%d, d%d, para%d);",
                    shapeCount_, shapeCount_-1, shapeCount_, paraCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::opSmoothIntersection()
{
    LOGD("Drawing SDF opSmoothIntersection module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_INTERSECTIOPN)] == false) {
        functionShader_ = functionShader_ + R"(
float opSmoothIntersection(float d1, float d2, float k)
{
    float h = clamp(0.5 - 0.5*(d2-d1)/k, 0.0, 1.0);
    return mix(d2, d1, h) + k*h*(1.0-h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_INTERSECTIOPN)] = true;
    }
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "d%d = opSmoothIntersection(d%d, d%d, para%d);",
                    shapeCount_, shapeCount_-1, shapeCount_, paraCount_);
    std::string shape = buf;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addCapsule()
{
    LOGD("Drawing SDF addCapsule module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] == false) {
        functionShader_ = functionShader_ + R"(
float sdSegment(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CAPSULE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCapsule(vec2 p, vec2 a, vec2 b, float r)
{
    return sdSegment(p, a, b) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CAPSULE)] = true;
    }
    
    for (int i = 0; i < 5; i++) { // 5 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdCapsule(p%d, vec2(para%d, para%d), vec2(para%d, para%d), para%d);",
                    shapeCount_, transCount_, (paraCount_ - 4), (paraCount_ - 3), // 4 is offset, 3 is offset.
                    (paraCount_ - 2), (paraCount_ - 1), paraCount_); // 2 is para offset.
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addArrow()
{
    LOGD("Drawing SDF addArrow module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_EQUILATERAL_TRIANGLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdEquilateralTriangle(vec2 p, float r)
{
    const float k = sqrt(3.0);
    p.x = abs(p.x) - r;
    p.y = p.y + r / k;
    if (p.x + k * p.y > 0.0) p = vec2(p.x - k * p.y, -k * p.x - p.y) / 2.0;
    p.x -= clamp(p.x, -2.0 * r, 0.0);
    return -length(p) * sign(p.y);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_EQUILATERAL_TRIANGLE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_ROUND)] == false) {
        functionShader_ = functionShader_ + R"(
float opRound(float d, float r)
{
    return d - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_ROUND)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_SUBSTRACTION)] == false) {
        functionShader_ = functionShader_ + R"(
float opSmoothSubtraction(float d1, float d2, float k)
{
    float h = clamp(0.5 - 0.5 * (d2 + d1) / k, 0.0, 1.0);
    return mix(d2, -d1, h) + k* h * (1.0 - h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_SUBSTRACTION)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_ARROW)] == false) {
        functionShader_ = functionShader_ + R"(
float sdArrow(vec2 p, float r)
{
    float d1 = sdEquilateralTriangle(p, r);
    d1 = opRound(d1, 0.2 * r);
    float d2 = sdCircle(p+vec2(0., 2.5 * r), 2. * r);
    float d = opSmoothSubtraction(d2, d1, 0.2 * r);
    return d;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_ARROW)] = true;
    }
    
    char buf1[ADD_STR_LEN] = {0};
    paraCount_++;
    (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
    std::string para = buf1;
    paraShader_ = paraShader_ + para;

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdArrow(p%d, para%d);", shapeCount_, transCount_, paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addCross()
{
    LOGD("Drawing SDF addCross module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] == false) {
        functionShader_ = functionShader_ + R"(
float sdSegment(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CAPSULE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCapsule(vec2 p, vec2 a, vec2 b, float r)
{
    return sdSegment(p, a, b) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CAPSULE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_UNION)] == false) {
        functionShader_ = functionShader_ + R"(
float opUnion(float d1, float d2)
{
    return min(d1, d2);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_UNION)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CROSS)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCross(vec2 p, float l, float w)
{
    float d1 = sdCapsule(p, vec2(0., l/2.), vec2(0., -l/2.), w);
    float d2 = sdCapsule(p, vec2(l/2., 0.), vec2(-l/2., 0.), w);
    float d = opUnion(d1, d2);
    return d;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CROSS)] = true;
    }

    for (int i = 0; i < 2; i++) { // 2 is para size
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdCross(p%d, para%d, para%d);",
                    shapeCount_, transCount_, (paraCount_ - 1), paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addRing()
{
    LOGD("Drawing SDF addRing module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_ONION)] == false) {
        functionShader_ = functionShader_ + R"(
float opOnion(float d, float thickness)
{
    return abs(d) - thickness;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_ONION)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_RING)] == false) {
        functionShader_ = functionShader_ + R"(
float sdRing(vec2 p, float r, float w)
{
    return opOnion(sdCircle(p, r), w);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_RING)] = true;
    }

    for (int i = 0; i < 2; i++) { // 2 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdRing(p%d, para%d, para%d);",
                    shapeCount_, transCount_, (paraCount_ - 1), paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addPartRingApprox()
{
    LOGD("Drawing SDF addPartRingApprox module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_ONION)] == false) {
        functionShader_ = functionShader_ + R"(
float opOnion(float d, float thickness)
{
    return abs(d) - thickness;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_ONION)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_RING)] == false) {
        functionShader_ = functionShader_ + R"(
float sdRing(vec2 p, float r, float w)
{
    return opOnion(sdCircle(p, r), w);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_RING)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_PART_RING_APPROX)] == false) {
        functionShader_ = functionShader_ + R"(
float sdPartRingApprox(vec2 p, float r, float w, float a)
{
    float d = sdRing(p, r, w);
    return acos(p.x / length(p)) > a ? d : 100.;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_PART_RING_APPROX)] = true;
    }

    for (int i = 0; i < 3; i++) { // 3 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdPartRingApprox(p%d, para%d, para%d, para%d);",
                    shapeCount_, transCount_, (paraCount_ - 2), (paraCount_ - 1), paraCount_); // 2 is para offset.
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addPartRing()
{
    LOGD("Drawing SDF addPartRing module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_PART_RING)] == false) {
        functionShader_ = functionShader_ + R"(
float sdPartRing(vec2 p, float r, float a, float w)
{
    p.x = abs(p.x);
    vec2 cs = vec2(cos(a), sin(a));
    p = mat2x2(cs.x, cs.y, -cs.y, cs.x)*p;

    return max(abs(length(p) - r)- w * 0.5,
               length(vec2(p.x, max(0.0, abs(r - p.y) - w * 0.5))) * sign(p.x));
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_PART_RING)] = true;
    }

    for (int i = 0; i < 3; i++) { // 3 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdPartRing(p%d, para%d, para%d, para%d);",
                    shapeCount_, transCount_, (paraCount_ - 2), (paraCount_ - 1), paraCount_); // 2 is para offset.
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

void SDFShape::addQuestionMark()
{
    LOGD("Drawing SDF addQuestionMark module");
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] == false) {
        functionShader_ = functionShader_ + R"(
float sdSegment(vec2 p, vec2 a, vec2 b)
{
    vec2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba*h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_SEGMENT)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CAPSULE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCapsule(vec2 p, vec2 a, vec2 b, float r)
{
    return sdSegment(p, a, b) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CAPSULE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_ARC)] == false) {
        functionShader_ = functionShader_ + R"(
float sdArc(vec2 p, float r, float a)
{
    vec2 sc = vec2(sin(a), cos(a));
    p.x = abs(p.x);
    return ((sc.y*p.x > sc.x*p.y) ? length(p - sc*r) :  abs(length(p) - r));
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_ARC)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_ROUND)] == false) {
        functionShader_ = functionShader_ + R"(
float opRound(float d, float r)
{
    return d - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_ROUND)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] == false) {
        functionShader_ = functionShader_ + R"(
float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_CIRCLE)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_UNION)] == false) {
        functionShader_ = functionShader_ + R"(
float opSmoothUnion(float d1, float d2, float k)
{
    float h = clamp(0.5 + 0.5*(d2-d1)/k, 0.0, 1.0);
    return mix(d2, d1, h) - k*h*(1.0 - h);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_SMOOTH_UNION)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::OP_UNION)] == false) {
        functionShader_ = functionShader_ + R"(
float opUnion(float d1, float d2)
{
    return min(d1, d2);
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::OP_UNION)] = true;
    }
    if (shapeSet_[static_cast<int>(SDF_TYPE::SD_QUESTION_MARK)] == false) {
        functionShader_ = functionShader_ + R"(
float sdQuestionMark(vec2 p, float r, float w)
{
    float d1 = sdArc(rotate(p, 1.570796*0.45), r, 1.570796*1.45);
    d1 = opRound(d1, w);
    float d2 = sdCapsule(translate(p, vec2(0., -(r+0.5*w))), vec2(0., 0.25*w), vec2(0., -0.25*w), w);
    float d3 = sdCircle(translate(p, vec2(0, -(r + w*4.))), w);
    float d = opSmoothUnion(d1, d2, 0.005);
    d = opUnion(d, d3);
    return d;
}
        )";
        shapeSet_[static_cast<int>(SDF_TYPE::SD_QUESTION_MARK)] = true;
    }

    for (int i = 0; i < 2; i++) { // 2 is para size.
        char buf1[ADD_STR_LEN] = {0};
        paraCount_++;
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float para%d;", paraCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }

    int n = transfrom_.size();
    std::string endTransString;
    for (int i = 0; i < n; i++) {
        std::string transString = transfrom_.top();
        transfrom_.pop();
        endTransString = transString + endTransString;
    }
    char buf[ADD_STR_LEN] = {0};
    shapeCount_++;
    (void)sprintf_s(buf, ADD_STR_LEN, "float d%d = sdQuestionMark(p%d, para%d, para%d);",
                    shapeCount_, transCount_, (paraCount_ - 1), paraCount_);
    std::string shape = buf;
    shape = endTransString + shape;
    shapeShader_ = shapeShader_ + shape;
}

SDFModule* SDFShape::createNewNode(SDFModule** node, int val, SDF_TYPE type) const
{
    SDFModule* newnode = nullptr;
    if (*node == nullptr) {
        *node = new SDFModule(type);
        (*node)->val_ = val;
        LOGD("Drawing SDF create root node");
        newnode = *node ;
    } else if (val == LEFT_NODE) {
        (*node)->leftModule_ = createNewNode(&((*node)->leftModule_), val, type);
        newnode = (*node)->leftModule_;
    } else {
        (*node)->rightModule_ = createNewNode(&((*node)->rightModule_), val, type);
        newnode = (*node)->rightModule_;
    }
    return newnode;
}

void SDFShape::PraseNode(SDFModule* node)
{
    (this->*sdfPraseLUT_[static_cast<int>(node->type_)])();
}

void SDFShape::AddTransform(SDFModule* node)
{
    for (int i = 0; i < 3; i++) { // translate para size is 3
        if (node->isAnimation_ == false) {
            if (node->trans_.size() < 3) { // tranlate para size must be 3.
                return;
            } else {
                transParaCount_++;
                transParams_.push_back(node->trans_[i]);
            }
        } else {
            if (node->animateTrans_.size() < 3) { // tranlate para size must be 3.
                return;
            } else {
                transParaCount_++;
                animateTransParams_.push_back(node->animateTrans_[i]);
                std::vector<std::pair<float, float>> para = node->animateTrans_[i];
                transParams_.push_back(para[0].second);
            }
        }
        char buf1[ADD_STR_LEN] = {0};
        (void)sprintf_s(buf1, ADD_STR_LEN, "uniform float transpara%d;", transParaCount_);
        std::string para = buf1;
        paraShader_ = paraShader_ + para;
    }
    char buf[ADD_STR_LEN] = {0};
    transCount_++;
    int num;
    if (transfromNums_.size() > 0) {
        num = transfromNums_.top();
    } else {
        num = 0;
    }
    (void)sprintf_s(buf, ADD_STR_LEN, "vec2 p%d = transform(p%d, transpara%d, vec2(transpara%d, transpara%d));",
                    transCount_, num, transParaCount_-2, transParaCount_-1, transParaCount_); // 2 is para offset.
    std::string trans = buf;
    transfrom_.push(trans);
    LOGD("sdf m_trandfrom size is %{public}lu.", transfrom_.size());
    transfromNums_.push(transCount_);
}

void SDFShape::DeleteTransform(SDFModule* node)
{
    if (transfromNums_.size() > 0 && (node->trans_.size() > 2 || node->animateTrans_.size() > 2)) { // para size > 2
        transfromNums_.pop();
    }
    if (node->isAnimation_  == false) {
        for (int i = 0; i < node->para_.size(); i++) {
            params_.push_back(node->para_[i]);
        }
    } else {
        for (int i = 0; i < node->animatePara_.size(); i++) {
            animateParams_.push_back(node->animatePara_[i]);
            std::vector<std::pair<float, float>> para = node->animatePara_[i];
            params_.push_back(para[0].second);
        }
    }
}

void SDFShape::SetColor(std::string fillColor, std::string strokeColor)
{
    if (fillColor.size() != 7 || strokeColor.size() != 7 || fillColor[0] != '#' || strokeColor[0] != '#') { //size is 7
        LOGE("sdf color string is error, please check");
        return;
    }
    fillColor = fillColor.substr(1, 6);
    int fillNum = std::stoi(fillColor, NULL, 16);
    int fillRed = (fillNum >> 16) & 0xFF;
    int fillGren = (fillNum >> 8) & 0xFF;
    int fillBlue = fillNum & 0xFF;
    color_[0] = fillRed / (255.0); // 255.0 is color maximum.
    color_[1] = fillGren / (255.0); // color_[1] is fillcolor green channel. 255.0 is color maximum.
    color_[2] = fillBlue / (255.0); // color_[2] is fillcolor blue channel. 255.0 is color maximum.
    strokeColor = strokeColor.substr(1, 6);
    int strokeNum = std::stoi(strokeColor, NULL, 16); // 16 is means hexadecimal.
    int strokeRed = (strokeNum >> 16) & 0xFF;
    int strokeGren = (strokeNum >> 8) & 0xFF;
    int strokeBlue = strokeNum & 0xFF;
    color_[3] = strokeRed / (255.0); // color_[3] is strokecolor red channel. 255.0 is color maximum.
    color_[4] = strokeGren / (255.0); // color_[4] is strokecolor green channel. 255.0 is color maximum.
    color_[5] = strokeBlue / (255.0); // color_[5] is strokecolor blue channel. 255.0 is color maximum.
}

void SDFShape::UpdateTime(float time)
{
    params_.clear();
    transParams_.clear();
    for (int i = 0; i < animateParams_.size(); i++) {
        std::vector<std::pair<float, float>> para = animateParams_[i];
        float tPara = UpdatePara(para, time);
        params_.push_back(tPara);
    }
    for (int i = 0; i < animateTransParams_.size(); i++) {
        std::vector<std::pair<float, float>> para = animateTransParams_[i];
        float tPara = UpdatePara(para, time);
        transParams_.push_back(tPara);
    }
}

float SDFShape::UpdatePara(std::vector<std::pair<float, float>> para, float time)
{
    int n = para.size();
    float value = 0.0;
    if (n < 2 || time < 0.0 || time > 1.0) { // 2 means size of para is error.
        LOGE("sdf animatepara num is error, please check");
        return value;
    }
    value = para[n-1].second;
    for (int i = 0; i < n; i++) {
        if (time < para[i].first) {
            value = para[i-1].second + ((time - para[i-1].first) *
                    ((para[i].second - para[i-1].second) / (para[i].first - para[i-1].first)));
            break;
        }
    }
    return value;
}

void SDFShape::PostOrder(SDFModule* node)
{
    if (node == nullptr) {
        return;
    }
    LOGD("postorder sdf tree node. the node para size is %{public}lu, the node transpara size is %{public}lu",
         node->para_.size(), node->trans_.size());
    AddTransform(node);
    PostOrder(node->leftModule_);
    PostOrder(node->rightModule_);
    PraseNode(node);
    DeleteTransform(node);
}

void SDFShape::DeletePostOrder(SDFModule* node)
{
    if (node == nullptr) {
        return;
    }
    DeletePostOrder(node->leftModule_);
    DeletePostOrder(node->rightModule_);
    DeleteNode(node);
}

void SDFShape::DeleteNode(SDFModule* node)
{
    delete node;
    node = nullptr;
}

void SDFShape::BuildShader()
{
    PostOrder(rootNode_);
    DeletePostOrder(rootNode_);
    char buf[ADD_STR_LEN] = {0};
    (void)sprintf_s(buf, ADD_STR_LEN, "vec4 col = shading(d%d);", shapeCount_);
    std::string str = buf;
    shader_ = paraShader_ + functionShader_ + FILL_STROKE_SHADER + BEGIN_SHADER + shapeShader_ + str + END_SHADER;
}
}
} // namespace Rosen
} // namespace OHOS