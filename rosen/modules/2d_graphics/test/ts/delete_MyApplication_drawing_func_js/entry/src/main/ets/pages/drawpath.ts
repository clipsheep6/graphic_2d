
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

export class DrawPath extends TestBase {
  public constructor() {
    super();
    // 根据需求，如果与默认值不一样，请继承重写
    this.fileName_ = "drawpath";
  }

  public OnDraw() {
    //离屏canvas绘制图案
    console.error('xyj DrawPath OnDraw');
    const pen = new drawing.Pen();
    const penColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    pen.setColor(penColor);
    pen.setStrokeWidth(10);
    this.canvas_.attachPen(pen);

    const path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(300, 300);

    this.canvas_.drawPath(path);
  }
}