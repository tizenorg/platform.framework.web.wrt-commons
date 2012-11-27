/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        popup_manager.cpp
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This is popup_manager implementation file
 */
#include <stddef.h>
#include <dpl/popup/popup_manager.h>
#include <dpl/popup/popup.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/singleton_impl.h>

IMPLEMENT_SINGLETON(DPL::Popup::PopupManager)

namespace DPL {
namespace Popup {

void PopupManager::Initialize(PopupRendererPtr renderer)
{
    Assert(!m_initialized);
    m_popupRenderer = renderer;
    m_popupRenderer->Initialize();
    m_initialized = true;
}

void PopupManager::Deinitialize()
{
    m_popupRenderer->Deinitialize();
    Assert(m_initialized);
    m_popupRenderer.reset();
    m_initialized = false;
}

IPopupPtr PopupManager::CreatePopup()
{
    Assert(m_initialized);
    return m_popupRenderer->CreatePopup();
}

} // namespace Popup
} // namespace DPL
