
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';

const TAG = '[DrawingTest]';

export class DrawTextBlob extends TestBase {
  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawtextblob";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    const brush = new drawing.Brush();
    brush.setColor({alpha: 255, red: 0, green: 255, blue: 255});
    const font = new drawing.Font();
    font.setSize(10);
    const textBlob = drawing.TextBlob.makeFromString("Keep your sentences short, but not overly so.", font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    canvas.attachBrush(brush);
    canvas.drawTextBlob(textBlob, 20, 20);
    canvas.detachBrush();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    const brush = new drawing.Brush();
    brush.setColor({alpha: 255, red: 0, green: 255, blue: 255});
    canvas.attachBrush(brush);

    let bigLoops = this.testCount_ * 100
    for (let index = 0; index < bigLoops; index++) {
      const font = new drawing.Font();
      font.setSize(10);
      const textBlob = drawing.TextBlob.makeFromString("Keep your sentences short, but not overly so.", font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(textBlob, 20, 20);
    }
    canvas.detachBrush();
  }
}