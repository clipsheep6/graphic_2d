
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

const TAG = '[DrawingTest]';

export class DrawRect extends TestBase {
  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawrect";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    console.log(TAG, 'DrawRect OnTestFunction');
    const brush = new drawing.Brush();
    const brushColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    brush.setColor(brushColor);
    canvas.attachBrush(brush);
    canvas.drawRect({left:0,right:50,top:0,bottom:50});
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const brush = new drawing.Brush();
    const brushColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    brush.setColor(brushColor);
    canvas.attachBrush(brush);

    for (let index = 0; index < this.testCount_; index++) {
      canvas.drawRect({left:0,right:100,top:0,bottom:100});
    }
  }
}