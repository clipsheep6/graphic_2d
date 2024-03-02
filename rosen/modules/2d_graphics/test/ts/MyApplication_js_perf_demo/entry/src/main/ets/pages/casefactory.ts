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
const TAG = '[DrawingTest]';

export class CaseFactory {
  static FunctionCpuMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
    ]
  );
  static PerformanceCpuMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
    ]
  );
  static FunctionGpuUpScreenMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
    ]
  );
  static PerformanceGpuUpScreenMap: Map<string, Function> = new Map(
    [
      ['drawrect', () => { return new DrawRect(); }],
      ['drawpath', () => { return new DrawPath(); }],
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