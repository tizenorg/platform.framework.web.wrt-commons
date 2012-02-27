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
 * @file        popup_object.h
 * @author      Justyna Mejzner (j.mejzner@samsung.com)
 * @version     1.0
 * @brief       This is declaration file of PopupObject
 */

#ifndef WRT_SRC_POPUP_POPUPOBJECT_H_
#define WRT_SRC_POPUP_POPUPOBJECT_H_

#include <dpl/foreach.h>

#include <list>
#include <string>

namespace DPL {
namespace Popup {

namespace PopupObject {
class IPopupObject;
class PopupObjectBase;
class Button;
class Label;
class Check;

typedef std::list<IPopupObject*> PopupObjects;

enum PopupType
{
    BUTTON,
    LABEL,
    CHECK
};

class IPopupObject
{
  public:
    virtual Button* asButton() = 0;
    virtual Label* asLabel() = 0;
    virtual Check* asCheck() = 0;
    virtual PopupType getType() const = 0;
    virtual ~IPopupObject()
    {
    }
};

class PopupObjectBase : public IPopupObject
{
  public:
    virtual Button* asButton()
    {
        Assert("wrong type");
        return NULL;
    }
    virtual Label* asLabel()
    {
        Assert("wrong type");
        return NULL;
    }
    virtual Check* asCheck()
    {
        Assert("wrong type");
        return NULL;
    }

    PopupType getType() const
    {
        return m_type;
    }

  protected:
    PopupObjectBase(PopupType type) : m_type(type)
    {
    }

    PopupType m_type;
};

class Button : public PopupObjectBase
{
  public:
    Button(const std::string& label,
            int labelId) :
        PopupObjectBase(BUTTON),
        m_label(label),
        m_labelId(labelId)
    {
    }

    Button* asButton()
    {
        return this;
    }

    const std::string& getLabel() const
    {
        return m_label;
    }

    int getLabelId() const
    {
        return m_labelId;
    }

  private:
    std::string m_label;
    int m_labelId;
};

class Label : public PopupObjectBase
{
  public:
    Label(const std::string &label) :
        PopupObjectBase(LABEL),
        m_label(label)
    {
    }

    Label* asLabel()
    {
        return this;
    }

    const std::string& getLabel() const
    {
        return m_label;
    }

  private:
    std::string m_label;
};

class Check : public PopupObjectBase
{
  public:
    Check(const std::string &label) :
        PopupObjectBase(CHECK),
        m_checkLabel(label)
    {
    }

    Check* asCheck()
    {
        return this;
    }

    const std::string& getCheckLabel() const
    {
        return m_checkLabel;
    }

  private:
    std::string m_checkLabel;
};
} /*PopupObject*/

}//namespace Popup
}//namespace DPL

#endif //WRT_SRC_POPUP_POPUPOBJECT_H_
