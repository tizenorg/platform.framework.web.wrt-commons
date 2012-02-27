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
 * @file    popup_controller.cpp
 * @author  Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version 1.0
 * @bref    Implementation file for popup controller
 */

#include <dpl/popup/popup_controller.h>

#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <dpl/singleton_impl.h>
IMPLEMENT_SINGLETON(DPL::Popup::PopupController)

namespace DPL {
namespace Popup {

void CtrlPopup::SetTitle(const std::string &title)
{
    Assert(m_popup);
    m_popup->SetTitle(title);
}

void CtrlPopup::Append(PopupObject::IPopupObject *object)
{
    Assert(m_popup);
    m_popup->Append(object);
}

CtrlPopup::CtrlPopup(IPopupPtr popup) :
    m_popup(popup),
    m_callback()
{
    Assert(m_popup);
}

CtrlPopup::~CtrlPopup()
{
}

void CtrlPopup::EmitAnswer(const AnswerCallbackData & answer)
{
    AnswerCallbackData l_answer = answer;
    l_answer.loopHandle = m_loopHandle;
    PopupAnswerEvent event(SharedFromThis(), m_callback, l_answer);
    DPL::Event::EventSupport<PopupAnswerEvent>::EmitEvent(event);
}

PopupController::PopupController() : m_canvas(NULL)
{
}

CtrlPopupPtr PopupController::CreatePopup() const
{
    return CtrlPopupPtr(
               new CtrlPopup(PopupManagerSingleton::Instance().CreatePopup()));
}

void PopupController::OnEventReceived(const ShowPopupEvent& event)
{
    CtrlPopupPtr popup = event.GetArg0();
    popup->m_callback = event.GetArg1();
    popup->m_loopHandle = event.GetArg2();

    //pass canvas from controller to manager
    //canvas is not passed earlier because value wasn't set properly
    PopupManagerSingleton::Instance().setExternalCanvas(getExternalCanvas());

    PopupManagerSingleton::Instance().RunAsyncWithArgType(
        popup->m_popup,
        &PopupController::StaticOnAnswerReceived,
        new CtrlPopupPtr(popup));
}

void PopupController::StaticOnAnswerReceived(const AnswerCallbackData & answer,
        CtrlPopupPtr* popup)
{
    Assert(popup != NULL);
    (*popup)->EmitAnswer(answer);
    delete popup; // Just SharedPtr is destroyed, not the popup itself
}

void PopupControllerUser::OnEventReceived(const PopupAnswerEvent& event)
{
    //Here we are in a proper context to call the callback
    PopupAnswerCallback answerCall = event.GetArg1();
    AnswerCallbackData answer = event.GetArg2();
    answerCall.Call(answer);
    event.GetArg0()->DPL::Event::EventSupport<PopupAnswerEvent>::RemoveListener(this);
}

void PopupControllerUser::ListenForAnswer(CtrlPopupPtr popup)
{
    popup->DPL::Event::EventSupport<PopupAnswerEvent>::AddListener(this);
}
} //namespace Popup
} //namespace DPL
