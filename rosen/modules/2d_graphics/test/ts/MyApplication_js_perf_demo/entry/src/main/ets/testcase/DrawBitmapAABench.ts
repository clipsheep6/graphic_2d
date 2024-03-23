import drawing from "@ohos.graphics.drawing";
import image from '@ohos.multimedia.image';
import {TestBase} from '../pages/testbase';
import globalThis from '../utils/globalThis'

const TAG = '[DrawingTest]';

export class DrawBitmapAABench extends TestBase {

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "drawimage";
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap()
    canvas.drawImage(pixelMap, 0, 0);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // draw_bitmap_aa_ident
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap()
    //接口重复调用，性能功耗测试 cpu/gpu调用接口一致
    for (let index = 0; index < this.testCount_; index++) {
      // canvas->concat(fMatrix);这个接口缺失
      canvas.drawImage(pixelMap, 0, 0);
    }
  }
}


