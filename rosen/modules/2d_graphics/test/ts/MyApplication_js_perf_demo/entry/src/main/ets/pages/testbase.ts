
import image from '@ohos.multimedia.image';
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import fs from '@ohos.file.fs'
import { BusinessError } from '@ohos.base';
import systemDateTime from '@ohos.systemDateTime';

const TAG = '[DrawingTest]';
/**
 * 测试基类
 */
export class TestBase {
  public constructor() {
    console.log(TAG, 'create TestBase');
    this.width_ = 500;
    this.height_ = 500;
    this.backgroundA_ = 0xFF;
    this.backgroundR_ = 0xFF;
    this.backgroundG_ = 0xFF;
    this.backgroundB_ = 0xFF;
    this.fileName_ = "undefined";
    this.testCount_ = 1000;
  }

  width_: number | 500;
  height_: number | 500;
  backgroundA_: number | 0xFF;
  backgroundR_: number | 0xFF;
  backgroundG_: number | 0xFF;
  backgroundB_: number | 0xFF;
  fileName_: string | "default";
  pixelMap_: image.PixelMap | undefined;
  canvas_: drawing.Canvas | undefined;
  testCount_: number | 1;
  time_: number | 0;

  public async CreateBitmapCanvas() {
    console.log(TAG, 'CreateBitmapCanvas'+ this.width_ + ' * ' + this.height_);
    const color : ArrayBuffer = new ArrayBuffer(this.height_ * this.width_ * 4);  //需要创建的像素buffer大小，取值为：height * width *4
    let opts : image.InitializationOptions = { editable: true, pixelFormat: 3, size: { height: this.height_, width: this.width_ } }
    this.pixelMap_ = await image.createPixelMap(color, opts);
    if (this.pixelMap_ == null || this.pixelMap_ == undefined) {
      console.log(TAG, 'create pixelMap error');
      return;
    }

    //创建离屏canvas，与pixmap绑定
    this.canvas_ = new drawing.Canvas(this.pixelMap_);
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.log(TAG, 'create canvas_ error');
      return;
    } else {
      console.log(TAG, 'create canvas_ success');
    }

    //离屏canvas绘制背景颜色
    const backgroundColor: common2D.Color = { alpha: this.backgroundA_, red: this.backgroundR_, green: this.backgroundG_, blue: this.backgroundB_ };
    this.canvas_.drawColor(backgroundColor);
  }

  public BitmapCanvasToFile(dir: string) {
    // const context : Context = getContext(this);
    // const path : string = context.cacheDir + "/xyjrect.jpg";
    if (this.pixelMap_ == null || this.pixelMap_ == undefined) {
      console.log(TAG, 'this.pixelMap_ is invalid');
      return;
    }
    const path : string = dir + "/" + this.fileName_ + ".jpg";
    let file = fs.openSync(path, fs.OpenMode.CREATE | fs.OpenMode.READ_WRITE);
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    let packOpts : image.PackingOption = { format:"image/jpeg", quality:100};
    imagePackerApi.packToFile(this.pixelMap_, file.fd, packOpts).then(() => {
      // 直接打包进文件
      console.log(TAG, 'Successfully packaged images');
    }).catch((error : BusinessError) => {
      console.log(TAG, 'Failed to pack the image. And the error is: ' + error);
    })
  }

  public OnTestFunctionCpu() {
    //离屏canvas绘制图案
  }
  public OnTestPerformanceCpu() {
    //离屏canvas绘制接口重复调用
  }

  public async TestFunctionCpu(dir: string) {
    console.log(TAG, 'TestFunctionCpu start');
    await this.CreateBitmapCanvas();
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.error('xyj canvas_ is invalid');
      return;
    }
    this.OnTestFunctionCpu();
    this.BitmapCanvasToFile(dir);
    console.log(TAG, 'TestFunctionCpu end');
  }

  public SetTestCount(count: number) {
    this.testCount_ = count;
  }

  public async TestPerformanceCpu() {
    console.log(TAG, 'TestPerformanceCpu start');
    await this.CreateBitmapCanvas();
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.log(TAG, 'canvas_ is invalid');
      return;
    }

    let startTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Started: [' + startTime + ']');

    await this.OnTestPerformanceCpu();

    let endTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Finished: [' + endTime + ']');
    this.time_ = endTime - startTime;
    console.log(TAG, 'DrawingApiTest TotalApiCallTime: [' + this.time_ + ']');
    console.log(TAG, 'DrawingApiTest TotalApiCallCount: [' + this.testCount_ + ']');
    console.log(TAG, 'TestPerformanceCpu end');
  }
}