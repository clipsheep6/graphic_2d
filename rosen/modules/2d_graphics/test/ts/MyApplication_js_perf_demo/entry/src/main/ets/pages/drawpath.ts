
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

export class DrawPath extends TestBase {

  public gCoord: number[] = [10, 10, 15, 5, 20, 20]

  public constructor() {
    super();
    // 根据需求，如果与默认值不一样，请继承重写
    this.fileName_ = "drawpath";
  }

  public OnTestFunctionCpu(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(5);
    canvas.attachPen(pen);

    const path = new drawing.Path();
    path.moveTo(this.gCoord[0], this.gCoord[1])
    path.lineTo(this.gCoord[2], this.gCoord[3])
    path.lineTo(this.gCoord[4], this.gCoord[5])
    path.close()

    canvas.drawPath(path);
  }

  public OnTestFunctionGpuUpScreen(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(5);
    canvas.attachPen(pen);

    const path = new drawing.Path();
    path.moveTo(this.gCoord[0], this.gCoord[1])
    path.lineTo(this.gCoord[2], this.gCoord[3])
    path.lineTo(this.gCoord[4], this.gCoord[5])
    path.close()


    canvas.drawPath(path);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(5);
    canvas.attachPen(pen);

    const path = new drawing.Path();
    path.moveTo(this.gCoord[0], this.gCoord[1])
    path.lineTo(this.gCoord[2], this.gCoord[3])
    path.lineTo(this.gCoord[4], this.gCoord[5])
    path.close()

    for (let index = 0; index < this.testCount_; index++) {
      canvas.attachPen(pen);
      canvas.drawPath(path);
      canvas.attachPen(pen);
    }
  }
}