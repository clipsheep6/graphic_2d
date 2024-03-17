
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

const TAG = '[DrawingTest]';

export class DrawPoint extends TestBase {

  public fStrokeWidth: number
  public fDoAA: boolean

  public constructor(fStrokeWidth: number, fDoAA: boolean) {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawpoint";

    this.fStrokeWidth = fStrokeWidth
    this.fDoAA = fDoAA
    console.log("www data new drawline")
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setAntiAlias(this.fDoAA)
    pen.setStrokeWidth(this.fStrokeWidth);
    canvas.attachPen(pen);
    canvas.drawPoint(150, 150);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setAntiAlias(this.fDoAA)
    pen.setStrokeWidth(this.fStrokeWidth);
    canvas.attachPen(pen);

    for (let index = 0; index < this.testCount_; index++) {
      canvas.drawPoint(150, 150);
    }
  }
}