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

import {TestBase, StyleType} from './testbase';
import { SrcModeRectBench } from '../testcase/bench/SrcModeRectBench';
import { BezierBench } from '../testcase/bench/BezierBench';
import { XfermodeBench } from '../testcase/bench/XfermodeBench';
import { BlitMaskBench } from '../testcase/bench/BlitMaskBench';
import { TextBlobFirstTimeBench } from '../testcase/bench/TextBlobFirstTimeBench';
import { RectBench } from '../testcase/bench/RectBench';
import {DashLineBench} from '../testcase/bench/DashLineBench';
import {DrawBitmapAABench} from '../testcase/bench/DrawBitmapAABench';
import {ClipStrategyBench} from '../testcase/bench/ClipStrategyBench'
import {TextBlobCachedBench} from '../testcase/bench/TextBlobCachedBench'
import {CanvasSaveRestoreBench} from '../testcase/bench/CanvasSaveRestoreBench'
import {LineBench} from '../testcase/bench/LineBench'
import {TrianglePathBench} from '../testcase/bench/TrianglePathBench'
import {CirclesBench} from '../testcase/bench/CirclesBench'
import {LongCurvedPathBench} from '../testcase/bench/LongCurvedPathBench'
import {CubicPathBench} from '../testcase/bench/CubicPathBench'
import  {PathCreateBench} from '../testcase/bench/PathCreateBench'
import { CanvasDrawRect, CanvasDrawLine, CanvasDrawPath, CanvasDrawPoint, CanvasDrawImage, CanvasDrawCircle, CanvasDrawTextBlob,
  CanvasDrawPixelMapMesh, CanvasDrawColor } from '../testcase/interface/canvastest';
import { PathLineTo, PathArcTo, PathQuadTo, PathCubicTo,PathClose, PathReset } from '../testcase/interface/pathtest';
import { MakeFromRunBuffer } from '../testcase/interface/textblobtest'
import { MakeFromString } from '../testcase/interface/textblobtest'
import { TextBlobBounds } from '../testcase/interface/textblobtest'

const TAG = '[DrawingTest]';

export class CaseFactory {
  static FunctionMap: Map<string, Function> = new Map(
    [
      ['canvasdrawrect', () => { return new CanvasDrawRect(); }],
      ['canvasdrawcircle', () => { return new CanvasDrawCircle(); }],
      ['canvasdrawcolor', () => { return new CanvasDrawColor(); }],
      ['canvasdrawpoint', () => { return new CanvasDrawPoint(); }],
      ['canvasdrawpath', () => { return new CanvasDrawPath(); }],
      ['canvasdrawline', () => { return new CanvasDrawLine(); }],
      ['canvasdrawtextblob', () => { return new CanvasDrawTextBlob(); }],
      ['canvasdrawimage', () => { return new CanvasDrawImage(); }],
    ]
  );
  static PerformanceMap: Map<string, Function> = new Map(
    [
      ['drawrectaa', () => { return new RectBench(0, 10, true, false); }], //drawrect 带aa，随机颜色位置
      ['drawrectnoaa', () => { return new RectBench(0, 10, false, false); }], //drawrect 不带aa，随机颜色位置
      ['drawrectblendmode', () => { return new SrcModeRectBench(); }], //drawrect 带blendmode，半透明
      ['drawrectmode', () => { return new XfermodeBench(0, 1); }], // drawrect，可设置belendmode，随机颜色
      ['drawline', () => { return new DashLineBench(1, false); }], // drawline
      ['drawimage', () => { return new DrawBitmapAABench(); }], // drawimage
      ['drawcircle', () => { return new ClipStrategyBench(10); }], // drawCircle,随机位置
      ['drawtextblob', () => { return new TextBlobCachedBench(); }], // drawTextBlob
      ['drawcolor', () => { return new CanvasSaveRestoreBench(); }], // drawColor
      ['drawpoint', () => { return new LineBench(1, true); }], // drawPoint 随机位置
      ['drawpointblitmask', () => { return new BlitMaskBench(); }], // drawpoint，随机位置，随机颜色，随机alpha
      ['pathcreate32', () => { return new PathCreateBench(); }], // 32次多种构建方式
      ['pathlineto', () => { return new TrianglePathBench(); }], // drawpath line
      ['patharcto', () => { return new CirclesBench(); }], // drawpath 2次arcto随机参数+1次close+drawpath
      ['pathquadto', () => { return new LongCurvedPathBench(); }], // drawpath quadto
      ['pathcubicto', () => { return new CubicPathBench(); }], // drawpath cubicto 随机参数
      ['pathquadpaint', () => { return new BezierBench(10, 0); }], // drawpath 可调整宽度，quadTo构造 其他pen配置未开放
      ['pathcubicpaint', () => { return new BezierBench(50, 1); }], // drawpath 可调整宽度，cubic构造 其他pen配置未开放
      ['drawtextblobmode', () => { return new XfermodeBench(0, 0); }], // drawTextBlob，可设置belendmode
      ['maketextblobdraw', () => { return new TextBlobFirstTimeBench(); }], // textblob构造+drawTextBlob
      
      ['canvas_drawrect', () => { return new CanvasDrawRect(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时109ms
      ['canvasdrawline', () => { return new CanvasDrawLine(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时347ms
      ['canvasdrawpath', () => { return new CanvasDrawPath(StyleType.DRAW_STYLE_COMPLEX); }], // 100次耗时506ms
      ['canvasdrawpoint', () => { return new CanvasDrawPoint(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时47ms
      ['canvasdrawimage', () => { return new CanvasDrawImage(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时2ms
      ['canvasdrawcircle', () => { return new CanvasDrawCircle(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时1016ms
      ['canvasdrawtextblob', () => { return new CanvasDrawTextBlob(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时125ms
      ['pathlineto', () => { return new PathLineTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时70ms
      ['patharcto', () => { return new PathArcTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时42ms
      ['pathquadto', () => { return new PathQuadTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时156ms
      ['pathcubicto', () => { return new PathCubicTo(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时174ms
      ['pathclose', () => { return new PathClose(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时625ms
      ['pathreset', () => { return new PathReset(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时816ms
      ['makefromrunbuffer', () => { return new MakeFromRunBuffer(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时339ms
      ['makefromstring', () => { return new MakeFromString(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时261ms
      ['textblobbounds', () => { return new TextBlobBounds(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时365ms
      ['drawpixelmapmesh', () => { return new CanvasDrawPixelMapMesh(StyleType.DRAW_STYLE_COMPLEX); }], // 1000次耗时261ms
    ]
  );

  static getFunctonCpuAllCase(): Map<string, Function> {
    return this.FunctionMap;
  }

   static getFunctonCpuCase(caseName: string): TestBase {
     let func:Function | undefined = this.FunctionMap.get(caseName);
     if (func == undefined || func == null) {
       console.error(TAG, 'Testcase name is invalid');
       return null;
     }
     return func();
   }

  static getPerformanceCpuCase(caseName: string): TestBase {
    let func:Function | undefined = this.PerformanceMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }
  static getFunctonGpuUpScreenCase(caseName: string): TestBase {
    let func:Function | undefined = this.FunctionMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }

  static getPerformanceGpuUpScreenCase(caseName: string): TestBase {
    let func:Function | undefined = this.PerformanceMap.get(caseName);
    if (func == undefined || func == null) {
      console.error(TAG, 'Testcase name is invalid');
      return null;
    }
    return func();
  }
}