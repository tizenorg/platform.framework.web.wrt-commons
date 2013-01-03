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
 * @file        popup_manager.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This is popup_manager declaration file
 */

#ifndef WRT_SRC_POPUP_POPUP_MANAGER_H_
#define WRT_SRC_POPUP_POPUP_MANAGER_H_

#include <memory>
#include <dpl/assert.h>
#include <dpl/noncopyable.h>
#include <dpl/singleton.h>
#include <dpl/optional.h>
#include <dpl/popup/popup.h>
#include <dpl/popup/popup_renderer.h>
#include <dpl/log/log.h>

namespace DPL {
namespace Popup {

class PopupManager : DPL::Noncopyable
{
    template <class ArgType>
    struct TemplatedPopupCallback
    {
        typedef void (*Type)(const AnswerCallbackData& answer, ArgType* data);
    };

  public:
    PopupManager() : m_initialized(false) {}
    ~PopupManager()
    {
        if (m_initialized) {
            LogError("Destroyed without Deinitialize");
        }
    }
    void Initialize (PopupRendererPtr creator);
    void Deinitialize();
    void SetPopupRenderer (PopupRendererPtr creator);
    IPopupPtr CreatePopup();

    template <class ArgType>
    void RunAsyncWithArgType(IPopupPtr popup,
            typename TemplatedPopupCallback<ArgType>::Type callback,
            ArgType* argument)
    {
        Assert(callback);
        WrapCbAndArg<ArgType>* wrapped =
            new WrapCbAndArg<ArgType>(callback, argument);
        popup->Show(&CallbackArgTypeTranslator<ArgType>, wrapped);
    }

    void Show(IPopupPtr popup,
            IPopup::PopupCallbackType callback,
            void* userdata)
    {
        popup->Show(callback, userdata);
    }

    void setExternalCanvas(void* externalCanvas)
    {
        Assert(m_initialized && "Manger should be initialized");
        m_popupRenderer->setExternalCanvas(externalCanvas);
    }

  private:
    template <class ArgType>
    struct WrapCbAndArg
    {
        WrapCbAndArg(typename TemplatedPopupCallback<ArgType>::Type cb,
                ArgType* arg) :
            callback(cb),
            argument(arg)
        {
        }

        typename TemplatedPopupCallback<ArgType>::Type callback;
        ArgType* argument;
    };

    template <class ArgType>
    static void CallbackArgTypeTranslator(const AnswerCallbackData & answer,
            void* data)
    {
        WrapCbAndArg<ArgType>* wrapped =
            static_cast< WrapCbAndArg<ArgType>* >(data);
        wrapped->callback(answer, wrapped->argument);
        delete wrapped;
    }

    bool m_initialized;
    PopupRendererPtr m_popupRenderer;
};

typedef DPL::Singleton<Popup::PopupManager> PopupManagerSingleton;

} // namespace Popup
} // namespace DPL

#endif //WRT_SRC_POPUP_POPUP_MANAGER_H_
