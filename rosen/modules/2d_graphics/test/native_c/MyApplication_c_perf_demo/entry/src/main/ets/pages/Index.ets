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

import XComponentContext from "../interface/XComponentContext";
import image from '@ohos.multimedia.image';

const TAG = '[DrawingTest]';

@Entry
@Component
struct Index {
  private xComponentContext: XComponentContext | undefined = undefined;
  // scroller: Scroller = new Scroller()
  @State testType: string = 'function';
  @State drawingType: string = 'cpu';
  @State caseNameStr: string  = 'drawrect';
  @State information: string = '';
  imagePackerApi: image.ImagePacker = image.createImagePacker();
  caseNameStrBefore: string  = '';
  scroller: Scroller = new Scroller();
  caseCountStr: string = '10';
  testAll: boolean = false;

  build() {
    Column() {
      Row() {
        XComponent({ id: 'xcomponentId', type: 'surface', libraryname: 'entry' })
          .onLoad((xComponentContext) => {
            this.xComponentContext = xComponentContext as XComponentContext;
          }).width('100%') // Multiples of 64
      }.height('60%')

      Row() {
        Select([
          { value: 'function' },
          { value: 'performance' },
        ])
          .selected(0)
          .value(this.testType)
          .padding({left:5})
          .border({width:1})
          .width('40%')
          .onSelect((index: number, value: string)=>{
            this.testType = value;
            console.log(TAG, "select: " + this.testType);
          })

        Select([
          { value: 'cpu' },
          { value: 'gpu' },
        ])
          .selected(0)
          .value(this.drawingType)
          .padding({left:5})
          .border({width:1})
          .width('40%')
          .onSelect((index: number, value: string)=>{
            this.drawingType = value;
            console.log(TAG, "select: " + this.drawingType);
          })
      }
      .height('8%')
      Row() {
        Text('TestCase')
          .fontSize(14)
          .textAlign(TextAlign.End)
          .height(38)
          .margin({right:5})
        Column() {
          TextInput({ placeholder: 'ex:drawrect', text: this.caseNameStr})
            .enterKeyType(EnterKeyType.Send)
            .placeholderFont({size: 15})
            .height(38)
            .width('60%')
            .border({width: 1, radius:10, color: '#88000000'})
            .onChange((value) => {
              if (value != "") {
                this.caseNameStr = value
              }
            })
        }
      }
      .height('8%')
      .width('80%')
      .margin({left:5})

      if(this.testType == 'performance') {
        Row() {
          Text('TestCount')
            .fontSize(14)
            .textAlign(TextAlign.End)
            .height(38)
            .margin({right:5})
          Column() {
            TextInput({ placeholder: '10'})
              .enterKeyType(EnterKeyType.Send)
              .placeholderFont({size: 15})
              .height(38)
              .width('60%')
              .border({width: 1, radius:10, color: '#88000000'})
              .onChange((value) => {
                if (value != "") {
                  this.caseCountStr = value
                }
              })
          }
        }
        .width('80%')
        .height('8%')
      } else {
        Row(){
          Text('TestAll')
            .fontSize(14)
            .textAlign(TextAlign.Start)
            .height(38)
            .margin({right:5})
          Toggle({type:ToggleType.Switch, isOn:false})
            .selectedColor('#007dff')
            .switchPointColor('#ffffff')
            .onChange((isOn:boolean)=>{
              this.testAll = isOn;
              if (this.testAll) {
                this.caseNameStrBefore = this.caseNameStr;
                this.caseNameStr = 'All';
              } else {
                this.caseNameStr = this.caseNameStrBefore;
              }
            })
        }
        .width('80%')
        .height('8%')
      }
      Row() {
        Button("draw")
          .fontSize('16fp')
          .fontWeight(500)
            // .margin({ bottom: 24, right: 12 })
          .onClick(() => {
            console.log(TAG, "Draw click");
            this.Draw();
          })
          .width('80%')
          .shadow(ShadowStyle.OUTER_DEFAULT_LG)
      }
      .height('8%')
      Row() {
        Text(this.information)
          .fontSize(20)
          .textAlign(TextAlign.Center)
      }
      .height('8%')
    }
  }


  LogMsg(msg: string) {
    this.information = msg;
    console.log(TAG, msg);
  }

  async Draw():Promise<void> {
    console.log(TAG, 'Drawing test start!');
    if (this.testType == 'function') {
      if (this.drawingType == 'cpu') {
        return this.TestFunctionalCpu();
      } else {
        this.LogMsg('GPU drawing function test not yet provided!');
      }
    }
    if (this.testType == 'performance') {
      if (this.drawingType == 'cpu') {
        return this.TestPerformanceCpu();
      } else {
        this.LogMsg('GPU drawing performance test not yet provided!');
      }
    }
  }

  async TestFunctionalCpu() {
    console.log(TAG, 'TestFunctionalCpu start');
    if (this.xComponentContext) {
      this.xComponentContext.testFunctionCpu(this.caseNameStr);
    }
    this.LogMsg('TestFunctionalCpu end');
  }

  async TestPerformanceCpu() {
    console.log(TAG, 'TestPerformanceCpu start');

    if (this.xComponentContext) {
      let testCount = Number.parseInt(this.caseCountStr, 10);
      this.xComponentContext.setTestCount(testCount);
      this.xComponentContext.testPerformanceCpu(this.caseNameStr);
      let time = this.xComponentContext.getTime();
      this.LogMsg('TestCount: ' + testCount + ', used: ' + time.toString() + 'ms');
    }
    console.log(TAG, 'TestPerformanceCpu end');
  }
}
