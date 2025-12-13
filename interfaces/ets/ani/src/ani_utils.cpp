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

#include "ani_utils.h"
#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001E00

#undef LOG_TAG
#define LOG_TAG "ConfigPolicyJs"

namespace OHOS {
namespace Customization {
namespace ConfigPolicy {
static const char* CLASS_NAME_BUSINESSERROR = "@ohos.base.BusinessError";

bool AniUtils::AniStrToString(ani_env *env, ani_string ani_str, std::string& out)
{
    ani_size strSize;
    if (env->String_GetUTF8Size(ani_str, &strSize) != ANI_OK) {
        return false;
    }

    std::vector<char> buffer(strSize + 1); // +1 for null terminator
    char *utf8_buffer = buffer.data();

    ani_size bytes_written = 0;
    if (env->String_GetUTF8(ani_str, utf8_buffer, strSize + 1, &bytes_written) != ANI_OK) {
        return false;
    }

    utf8_buffer[bytes_written] = '\0';
    out = std::string(utf8_buffer);
    return true;
}

ani_string AniUtils::StringToAniStr(ani_env *env, const std::string &str)
{
    ani_string result{};
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &result)) {
        return nullptr;
    }
    return result;
}

ani_ref AniUtils::CreateAniStringArray(ani_env *env, const std::vector<std::string> &paths)
{
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        HILOG_ERROR(LOG_CORE, "GetUndefined Failed.");
    }

    ani_array array;
    if (ANI_OK != env->Array_New(paths.size(), undefinedRef, &array)) {
        HILOG_ERROR(LOG_CORE, "new array ref error.");
        return array;
    }
    for (size_t i = 0; i < paths.size(); ++i) {
        auto item = AniUtils::StringToAniStr(env, paths[i]);
        if (ANI_OK != env->Array_Set(array, i, item)) {
            return array;
        }
    }
    return array;
}

void AniUtils::ThrowAniError(ani_env *env, int32_t code, const std::string &message)
{
    ani_class cls {};
    if (ANI_OK != env->FindClass(CLASS_NAME_BUSINESSERROR, &cls)) {
        HILOG_ERROR(LOG_CORE, "find class %{public}s failed", CLASS_NAME_BUSINESSERROR);
        return;
    }
    ani_method ctor {};
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", ":", &ctor)) {
        HILOG_ERROR(LOG_CORE, "find method BusinessError constructor failed");
        return;
    }
    ani_object error {};
    if (ANI_OK != env->Object_New(cls, ctor, &error)) {
        HILOG_ERROR(LOG_CORE, "new object %{public}s failed", CLASS_NAME_BUSINESSERROR);
        return;
    }
    if (ANI_OK != env->Object_SetPropertyByName_Int(error, "code_", static_cast<ani_int>(code))) {
        HILOG_ERROR(LOG_CORE, "set property BusinessError.code failed");
        return;
    }
    ani_string messageRef {};
    if (ANI_OK != env->String_NewUTF8(message.c_str(), message.size(), &messageRef)) {
        HILOG_ERROR(LOG_CORE, "new message string failed");
        return;
    }
    if (ANI_OK != env->Object_SetPropertyByName_Ref(error, "message", static_cast<ani_ref>(messageRef))) {
        HILOG_ERROR(LOG_CORE, "set property BusinessError.message failed");
        return;
    }
    if (ANI_OK != env->ThrowError(static_cast<ani_error>(error))) {
        HILOG_ERROR(LOG_CORE, "throwError ani_error object failed");
    }
}
} // namespace ConfigPolicy
} // namespace Customization
} // namespace OHOS
