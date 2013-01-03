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
/**
 * @file    popup_controller.h
 * @author  Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version 1.0
 * @bref    Header file for popup controller
 */

/**
 * To display a popup from a given class:
 *
 **class ABC
 **{
 *    void AskUser()
 *    {
 *    }
 *
 *    void DoSomeLogicWithAnswer()
 *    {
 *    }
 **};
 *
 * ... update the class to something simmilar:
 *
 **class ABC : Popup::PopupControllerUser
 **{
 *    void AskUser() {
 *        using namespace Popup;
 *        CtrlPopupPtr popup =
 *                PopupControllerSingletion::Instance().CreatePopup();
 *        popup->SetTitle("Title");
 *        popup->SetContent("Content");
 *        popup->AddButton("name1", 1);
 *        popup->AddButton("name2", 2);
 *        ListenForAnswer(popup);
 *        ShowPopupEvent event(popup,
 *                             MakeAnswerCallback(this,
 *                                                &ABC::DoSomeLogicWithAnswer));
 *        CONTROLLER_POST_EVENT(PopupController, event);
 *    }
 *
 *    void DoSomeLogicWithAnswer(Popup::LabelId answer) {
 *        if (answer == 1)
 *            ;//name1 pressed
 *        else if (answer == 2)
 *            ;//name2 pressed
 *    }
 **};
 **/

#ifndef WRT_SRC_POPUP_CONTROLLER_POPUP_CONTROLLER_H_
#define WRT_SRC_POPUP_CONTROLLER_POPUP_CONTROLLER_H_

#include <memory>
#include <dpl/singleton.h>
#include <dpl/event/controller.h>
#include <dpl/event/event_listener.h>
#include <dpl/generic_event.h>
#include <dpl/mutex.h>
#include <dpl/exception.h>
#include <dpl/noncopyable.h>
#include <dpl/log/log.h>
#include <dpl/popup/popup_manager.h>

namespace DPL {
namespace Popup {
typedef int LabelId;

struct PopupAnswerCallback
{
    typedef void (PopupAnswerCallback::*MemberPtr)();

    void* callee;
    MemberPtr member;
    void (*callTranslator)(PopupAnswerCallback* callData,
            const AnswerCallbackData& answer);

    void Call(const AnswerCallbackData& answer)
    {
        callTranslator(this, answer);
    }
};

class PopupController;
class CtrlPopup;

typedef std::shared_ptr<CtrlPopup> CtrlPopupPtr;

DECLARE_GENERIC_EVENT_3(PopupAnswerEvent,
                        CtrlPopupPtr,
                        PopupAnswerCallback,
                        AnswerCallbackData)

DECLARE_GENERIC_EVENT_2(ShowPopupEventShort,
                        CtrlPopupPtr,
                        PopupAnswerCallback)

class CtrlPopup : public DPL::Event::EventSupport<PopupAnswerEvent>,
    public std::enable_shared_from_this<CtrlPopup>
{
  public:
    void SetTitle(const std::string &title);
    void Append(PopupObject::IPopupObject *object);

    ~CtrlPopup();
  private:
    friend class PopupController;
    friend class std::shared_ptr<CtrlPopup>;

    explicit CtrlPopup(IPopupPtr popup);
    void EmitAnswer(const AnswerCallbackData& answer);

    IPopupPtr m_popup;
    PopupAnswerCallback m_callback;
};

class PopupController :
    public DPL::Event::Controller<DPL::TypeListDecl<ShowPopupEventShort>::Type>
{
  public:
    CtrlPopupPtr CreatePopup() const;

    void setExternalCanvas(void* canvas)
    {
        m_canvas = canvas;
    }
    void* getExternalCanvas() const
    {
        return m_canvas;
    }
    void* m_canvas;

  protected:
    virtual void OnEventReceived(const ShowPopupEventShort& event);
    PopupController();

  private:
    static void StaticOnAnswerReceived(const AnswerCallbackData& answer,
            CtrlPopupPtr* popup);
};

class PopupControllerUser : DPL::Event::EventListener<PopupAnswerEvent>
{
    template <class Type>
    struct PopupAnswerCallbackCreator
    {
        typedef void (Type::*MemberPtr)(const AnswerCallbackData& answer);
        union Caster
        {
            MemberPtr specific;
            PopupAnswerCallback::MemberPtr generic;
        };

        static PopupAnswerCallback Create(Type* callee,
                MemberPtr callback)
        {
            PopupAnswerCallback callData;

            callData.callee = callee;

            Caster caster;
            caster.specific = callback;
            callData.member = caster.generic;

            callData.callTranslator =
                &PopupAnswerCallbackCreator::MemberCallbackTranslator;

            return callData;
        }

        static void MemberCallbackTranslator(PopupAnswerCallback* callData,
                const AnswerCallbackData& answer)
        {
            Type* typedThis = static_cast<Type*>(callData->callee);
            Caster caster;
            caster.generic = callData->member;
            MemberPtr typedCallback = caster.specific;
            (typedThis->*typedCallback)(answer);
        }
    };

  protected:
    virtual void OnEventReceived(const PopupAnswerEvent& event);
    void ListenForAnswer(CtrlPopupPtr popup);

    template <class Type>
    PopupAnswerCallback MakeAnswerCallback(Type* This,
            void (Type::*callback)
            (const AnswerCallbackData &))
    {
        return PopupAnswerCallbackCreator<Type>::Create(This, callback);
    }
};

typedef DPL::Singleton<PopupController> PopupControllerSingleton;

} //namespace Popup
} //namespace DPL

#endif //WRT_SRC_POPUP_CONTROLLER_POPUP_CONTROLLER_H_
