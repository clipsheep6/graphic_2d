
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { OHRandom } from '../utils/OHRandom'

const TAG = '[DrawingTest]';

export class DrawQuadTo extends TestBase {

  public rand: OHRandom = new OHRandom(12)

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawarcto";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const path = new drawing.Path();
    canvas.attachPen(pen);

    // 该值与c++中的位运算得到的数字不同，此处仅模拟运算，不适用该值
    this.rand.nextUScalar1()
    this.rand.nextUScalar1()
    this.rand.nextUScalar1()
    this.rand.nextUScalar1()

    let x1 = Math.random() * this.width_
    let y1 = Math.random() * this.height_
    let x2 = Math.random() * this.width_
    let y2 = Math.random() * this.height_

    path.quadTo(x1,y1,x2,y2)
    path.close()

    canvas.drawPath(path);
    canvas.detachPen()

  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    for (let index = 0; index < this.testCount_; index++) {
      const pen = new drawing.Pen();
      const path = new drawing.Path();
      canvas.attachPen(pen);

      // ts位运算与c++行为不一致，此处仅模拟运算，不使用该值
      this.rand.nextUScalar1()
      this.rand.nextUScalar1()
      this.rand.nextUScalar1()
      this.rand.nextUScalar1()

      let x1 = Math.random() * this.width_
      let y1 = Math.random() * this.height_
      let x2 = Math.random() * this.width_
      let y2 = Math.random() * this.height_

      path.quadTo(x1,y1,x2,y2)
      path.close()

      canvas.drawPath(path);
      canvas.detachPen()
    }
  }
}