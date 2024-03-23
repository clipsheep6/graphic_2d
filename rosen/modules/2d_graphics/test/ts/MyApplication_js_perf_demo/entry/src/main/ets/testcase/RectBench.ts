
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from '../pages/testbase';
import { N, OHRandom } from '../utils/OHRandom';
import ArrayList from '@ohos.util.ArrayList';


const TAG = '[RectBenchTest]';
// test setAntiAlias (Pen and Brush)
export class RectBench extends TestBase {
  private shift_test: number = 0;
  private stroke_test: number = 0;
  private faa_test: boolean = true;
  private prespective_test: boolean = true;
  private ohRandom : OHRandom = new OHRandom();

  private paintColors = new ArrayList<common2D.Color>();
  private canvasRects = new ArrayList<common2D.Rect>();
  public constructor(shift: number, stroke: number, aa: boolean, prespective: boolean) {
    super();
    this.shift_test = shift;
    this.stroke_test = stroke;
    this.faa_test = aa;
    this.prespective_test = prespective;
    this.ohRandom.setColorAndRect(this.paintColors, this.canvasRects, this.shift_test);
    this.fileName_ = "rectbench";
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // rects_0_10_aa
    // rects_0_10_bw
    let paint = new drawing.Pen();
    let brush = new drawing.Brush();
    if (this.stroke_test > 0) {
      paint.setStrokeWidth(this.stroke_test);
    }
    for (let i = 0; i < this.testCount_; i++) {
      paint.setColor(this.paintColors[i % N]);
      brush.setColor(this.paintColors[i % N]);
      paint.setAntiAlias(this.faa_test);
      brush.setAntiAlias(this.faa_test);
      canvas.attachPen(paint);
      canvas.attachBrush(brush);
      canvas.drawRect(this.canvasRects[i % N]);
      canvas.detachPen();
      canvas.detachBrush();
    }
  }
}