
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from '../pages/testbase';
import { OHRandom } from '../utils/OHRandom'

const TAG = '[DrawingTest]';

export class CubicPathBench extends TestBase {

  public rand: OHRandom = new OHRandom(12)

  public points: number[] = [
    10, 10, 15, 5, 20, 20,
    30, 5, 25, 20, 15, 12,
    21, 21, 30, 30, 12, 4,
    32, 28, 20, 18, 12, 10
  ]

  public kMaxPathSize: number = 10;

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawcubicto";

    this.points = this.points.map(x => x * 10)

  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const path = new drawing.Path();
    pen.setAntiAlias(true)
    canvas.attachPen(pen);

    // 该值与c++中的位运算得到的数字不同，此处仅模拟运算，不适用该值
    let size: number = this.points.length
    let hsize: number = size / 2
    for (let i = 0; i < this.kMaxPathSize; i++) {
      let xTrans: number = 1 + 40 * (i%(this.kMaxPathSize/2))
      let yTrans: number = 0
      if (i > this.kMaxPathSize/2 - 1) {
        yTrans = 40
      }
      let base1 = 2 * this.rand.nextULessThan(hsize)
      let base2 = 2 * this.rand.nextULessThan(hsize)
      let base3 = 2 * this.rand.nextULessThan(hsize)
      let base4 = 2 * this.rand.nextULessThan(hsize)

      console.log(`www data hsize${hsize} base1 ${base1} base1 ${base2} base1 ${base3}  base1 ${base4} `)
      console.log(`www data x1${this.points[base2] + xTrans} ${this.points[base2+1] + yTrans}`)

      path.moveTo(this.points[base1] + xTrans, this.points[base1+1] + yTrans)
      path.cubicTo(
        this.points[base2] + xTrans, this.points[base2+1] + yTrans,
        this.points[base3] + xTrans, this.points[base3+1] + yTrans,
        this.points[base4] + xTrans, this.points[base4+1] + yTrans,
      )
    }

    canvas.drawPath(path);
    canvas.detachPen()

  }

  // "path_hairline_small_AA_cubic"
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const path = new drawing.Path();
    pen.setAntiAlias(true)
    canvas.attachPen(pen);

    let size: number = this.points.length
    let hsize: number = size / 2
    for (let i = 0; i < this.kMaxPathSize; i++) {
      let xTrans: number = 1 + 40 * (i%(this.kMaxPathSize/2))
      let yTrans: number = 0
      if (i > this.kMaxPathSize/2 - 1) {
        yTrans = 40
      }
      let base1 = 2 * this.rand.nextULessThan(hsize)
      let base2 = 2 * this.rand.nextULessThan(hsize)
      let base3 = 2 * this.rand.nextULessThan(hsize)
      let base4 = 2 * this.rand.nextULessThan(hsize)

      // console.log(`www data hsize${hsize} base1 ${base1} base1 ${base2} base1 ${base3}  base1 ${base4} `)
      // console.log(`www data x1${this.points[base2] + xTrans} ${this.points[base2+1] + yTrans}`)
      path.moveTo(this.points[base1] + xTrans, this.points[base1+1] + yTrans)
      path.cubicTo(
        this.points[base2] + xTrans, this.points[base2+1] + yTrans,
        this.points[base3] + xTrans, this.points[base3+1] + yTrans,
        this.points[base4] + xTrans, this.points[base4+1] + yTrans,
      )
    }

    for (let index = 0; index < this.testCount_; index++) {
        canvas.drawPath(path);
    }
    canvas.detachPen()
  }
}