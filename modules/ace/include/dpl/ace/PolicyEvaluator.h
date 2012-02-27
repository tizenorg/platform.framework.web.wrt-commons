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
//  @ File Name : PolicyEvaluator.h
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#ifndef _POLICY_EVALUATOR_H
#define _POLICY_EVALUATOR_H

#include <memory>
#include <set>
#include <string>

#include <dpl/event/event_listener.h>
#include <dpl/log/log.h>
#include <dpl/noncopyable.h>

#include <dpl/ace/AsyncVerdictResultListener.h>
#include <dpl/ace/Attribute.h>
#include <dpl/ace/ConfigurationManager.h>
#include <dpl/ace/Constants.h>
#include <dpl/ace/Effect.h>
#include <dpl/ace/Policy.h>
#include <dpl/ace/PolicyInformationPoint.h>
#include <dpl/ace/PolicyResult.h>
#include <dpl/ace/Request.h>
#include <dpl/ace/Subject.h>
#include <dpl/ace/Verdict.h>
#include <dpl/ace/UserDecision.h>
#include <dpl/ace/CombinerImpl.h>


class PolicyEvaluator : DPL::Noncopyable
{
  protected:

    /**
     * Internal method used to initiate policy evaluation. Called after attribute set has been fetched
     * by PIP.
     * @param root root of the policies tree to be evaluated
     */
    virtual Effect evaluatePolicies(const TreeNode * root);

    enum updateErrors
    {
        POLICY_PARSING_SUCCESS = 0,
        POLICY_FILE_ERROR = 1,
        PARSER_CREATION_ERROR,
        POLICY_PARSING_ERROR
    };
  private:
    AttributeSet m_attributeSet;

    TreeNode * m_root;
    Combiner * m_combiner;
    AsyncVerdictResultListener * m_verdictListener;
    PolicyInformationPoint * m_pip;
    // Required by unittests.
    std::string m_currentPolicyFile;

    /**
     * Method used to extract attributes from subtree defined by PolicySet
     * @param root original TreeStructure root node
     * @param newRoot copy of TreeStructure containing only policies that matches current request
     *
     */
    void extractAttributesFromSubtree(const TreeNode *root);

    /**
     * Method used to extract attributes from Tree Structure
     * @return pointer to set of attributes needed to evaluate current request
     * @return if extraction has been successful
     * TODO return reducte tree structure
     * TODO change comments
     */
    bool extractAttributesFromRules(const TreeNode *);

    /**
     * Extracts attributes from target of a given policy that are required to be fetched by PIP
     */
    void extractTargetAttributes(const Policy *policy);
    bool extractAttributes();

    OptionalPolicyResult getPolicyForRequestInternal(bool fromCacheOnly);
    PolicyResult effectToPolicyResult(Effect effect);
  public:
    PolicyEvaluator(PolicyInformationPoint * pip) :
        m_root(NULL),
        m_combiner(new CombinerImpl()),
        m_verdictListener(NULL),
        m_pip(pip)
    {
    }

    bool extractAttributesTest()
    {
        m_attributeSet.clear();
        if (!extractAttributes()) {
            LogInfo("Warnign attribute set cannot be extracted. Returning Deny");
            return true;
        }

        return extractAttributes();
    }

    AttributeSet * getAttributeSet()
    {
        return &m_attributeSet;
    }

    virtual bool initPDP();
    virtual ~PolicyEvaluator();
    virtual PolicyResult getPolicyForRequest(const Request &request);
    virtual OptionalPolicyResult getPolicyForRequestFromCache(
        const Request &request);
    virtual OptionalPolicyResult getPolicyForRequest(const Request &request,
                                                     bool fromCacheOnly);
    bool fillAttributeWithPolicy();

    virtual int updatePolicy(const char *);
    // Required by unittests.
    // It's used to check environment before each unittest.
    std::string getCurrentPolicy();
};

#endif  //_POLICYEVALUATOR_H
