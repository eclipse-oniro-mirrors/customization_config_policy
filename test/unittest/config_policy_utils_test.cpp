/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "config_policy_utils_test.h"

#include <gtest/gtest.h>

#include "config_policy_utils.h"
#include "config_policy_impl.h"
#ifndef __LITEOS__
#include "init_param.h"

constexpr const char *CUST_OPKEY0 = "telephony.sim.opkey0";
constexpr const char *CUST_OPKEY1 = "telephony.sim.opkey1";
constexpr const char *CUST_OPKEY3 = "telephony.sim.opkey3";
#endif

using namespace testing::ext;

namespace OHOS {
class ConfigPolicyUtilsTest : public testing::Test {
    public:
        static void SetUpTestCase(void);
};

#ifndef __LITEOS__
void ConfigPolicyUtilsTest::SetUpTestCase(void)
{
    SystemSetParameter(CUST_OPKEY0, "46060");
    SystemSetParameter(CUST_OPKEY1, "46061");
    SystemSetParameter(CUST_OPKEY3, "46062");
}
#endif

bool TestGetCfgFile(const char *testPathSuffix, int type, const char *extra)
{
    CfgFiles *cfgFiles = GetCfgFilesEx(testPathSuffix, type, extra);
    EXPECT_TRUE(cfgFiles != NULL);
    bool flag = false;
    char *filePath = nullptr;
    for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
        filePath = cfgFiles->paths[i];
        if (filePath && *filePath != '\0') {
            std::cout << "type: " << type << ", filePath: " << filePath << std::endl;
            flag = true;
        }
    }
    FreeCfgFiles(cfgFiles);
    char buf[MAX_PATH_LEN] = {0};
    filePath = GetOneCfgFileEx(testPathSuffix, buf, MAX_PATH_LEN, type, extra);
    if (filePath && *filePath != '\0') {
        std::cout << "type: " << type << ", one filePath: " << filePath << std::endl;
        flag = flag && true;
    }
    return flag;
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest001
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, none file case.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest001, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/none.xml";
    EXPECT_FALSE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_NO_RULE_FOLLOWED, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest002
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, system file case.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest002, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/system.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_NO_RULE_FOLLOWED, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest003
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, user file case.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest003, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/user.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_NO_RULE_FOLLOWED, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest004
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, both files case.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest004, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/both.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_NO_RULE_FOLLOWED, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest005
 * @tc.desc: Test struct CfgDir *GetCfgDirList(void) function.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest005, TestSize.Level0)
{
    CfgDir *cfgDir = GetCfgDirList();
    EXPECT_TRUE(cfgDir != NULL);
    bool flag = false;
    for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
        char *filePath = cfgDir->paths[i];
        if (filePath && *filePath != '\0') {
            std::cout << "filePath: " << filePath << std::endl;
            flag = true;
        }
    }
    FreeCfgDirList(cfgDir);
    EXPECT_TRUE(flag);
}

#ifndef __LITEOS__
/**
 * @tc.name: CfgPolicyUtilsFuncTest006
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, follow default sim rule.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest006, TestSize.Level1)
{
    SystemSetParameter(CUST_OPKEY0, "-1");
    const char *testPathSuffix = "etc/custxmltest/user.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_SIM_DEFAULT, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest007
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, follow sim1 rule.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest007, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/user.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_SIM_1, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest008
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, follow sim2 rule.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest008, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/user.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_SIM_2, NULL));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest009
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, follow default x rule rule.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest009, TestSize.Level1)
{
    int ret = SystemSetParameter(CUST_FOLLOW_X_RULES,
        ":etc/custxmltest/user.xml,10:etc/custxmltest/both.xml,100,etc/carrier/${test:-46061}");
    if (ret == 0) {
        std::cout << "set " << CUST_FOLLOW_X_RULES << " success." << std::endl;
        EXPECT_TRUE(TestGetCfgFile("etc/custxmltest/user.xml", FOLLOWX_MODE_DEFAULT, NULL));
        EXPECT_TRUE(TestGetCfgFile("etc/custxmltest/both.xml", FOLLOWX_MODE_DEFAULT, NULL));
    } else {
        std::cout << "set " << CUST_FOLLOW_X_RULES << " failed, use OpkeyInfo.json for test." << std::endl;
        EXPECT_TRUE(TestGetCfgFile("etc/telephony/OpkeyInfo.json", FOLLOWX_MODE_DEFAULT, NULL));
    }
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest010
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, follow default x rule rule.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest010, TestSize.Level1)
{
    std::string extraString;
    extraString.append("etc/carrier/${").append(CUST_OPKEY1).append(":-46060},etc/carrier/${")
        .append(CUST_OPKEY0).append("}/etc/${testkey:-custxmltest}");
    std::cout << "extra: " << extraString << std::endl;
    EXPECT_TRUE(TestGetCfgFile("etc/custxmltest/user.xml", FOLLOWX_MODE_USER_DEFINED, extraString.c_str()));
    EXPECT_TRUE(TestGetCfgFile("etc/custxmltest/both.xml", FOLLOWX_MODE_USER_DEFINED, extraString.c_str()));
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest011
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, with nullptr input
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest011, TestSize.Level1)
{
    char buf[MAX_PATH_LEN] = {0};
    char *filePath = nullptr;
    filePath = GetOneCfgFileEx(nullptr, buf, MAX_PATH_LEN, FOLLOWX_MODE_DEFAULT, nullptr);
    EXPECT_TRUE(filePath == nullptr);
    filePath = GetOneCfgFileEx("etc/custxmltest/user.xml", nullptr, MAX_PATH_LEN, FOLLOWX_MODE_DEFAULT, nullptr);
    EXPECT_TRUE(filePath == nullptr);
    filePath = GetOneCfgFileEx("etc/custxmltest/user.xml", buf, MAX_PATH_LEN - 1, FOLLOWX_MODE_DEFAULT, nullptr);
    EXPECT_TRUE(filePath == nullptr);
    CfgFiles *cfgFiles = GetCfgFilesEx(nullptr, FOLLOWX_MODE_DEFAULT, nullptr);
    EXPECT_TRUE(cfgFiles == nullptr);
    FreeCfgFiles(cfgFiles);
    EXPECT_TRUE(cfgFiles == nullptr);
    CfgDir *res = nullptr;
    FreeCfgDirList(res);
    EXPECT_TRUE(res == nullptr);
}

/**
 * @tc.name: CfgPolicyUtilsFuncTest012
 * @tc.desc: Test GetOneCfgFile & GetCfgFiles function, follow sim3 rule.
 * @tc.type: FUNC
 * @tc.require: issueI5NYDH
 */
HWTEST_F(ConfigPolicyUtilsTest, CfgPolicyUtilsFuncTest012, TestSize.Level1)
{
    const char *testPathSuffix = "etc/custxmltest/user.xml";
    EXPECT_TRUE(TestGetCfgFile(testPathSuffix, FOLLOWX_MODE_SIM_3, NULL));
}
#endif // __LITEOS__
} // namespace OHOS
