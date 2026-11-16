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
import { AsyncCallback, BusinessError } from '@ohos.base';

namespace configPolicy {

    loadLibrary("configpolicy_ani.z");

    export enum FollowXMode {
        DEFAULT = 0,
        NO_RULE_FOLLOWED = 1,
        SIM_DEFAULT = 10,
        SIM_1 = 11,
        SIM_2 = 12,
        SIM_3 = 13,
        USER_DEFINED = 100
    }

    native function getOneCfgFileSync(relPath: string, followMode?: FollowXMode, extra?: string): string;

    native function getCfgFilesSync(relPath: string, followMode?: FollowXMode, extra?: string): Array<string>;

    native function getCfgDirListSync(): Array<string>;

    function getOneCfgFile(relPath: string, callback: AsyncCallback<string>) : void {
        taskpool.execute((): string => {
            return getOneCfgFileSync(relPath);
        }).then((content: Any) => {
            callback(null, content as string);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getOneCfgFile(relPath: string, followMode: FollowXMode, callback: AsyncCallback<string>) : void {
        taskpool.execute((): string => {
            return getOneCfgFileSync(relPath, followMode);
        }).then((content: Any) => {
            callback(null, content as string);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getOneCfgFile(
        relPath: string, followMode: FollowXMode, extra: string, callback: AsyncCallback<string>) : void {
        taskpool.execute((): string => {
            return getOneCfgFileSync(relPath, followMode, extra);
        }).then((content: Any) => {
            callback(null, content as string);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getOneCfgFile(relPath: string): Promise<string> {
        return new Promise<string>((resolve, reject) => {
            taskpool.execute((): string => {
                return getOneCfgFileSync(relPath);
            }).then((content: Any) => {
                resolve(content as string);
            }, (err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    function getOneCfgFile(relPath: string, followMode: FollowXMode, extra?: string): Promise<string> {
        return new Promise<string>((resolve, reject) => {
            taskpool.execute((): string => {
                return getOneCfgFileSync(relPath, followMode, extra);
            }).then((content: Any) => {
                resolve(content as string);
            }, (err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    function getCfgFiles(relPath: string, callback: AsyncCallback<Array<string>>) : void {
        taskpool.execute((): Array<string> => {
            return getCfgFilesSync(relPath);
        }).then((content: Any) => {
            callback(null, content as Array<string>);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getCfgFiles(relPath: string, followMode: FollowXMode, callback: AsyncCallback<Array<string>>) : void {
        taskpool.execute((): Array<string> => {
            return getCfgFilesSync(relPath, followMode);
        }).then((content: Any) => {
            callback(null, content as Array<string>);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getCfgFiles(
        relPath: string, followMode: FollowXMode, extra: string, callback: AsyncCallback<Array<string>>) : void {
        taskpool.execute((): Array<string> => {
            return getCfgFilesSync(relPath, followMode, extra);
        }).then((content: Any) => {
            callback(null, content as Array<string>);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getCfgFiles(relPath: string): Promise<Array<string>> {
        return new Promise<Array<string>>((resolve, reject) => {
            taskpool.execute((): Array<string> => {
                return getCfgFilesSync(relPath);
            }).then((content: Any) => {
                resolve(content as Array<string>);
            }, (err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    function getCfgFiles(relPath: string, followMode: FollowXMode, extra?: string): Promise<Array<string>> {
        return new Promise<Array<string>>((resolve, reject) => {
            taskpool.execute((): Array<string> => {
                return getCfgFilesSync(relPath, followMode, extra);
            }).then((content: Any) => {
                resolve(content as Array<string>);
            }, (err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    function getCfgDirList(callback: AsyncCallback<Array<string>>) : void {
        taskpool.execute((): Array<string> => {
            return getCfgDirListSync();
        }).then((content: Any) => {
            callback(null, content as Array<string>);
        }).catch((err: Any) : void => {
            callback(err as BusinessError, undefined);
        });
    }

    function getCfgDirList(): Promise<Array<string>> {
        return new Promise<Array<string>>((resolve, reject) => {
            taskpool.execute((): Array<string> => {
                return getCfgDirListSync();
            }).then((content: Any) => {
                resolve(content as Array<string>);
            }, (err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }
}

export default configPolicy;
