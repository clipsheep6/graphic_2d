
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase, StyleType} from '../pages/testbase';
import { N, OHRandom } from '../utils/OHRandom';
import ArrayList from '@ohos.util.ArrayList';

const TAG = '[BezierBench]';

export class CanvasDrawRect extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    console.log(TAG, 'xyj DRAW_ STYLE_COMPLEX end');
    for (let i = 0; i < this.testCount_; i++) {
      let l = i % this.width_;
      let t = (i + 100) % this.height_;
      let r = ((l + 100) > this.width_) ? this.width_ : (l + 100);
      let b = ((t + 100) > this.height_) ? this.height_ : (t + 100);
      //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
      let rect: common2D.Rect = {left: l, top: t, right: r, bottom: b};
      canvas.drawRect(rect);
    }
  }
}