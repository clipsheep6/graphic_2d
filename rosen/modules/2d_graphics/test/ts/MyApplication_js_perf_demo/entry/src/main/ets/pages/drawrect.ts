
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

export class DrawRect extends TestBase {
  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawrect";
  }

  public OnTestFunctionCpu() {
    //离屏canvas绘制图案
    const brush = new drawing.Brush();
    const brushColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    brush.setColor(brushColor);
    this.canvas_.attachBrush(brush);
    this.canvas_.drawRect({left:0,right:300,top:0,bottom:300});
  }
  
  public OnTestPerformanceCpu() {
    //离屏canvas绘制性能测试
    const brush = new drawing.Brush();
    const brushColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    brush.setColor(brushColor);
    this.canvas_.attachBrush(brush);

    for (let index = 0; index < this.testCount_; index++) {
      this.canvas_.drawRect({left:0,right:100,top:0,bottom:100});
    }
  }
}