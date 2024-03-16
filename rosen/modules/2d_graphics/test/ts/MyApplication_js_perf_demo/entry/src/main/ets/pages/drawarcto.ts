
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { OHRandom } from '../utils/OHRandom'

const TAG = '[DrawingTest]';

export class DrawArcTo extends TestBase {

  public rand: OHRandom = new OHRandom(0)

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawarcto";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致

    console.log("www data on arc to");

    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(5);
    canvas.attachPen(pen);

    const path = new drawing.Path();

    let radius = this.rand.nextUScalar1() * 3
    let fLeft = this.rand.nextUScalar1() * 300
    let fTop = this.rand.nextUScalar1() * 300
    let fRight = fLeft + 2 * radius
    let fBottom = fTop + 2 * radius
    console.log(`www data left: ${fLeft} right: ${fRight} top: ${fTop} bottom: ${fBottom} radius: ${radius}`)
    path.arcTo(150, 150, 300, 300, 0, 180);
    path.close()

    canvas.drawPath(path);
    canvas.detachPen()
    // canvas.drawRect({ left: 150, right: 300, top: 150, bottom: 300 })

  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const pen = new drawing.Pen();
    const path = new drawing.Path();

    for (let index = 0; index < this.testCount_; index++) {
      canvas.attachPen(pen);
      let radius = this.rand.nextUScalar1() * 3
      let fLeft = this.rand.nextUScalar1() * 300
      let fTop = this.rand.nextUScalar1() * 300
      let fRight = fLeft + 2 * radius
      let fBottom = fTop + 2 * radius
      console.log(`www data left: ${fLeft} right: ${fRight} top: ${fTop} bottom: ${fBottom} radius: ${radius}`)
      path.arcTo(150, 150, 300, 300, 0, 180);
      path.close()

      canvas.drawPath(path);
      canvas.detachPen()



      // path.lineTo(400, 400);
    }
    canvas.drawPath(path);
  }
}