
import drawing from "@ohos.graphics.drawing";
import {TestBase} from './testbase';

const TAG = '[TextBlobFirstTimeBench]';

// test setTypeface and makeFromString
export class TextBlobFirstTimeBench extends TestBase {
  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();
    this.fileName_ = "TextBlobFirstTimeBench";
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    // TextBlobFirstTimeBench
    let font : drawing.Font = new drawing.Font();
    let text : string = 'Keep your sentences short, but not overly so.';
    let blob : drawing.TextBlob = undefined;
    font.setTypeface(new drawing.Typeface());
    font.enableSubpixel(true);
    for (let i = 0; i < this.testCount_; i++) {
      for (let j = 0; j < 100; j++) {
        blob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
        canvas.drawTextBlob(blob, 100, 100);
      }
    }
  }
}