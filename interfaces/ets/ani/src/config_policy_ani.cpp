/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "config_policy_ani.h"

#include <vector>
#include <string>
#include "hilog/log.h"
#include "config_policy_utils.h"
#include "hisysevent_adapter.h"
#include "ani_utils.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001E00

#undef LOG_TAG
#define LOG_TAG "ConfigPolicyJs"

namespace OHOS {
namespace Customization {
namespace ConfigPolicy {
using namespace OHOS::HiviewDFX;

static const char* NAMESPACE_NAME = "@ohos.configPolicy.configPolicy";

// Param Error Code
static constexpr int32_t PARAM_ERROR = 401;

ani_status ConfigPolicyAni::Init(ani_env* env)
{
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(NAMESPACE_NAME, &ns)) {
        HILOG_ERROR(LOG_CORE, "Not found namespace %{public}s.", NAMESPACE_NAME);
        return ANI_ERROR;
    }
    std::array methods = {
        ani_native_function { "getOneCfgFileSync", nullptr, reinterpret_cast<void*>(GetOneCfgFileSync) },
        ani_native_function { "getCfgFilesSync", nullptr, reinterpret_cast<void*>(GetCfgFilesSync) },
        ani_native_function { "getCfgDirListSync", nullptr,  reinterpret_cast<void*>(GetCfgDirListSync) },
    };
    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size())) {
        HILOG_ERROR(LOG_CORE, "Cannot bind native methods to namespace[%{public}s]", NAMESPACE_NAME);
        return ANI_ERROR;
    };
    return ANI_OK;
}

bool ConfigPolicyAni::TransformParams(
    ani_env* env, ani_string relPath, ani_enum_item followMode, ani_string extra, ConfigSyncContext& context)
{
    if (!AniUtils::AniStrToString(env, relPath, context.relPath)) {
        HILOG_ERROR(LOG_CORE, "parse relPath error.");
        return false;
    }
    ani_boolean modeIsUndefined;
    auto followModeRet = env->Reference_IsUndefined(followMode, &modeIsUndefined);
    if (followModeRet != ANI_OK || modeIsUndefined) {
        return true;
    }
    ani_boolean extraIsUndefined;
    auto extraRet = env->Reference_IsUndefined(extra, &extraIsUndefined);
    if (extraRet != ANI_OK || extraIsUndefined) {
        return ParseFollowMode(env, followMode, context, false);
    }
    return ParseFollowMode(env, followMode, context, true) && ParseExtra(env, extra, context);
}

bool ConfigPolicyAni::ParseFollowMode(
    ani_env* env, ani_enum_item followMode, ConfigSyncContext& context, bool hasExtra)
{
    ani_int enumValue;
    if (ANI_OK != env->EnumItem_GetValue_Int(followMode, &enumValue)) {
        AniUtils::ThrowAniError(env, PARAM_ERROR, "Parameter error. The type of followMode must be number.");
        return false;
    }
    switch (enumValue) {
        case FOLLOWX_MODE_DEFAULT:
            [[fallthrough]];
        case FOLLOWX_MODE_NO_RULE_FOLLOWED:
            [[fallthrough]];
        case FOLLOWX_MODE_SIM_DEFAULT:
            [[fallthrough]];
        case FOLLOWX_MODE_SIM_1:
            [[fallthrough]];
        case FOLLOWX_MODE_SIM_2:
            [[fallthrough]];
        case FOLLOWX_MODE_SIM_3:
            break;
        case FOLLOWX_MODE_USER_DEFINED:
            if (!hasExtra) {
                AniUtils::ThrowAniError(env, PARAM_ERROR,
                    "Parameter error. The followMode is USER_DEFINED, extra must be set.");
                return false;
            }
            break;
        default:
            AniUtils::ThrowAniError(env, PARAM_ERROR,
                "Parameter error. The value of followMode should be in the enumeration value of FollowXMode.");
            return false;
    }
    context.followMode = enumValue;
    return true;
}

bool ConfigPolicyAni::ParseExtra(ani_env* env, ani_string extra, ConfigSyncContext& context)
{
    return AniUtils::AniStrToString(env, extra, context.extra);
}

ani_string ConfigPolicyAni::GetOneCfgFileSync(
    ani_env* env, ani_string relPath, ani_enum_item followMode, ani_string extra)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetOneCfgFileSync start.");
    ConfigSyncContext context;
    context.followMode = FOLLOWX_MODE_DEFAULT;
    if (!TransformParams(env, relPath, followMode, extra, context)) {
        HILOG_ERROR(LOG_CORE, "trans form params error.");
        return AniUtils::StringToAniStr(env, "");
    }
    char outBuf[MAX_PATH_LEN] = {0};
    char *filePath = GetOneCfgFileEx(
        context.relPath.c_str(), outBuf, MAX_PATH_LEN, context.followMode, context.extra.c_str());
    std::string result;
    if (filePath == nullptr) {
        HILOG_DEBUG(LOG_CORE, "GetOneCfgFileEx result is nullptr.");
        ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "getOneCfgFileSync", "CfgFile path is nullptr.");
    } else {
        result = std::string(filePath);
    }
    return AniUtils::StringToAniStr(env, result);
}

ani_ref ConfigPolicyAni::GetCfgFilesSync(ani_env* env, ani_string relPath, ani_enum_item followMode, ani_string extra)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetCfgFiles start.");
    std::vector<std::string> paths;
    ConfigSyncContext context;
    context.followMode = FOLLOWX_MODE_DEFAULT;
    if (!TransformParams(env, relPath, followMode, extra, context)) {
        HILOG_ERROR(LOG_CORE, "trans form params error.");
        return AniUtils::CreateAniStringArray(env, paths);
    }
    CfgFiles *cfgFiles = GetCfgFilesEx(context.relPath.c_str(), context.followMode, context.extra.c_str());
    if (cfgFiles == nullptr) {
        HILOG_DEBUG(LOG_CORE, "GetCfgFilesSync result is nullptr.");
        ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "getCfgFilesSync", "CfgFiles is nullptr.");
        return AniUtils::CreateAniStringArray(env, paths);
    }
    for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
        if (cfgFiles->paths[i] != nullptr) {
            paths.push_back(cfgFiles->paths[i]);
        }
    }
    FreeCfgFiles(cfgFiles);
    return AniUtils::CreateAniStringArray(env, paths);
}

ani_ref ConfigPolicyAni::GetCfgDirListSync(ani_env* env)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetCfgDirListSync start.");
    std::vector<std::string> paths;
    CfgDir *cfgDir = GetCfgDirList();
    if (cfgDir == nullptr) {
        HILOG_DEBUG(LOG_CORE, "GetCfgDirList result is nullptr.");
        ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "getCfgDirListSync", "CfgDirList is nullptr.");
        return AniUtils::CreateAniStringArray(env, paths);
    }
    for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
        if (cfgDir->paths[i] != nullptr) {
            paths.push_back(cfgDir->paths[i]);
        }
    }
    FreeCfgDirList(cfgDir);
    return AniUtils::CreateAniStringArray(env, paths);
}
} // namespace ConfigPolicy
} // namespace Customization
} // namespace OHOS

ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        HILOG_ERROR(LOG_CORE, "Unsupported ANI_VERSION_1.");
        return (ani_status)ANI_ERROR;
    }

    auto status = OHOS::Customization::ConfigPolicy::ConfigPolicyAni::Init(env);
    if (status != ANI_OK) {
        return status;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
