
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

const TAG = '[DrawingTest]';

export class DrawCircle extends TestBase {

  size_t: number = 100

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawcircle";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(10);
    canvas.attachPen(pen);
    canvas.drawCircle(150, 150, 150)
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 }
    let q: number = this.width_ / (this.testCount_ + 1)

    for (let index = 0; index < this.testCount_; index++) {

      let x: number = q * index

      pen.setColor(penColor);
      pen.setStrokeWidth(10);
      canvas.attachPen(pen);
      canvas.drawCircle(x, x, q / 2)
      canvas.detachPen()
    }
  }
}