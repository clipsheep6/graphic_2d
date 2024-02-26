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
import image from '@ohos.multimedia.image';
import {TestBase} from './testbase';
import {DrawRect} from './drawrect';
import {DrawPath} from './drawpath';
const TAG = '[Sample_DrawingAPI]';

const DRAWALL: string = 'drawALL';
const DRAWRECT: string = 'drawrect';
const DRAWPATH: string = 'drawpath';

@Entry
@Component
struct Index {
  //用来配置CanvasRenderingContext2D对象的参数，包括是否开启抗锯齿，true表明开启抗锯齿。
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  //用来创建CanvasRenderingContext2D对象，通过在canvas中调用CanvasRenderingContext2D对象来绘制。
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  @State @Watch('Draw')isClick: boolean = false;

  private testMap: Map<string, Function> = new Map();
  aboutToAppear() {
    this.testMap.set(DRAWRECT, () => { return new DrawRect(); });
    this.testMap.set(DRAWPATH, () => { return new DrawPath(); });
  }

  build() {
    Column() {
      Row() {
        Canvas(this.context)
          .width('100%')
          .height('100%')
          .backgroundColor('#000000')
          .onReady(() => {
            //全部绘制
            this.DrawALL();
          })
      }
      .height('50%')

      Row() {
        Button(DRAWRECT)
          .fontSize('16fp')
          .fontWeight(500)
          .margin({ bottom: 24, right: 12 })
          .onClick(() => {
            console.log(TAG, "Draw Path click");
            this.Draw(DRAWRECT);
          })
          .width('30%')
          .height(40)
          .shadow(ShadowStyle.OUTER_DEFAULT_LG)
        Button(DRAWPATH)
          .fontSize('16fp')
          .fontWeight(500)
          .margin({ bottom: 24, left: 12 })
          .onClick(() => {
            console.log(TAG, "draw text click");
            this.Draw(DRAWPATH);
          })
          .width('30%')
          .height(40)
          .shadow(ShadowStyle.OUTER_DEFAULT_LG)
        Button(DRAWALL)
          .fontSize('16fp')
          .fontWeight(500)
          .margin({ bottom: 24, left: 12 })
          .onClick(() => {
            console.log(TAG, "draw text click");
            this.DrawALL();
          })
          .width('30%')
          .height(40)
          .shadow(ShadowStyle.OUTER_DEFAULT_LG)
      }
      .height('10%')
      .width('100%')
      .justifyContent(FlexAlign.Center)
      .shadow(ShadowStyle.OUTER_DEFAULT_SM)
      .alignItems(VerticalAlign.Bottom)
      .layoutWeight(1)
    }
  }

  async Draw(caseName: string) {
    console.error('xyj case name:' + caseName);
    let func:Function | undefined = this.testMap.get(caseName);
    if (func == undefined || func == null) {
      console.error('xyj testcase name is invalid');
      return;
    }

    let test:TestBase = func();
    const context : Context = getContext(this);
    await test.Draw(context.cacheDir);
    //绘制到屏幕上
    this.context.drawImage(test.pixelMap_, 50, 50)
  }

  async DrawALL() {
    console.error('xyj DrawALL');
    const context : Context = getContext(this);
    this.testMap.forEach((func: Function, key: string) => {
      if (func) {
        let test:TestBase = func();
        test.Draw(context.cacheDir);
      }
    });
  }
}
