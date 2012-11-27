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
 * @file        popup.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This is popup inteface declaration
 */

#ifndef WRT_SRC_POPUP_POPUP_H_
#define WRT_SRC_POPUP_POPUP_H_

#include <memory>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <string>
#include <dpl/optional.h>
#include <dpl/popup/popup_object.h>

namespace DPL {
namespace Popup {

struct AnswerCallbackData
{
    int buttonAnswer;
    DPL::Optional<std::string> password;
    bool chackState;
};

class PopupManager;
class IPopup;
typedef std::shared_ptr<IPopup> IPopupPtr;

class IPopup : public std::enable_shared_from_this<IPopup>
{
  public:
    virtual void SetTitle(const std::string &title) = 0;
    /*The object is deleted automatically after rendered */
    virtual void Append(PopupObject::IPopupObject *object) = 0;

  protected:
    typedef void (*PopupCallbackType)(const AnswerCallbackData& answer,
                                      void *data);
    virtual void Show(PopupCallbackType callback,
            void* data) = 0;
    virtual ~IPopup()
    {
    }

  private:
    friend class PopupManager;
    friend class std::shared_ptr<IPopup>;
};

} // namespace Popup
} // namespace DPL

#endif //WRT_SRC_POPUP_POPUP_H_
