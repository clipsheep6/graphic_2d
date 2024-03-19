import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { OHRandom } from '../utils/OHRandom';

const TAG = '[DrawingTest]';

// LineBench
export class DrawPoint extends TestBase {

  public fStrokeWidth: number
  public fDoAA: boolean
  PTS: number = 500
  fPts = []

  public constructor(fStrokeWidth: number, fDoAA: boolean) {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawpoint";

    this.fStrokeWidth = fStrokeWidth
    this.fDoAA = fDoAA
    console.log("www data new drawline")

    let rand = new OHRandom()
    for (let i = 0; i < this.PTS; i++) {
      this.fPts.push({ x: rand.nextUScalar1() * this.width_, y: rand.nextUScalar1() * this.height_ })
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    pen.setAntiAlias(this.fDoAA)
    pen.setStrokeWidth(this.fStrokeWidth);
    canvas.attachPen(pen);
    canvas.drawPoint(this.fPts[0].x, this.fPts[0].y)
    canvas.detachPen()
  }

  // "lines_1_AA"
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    pen.setAntiAlias(this.fDoAA)
    pen.setStrokeWidth(this.fStrokeWidth);
    canvas.attachPen(pen);

    for (let index = 0; index < this.testCount_; index++) {
      for (let i = 0; i < this.fPts.length; i++) {
        canvas.drawPoint(this.fPts[i].x, this.fPts[i].y)
      }
    }

    canvas.detachPen()
  }
}