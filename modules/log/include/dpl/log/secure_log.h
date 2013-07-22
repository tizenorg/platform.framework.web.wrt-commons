/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file       secure_log.h
 * @author     Jihoon Chung(jihoon.chung@samsung.com)
 * @version    0.1
 * @brief
 */

#ifndef DPL_SECURE_LOG_H
#define DPL_SECURE_LOG_H

#include <dlog.h>

#define COLOR_ERROR   "\e[1;31m"
#define COLOR_WARNING "\e[2;31m"
#define COLOR_END     "\e[0m"
#define COLOR_TAG     "\e[0m"

// default TAG
#undef LOG_TAG
#define LOG_TAG "WRT_UNDEFINED"

#ifdef WRT_LOG
#undef LOG_TAG
#define LOG_TAG "WRT"
#undef COLOR_TAG
#define COLOR_TAG "\e[1;32m"
#endif

#ifdef WRT_BUNDLE_LOG
#undef LOG_TAG
#define LOG_TAG "WRT_BUNDLE"
#undef COLOR_TAG
#define COLOR_TAG "\e[1;34m"
#endif

#ifdef WRT_PLUGINS_COMMON_LOG
#undef LOG_TAG
#define LOG_TAG "WRT_PLUGINS/COMMON"
#undef COLOR_TAG
#define COLOR_TAG "\e[1;36m"
#endif

#ifdef WRT_PLUGINS_WIDGET_LOG
#undef LOG_TAG
#define LOG_TAG "WRT_PLUGINS/WIDGET"
#undef COLOR_TAG
#define COLOR_TAG "\e[1;35m"
#endif

#ifdef WRT_INSTALLER_LOG
#undef LOG_TAG
#define LOG_TAG "WRT_INSTALLER"
#undef COLOR_TAG
#define COLOR_TAG "\e[1;32m"
#endif

#ifndef SECURE_SLOGD
#define SECURE_SLOGD(fmt, arg...) SLOGD(fmt,##arg)
#endif

#ifndef SECURE_SLOGW
#define SECURE_SLOGW(fmt, arg...) SLOGW(fmt,##arg)
#endif

#ifndef SECURE_SLOGE
#define SECURE_SLOGE(fmt, arg...) SLOGE(fmt,##arg)
#endif

#define _D(fmt, arg ...) SECURE_SLOGD(COLOR_TAG fmt COLOR_END,##arg)
#define _W(fmt, arg ...) SECURE_SLOGW(COLOR_WARNING fmt COLOR_END,##arg)
#define _E(fmt, arg ...) SECURE_SLOGE(COLOR_ERROR fmt COLOR_END,##arg)

#endif // DPL_SECURE_LOG_H

