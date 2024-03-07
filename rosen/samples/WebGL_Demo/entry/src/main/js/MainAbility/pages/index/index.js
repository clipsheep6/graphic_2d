/*

Copyright (C) 2024 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
import { main5 } from './sample5/webgl-demo';
import { main7 } from './sample7/webgl-demo';
import { main1 } from './sample1/webgl-demo';
import { main2 } from './sample2/webgl-demo';
import { main3 } from './sample3/webgl-demo';
import { main4 } from './sample4/webgl-demo';
import { main6 } from './sample6/webgl-demo';
global.closed = false;
global.animationFrameId = null;

export default {
    data: {
        title: "",
        canvas: null,
    },
    onInit() {

    },
    onShow() {
        this.canvas = this.$refs.canvas;
        global.closed = false;


    },
    onHide() {
        global.closed = true;
    },
    clickFunc(buttonId) {
        if (global.animationFrameId != null) {
            cancelAnimationFrame(global.animationFrameId);
        }
        switch (buttonId) {
            case "button1":
                main1(this.canvas)
                break;
        case "button2":
            main2(this.canvas);
            break;
        case "button3":
            main3(this.canvas);
            break;
        case "button4":
            main4(this.canvas);
            break;
        case "button5":
            main5(this.canvas);
            break;
        case "button6":
            main6(this.canvas);
            break;
        case "button7":
            main7(this.canvas);
            break;
            default:
                console.log("Unknown button clicked!");
        }
    }
}