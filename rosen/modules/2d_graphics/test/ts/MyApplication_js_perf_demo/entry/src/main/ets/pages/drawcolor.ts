/*
 * CanvasSaveRestoreBench
 */

import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

const TAG = '[DrawingTest]';


export class DrawColor extends TestBase {

  fDepth: number = 8

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawcolor";

  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let color: common2D.Color = {
      alpha : 255,
      red: 0,
      green: 255,
      blue: 255
    }
    canvas.drawColor(color);
  }

  // canvas_save_restore_1
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    let color: common2D.Color = {
      alpha : 255,
      red: 0,
      green: 255,
      blue: 255
    }

    for (let index = 0; index < this.testCount_; index++) {
      // canvas->save(); 该接口未实现
      // canvas->concat(m); 该接口未实现
      canvas.drawColor(color);
      // canvas->restore(); 该接口未实现
    }
  }
}