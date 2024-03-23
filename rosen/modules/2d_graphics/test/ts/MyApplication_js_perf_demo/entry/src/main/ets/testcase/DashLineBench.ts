
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from '../pages/testbase';

const TAG = '[DrawingTest]';

let SK_Scalar1 = 1

export class DashLineBench extends TestBase {

  strokeWidth: number = 0
  fDoAA: boolean

  public constructor(width: number, doAA: boolean) {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "pathlineto";

    this.strokeWidth = width
    this.fDoAA = doAA

    console.log("www data new drawline")
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致

    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(10);
    pen.setAntiAlias(true)
    canvas.attachPen(pen);
    canvas.drawLine(10 * SK_Scalar1, 10 * SK_Scalar1 , 640 * SK_Scalar1, 10 * SK_Scalar1);
    canvas.detachPen()
  }

  // dashline_1_square
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    // pen.setStyle缺失
    pen.setAntiAlias(true)
    pen.setStrokeWidth(1);
    canvas.attachPen(pen);
    for (let index = 0; index < this.testCount_; index++) {
      canvas.drawLine(10, 10, 640, 10);
    }
    canvas.detachPen();
  }
}