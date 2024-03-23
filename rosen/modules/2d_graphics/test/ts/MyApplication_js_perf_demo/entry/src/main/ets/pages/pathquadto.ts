
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { OHRandom } from '../utils/OHRandom'

const TAG = '[DrawingTest]';

export class PathQuadto extends TestBase {

  public rand: OHRandom = new OHRandom(12)

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "pathquadto";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const path = new drawing.Path();
    canvas.attachPen(pen);

    // 该值与c++中的位运算得到的数字不同，此处仅模拟运算，不使用该值
    for (let index = 0; index < 100; index++) {
      // ts位运算与c++行为不一致，此处仅模拟运算，不使用该值
      let x1 = this.rand.nextUScalar1() * this.width_
      let y1 = this.rand.nextUScalar1() * this.height_
      let x2 = this.rand.nextUScalar1() * this.width_
      let y2 = this.rand.nextUScalar1() * this.height_
      path.quadTo(x1,y1,x2,y2)
    }
    path.close()

    canvas.drawPath(path);
    canvas.detachPen()

  }

  // path_stroke_small_long_curved
  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const path = new drawing.Path();
    canvas.attachPen(pen);

    for (let index = 0; index < 100; index++) {
      // ts位运算与c++行为不一致，此处仅模拟运算，不使用该值
      let x1 = this.rand.nextUScalar1() * this.width_
      let y1 = this.rand.nextUScalar1() * this.height_
      let x2 = this.rand.nextUScalar1() * this.width_
      let y2 = this.rand.nextUScalar1() * this.height_
      path.quadTo(x1,y1,x2,y2)
    }
    path.close()
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPath(path)
    }
    canvas.detachPen()
  }
}