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
 * @file        popup_renderer.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This is declaration file of PopupRenderer
 */

#ifndef WRT_SRC_POPUP_POPUP_RENDERER_H_
#define WRT_SRC_POPUP_POPUP_RENDERER_H_

#include <map>
#include <string>
#include <memory>

#include <dpl/noncopyable.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/foreach.h>
#include <dpl/popup/popup.h>

namespace DPL {
namespace Popup {

class PopupRenderer : public std::enable_shared_from_this<PopupRenderer>
{
  public:
    PopupRenderer();
    ~PopupRenderer();
    void Initialize();
    void Deinitialize();
    IPopupPtr CreatePopup();
    virtual void setExternalCanvas(void* externalCanvas);
  protected:
    class Popup;
    typedef std::shared_ptr<Popup> PopupPtr;

    class Popup : public IPopup
    {
      public:
        typedef std::map<int, std::string> ButtonMap;
        virtual void SetTitle(const std::string &title)
        {
            LogDebug(title);
            Assert(m_title.empty());
            m_title = title;
        }

        virtual void Append(PopupObject::IPopupObject *object)
        {
            m_popupObjectList.push_back(object);
        }

        virtual void Show(IPopup::PopupCallbackType callback,
                void* data)
        {
            Assert(callback);
            m_data = data;
            m_callback = callback;
            m_renderer->Render(std::static_pointer_cast<Popup>(shared_from_this()));
        }

        const std::string& GetTitle() const
        {
            return m_title;
        }

        PopupObject::PopupObjects& GetPopupObjects()
        {
            return m_popupObjectList;
        }

        virtual ~Popup()
        {
            FOREACH(it, m_popupObjectList) {
                delete *it;
            }
        }

      private:
        friend class PopupRenderer;
        friend class std::shared_ptr<Popup>;
        friend class PopupObjectTheme;

        Popup(std::shared_ptr<PopupRenderer> renderer) : m_renderer(renderer)
        {
        }

        void ForwardAnswer(const AnswerCallbackData & answer) const
        {
            m_callback(answer, m_data);
        }

        std::string m_title;
        void* m_data;
        IPopup::PopupCallbackType m_callback;
        PopupObject::PopupObjects m_popupObjectList;
        std::shared_ptr<PopupRenderer> m_renderer;
    };

  private:
    void Render (PopupPtr popup);

    class Impl;
    Impl* m_impl;
};

typedef std::shared_ptr<PopupRenderer> PopupRendererPtr;

} // namespace Popup
} // namespace DPL

#endif //WRT_SRC_POPUP_POPUP_RENDERER_H_
