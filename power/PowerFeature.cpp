/*
 * Copyright (C) 2021, Paranoid Android
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "vendor.aospa.power-service.xiaomi_sdm845"

#include <android-base/file.h>
#include <android-base/logging.h>

#include "PowerFeature.h"

namespace {
int open_ts_input() {
    int fd = -1;
    DIR* dir = opendir("/dev/input");

    if (dir != NULL) {
        struct dirent* ent;

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_CHR) {
                char absolute_path[PATH_MAX] = {0};
                char name[80] = {0};

                strcpy(absolute_path, "/dev/input/");
                strcat(absolute_path, ent->d_name);

                fd = open(absolute_path, O_RDWR);
                if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) > 0) {
                    if (strcmp(name, "atmel_mxt_ts") == 0 || strcmp(name, "fts_ts") == 0 ||
                            strcmp(name, "fts") == 0 || strcmp(name, "ft5x46") == 0 ||
                            strcmp(name, "synaptics_dsx") == 0 ||
                            strcmp(name, "NVTCapacitiveTouchScreen") == 0)
                        break;
                }

                close(fd);
                fd = -1;
            }
        }

        closedir(dir);
    }

    return fd;
}
}  // anonymous namespace

namespace aidl {
namespace vendor {
namespace aospa {
namespace power {

static constexpr int kInputEventWakeupModeOff = 4;
static constexpr int kInputEventWakeupModeOn = 5;

ndk::ScopedAStatus PowerFeature::setFeature(Feature feature, bool enabled) {
    switch (feature) {
        case Feature::DOUBLE_TAP: {
            int fd = open_ts_input();
            if (fd == -1) {
                LOG(WARNING)
                    << "DT2W won't work because no supported touchscreen input devices were found";
                return ndk::ScopedAStatus::fromServiceSpecificError(ENOTSUP);
            }
            struct input_event ev;
            ev.type = EV_SYN;
            ev.code = SYN_CONFIG;
            ev.value = enabled ? kInputEventWakeupModeOn : kInputEventWakeupModeOff;
            write(fd, &ev, sizeof(ev));
            close(fd);
        }
            break;
        default:
            return ndk::ScopedAStatus::fromServiceSpecificError(ENOTSUP);
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace power
}  // namespace aospa
}  // namespace vendor
}  // namespace aidl
