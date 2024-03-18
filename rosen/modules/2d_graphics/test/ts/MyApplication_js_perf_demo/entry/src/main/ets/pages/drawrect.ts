
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase} from './testbase';
import { OHRandom } from '../utils/OHRandom';
import ArrayList from '@ohos.util.ArrayList';

const W = 640;
const H = 480;
const N = 300;
const OFFSET = 1.0 / 3;
const TAG = '[RectBenchTest]';

// onDraw
export class DrawRect extends TestBase {
  private shift_test: number = 0;
  private stroke_test: number = 0;
  private faa_test: boolean = true;
  private prespective_test: boolean = true;
  private ohRandom : OHRandom = new OHRandom();
  private canvasRects = new ArrayList<common2D.Rect>();
  private panClours = new ArrayList<common2D.Color>();
  private brushClours = new ArrayList<common2D.Color>();
  private alphaValues = new ArrayList<number>();
  public constructor(shift: number, stroke: number, aa: boolean, prespective: boolean) {
    super();
    console.log(TAG, 'start rectbench test');
    this.shift_test = shift;
    this.stroke_test = stroke;
    this.faa_test = aa;
    this.prespective_test = prespective;
    this.Init();
    this.fileName_ = "rectbench";
    console.log(TAG, 'end rectbench test');
  }
  public Init() {
    for (let i = 0; i < 300; i++) {
      console.log(TAG, 'Init test math random is"' + Math.round(Math.random() * 255));
      let rect : common2D.Rect; // = this.ohRandom.createRect(i + 1);
      let clour : common2D.Color = {alpha : Math.round(Math.random() * 255), red : Math.round(Math.random() * 255),
        green : Math.round(Math.random() * 255), blue : Math.round(Math.random() * 255)};
      this.canvasRects.add(rect);
      this.panClours.add(clour);
      this.brushClours.add(clour);
      this.alphaValues.add(Math.round(Math.random() * 255));
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let paint = new drawing.Pen();
    let brush = new drawing.Brush();
    paint.setColor(this.panClours[0]);
    brush.setColor(this.brushClours[0]);
    paint.setAntiAlias(this.faa_test);
    brush.setAntiAlias(this.faa_test);
    canvas.attachPen(paint);
    canvas.attachBrush(brush);
    canvas.drawRect(this.canvasRects[0]);
    canvas.detachPen();
    canvas.detachBrush();
  }

  // rects_0_stroke_1_aa
  public OnTestPerformance(canvas: drawing.Canvas) {
    console.log(TAG, 'start OnTestPerformance test');
    let paint = new drawing.Pen();
    let brush = new drawing.Brush();

    if (this.stroke_test > 0) {
      //paint.setStyle() // 未开放
      paint.setStrokeWidth(this.stroke_test);
      //brush.setStyle() // 未开放
    }

    for (let i = 0; i < this.testCount_; i++) {
      paint.setColor(this.panClours[i % N]);
      brush.setColor(this.brushClours[i % N]);
      paint.setAntiAlias(this.faa_test);
      brush.setAntiAlias(this.faa_test);
      canvas.attachPen(paint);
      canvas.attachBrush(brush);
      canvas.drawRect(this.canvasRects[i % N]);
      canvas.detachPen();
      canvas.detachBrush();
    }
    console.log(TAG, 'end OnTestPerformance test');
  }
}