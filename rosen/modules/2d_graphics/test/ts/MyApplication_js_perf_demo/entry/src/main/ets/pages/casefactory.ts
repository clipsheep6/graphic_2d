/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import {TestBase} from './testbase';
import {DrawRect} from './drawrect';
import {DrawPath} from './drawpath';
import { SrcModeRectBench } from './SrcModeRectBench';
import { BezierBench } from './BezierBench';
import { XfermodeBench } from './XfermodeBench';
import { BlitMaskBench } from './BlitMaskBench';
import { TextBlobFirstTimeBench } from './TextBlobFirstTimeBench';
import { RectBench } from './rectBench';

import {DrawLine} from './drawline';
import {DrawImage} from './drawimage';
import {DrawCircle} from './drawcircle'
import {DrawTextBlob} from './drawtextblob'
import {DrawColor} from './drawcolor'
import {DrawPoint} from './drawpoint'
import {DrawMoveTo} from './drawmoveto'
import {DrawLineTo} from './drawlineto'
import {DrawArcTo} from './drawarcto'
import {DrawQuadTo} from './drawquadto'
import {DrawCubicTo} from './drawcubicto'
const TAG = '[DrawingTest]';

export class CaseFactory {
  static FunctionCpuMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
      ['drawline', () => { return new DrawLine(1, false); }],
      ['drawimage', () => { return new DrawImage(); }],
      ['drawcircle', () => { return new DrawCircle(); }],
      ['drawtextblob', () => { return new DrawTextBlob(); }],
      ['drawcolor', () => { return new DrawColor(); }],
      ['drawpoint', () => { return new DrawPoint(); }],
      ['drawmoveto', () => { return new DrawMoveTo(); }],
      ['drawlineto', () => { return new DrawLineTo(); }],
      ['drawarcto', () => { return new DrawArcTo(); }],
      ['drawquadto', () => { return new DrawQuadTo(); }],
      ['drawcubicto', () => { return new DrawCubicTo(); }],
    ]
  );
  static PerformanceCpuMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
      ['drawline', () => { return new DrawLine(1, false); }],
      ['drawimage', () => { return new DrawImage(); }],
      ['drawcircle', () => { return new DrawCircle(); }],
      ['drawtextblob', () => { return new DrawTextBlob(); }],
      ['drawcolor', () => { return new DrawColor(); }],
      ['drawpoint', () => { return new DrawPoint(); }],
      ['drawmoveto', () => { return new DrawMoveTo(); }],
      ['drawlineto', () => { return new DrawLineTo(); }],
      ['drawarcto', () => { return new DrawArcTo(); }],
      ['drawquadto', () => { return new DrawQuadTo(); }],
      ['drawcubicto', () => { return new DrawCubicTo(); }],
    ]
  );
  static FunctionGpuUpScreenMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
      ['drawline', () => { return new DrawLine(1, false); }],
      ['drawimage', () => { return new DrawImage(); }],
      ['drawcircle', () => { return new DrawCircle(); }],
      ['drawtextblob', () => { return new DrawTextBlob(); }],
      ['drawcolor', () => { return new DrawColor(); }],
      ['drawpoint', () => { return new DrawPoint(); }],
      ['drawmoveto', () => { return new DrawMoveTo(); }],
      ['drawlineto', () => { return new DrawLineTo(); }],
      ['drawarcto', () => { return new DrawArcTo(); }],
      ['drawquadto', () => { return new DrawQuadTo(); }],
      ['drawcubicto', () => { return new DrawCubicTo(); }],
    ]
  );
  static PerformanceGpuUpScreenMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
      ['rectbenchtrue', () => { return new RectBench(0, 10, true, false); }],
      ['rectbenchfalse', () => { return new RectBench(0, 10, false, false); }],
      ['srcmoderect', () => { return new SrcModeRectBench(); }],
      ['bezierquad', () => { return new BezierBench(10, 0); }],
      ['beziercubic', () => { return new BezierBench(50, 1); }],
      ['xfermodemask', () => { return new XfermodeBench(0, 0); }],
      ['xfermoderect', () => { return new XfermodeBench(0, 1); }],
      ['blitmask', () => { return new BlitMaskBench(); }],
      ['textblob', () => { return new TextBlobFirstTimeBench(); }],
      ['drawline', () => { return new DrawLine(1, false); }],
      ['drawimage', () => { return new DrawImage(); }],
      ['drawcircle', () => { return new DrawCircle(); }],
      ['drawtextblob', () => { return new DrawTextBlob(); }],
      ['drawcolor', () => { return new DrawColor(); }],
      ['drawpoint', () => { return new DrawPoint(); }],
      ['drawmoveto', () => { return new DrawMoveTo(); }],
      ['drawlineto', () => { return new DrawLineTo(); }],
      ['drawarcto', () => { return new DrawArcTo(); }],
      ['drawquadto', () => { return new DrawQuadTo(); }],
      ['drawcubicto', () => { return new DrawCubicTo(); }],
    ]
  );

  static getFunctonCpuAllCase(): Map<string, Function> {
    return this.FunctionCpuMap;
  }

   static getFunctonCpuCase(caseName: string): TestBase {
     let func:Function | undefined = this.FunctionCpuMap.get(caseName);
     if (func == undefined || func == null) {
       console.error(TAG, 'Testcase name is invalid');
       return null;
     }
     return func();
   }

  static getPerformanceCpuCase(caseName: string): TestBase {
    let func:Function | undefined = this.FunctionGpuUpScreenMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }
  static getFunctonGpuUpScreenCase(caseName: string): TestBase {
    let func:Function | undefined = this.FunctionCpuMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }

  static getPerformanceGpuUpScreenCase(caseName: string): TestBase {
    let func:Function | undefined = this.PerformanceGpuUpScreenMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }
}