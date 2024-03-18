
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { OHRandom } from '../utils/OHRandom'

const TAG = 'PATHRESET'

export class PathReset extends TestBase {

  fCurrPath = 0
  fCurrVerb = 0
  fCurrPoint = 0

  kNumVerbCnts = 1 << 5
  kNumVerbs    = 1 << 5
  kNumPoints   = 3 << 5
  fVerbCnts = []
  fVerbs = []
  fPoints = []
  rand: OHRandom = new OHRandom()

  createData(minVerbs: number, maxVerbs: number, allowMoves: boolean = true) {
    for (let i = 0; i < this.kNumVerbCnts; i++) {
      this.fVerbCnts[i] = this.rand.nextRangeU(minVerbs, maxVerbs + 1)
    }
    for (let i = 0; i < this.kNumVerbs; i++) {
      //   fVerbs[i] = static_cast<SkPath::Verb>(fRandom.nextULessThan(SkPath::kDone_Verb));
      this.fVerbs.push(this.rand.nextULessThan(6))
    }
    for (let i = 0; i < this.kNumPoints; i++) {
      this.fPoints.push({ x: this.rand.nextRangeU(minVerbs, maxVerbs + 1), y: this.rand.nextRangeU(minVerbs, maxVerbs + 1) })
    }
    this.restartMakingPaths()
  }

  restartMakingPaths() {
    this.fCurrPath = 0;
    this.fCurrVerb = 0;
    this.fCurrPoint = 0;
  }

  makePath(path: drawing.Path) {
    // let vCount: number = this.fVerbCnts[(this.fCurrPath++)&(this.kNumVerbs - 1)]
    // 待实现
    let vCount = 1 << 5
    for (let v = 0; v < vCount; v++) {
      console.log(TAG + ` before makePath v:${v} fCurrePoint: ${this.fCurrPoint}`)
      let verb: number = this.fVerbs[v]
      let point = this.fPoints[this.fCurrPoint++]
      console.log(TAG + ` in makePath v:${v} verb: ${verb}`)
      switch (verb) {
        case 0:
          path.moveTo(point.x, point.y)
          break
        case 1:
          path.lineTo(point.x, point.y)
          break
        case 2:
          let point1 = this.fPoints[this.fCurrPoint++]
          path.quadTo(point.x, point.y, point1.x, point1.y)
          break
        case 3:
          let point2 = this.fPoints[this.fCurrPoint++]
          // canvas没有conicTo
          path.quadTo(point.x, point.y, point2.x, point2.y)
          break
        case 4:
          let point3 = this.fPoints[this.fCurrPoint++]
          let point4 = this.fPoints[this.fCurrPoint++]
          path.cubicTo(point.x, point.y, point3.x, point3.y, point4.x, point4.y)
          break
        case 5:
          path.close()
          break
        default:
          console.log(TAG + ' Unexpected path verb')
          break
      }
    }
  }

  public constructor() {
    super();
    // 根据需求，如果与默认值不一样，请继承重写
    this.fileName_ = "pathreset";
    this.createData(10, 100)

    console.log(TAG + ' fVerbCnts' + this.fVerbCnts);
    console.log(TAG + ' fVerb' + this.fVerbs);
    console.log(TAG + ' fPoints' + JSON.stringify(this.fPoints));
  }

  public OnTestFunctionGpuUpScreen(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    console.log(TAG + "start ")
    const path = new drawing.Path();

    try {
      this.makePath(path)
      this.restartMakingPaths()
    } catch (e) {
      console.log(TAG + 'failed make path ' + JSON.stringify(e))
    }

    const pen = new drawing.Pen();
    pen.setStrokeWidth(5);
    pen.setColor({alpha: 255, red: 255, green: 0, blue: 0});
    canvas.attachPen(pen);
    canvas.drawPath(path)
    canvas.detachPen();
    console.log(TAG + " end")
  }

  // path_create
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试
    const path = new drawing.Path();
    for (let i = 0; i < this.testCount_; i++) {
      if (i % 1000 === 0) {
        path.reset()
      }
      this.makePath(path)
      this.restartMakingPaths()
    }
    const pen = new drawing.Pen();
    // pen.setStrokeWidth(5);
    pen.setColor({alpha: 255, red: 255, green: 255, blue: 255});
    pen.setAntiAlias(true)
    // paint.setStyle(SkPaint::kStroke_Style) // 未实现
    canvas.attachPen(pen);
    canvas.drawPath(path);
    canvas.detachPen();
  }
}