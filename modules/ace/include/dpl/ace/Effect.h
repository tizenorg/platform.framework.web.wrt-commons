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
//
//
//
//  @ Project : Access Control Engine
//  @ File Name : Effect.h
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#if !defined(_EFFECT_H)
#define _EFFECT_H

enum Effect
{
    Deny =0,
    Undetermined=1,    // jk mb added this enum, so the ones below are inceremented!!!!!!!
    PromptOneShot =2,
    PromptSession =3,
    PromptBlanket =4,
    Permit =5,
    Inapplicable =6,
    NotMatchingTarget=7,
    Error
};

const char * toString(Effect);

#endif  //_EFFECT_H
