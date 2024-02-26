
import image from '@ohos.multimedia.image';
import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import fs from '@ohos.file.fs'
import { BusinessError } from '@ohos.base';
/**
 * 测试基类
 */
export class TestBase {
  public constructor() {
    console.error('xyj xyj create TestBase');
    this.width_ = 500;
    this.height_ = 500;
    this.backgroundA_ = 0xFF;
    this.backgroundR_ = 0xFF;
    this.backgroundG_ = 0xFF;
    this.backgroundB_ = 0xFF;
    this.fileName_ = "undefined";
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

  public async CreateBitmapCanvas() {
    console.error('xyj CreateBitmapCanvas'+ this.width_ + ' * ' + this.height_);
    const color : ArrayBuffer = new ArrayBuffer(this.height_ * this.width_ * 4);  //需要创建的像素buffer大小，取值为：height * width *4
    let opts : image.InitializationOptions = { editable: true, pixelFormat: 3, size: { height: this.height_, width: this.width_ } }
    this.pixelMap_ = await image.createPixelMap(color, opts);
    if (this.pixelMap_ == null || this.pixelMap_ == undefined) {
      console.error('xyj create pixelMap error');
      return;
    }

    //创建离屏canvas，与pixmap绑定
    this.canvas_ = new drawing.Canvas(this.pixelMap_);
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.error('xyj create canvas_ error');
      return;
    } else {
      console.error('xyj create canvas_ success');
    }

    //离屏canvas绘制背景颜色
    const backgroundColor: common2D.Color = { alpha: this.backgroundA_, red: this.backgroundR_, green: this.backgroundG_, blue: this.backgroundB_ };
    this.canvas_.drawColor(backgroundColor);
  }

  public BitmapCanvasToFile(dir: string) {
    // const context : Context = getContext(this);
    // const path : string = context.cacheDir + "/xyjrect.jpg";
    if (this.pixelMap_ == null || this.pixelMap_ == undefined) {
      console.error('xyj this.pixelMap_ is invalid');
      return;
    }
    const path : string = dir + "/" + this.fileName_ + ".jpg";
    let file = fs.openSync(path, fs.OpenMode.CREATE | fs.OpenMode.READ_WRITE);
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    let packOpts : image.PackingOption = { format:"image/jpeg", quality:100};
    imagePackerApi.packToFile(this.pixelMap_, file.fd, packOpts).then(() => {
      // 直接打包进文件
    }).catch((error : BusinessError) => {
      console.error('xyj Failed to pack the image. And the error is: ' + error);
    })
  }

  public OnDraw() {
    //离屏canvas绘制图案
    // console.error('xyj xyj OnDraw');
    // const brush = new drawing.Brush();
    // const brushColor: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 }
    // brush.setColor(brushColor);
    // this.canvas_.attachBrush(brush);
    // this.canvas_.drawRect({left:0,right:100,top:0,bottom:100});
  }

  public async Draw(dir: string) {
    console.error('xyj xyj testbase Draw');
    await this.CreateBitmapCanvas();
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.error('xyj canvas_ is invalid');
      return;
    }
    this.OnDraw();
    this.BitmapCanvasToFile(dir);
  }
}