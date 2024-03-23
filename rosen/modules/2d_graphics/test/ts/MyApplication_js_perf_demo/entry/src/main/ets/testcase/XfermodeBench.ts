
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from '../pages/testbase';
import ArrayList from '@ohos.util.ArrayList';
import { N, OHRandom } from '../utils/OHRandom';

const TAG = '[XfermodeBench]';

// test setBlendMode (Pen and Brush)
export class XfermodeBench extends TestBase {
  private ohRandom : OHRandom = new OHRandom();
  private canvasRects = new ArrayList<common2D.Rect>();
  private panColors = new ArrayList<common2D.Color>();
  private alphaValues = new ArrayList<number>();
  private blendMode : number = 0;
  private type : number = 0
  public constructor(blendMode: number, type : number) {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.blendMode = blendMode;
    this.type = type;
    this.Init();
    this.fileName_ = "XfermodeBench";
  }

  public Init() {
    this.ohRandom.setColorAndRect(this.panColors, this.canvasRects, 0);
    for (let i = 0; i < N; i++) {
      this.alphaValues.add(this.ohRandom.nextU() & 255);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // blendmicro_rect_clear
    // blendmicro_mask_clear
    let pen = new drawing.Pen();
    let brush = new drawing.Brush();
    pen.setStrokeWidth(10);
    pen.setBlendMode(this.blendMode);
    brush.setBlendMode(this.blendMode);
    canvas.attachPen(pen);
    canvas.attachBrush(brush);
    if (this.type === 0) {
      let font : drawing.Font = new drawing.Font();
      let blob : drawing.TextBlob = undefined;
      let text : string = 'Hamburgefons';
      blob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      for (let i = 0; i < this.testCount_; i++) {
        canvas.drawTextBlob(blob, 100, 100);
      }
    } else {
      for (let i = 0; i < this.testCount_; i++) {
        pen.setColor(this.panColors[i % N]);
        brush.setColor(this.panColors[i % N]);
        canvas.attachPen(pen);
        canvas.attachBrush(brush);
        canvas.drawRect(this.canvasRects[i % N]);
      }
    }
    canvas.detachPen();
    canvas.detachBrush();
  }
}