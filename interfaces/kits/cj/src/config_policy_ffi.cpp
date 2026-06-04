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

#include "config_policy_ffi.h"

#include <string>
#include <vector>
#include <securec.h>
#include "config_policy_utils.h"
#include "hisysevent_adapter.h"
#include "config_policy_log.h"

namespace OHOS::Customization::ConfigPolicy {

static constexpr int MAX_MALLOC_LEN = 1024;

char** MallocCStringArr(const std::vector<std::string>& origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto size = origin.size();
    if (size > MAX_MALLOC_LEN) {
        return nullptr;
    }
    auto arr = static_cast<char**>(malloc(sizeof(char*) * size));
    if (arr == nullptr) {
        return nullptr;
    }
    for (size_t i = 0; i < size; i++) {
        size_t len = origin[i].size() + 1;
        arr[i] = static_cast<char*>(malloc(sizeof(char) * len));
        if (arr[i] == nullptr) {
            continue;
        }
        errno_t ret = strcpy_s(arr[i], len, origin[i].c_str());
        if (ret != 0) {
            free(arr[i]);
            arr[i] = nullptr;
        }
    }
    return arr;
}

extern "C" {
    RetDataCArrString CJ_GetCfgDirList()
    {
        LOGI("CJ_GetCfgDirList start");
        RetDataCArrString ret = { .code = SUCCESS_CODE, .data = { .head = nullptr, .size = 0 } };
        CfgDir *cfgDir = GetCfgDirList();

        std::vector<std::string> dirList;
        if (cfgDir != nullptr) {
            for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
                if (cfgDir->paths[i] != nullptr) {
                    dirList.push_back(cfgDir->paths[i]);
                }
            }
            FreeCfgDirList(cfgDir);
        } else {
            ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "CJ_GetCfgDirList", "CfgDirList is nullptr.");
        }

        ret.data.head = MallocCStringArr(dirList);
        ret.data.size = static_cast<int64_t>(ret.data.head == nullptr ? 0 : dirList.size());
        LOGI("CJ_GetCfgDirList ok");
        return ret;
    }

    RetDataCArrString CJ_GetCfgFiles(const char* relPath)
    {
        LOGI("CJ_GetCfgFiles start");
        RetDataCArrString ret = { .code = SUCCESS_CODE, .data = { .head = nullptr, .size = 0 } };
        LOGI("input path: [%{public}s]", relPath)
        std::string extra("");
        CfgFiles *cfgFiles = GetCfgFilesEx(relPath, FOLLOWX_MODE_DEFAULT, extra.c_str());

        std::vector<std::string> fileList;
        if (cfgFiles != nullptr) {
            for (size_t i = 0; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
                if (cfgFiles->paths[i] != nullptr) {
                    fileList.push_back(cfgFiles->paths[i]);
                }
            }
            FreeCfgFiles(cfgFiles);
        } else {
            ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "CJ_GetCfgFiles", "CfgFiles is nullptr.");
        }

        ret.data.head = MallocCStringArr(fileList);
        ret.data.size = static_cast<int64_t>(ret.data.head == nullptr ? 0 : fileList.size());
        return ret;
    }

    RetDataCString CJ_GetOneCfgFile(const char* relPath)
    {
        LOGI("CJ_GetOneCfgFile start");
        RetDataCString ret = { .code = SUCCESS_CODE, .data = nullptr };
        char outBuf[MAX_PATH_LEN] = {0};
        std::string extra("");
        char* filePath = GetOneCfgFileEx(relPath, outBuf, MAX_PATH_LEN, FOLLOWX_MODE_DEFAULT, extra.c_str());

        if (filePath == nullptr) {
            LOGI("GetOneCfgFileEx result is nullptr.");
            ReportConfigPolicyEvent(ReportType::CONFIG_POLICY_FAILED, "CJ_GetOneCfgFile", "CfgFile path is nullptr.");
            return ret;
        } else {
            LOGI("GetOneCfgFile return [%{public}s]", filePath);
        }
        size_t pathLen = strlen(filePath) + 1;
        ret.data = static_cast<char*>(malloc(sizeof(char) * pathLen));
        if (ret.data == nullptr) {
            return ret;
        }
        errno_t err = strcpy_s(ret.data, pathLen, filePath);
        if (err != 0) {
            free(ret.data);
            ret.data = nullptr;
        }
        return ret;
    }
}
} // namespace OHOS::Customization::ConfigPolicy
