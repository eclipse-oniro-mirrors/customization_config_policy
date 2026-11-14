/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "config_policy_napi.h"

#include <vector>

#include "hilog/log.h"
#include "config_policy_utils.h"
#include "hisysevent_adapter.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001E00

#undef LOG_TAG
#define LOG_TAG "ConfigPolicyJs"

namespace OHOS {
namespace Customization {
namespace ConfigPolicy {
using namespace OHOS::HiviewDFX;

static constexpr size_t ARGS_SIZE_ONE = 1;
static constexpr size_t ARGS_SIZE_TWO = 2;
static constexpr size_t ARGS_SIZE_THREE = 3;
static constexpr size_t ARGS_SIZE_FOUR = 4;
static constexpr int32_t ARR_INDEX_ZERO = 0;
static constexpr int32_t ARR_INDEX_ONE = 1;
static constexpr int32_t ARR_INDEX_TWO = 2;
static constexpr int32_t ARR_INDEX_THREE = 3;
static constexpr int32_t NAPI_RETURN_ZERO = 0;
static constexpr int32_t NAPI_RETURN_ONE = 1;
// Param Error Code
static constexpr int32_t PARAM_ERROR = 401;

napi_value ConfigPolicyNapi::Init(napi_env env, napi_value exports)
{
    napi_value nFollowXMode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nFollowXMode));
    CreateFollowXModeObject(env, nFollowXMode);

    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("getOneCfgFile", ConfigPolicyNapi::NAPIGetOneCfgFile),
        DECLARE_NAPI_FUNCTION("getOneCfgFileSync", ConfigPolicyNapi::NAPIGetOneCfgFileSync),
        DECLARE_NAPI_FUNCTION("getCfgFiles", ConfigPolicyNapi::NAPIGetCfgFiles),
        DECLARE_NAPI_FUNCTION("getCfgFilesSync", ConfigPolicyNapi::NAPIGetCfgFilesSync),
        DECLARE_NAPI_FUNCTION("getCfgDirList", ConfigPolicyNapi::NAPIGetCfgDirList),
        DECLARE_NAPI_FUNCTION("getCfgDirListSync", ConfigPolicyNapi::NAPIGetCfgDirListSync),

        DECLARE_NAPI_PROPERTY("FollowXMode", nFollowXMode)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

void ConfigPolicyNapi::CreateFollowXModeObject(napi_env env, napi_value value)
{
    napi_value nDefaultMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_DEFAULT), &nDefaultMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "DEFAULT", nDefaultMode));
    napi_value nNoFollowMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_NO_RULE_FOLLOWED),
                          &nNoFollowMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "NO_RULE_FOLLOWED", nNoFollowMode));
    napi_value nSimDefaultMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_SIM_DEFAULT),
        &nSimDefaultMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SIM_DEFAULT", nSimDefaultMode));
    napi_value nSim1Mode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_SIM_1), &nSim1Mode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SIM_1", nSim1Mode));
    napi_value nSim2Mode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_SIM_2), &nSim2Mode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SIM_2", nSim2Mode));
    napi_value nSim3Mode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_SIM_3), &nSim3Mode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "SIM_3", nSim3Mode));
    napi_value nUserDefineMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, static_cast<int32_t>(FOLLOWX_MODE_USER_DEFINED),
        &nUserDefineMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, value, "USER_DEFINED", nUserDefineMode));
}

napi_value ConfigPolicyNapi::NAPIGetOneCfgFile(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG(LOG_CORE, "NAPIGetOneCfgFile start.");
    return GetOneCfgFileOrAllCfgFiles(env, info, "NAPIGetOneCfgFile", NativeGetOneCfgFile);
}

napi_value ConfigPolicyNapi::NAPIGetOneCfgFileSync(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG(LOG_CORE, "NAPIGetOneCfgFileSync start.");
    return GetOneCfgFileOrAllCfgFilesSync(env, info, NativeGetOneCfgFileSync);
}

napi_value ConfigPolicyNapi::GetOneCfgFileOrAllCfgFilesSync(napi_env env, napi_callback_info info, NapiFunction func)
{
    size_t argc = ARGS_SIZE_THREE;
    napi_value argv[ARGS_SIZE_THREE] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (argc < ARGS_SIZE_ONE) {
        return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The number of parameters is incorrect.");
    }

    auto asyncContext = std::make_shared<ConfigAsyncContext>();
    asyncContext->followMode_ = FOLLOWX_MODE_DEFAULT;
    bool hasExtra = false;
    if (ParseRelPath(env, asyncContext->relPath_, argv[ARR_INDEX_ZERO]) == nullptr) {
        return nullptr;
    }

    if (argc == ARGS_SIZE_TWO) {
        if (ParseFollowMode(env, asyncContext->followMode_, argv[ARR_INDEX_ONE], hasExtra) == nullptr) {
            return nullptr;
        }
    }
    if (argc >= ARGS_SIZE_THREE) {
        hasExtra = true;
        if (ParseFollowMode(env, asyncContext->followMode_, argv[ARR_INDEX_ONE], hasExtra) == nullptr ||
            ParseExtra(env, asyncContext->extra_, argv[ARR_INDEX_TWO]) == nullptr) {
            return nullptr;
        }
    }
    return func(env, asyncContext);
}

napi_value ConfigPolicyNapi::GetOneCfgFileOrAllCfgFiles(napi_env env, napi_callback_info info,
    const std::string &workName, napi_async_execute_callback execute)
{
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    if (argc < ARGS_SIZE_ONE) {
        return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The number of parameters is incorrect.");
    }

    auto asyncContext = std::make_unique<ConfigAsyncContext>();
    asyncContext->followMode_ = FOLLOWX_MODE_DEFAULT;
    bool hasExtra = false;
    if (ParseRelPath(env, asyncContext->relPath_, argv[ARR_INDEX_ZERO]) == nullptr) {
        return nullptr;
    }
    if (argc == ARGS_SIZE_TWO) {
        if (MatchValueType(env, argv[ARR_INDEX_ONE], napi_function)) {
            napi_create_reference(env, argv[ARR_INDEX_ONE], NAPI_RETURN_ONE, &asyncContext->callbackRef_);
        } else {
            if (ParseFollowMode(env, asyncContext->followMode_, argv[ARR_INDEX_ONE], hasExtra) == nullptr) {
                return nullptr;
            }
        }
    }

    if (argc == ARGS_SIZE_THREE) {
        if (MatchValueType(env, argv[ARR_INDEX_TWO], napi_function)) {
            napi_create_reference(env, argv[ARR_INDEX_TWO], NAPI_RETURN_ONE, &asyncContext->callbackRef_);
        } else {
            if (ParseExtra(env, asyncContext->extra_, argv[ARR_INDEX_TWO]) == nullptr) {
                return nullptr;
            }
            hasExtra = true;
        }
        if (ParseFollowMode(env, asyncContext->followMode_, argv[ARR_INDEX_ONE], hasExtra) == nullptr) {
            return nullptr;
        }
    }

    if (argc == ARGS_SIZE_FOUR) {
        hasExtra = true;
        if (ParseFollowMode(env, asyncContext->followMode_, argv[ARR_INDEX_ONE], hasExtra) == nullptr ||
            ParseExtra(env, asyncContext->extra_, argv[ARR_INDEX_TWO]) == nullptr) {
            return nullptr;
        }
        if (!MatchValueType(env, argv[ARR_INDEX_THREE], napi_function)) {
            return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The type of callback must be AsyncCallback.");
        }
        napi_create_reference(env, argv[ARR_INDEX_THREE], NAPI_RETURN_ONE, &asyncContext->callbackRef_);
    }
    return HandleAsyncWork(env, asyncContext.release(), workName, execute, NativeCallbackComplete);
}

napi_value ConfigPolicyNapi::NAPIGetCfgFiles(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG(LOG_CORE, "NAPIGetCfgFiles start.");
    return GetOneCfgFileOrAllCfgFiles(env, info, "NAPIGetCfgFiles", NativeGetCfgFiles);
}

napi_value ConfigPolicyNapi::NAPIGetCfgFilesSync(napi_env env, napi_callback_info info)
{
    HILOG_DEBUG(LOG_CORE, "NAPIGetCfgFilesSync start.");
    return GetOneCfgFileOrAllCfgFilesSync(env, info, NativeGetCfgFilesSync);
}

napi_value ConfigPolicyNapi::NAPIGetCfgDirList(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    auto asyncContext = std::make_unique<ConfigAsyncContext>();
    if (argc == ARGS_SIZE_ONE) {
        bool matchFlag = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_function);
        if (!matchFlag) {
            return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The type of callback must be AsyncCallback.");
        }
        napi_create_reference(env, argv[ARR_INDEX_ZERO], NAPI_RETURN_ONE, &asyncContext->callbackRef_);
    }
    return HandleAsyncWork(env, asyncContext.release(), "NAPIGetCfgDirList", NativeGetCfgDirList,
        NativeCallbackComplete);
}

napi_value ConfigPolicyNapi::NAPIGetCfgDirListSync(napi_env env, napi_callback_info info)
{
    auto asyncContext = std::make_shared<ConfigAsyncContext>();
    return NativeGetCfgDirListSync(env, asyncContext);
}

napi_value ConfigPolicyNapi::CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

std::string ConfigPolicyNapi::GetStringFromNAPI(napi_env env, napi_value value)
{
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, NAPI_RETURN_ZERO, &size) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "can not get string size.");
        return "";
    }
    result.reserve(size + NAPI_RETURN_ONE);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + NAPI_RETURN_ONE), &size) != napi_ok) {
        HILOG_ERROR(LOG_CORE, "can not get string value.");
        return "";
    }
    return result;
}

napi_value ConfigPolicyNapi::HandleAsyncWork(napi_env env, ConfigAsyncContext *context, std::string workName,
    napi_async_execute_callback execute, napi_async_complete_callback complete)
{
    HILOG_DEBUG(LOG_CORE, "HandleAsyncWork start.");
    if (context == nullptr) {
        HILOG_ERROR(LOG_CORE, "context is nullptr");
        return nullptr;
    }

    napi_value result = nullptr;
    if (context->callbackRef_ == nullptr) {
        napi_create_promise(env, &context->deferred_, &result);
    } else {
        napi_get_undefined(env, &result);
    }
    napi_value resource = CreateUndefined(env);
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, workName.data(), NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, resource, resourceName, execute, complete, static_cast<void *>(context),
        &context->work_);
    napi_queue_async_work_with_qos(env, context->work_, napi_qos_user_initiated);
    HILOG_DEBUG(LOG_CORE, "HandleAsyncWork end.");
    return result;
}

bool ConfigPolicyNapi::MatchValueType(napi_env env, napi_value value, napi_valuetype targetType)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    return valueType == targetType;
}

void ConfigPolicyNapi::NativeGetOneCfgFile(napi_env env, void *data)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetOneCfgFile start.");
    if (data == nullptr) {
        HILOG_ERROR(LOG_CORE, "data is nullptr");
        return;
    }
    ConfigAsyncContext *asyncCallbackInfo = static_cast<ConfigAsyncContext *>(data);
    char outBuf[MAX_PATH_LEN] = {0};
    char *filePath = GetOneCfgFileEx(asyncCallbackInfo->relPath_.c_str(), outBuf, MAX_PATH_LEN,
                                     asyncCallbackInfo->followMode_, asyncCallbackInfo->extra_.c_str());
    if (filePath == nullptr) {
        HILOG_DEBUG(LOG_CORE, "GetOneCfgFileEx result is nullptr.");
    } else {
        asyncCallbackInfo->pathValue_ = std::string(filePath);
    }
    asyncCallbackInfo->createValueFunc_ = [](napi_env env, ConfigAsyncContext &context) -> napi_value {
        napi_value result;
        NAPI_CALL(env, napi_create_string_utf8(env, context.pathValue_.c_str(), NAPI_AUTO_LENGTH, &result));
        return result;
    };
}

napi_value ConfigPolicyNapi::NativeGetOneCfgFileSync(napi_env env, std::shared_ptr<ConfigAsyncContext> context)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetOneCfgFileSync start.");
    char outBuf[MAX_PATH_LEN] = {0};
    char *filePath = GetOneCfgFileEx(context->relPath_.c_str(), outBuf, MAX_PATH_LEN,
                                     context->followMode_, context->extra_.c_str());
    if (filePath == nullptr) {
        HILOG_DEBUG(LOG_CORE, "GetOneCfgFileEx result is nullptr.");
    } else {
        context->pathValue_ = std::string(filePath);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, context->pathValue_.c_str(), NAPI_AUTO_LENGTH, &result));
    return result;
}

void ConfigPolicyNapi::NativeGetCfgFiles(napi_env env, void *data)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetCfgFiles start.");
    if (data == nullptr) {
        HILOG_ERROR(LOG_CORE, "data is nullptr");
        return;
    }

    ConfigAsyncContext *asyncCallbackInfo = static_cast<ConfigAsyncContext *>(data);
    CfgFiles *cfgFiles = GetCfgFilesEx(asyncCallbackInfo->relPath_.c_str(), asyncCallbackInfo->followMode_,
                                       asyncCallbackInfo->extra_.c_str());
    if (cfgFiles != nullptr) {
        for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
            if (cfgFiles->paths[i] != nullptr) {
                asyncCallbackInfo->paths_.push_back(cfgFiles->paths[i]);
            }
        }
        FreeCfgFiles(cfgFiles);
    }
    CreateArraysValueFunc(*asyncCallbackInfo);
}

napi_value ConfigPolicyNapi::NativeGetCfgFilesSync(napi_env env, std::shared_ptr<ConfigAsyncContext> context)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetCfgFiles start.");
    CfgFiles *cfgFiles = GetCfgFilesEx(context->relPath_.c_str(), context->followMode_,
                                       context->extra_.c_str());
    if (cfgFiles != nullptr) {
        for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
            if (cfgFiles->paths[i] != nullptr) {
                context->paths_.push_back(cfgFiles->paths[i]);
            }
        }
        FreeCfgFiles(cfgFiles);
    }
    return CreateArraysValue(env, context);
}

void ConfigPolicyNapi::NativeGetCfgDirList(napi_env env, void *data)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetCfgDirList start.");
    if (data == nullptr) {
        HILOG_ERROR(LOG_CORE, "data is nullptr.");
        return;
    }

    ConfigAsyncContext *asyncCallbackInfo = static_cast<ConfigAsyncContext *>(data);
    CfgDir *cfgDir = GetCfgDirList();
    if (cfgDir != nullptr) {
        for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
            if (cfgDir->paths[i] != nullptr) {
                asyncCallbackInfo->paths_.push_back(cfgDir->paths[i]);
            }
        }
        FreeCfgDirList(cfgDir);
    } else {
        ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "getCfgDirList", "CfgDirList is nullptr.");
    }
    CreateArraysValueFunc(*asyncCallbackInfo);
}

napi_value ConfigPolicyNapi::NativeGetCfgDirListSync(napi_env env, std::shared_ptr<ConfigAsyncContext> context)
{
    HILOG_DEBUG(LOG_CORE, "NativeGetCfgDirListSync start.");
    CfgDir *cfgDir = GetCfgDirList();
    if (cfgDir != nullptr) {
        for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
            if (cfgDir->paths[i] != nullptr) {
                context->paths_.push_back(cfgDir->paths[i]);
            }
        }
        FreeCfgDirList(cfgDir);
    } else {
        ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "getCfgDirListSync", "CfgDirList is nullptr.");
    }
    return CreateArraysValue(env, context);
}

napi_value ConfigPolicyNapi::CreateArraysValue(napi_env env, std::shared_ptr<ConfigAsyncContext> context)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, context->paths_.size(), &result));
    for (size_t i = 0; i < context->paths_.size(); i++) {
        napi_value element = nullptr;
        NAPI_CALL(env, napi_create_string_utf8(env, context->paths_[i].c_str(), NAPI_AUTO_LENGTH, &element));
        NAPI_CALL(env, napi_set_element(env, result, i, element));
    }
    return result;
}

void ConfigPolicyNapi::CreateArraysValueFunc(ConfigAsyncContext &asyncCallbackInfo)
{
    asyncCallbackInfo.createValueFunc_ = [](napi_env env, ConfigAsyncContext &context) -> napi_value {
        napi_value result = nullptr;
        NAPI_CALL(env, napi_create_array_with_length(env, context.paths_.size(), &result));
        for (size_t i = 0; i < context.paths_.size(); i++) {
            napi_value element = nullptr;
            NAPI_CALL(env, napi_create_string_utf8(env, context.paths_[i].c_str(), NAPI_AUTO_LENGTH, &element));
            NAPI_CALL(env, napi_set_element(env, result, i, element));
        }
        return result;
    };
}

void ConfigPolicyNapi::NativeCallbackComplete(napi_env env, napi_status status, void *data)
{
    HILOG_DEBUG(LOG_CORE, "NativeCallbackComplete start.");
    if (data == nullptr) {
        HILOG_ERROR(LOG_CORE, "data is nullptr");
        return;
    }

    napi_value finalResult = nullptr;
    ConfigAsyncContext *asyncContext = static_cast<ConfigAsyncContext *>(data);
    if (asyncContext->createValueFunc_ != nullptr) {
        finalResult = asyncContext->createValueFunc_(env, *asyncContext);
    }

    napi_value result[] = { nullptr, nullptr };
    napi_get_null(env, &result[0]);
    result[1] = finalResult;

    if (asyncContext->deferred_ != nullptr) {
        napi_resolve_deferred(env, asyncContext->deferred_, result[1]);
    } else {
        napi_value callback = nullptr;
        napi_value userRet = nullptr;
        napi_get_reference_value(env, asyncContext->callbackRef_, &callback);
        napi_call_function(env, nullptr, callback, sizeof(result) / sizeof(napi_value), result, &userRet);
        napi_delete_reference(env, asyncContext->callbackRef_);
    }
    napi_delete_async_work(env, asyncContext->work_);
    delete asyncContext;
    HILOG_DEBUG(LOG_CORE, "NativeCallbackComplete end.");
}

napi_value ConfigPolicyNapi::ParseRelPath(napi_env env, std::string &param, napi_value args)
{
    bool matchFlag = MatchValueType(env, args, napi_string);
    if (!matchFlag) {
        return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The type of relPath must be string.");
    }
    param = GetStringFromNAPI(env, args);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, NAPI_RETURN_ONE, &result));
    return result;
}

napi_value ConfigPolicyNapi::ParseExtra(napi_env env, std::string &param, napi_value args)
{
    bool matchFlag = MatchValueType(env, args, napi_string);
    if (!matchFlag) {
        return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The type of extra must be string.");
    }
    param = GetStringFromNAPI(env, args);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, NAPI_RETURN_ONE, &result));
    return result;
}

napi_value ConfigPolicyNapi::ParseFollowMode(napi_env env, int32_t &param, napi_value args, bool hasExtra)
{
    bool matchFlag = MatchValueType(env, args, napi_number);
    if (!matchFlag) {
        return ThrowNapiError(env, PARAM_ERROR, "Parameter error. The type of followMode must be number.");
    }
    NAPI_CALL(env, napi_get_value_int32(env, args, &param));
    switch (param) {
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
                return ThrowNapiError(env, PARAM_ERROR,
                    "Parameter error. The followMode is USER_DEFINED, extra must be set.");
            }
            break;
        default:
            return ThrowNapiError(env, PARAM_ERROR,
                "Parameter error. The value of followMode should be in the enumeration value of FollowXMode.");
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, NAPI_RETURN_ONE, &result));
    return result;
}

napi_value ConfigPolicyNapi::ThrowNapiError(napi_env env, int32_t errCode, const std::string &errMessage)
{
    napi_throw_error(env, std::to_string(errCode).c_str(), errMessage.c_str());
    return nullptr;
}

static napi_value ConfigPolicyInit(napi_env env, napi_value exports)
{
    return ConfigPolicyNapi::Init(env, exports);
}

static napi_module g_configPolicyModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = ConfigPolicyInit,
    .nm_modname = "configPolicy",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void ConfigPolicyRegister()
{
    napi_module_register(&g_configPolicyModule);
}
} // namespace ConfigPolicy
} // namespace Customization
} // namespace OHOS
