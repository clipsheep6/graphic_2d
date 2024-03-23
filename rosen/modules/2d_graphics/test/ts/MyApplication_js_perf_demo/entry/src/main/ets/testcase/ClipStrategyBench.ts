
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from '../pages/testbase';

const TAG = '[DrawingTest]';

// ClipStrategyBench
export class ClipStrategyBench extends TestBase {

  fCount: number = 100

  public constructor(fCount: number) {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawcircle";
    this.fCount = fCount
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const brush = new drawing.Brush()
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    brush.setColor(penColor)
    brush.setBlendMode(drawing.BlendMode.SRC_IN)
    pen.setColor(penColor);
    pen.setStrokeWidth(10);
    pen.setAntiAlias(true)
    pen.setBlendMode(drawing.BlendMode.SRC_IN)
    canvas.attachPen(pen);
    canvas.attachBrush(pen)
    canvas.drawCircle(150, 150, 150)
    canvas.detachPen()
    canvas.detachPen()
  }

  // ClipStrategyBench_kMask_100
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const brush = new drawing.Brush();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    brush.setColor(penColor)
    pen.setBlendMode(drawing.BlendMode.SRC_IN)
    pen.setAntiAlias(true)
    canvas.attachPen(pen)
    canvas.attachBrush(brush)

    
    let q: number = this.width_ / (this.fCount + 1)
    for (let index = 0; index < this.testCount_; index++) {
        let x: number = q * (index % this.fCount)
        canvas.drawCircle(x, x, q / 2)
    }
    canvas.detachPen()
    canvas.detachBrush()
  }
}