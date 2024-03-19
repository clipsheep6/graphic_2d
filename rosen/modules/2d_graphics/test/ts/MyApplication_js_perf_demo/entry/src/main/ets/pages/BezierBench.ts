
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { N, OHRandom } from '../utils/OHRandom';
import ArrayList from '@ohos.util.ArrayList';

const TAG = '[BezierBench]';

// test Pen::setStrokeWidth
export class BezierBench extends TestBase {
  private ohRandom : OHRandom = new OHRandom();
  private canvasRects = new ArrayList<common2D.Rect>();
  private panColors = new ArrayList<common2D.Color>();
  private strokeWidth : number = 0;
  private mode : number = 0;
  public constructor(width : number, mode : number) {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.mode = mode;
    this.strokeWidth = width;
    this.Init();
    this.fileName_ = "BezierBench";
  }

  public Init() {
    this.ohRandom.setColorAndRect(this.panColors, this.canvasRects, 0);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // draw_stroke_bezier_quad_round_round_10
    // draw_stroke_bezier_cubic_round_round_10
    let paint = new drawing.Pen();
    paint.setAntiAlias(true);
    let path = new drawing.Path();
    if (this.mode === 0) {
      path.moveTo(20, 20);
      path.quadTo(60, 20, 60, 60);
      path.quadTo(20, 60, 20, 100);
    } else {
      path.moveTo(20, 20);
      path.cubicTo(40, 20, 60, 40, 60, 60);
      path.cubicTo(40, 60, 20, 80, 20, 100);
    }
    for (let i = 0; i < this.testCount_; i++) {
      paint.setStrokeWidth(this.strokeWidth);
      canvas.attachPen(paint);
      canvas.drawPath(path);
      canvas.detachPen();
    }
  }
}