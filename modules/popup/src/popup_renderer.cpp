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
 * @file        popup_renderer.cpp
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This is efl specific implementation for PopupRenderer
 */
#include <stddef.h>
#include <dpl/popup/popup_manager.h>
#include <dpl/popup/popup_renderer.h>
#include <dpl/popup/popup_manager.h>
#include <dpl/popup/evas_object.h>
#include <dpl/scoped_array.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/framework_efl.h>
#include <dpl/lexical_cast.h>
#include <queue>

namespace DPL {

namespace {
using namespace Popup;
const char* EDJ_NAME = "/usr/share/edje/ace/generic_popup.edj";
const char* POPUP_LAYOUT1 = "popup_layout1";
const char* POPUP_LAYOUT2 = "popup_layout2";
const char* POPUP_PART_TITLE = "title,text";
const char* POPUP_PART_BUTTON1 = "button1";
const char* POPUP_PART_BUTTON2 = "button2";
const char* POPUP_PART_BUTTON3 = "button3";
const char* BUTTON_CLICKED_CALLBACK_NAME = "clicked";
const char* CHANGED_CALLBACK_NAME = "changed";
const unsigned int MAX_NUMBER_OF_VERTICAL = 2;

Evas_Object* create_layout_main(Evas_Object* parent, int totalV)
{
    Evas_Object *layout = elm_layout_add(parent);

    if (totalV == 1) {
        elm_layout_file_set(layout, EDJ_NAME, POPUP_LAYOUT1);
    } else if (totalV == 2) {
        elm_layout_file_set(layout, EDJ_NAME, POPUP_LAYOUT2);
    } else {
        Assert("popup needs define new group in the edc");
    }

    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
                                     EVAS_HINT_EXPAND);
    return layout;
}
} //namespace

namespace Popup {
class PopupRenderer::Impl
{
  public:
    Impl() :
        m_popupsToRender(),
        m_current(),
        m_initialized(false)
    {
    }

    ~Impl()
    {
        if (m_initialized) {
            LogError("Destroyed without Deinitialize");
        }
    }

    void Initialize()
    {
        Assert(!m_initialized);
        m_initialized = true;
    }

    void Deinitialize()
    {
        Assert(m_initialized);
        m_current.reset();
        while (!m_popupsToRender.empty()) {
            m_popupsToRender.pop();
        }
        m_initialized = false;
    }

    void ButtonCallback(EvasObject::IConnection* /*connection*/,
            void* /*event_info*/,
            void* data)
    {
        LogInfo("ButtonCallback");
        Assert(m_initialized);
        AnswerCallbackData answerData;

        answerData.buttonAnswer = reinterpret_cast<int>(data);
        answerData.chackState = m_checkState;
        answerData.password = m_password;
        m_current->ForwardAnswer(answerData);
        m_current.reset();

        FOREACH(it, m_createdObjects)
        {
            if (it->IsValid()) {
                evas_object_del(*it);
            }
        }
        m_createdObjects.clear();
        m_checkState = false;
        DoRender();
    }

    void CheckCallback(EvasObject::IConnection* connection,
            void* /*event_info*/,
            void* /* unused */)
    {
        m_checkState =
            elm_check_state_get(connection->GetEvasObject());
    }

    void Render (PopupPtr popup)
    {
        Assert(m_initialized);
        m_popupsToRender.push(popup);
        DoRender();
    }

    void DoRender(const PopupObject::Label& object,
            EvasObject& parent,
            EvasObject& layout,
            int themeIndex)
    {
        EvasObject label(elm_label_add(parent));

        elm_object_style_set(label, "popup_description/default");
        elm_label_line_wrap_set(label, ELM_WRAP_WORD);
        elm_object_text_set(label, object.getLabel().c_str());
        evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_show(label);

        elm_object_part_content_set(
            layout,
            DPL::lexical_cast<std::string>(themeIndex).c_str(),
            label);
        m_createdObjects.push_back(label);
    }

    void DoRender(const PopupObject::Check& object,
            EvasObject& parent,
            EvasObject& layout,
            int themeIndex)
    {
        EvasObject check(elm_check_add(parent));

        evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND, 0.0);
        elm_object_text_set(check,
                            object.getCheckLabel().c_str());
        elm_object_part_content_set(
            layout,
            DPL::lexical_cast<std::string>(themeIndex).c_str(),
            check);

        check.ConnectMemberSmartCallback(CHANGED_CALLBACK_NAME,
                                         &Impl::CheckCallback,
                                         this,
                                         static_cast<void*>(NULL));
        evas_object_show(check);
        m_createdObjects.push_back(check);
    }

    void DoRender(const PopupObject::Button& object,
            EvasObject &parent)
    {
        EvasObject btn(elm_button_add(parent));

        elm_object_text_set(btn, object.getLabel().c_str());
        elm_object_part_content_set(parent, POPUP_PART_BUTTON1, btn);
        btn.ConnectMemberSmartCallback(BUTTON_CLICKED_CALLBACK_NAME,
                                       &Impl::ButtonCallback,
                                       this,
                                       reinterpret_cast<void*>(object.getLabelId()));
        m_createdObjects.push_back(btn);
    }

    void DoRender(const PopupObject::Button& object1,
            const PopupObject::Button& object2,
            EvasObject &parent)
    {
        DoRender(object1, parent);

        EvasObject btn2(elm_button_add(parent));

        elm_object_text_set(btn2, object2.getLabel().c_str());
        elm_object_part_content_set(parent, POPUP_PART_BUTTON2, btn2);
        btn2.ConnectMemberSmartCallback(BUTTON_CLICKED_CALLBACK_NAME,
                                        &Impl::ButtonCallback,
                                        this,
                                        reinterpret_cast<void*>(object2.getLabelId()));
        m_createdObjects.push_back(btn2);
    }

    void DoRender(const PopupObject::Button& object1,
            const PopupObject::Button& object2,
            const PopupObject::Button& object3,
            EvasObject &parent)
    {
        DoRender(object1, object2, parent);

        EvasObject btn3(elm_button_add(parent));

        elm_object_text_set(btn3, object3.getLabel().c_str());
        elm_object_part_content_set(parent, POPUP_PART_BUTTON3, btn3);
        btn3.ConnectMemberSmartCallback(BUTTON_CLICKED_CALLBACK_NAME,
                                        &Impl::ButtonCallback,
                                        this,
                                        reinterpret_cast<void*>(object3.getLabelId()));
        m_createdObjects.push_back(btn3);
    }

    EvasObject getBaseObject()
    {
        if (getExternalCanvas() == NULL) {
            LogInfo("Create old style popup");
            EvasObject win(elm_win_add(NULL, "Popup", ELM_WIN_DIALOG_BASIC));
            elm_win_borderless_set(win, EINA_TRUE);
            elm_win_alpha_set(win, EINA_TRUE);
            elm_win_raise(win);
            {
                int w, h, x, y;
                ecore_x_window_geometry_get(ecore_x_window_root_first_get(),
                                            &x,
                                            &y,
                                            &w,
                                            &h);
                evas_object_resize(win, w, h);
                evas_object_move(win, x, y);
            }
            m_createdObjects.push_back(win);
            evas_object_show(win);
            return win;
        } else {
            LogInfo("Create new style popup");
            EvasObject win(getExternalCanvas());
            evas_object_show(win);
            return win;
        }
    }

    void DoRender()
    {
        if (!m_current && !m_popupsToRender.empty()) {
            m_current = m_popupsToRender.front();
            m_popupsToRender.pop();

            m_themeIndexV = 0;

            // preprocessing
            std::vector<int> countPopupObjects = {0 /* PopupObject::BUTTON */,
                                                  0 /* PopupObject::LABEL */,
                                                  0 /* PopupObject::CHECK */};
            FOREACH(it, m_current->GetPopupObjects()) {
                Assert((*it)->getType() < countPopupObjects.size() &&
                    "Wrong PopupObject assigned");
                countPopupObjects[(*it)->getType()]++;
            }
            int needsIndexV = countPopupObjects[PopupObject::LABEL] +
                countPopupObjects[PopupObject::CHECK];

            EvasObject win = getBaseObject();
            EvasObject main(elm_popup_add(win));

            evas_object_size_hint_weight_set(main,
                                             EVAS_HINT_EXPAND,
                                             EVAS_HINT_EXPAND);
            elm_object_part_text_set(main,
                                     POPUP_PART_TITLE,
                                     m_current->GetTitle().c_str());

            m_createdObjects.push_back(main);
            std::vector<PopupObject::Button> buttonObjectList;
            EvasObject layout(create_layout_main(main, needsIndexV));
            m_createdObjects.push_back(layout);

            FOREACH(it, m_current->GetPopupObjects()) {
                switch ((*it)->getType()) {
                case PopupObject::BUTTON:
                    buttonObjectList.push_back(*(*it)->asButton());
                    break;
                case PopupObject::LABEL:
                    DoRender(*(*it)->asLabel(),
                             main,
                             layout,
                             m_themeIndexV++);
                    break;
                case PopupObject::CHECK:
                    DoRender(*(*it)->asCheck(),
                             main,
                             layout,
                             m_themeIndexV++);
                    break;
                default:
                    Assert("incorrect type");
                }
                Assert(m_themeIndexV <= MAX_NUMBER_OF_VERTICAL);
            }
            elm_object_content_set(main,
                                   layout);

            // show buution
            switch(buttonObjectList.size()) {
            case 0:
                LogInfo("no button");
                break;
            case 1:
                DoRender(buttonObjectList.at(0),
                         main);
                break;
            case 2:
                DoRender(buttonObjectList.at(0),
                         buttonObjectList.at(1),
                         main);
                break;
            case 3:
                DoRender(buttonObjectList.at(0),
                         buttonObjectList.at(1),
                         buttonObjectList.at(2),
                         main);
                break;
            default:
                Assert("incorrect button number");
                break;
            }

            evas_object_show(main);
        }
    }

    void setExternalCanvas(void* externalCanvas)
    {
        m_externalCanvas = static_cast<Evas_Object*>(externalCanvas);
    }

    Evas_Object* getExternalCanvas() const
    {
        return m_externalCanvas;
    }

    std::queue<PopupPtr> m_popupsToRender;
    std::list<EvasObject> m_createdObjects;
    PopupPtr m_current;
    bool m_initialized;
    bool m_checkState;
    DPL::Optional<std::string> m_password;
    unsigned int m_themeIndexV;

  private:
    Evas_Object* m_externalCanvas;
};

PopupRenderer::PopupRenderer()
{
    m_impl = new PopupRenderer::Impl();
}

PopupRenderer::~PopupRenderer()
{
    delete m_impl;
}

void PopupRenderer::Initialize()
{
    Assert(m_impl);
    m_impl->Initialize();
}

void PopupRenderer::Deinitialize()
{
    Assert(m_impl);
    m_impl->Deinitialize();
}

IPopupPtr PopupRenderer::CreatePopup()
{
    return std::static_pointer_cast<IPopup>(IPopupPtr
                                              (new Popup(shared_from_this())));
}

void PopupRenderer::Render(PopupPtr popup)
{
    m_impl->Render(popup);
}

void PopupRenderer::setExternalCanvas(void* externalCanvas)
{
    m_impl->setExternalCanvas(externalCanvas);
}

} // namespace Popup
} // namespace DPL
