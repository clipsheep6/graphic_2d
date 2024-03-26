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
import { SrcModeRectBench } from '../testcase/SrcModeRectBench';
import { BezierBench } from '../testcase/BezierBench';
import { XfermodeBench } from '../testcase/XfermodeBench';
import { BlitMaskBench } from '../testcase/BlitMaskBench';
import { TextBlobFirstTimeBench } from '../testcase/TextBlobFirstTimeBench';
import { RectBench } from '../testcase/RectBench';

import {DashLineBench} from '../testcase/DashLineBench';
import {DrawBitmapAABench} from '../testcase/DrawBitmapAABench';
import {ClipStrategyBench} from '../testcase/ClipStrategyBench'
import {TextBlobCachedBench} from '../testcase/TextBlobCachedBench'
import {CanvasSaveRestoreBench} from '../testcase/CanvasSaveRestoreBench'
import {LineBench} from '../testcase/LineBench'
import {TrianglePathBench} from '../testcase/TrianglePathBench'
import {CirclesBench} from '../testcase/CirclesBench'
import {LongCurvedPathBench} from '../testcase/LongCurvedPathBench'
import {CubicPathBench} from '../testcase/CubicPathBench'
import  {PathCreateBench} from '../testcase/PathCreateBench'
const TAG = '[DrawingTest]';

export class CaseFactory {
  static FunctionCpuMap: Map<string, Function> = new Map(
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
    ]
  );
  static PerformanceCpuMap: Map<string, Function> = new Map(
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
    ]
  );
  static FunctionGpuUpScreenMap: Map<string, Function> = new Map(
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
    ]
  );
  static PerformanceGpuUpScreenMap: Map<string, Function> = new Map(
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