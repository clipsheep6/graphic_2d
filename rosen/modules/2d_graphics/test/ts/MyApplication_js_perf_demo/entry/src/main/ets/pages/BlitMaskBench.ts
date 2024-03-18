
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { N, OHRandom } from '../utils/OHRandom';
import ArrayList from '@ohos.util.ArrayList';

const TAG = '[BlitMaskBench]';

// test Brush::setAlpha
export class BlitMaskBench extends TestBase {
  private ohRandom : OHRandom = new OHRandom();
  private canvasRects = new ArrayList<common2D.Rect>();
  private panColors = new ArrayList<common2D.Color>();
  private alphaValues = new ArrayList<number>();
  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.Init();
    this.fileName_ = "BlitMaskBench";
  }

  public Init() {
    this.ohRandom.setColorAndRect(this.panColors, this.canvasRects, 0);
    for (let i = 0; i < N; i++) {
      this.alphaValues.add(this.ohRandom.nextU() & 255);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // maskopaque_0_bw
    let paint = new drawing.Pen();
    let brush = new drawing.Brush();
    paint.setStrokeWidth(10);
    for (let i = 0; i < this.testCount_; i++) {
      paint.setColor(this.panColors[i % N]);
      brush.setColor(this.panColors[i % N]);
      brush.setAlpha(this.alphaValues[i % N]);
      canvas.attachPen(paint);
      canvas.attachBrush(brush);
      canvas.drawPoint(this.canvasRects[i % N].left, this.canvasRects[i % N].top);
      canvas.detachPen();
      canvas.detachBrush();
    }
  }
}