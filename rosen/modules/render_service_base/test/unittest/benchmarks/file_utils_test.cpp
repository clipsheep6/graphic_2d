/*
* Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "benchmarks/file_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Benchmarks {
class FileUtilsTest : public testing:Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
}

void FileUtilsTest::SetUpTestCase(void) {}

void FileUtilsTest::TearDownTestCase(void) {}

void FileUtilsTest::SetUp () {}

void FileUtilsTest::TearDown() {}

/**
 * @tc.name: IsValidFile001
 * @tc.desc: Verify the IsValidFile
 * @tc.type:FUNC
 */
HWTEST(FileUtilsTest, IsValidFile001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsValidFile001 start";

    const std::string VALID_PATH = "/data/";

    std::string path1 = "1";
    ASSERT_FALSE(IsValidFile());
    std::string path2 = "/ppp/data/xx";
    ASSERT_FALSE(IsValidFile());
    std::string path3 = "/data";
    ASSERT_FALSE(IsValidFile());
    std::string path4 = "/data/";
    ASSERT_TRUE(IsValidFile());
    std::string path5 = "/data/xx";
    ASSERT_TRUE(IsValidFile());

    GTEST_LOG_(INFO) << "RSAnimationFractionTest IsValidFile001 start";
}

HWTEST(FileUtilsTest, GetRealAndValidPath001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetRealAndValidPath001 start";

    // TODO
    ASSERT_TRUE(true);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetRealAndValidPath001 end";
}

HWTEST(FileUtilsTest, CreateFile001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest CreateFile001 start";

    // TODO
    ASSERT_TRUE(true);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest CreateFile001 end";
}

HWTEST(FileUtilsTest, WriteToFile001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteToFile001 start";

    // TODO
    ASSERT_TRUE(true);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteToFile001 end";
}

HWTEST(FileUtilsTest, WriteStringToFile001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteStringToFile001 start";

    // TODO
    ASSERT_TRUE(true);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteStringToFile001 end";
}

HWTEST(FileUtilsTest, WriteStringToFile002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteStringToFile002 start";

    // TODO
    ASSERT_TRUE(true);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteStringToFile002 end";
}

HWTEST(FileUtilsTest, WriteMessageParcelToFile001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteMessageParcelToFile001 start";

    // TODO
    ASSERT_TRUE(true);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest WriteMessageParcelToFile001 end";
}
} // Benchmarks
} // Rosen
} // OHOS