
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { N, OHRandom } from '../utils/OHRandom';
import ArrayList from '@ohos.util.ArrayList';

const TAG = '[SrcModeRectBench]';

// test Pen::setAlpha
export class SrcModeRectBench extends TestBase {
  private ohRandom : OHRandom = new OHRandom();
  private canvasRects = new ArrayList<common2D.Rect>();
  private panColors = new ArrayList<common2D.Color>();
  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.Init();
    this.fileName_ = "SrcModeRectBench";
  }
  public Init() {
    this.ohRandom.setColorAndRect(this.panColors, this.canvasRects, 0);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // srcmode_0_bw
    let paint = new drawing.Pen();
    let brush = new drawing.Brush();
    paint.setStrokeWidth(10);
    for (let i = 0; i < this.testCount_; i++) {
      paint.setColor(this.panColors[i % N]);
      brush.setColor(this.panColors[i % N]);
      paint.setAlpha(0x80);
      paint.setBlendMode(1);
      canvas.attachPen(paint);
      canvas.attachBrush(brush);
      canvas.drawRect(this.canvasRects[i % N]);
      canvas.detachPen();
      canvas.detachBrush();
    }
  }
}