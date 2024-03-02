
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
    this.width_ = 720;
    this.height_ = 720;
    this.backgroundA_ = 0xFF;
    this.backgroundR_ = 0xFF;
    this.backgroundG_ = 0xFF;
    this.backgroundB_ = 0xFF;
    this.fileName_ = "undefined";
    this.testCount_ = 1;
  }

  width_: number | 720;  //gpu离屏绘制默认创建的画布宽
  height_: number | 720; //gpu离屏绘制默认创建的画布高
  backgroundA_: number | 0xFF; //gpu离屏绘制默认创建的画布默认背景色，默认白色
  backgroundR_: number | 0xFF;
  backgroundG_: number | 0xFF;
  backgroundB_: number | 0xFF;
  fileName_: string | "default"; //gpu离屏绘制后保存的图片名字
  pixelMap_: image.PixelMap | undefined; //gpu离屏创建的pixmap
  canvas_: drawing.Canvas | undefined; //gpu离屏绘制canvas
  testCount_: number | 1; //性能功耗测试时候，关键接口的默认循环次数
  time_: number | 0; //性能测试耗时

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

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试
    console.log(TAG, 'TestBase OnTestFunction');
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试
  }

  public OnTestFunctionCpu(canvas: drawing.Canvas) {
    //离屏canvas绘制图案
    this.OnTestFunction(canvas)
  }

  public OnTestPerformanceCpu(canvas: drawing.Canvas) {
    //离屏canvas绘制接口重复调用
    this.OnTestPerformance(canvas)
  }

  public OnTestFunctionGpuUpScreen(canvas: drawing.Canvas) {
    //gpu上屏幕绘制图案
    console.info(TAG, 'TestBase MyRenderNode TestFunctionGpuUpScreen');
    this.OnTestFunction(canvas)
  }

  public OnTestPerformanceGpuUpScreen(canvas: drawing.Canvas) {
    //gpu上屏幕绘制接口重复调用
    this.OnTestPerformance(canvas)
  }

  public async TestFunctionCpu(dir: string) {
    console.log(TAG, 'TestFunctionCpu start');
    await this.CreateBitmapCanvas();
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.error('canvas_ is invalid');
      return;
    }
    this.OnTestFunctionCpu(this.canvas_);
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

    await this.OnTestPerformanceCpu(this.canvas_);

    let endTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.error(TAG, 'DrawingApiTest Finished: [' + endTime + ']');
    this.time_ = endTime - startTime;
    console.error(TAG, 'DrawingApiTest TotalApiCallTime: [' + this.time_ + ']');
    console.error(TAG, 'DrawingApiTest TotalApiCallCount: [' + this.testCount_ + ']');
    console.error(TAG, 'TestPerformanceCpu end');
  }

  public async TestFunctionGpuUpScreen(canvas: drawing.Canvas) {
    console.log(TAG, 'TestFunctionGpu start');
    if (canvas == null || canvas == undefined) {
      console.error(TAG, 'canvas is invalid');
      return;
    }
    this.OnTestFunctionGpuUpScreen(canvas);
    console.log(TAG, 'TestFunctionGpu end');
  }

  public async TestPerformanceGpuUpScreen(canvas: drawing.Canvas) {
    console.log(TAG, 'TestPerformanceGpuUpScreen start');
    if (canvas == null || canvas == undefined) {
      console.error(TAG, 'canvas is invalid');
      return;
    }

    let startTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Started: [' + startTime + ']');

    await this.OnTestPerformanceGpuUpScreen(canvas);

    let endTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.error(TAG, 'DrawingApiTest Finished: [' + endTime + ']');
    this.time_ = endTime - startTime;
    console.error(TAG, 'DrawingApiTest TotalApiCallTime: [' + this.time_ + ']');
    console.error(TAG, 'DrawingApiTest TotalApiCallCount: [' + this.testCount_ + ']');
    console.log(TAG, 'TestPerformanceGpuUpScreen end');
  }
}