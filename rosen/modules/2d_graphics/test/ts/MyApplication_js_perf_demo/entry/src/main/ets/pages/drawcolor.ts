
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

const TAG = '[DrawingTest]';

export class DrawColor extends TestBase {

  PTS: number = 500
  fPts = [this.PTS]

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawcolor";

    for (let i = 0; i < this.PTS; i++) {
      this.fPts.push()
    }

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

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    let color: common2D.Color = {
      alpha : 255,
      red: 0,
      green: 255,
      blue: 255
    }


    for (let index = 0; index < this.testCount_; index++) {
      canvas.drawColor(color);
    }
  }
}