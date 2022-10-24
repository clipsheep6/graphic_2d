/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <texgine/dynamic_file_font_provider.h>
#include <texgine/system_font_provider.h>
#include <texgine/typography_builder.h>

#include "feature_test_framework.h"
#include "text_define.h"

using namespace Texgine;

namespace {
#define Family1 Woman Skin1 ZWJ RedHeart ZWJ Man Skin0
#define Family2 Person Skin4 ZWJ RedHeart ZWJ Man ZWJ Person Skin2
#define Role1 Person Skin3 ZWJ Airplane
#define Role2 Person Skin1 ZWJ Laptop
#define Genered1 BouncingBall Skin2 ZWJ WomanSportVariant EmojiVariant
#define Genered2 LiftingWeights Skin0 ZWJ ManSportVariant EmojiVariant
#define Hair1 Man Skin2 ZWJ HairBald
#define Hair2 Person Skin4 ZWJ HairCurly
#define Other1 RedHeart ZWJ "\U0001F525"
#define Other2 Flag EmojiVariant ZWJ Transgender EmojiVariant

struct Emoji {
    TextStyle style_ = { .fontSize_ = 32 };
    std::string emoji_;
    std::string title_;
} emojis[] = {
    {
        .emoji_ = Watch,
        .title_ = "watch",
    },
    {
        .emoji_ = FastForwardButton,
        .title_ = "fast forward button",
    },
    {
        .emoji_ = AlartClock,
        .title_ = "alart clock",
    },
    {
        .emoji_ = Umbrella,
        .title_ = "umbrella",
    },
    {
        .emoji_ = RedCircle,
        .title_ = "red circle",
    },
    {
        .emoji_ = BlackLargeSquare,
        .title_ = "black large square",
    },
    {
        .emoji_ = CurlyLoop,
        .title_ = "curly loop",
    },
    {
        .emoji_ = Plus,
        .title_ = "plus",
    },
    {
        .emoji_ = Snowman,
        .title_ = "snowman",
    },
    {
        .emoji_ = Voltage,
        .title_ = "voltage",
    },
    // =======================
    {
        .emoji_ = Copyright,
        .title_ = "copyright",
    },
    {
        .emoji_ = Registered,
        .title_ = "registered",
    },
    {
        .emoji_ = DoubleExclamationMark,
        .title_ = "double exclamation mark",
    },
    {
        .emoji_ = ExclamationQuestionMark,
        .title_ = "exclamation question mark",
    },
    {
        .emoji_ = WavyDash,
        .title_ = "wavy dash",
    },
    {
        .emoji_ = LeftArrow,
        .title_ = "left arrow",
    },
    {
        .emoji_ = RedHeart,
        .title_ = "red heart",
    },
    {
        .emoji_ = Pencil,
        .title_ = "pencil",
    },
    {
        .emoji_ = Airplane,
        .title_ = "airplan",
    },
    {
        .emoji_ = Atom,
        .title_ = "atom",
    },
    // =======================
    {
        .emoji_ = RedDragon,
        .title_ = "red dragon",
    },
    {
        .emoji_ = Joker,
        .title_ = "joker",
    },
    {
        .emoji_ = Cyclone,
        .title_ = "cyclone",
    },
    {
        .emoji_ = NewMoonFace,
        .title_ = "new moon face",
    },
    {
        .emoji_ = Cat,
        .title_ = "cat",
    },
    {
        .emoji_ = Dog,
        .title_ = "dog",
    },
    {
        .emoji_ = Boy,
        .title_ = "boy",
    },
    {
        .emoji_ = PostBox,
        .title_ = "postbox",
    },
    {
        .emoji_ = GrinningFace,
        .title_ = "grinning face",
    },
    {
        .emoji_ = Rocket,
        .title_ = "rocket",
    },
    // =======================
    {
        .emoji_ = CloudWithRain,
        .title_ = "cloud with rain",
    },
    {
        .emoji_ = Tornado,
        .title_ = "tornado",
    },
    {
        .emoji_ = Fog,
        .title_ = "fog",
    },
    {
        .emoji_ = PersonGolfing,
        .title_ = "person golfing",
    },
    {
        .emoji_ = Motorcycle,
        .title_ = "motorcycle",
    },
    {
        .emoji_ = Candle,
        .title_ = "candle",
    },
    {
        .emoji_ = Spider,
        .title_ = "spider",
    },
    {
        .emoji_ = Bed,
        .title_ = "bed",
    },
    {
        .emoji_ = WorldMap,
        .title_ = "world map",
    },
    {
        .emoji_ = SpeakingHead,
        .title_ = "speaking head",
    },
    // =======================
    {
        .emoji_ = Sharp,
        .title_ = "sharp",
    },
    {
        .emoji_ = Asterisk,
        .title_ = "asterisk",
    },
    {
        .emoji_ = Zero,
        .title_ = "zero",
    },
    {
        .emoji_ = One,
        .title_ = "one",
    },
    {
        .emoji_ = Two,
        .title_ = "two",
    },
    {
        .emoji_ = Three,
        .title_ = "three",
    },
    {
        .emoji_ = Four,
        .title_ = "four",
    },
    {
        .emoji_ = Five,
        .title_ = "five",
    },
    {
        .emoji_ = Six,
        .title_ = "six",
    },
    {
        .emoji_ = Seven,
        .title_ = "seven"
    },
    // =======================
    {
        .emoji_ = China,
        .title_ = "China",
    },
    {
        .emoji_ = Australia,
        .title_ = "Australia",
    },
    {
        .emoji_ = Canada,
        .title_ = "Canada",
    },
    {
        .emoji_ = France,
        .title_ = "France",
    },
    {
        .emoji_ = Ireland,
        .title_ = "Ireland",
    },
    {
        .emoji_ = India,
        .title_ = "India",
    },
    {
        .emoji_ = Iceland,
        .title_ = "Iceland",
    },
    {
        .emoji_ = Italy,
        .title_ = "Italy",
    },
    {
        .emoji_ = Japan,
        .title_ = "Japan",
    },
    {
        .emoji_ = Russia,
        .title_ = "Russia",
    },
    // =======================
    {
        .emoji_ = Boy0,
        .title_ = "boy ligth",
    },
    {
        .emoji_ = Boy1,
        .title_ = "boy medium light",
    },
    {
        .emoji_ = Boy2,
        .title_ = "boy medium skin",
    },
    {
        .emoji_ = Boy3,
        .title_ = "boy medium dark",
    },
    {
        .emoji_ = Boy4,
        .title_ = "boy dark",
    },
    {
        .emoji_ = Girl0,
        .title_ = "girl ligth",
    },
    {
        .emoji_ = Girl1,
        .title_ = "girl medium light",
    },
    {
        .emoji_ = Girl2,
        .title_ = "girl medium skin",
    },
    {
        .emoji_ = Girl3,
        .title_ = "girl medium dark",
    },
    {
        .emoji_ = Girl4,
        .title_ = "girl dark",
    },
    // =======================
    {
        .emoji_ = Family1,
        .title_ = "family 1",
    },
    {
        .emoji_ = Family2,
        .title_ = "family 2",
    },
    {
        .emoji_ = Role1,
        .title_ = "role 1",
    },
    {
        .emoji_ = Role2,
        .title_ = "role 2",
    },
    {
        .emoji_ = Genered1,
        .title_ = "genered 1",
    },
    {
        .emoji_ = Genered2,
        .title_ = "genered 2",
    },
    {
        .emoji_ = Hair1,
        .title_ = "hair 1",
    },
    {
        .emoji_ = Hair2,
        .title_ = "hair 2",
    },
    {
        .emoji_ = Other1,
        .title_ = "other 1",
    },
    {
        .emoji_ = Other2,
        .title_ = "other 2",
    }
};

class EmojiTest : public TestFeature {
public:
    EmojiTest() : TestFeature("EmojiTest")
    {
    }

    void Layout()
    {
        for (auto &emoji : emojis) {
            TypographyStyle tystyle;
            auto dfprovider = DynamicFileFontProvider::Create();
            dfprovider->LoadFont("Segoe UI Emoji", RESOURCE_PATH_PREFIX "seguiemj.ttf");
            auto fps = FontProviders::Create();
            fps->AppendFontProvider(dfprovider);
            fps->AppendFontProvider(SystemFontProvider::GetInstance());
            auto builder = TypographyBuilder::Create(tystyle, std::move(fps));

            emoji.style_.fontFamilies_ = {"Segoe UI Emoji"};
            builder->PushStyle(emoji.style_);
            builder->AppendSpan(emoji.emoji_);
            builder->PopStyle();

            auto typography = builder->Build();
            typography->Layout(100);
            typographies_.push_back({.typography = typography, .comment = emoji.title_});
        }
    }
} g_test;
} // namespace
