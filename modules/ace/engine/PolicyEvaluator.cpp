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
//  @ File Name : PolicyEvaluator.cpp
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#include <dpl/assert.h>
#include <dpl/foreach.h>

#include <dpl/ace/PolicyEvaluator.h>
#include <dpl/ace/TreeNode.h>
#include <dpl/ace/Policy.h>
#include <dpl/ace/Rule.h>
#include <dpl/ace/Attribute.h>
//#include <dpl/ace/"UserDecisionTestStub.h>
#include <dpl/ace/SettingsLogic.h>
#include <dpl/ace-dao-rw/AceDAO.h>
#include <dpl/ace-dao-ro/PreferenceTypes.h>
#include <dpl/ace/parser.h>

using namespace AceDB;

PolicyEvaluator::~PolicyEvaluator()
{
    delete m_combiner;
}

bool PolicyEvaluator::initPDP()
{
    ConfigurationManager * configMgr = ConfigurationManager::getInstance();
    if (configMgr == NULL) {
        LogError("ACE fatal error: failed to create configuration manager");
        return false;
    }

    Parser parser;
    m_root = parser.parse(
            configMgr->getFullPathToCurrentPolicyFile(),
            configMgr->getFullPathToCurrentPolicyXMLSchema());

    if (NULL == m_root) {
        LogError("ACE fatal error: cannot parse XML file");
    }

    return true;
}

bool PolicyEvaluator::fillAttributeWithPolicy()
{
    if (m_attributeSet.empty()) {
        if (!extractAttributes()) {
            LogInfo(
                "Warning attribute set cannot be extracted. Returning Deny");
            return false;
        }
    } else {
        LogDebug("Required attribute set found in database");
    }
    return true;
}

PolicyResult PolicyEvaluator::effectToPolicyResult(Effect effect){
    if (Effect::Deny == effect) {
        return PolicyEffect::DENY;
    }
    if (Effect::Undetermined == effect) {
        return PolicyResult::Value::UNDETERMINED;
    }
    if (Effect::PromptOneShot == effect) {
        return PolicyEffect::PROMPT_ONESHOT;
    }
    if (Effect::PromptSession == effect) {
        return PolicyEffect::PROMPT_SESSION;
    }
    if (Effect::PromptBlanket == effect) {
        return PolicyEffect::PROMPT_BLANKET;
    }
    if (Effect::Permit == effect) {
        return PolicyEffect::PERMIT;
    }
    if (Effect::Inapplicable == effect) {
        return PolicyDecision::Value::NOT_APPLICABLE;
    }
    return PolicyEffect::DENY;
}

OptionalPolicyResult PolicyEvaluator::getPolicyForRequestInternal(bool fromCacheOnly)
{
    //ADD_PROFILING_POINT("Search cached verdict in database", "start");

    OptionalPolicyResult result = AceDAO::getPolicyResult(
        m_attributeSet);

    //ADD_PROFILING_POINT("Search cached verdict in database", "stop");

    if (fromCacheOnly || !result.IsNull()) {
        return result;
    }

    //ADD_PROFILING_POINT("EvaluatePolicy", "start");

    Effect policyEffect = evaluatePolicies(m_root);

    //ADD_PROFILING_POINT("EvaluatePolicy", "stop");

    LogDebug("Policy effect is: " << toString(policyEffect));

    result = effectToPolicyResult(policyEffect);

    AceDAO::setPolicyResult(this->m_attributeSet, *result);
    return result;
}

// +----------------+---------+---------+------+--------+
// |\User setting   | PERMIT  | PROMPT* | DENY | DEF    |
// |      \         |         |         |      |        |
// |Policy result\  |         |         |      |        |
// |----------------+---------+---------+------+--------+
// |PERMIT          | PERMIT  | PROMPT* | DENY | PERMIT |
// |----------------+---------+---------+------+--------+
// |PROMPT*         | PROMPT* | PR MIN  | DENY | PROMPT*|
// |----------------+---------+---------+------+--------+
// |DENY            | DENY    | DENY    | DENY | DENY   |
// |----------------+---------+---------+------+--------+
// |UNDETERMIND     | UNDET   | UNDET   | DENY | UNDET  |
// |----------------+---------+---------+------+--------+
// |NOT_AP          | PEMIT   | PROMPT* | DENY | NOT_AP |
// +----------------+---------+---------+------+--------+

static PolicyResult getMostRestrict(PreferenceTypes globalPreference,
                                    const PolicyResult &policyResult)
{
        if (globalPreference == PreferenceTypes::PREFERENCE_PERMIT &&
            policyResult == PolicyEffect::PERMIT)
        {
            return PolicyEffect::PERMIT;
        }

        if (globalPreference == PreferenceTypes::PREFERENCE_DENY ||
            policyResult == PolicyEffect::DENY)
        {
            return PolicyEffect::DENY;
        }

        if (policyResult == PolicyResult::UNDETERMINED) {
            return PolicyResult::UNDETERMINED;
        }

        if (globalPreference == PreferenceTypes::PREFERENCE_DEFAULT)
        {
            return policyResult;
        }

        if (globalPreference == PreferenceTypes::PREFERENCE_ONE_SHOT_PROMPT ||
            policyResult == PolicyEffect::PROMPT_ONESHOT)
        {
            return PolicyEffect::PROMPT_ONESHOT;
        }

        if (globalPreference == PreferenceTypes::PREFERENCE_SESSION_PROMPT ||
            policyResult == PolicyEffect::PROMPT_SESSION)
        {
            return PolicyEffect::PROMPT_SESSION;
        }

        if (globalPreference == PreferenceTypes::PREFERENCE_BLANKET_PROMPT ||
            policyResult == PolicyEffect::PROMPT_BLANKET)
        {
            return PolicyEffect::PROMPT_BLANKET;
        }

        return PolicyEffect::PERMIT;
}

OptionalPolicyResult PolicyEvaluator::getPolicyForRequestFromCache(
    const Request &request)
{
    return getPolicyForRequest(request, true);
}

PolicyResult PolicyEvaluator::getPolicyForRequest(const Request &request)
{
    auto result = this->getPolicyForRequest(request, false);
    Assert(!result.IsNull() &&
           "Policy always has to be evaluated to valid state");
    return *result;
}

OptionalPolicyResult PolicyEvaluator::getPolicyForRequest(
    const Request &request,
    bool fromCacheOnly)
{
    //ADD_PROFILING_POINT("getPolicyForRequest", "start");
    m_attributeSet.clear();

    try {
        // Check which attributes should be used
        // memory alocated, free in destructor
        //ADD_PROFILING_POINT("getAttributes", "start");
        AceDB::AceDAO::getAttributes(&m_attributeSet);
        //ADD_PROFILING_POINT("getAttributes", "stop");

        // If attributes can't be resolved then check the policy
        if (!fillAttributeWithPolicy()) {
            //ADD_PROFILING_POINT("getPolicyForRequest", "stop");
            return OptionalPolicyResult(PolicyEffect::DENY);
        }

        //ADD_PROFILING_POINT("getAttributesValues", "start");
        m_pip->getAttributesValues(&request, &m_attributeSet);
        //ADD_PROFILING_POINT("getAttributesValues", "stop");
        LogDebug("==== Attributes set by PIP ====");
        printAttributes(m_attributeSet);
        LogDebug("==== End of attributes set by PIP ====");

        OptionalPolicyResult policyResult =
            getPolicyForRequestInternal(fromCacheOnly);

        LogDebug("==== getPolicyForRequestInternal result (PolicyResult): "
                 << policyResult << "=====");

        if (policyResult.IsNull()) {
            if (fromCacheOnly) {
                return OptionalPolicyResult::Null;
            } else {
                LogError("Policy evaluated to NULL value");
                Assert(false && "Policy evaluated to NULL value");
                return OptionalPolicyResult::Null;
            }
        }

        PreferenceTypes globalPreference =
            SettingsLogic::findGlobalUserSettings(request);

        auto ret = getMostRestrict(globalPreference, *policyResult);
        //ADD_PROFILING_POINT("getPolicyForRequest", "stop");
        return OptionalPolicyResult(ret);

    } catch (AceDB::AceDAO::Exception::DatabaseError &e) {
        LogError("Database error");
        DPL::Exception::DisplayKnownException(e);
        //ADD_PROFILING_POINT("getPolicyForRequest", "stop");
        return OptionalPolicyResult(PolicyEffect::DENY);
    }
}


bool PolicyEvaluator::extractAttributes()
{
    if (m_root == NULL) {
        return false;
    }

    //We check if root target matches. In general the root's target should be empty
    //Otherwise it would have to have all the subjects available specified
    //But just to be on the safe side ( and for tests) add this checking
    const Policy * policy = dynamic_cast<const Policy *>(m_root->getElement());
            Assert(
            policy != NULL &&
                    "Policy element has been null while attribute extracting");

    extractTargetAttributes(policy);
    extractAttributesFromSubtree(m_root); //Enter recursion

    return true;
}

void PolicyEvaluator::extractTargetAttributes(const Policy *policy)
{
    std::list<const Subject *>::const_iterator it =
        policy->getSubjects()->begin();
    for (; it != policy->getSubjects()->end(); ++it)
    {
        const std::list<Attribute> & attrList = (*it)->getTargetAttributes();
        FOREACH(it2, attrList)
        {
            BaseAttributePtr attr(new Attribute((*it2).getName(),
                    (*it2).getMatchFunction(), (*it2).getType()));
            m_attributeSet.insert(attr);
        }
    }
}

/**
 *
 * @param *root - the root of the original (full) subtree of politics
 * @param *newRoot - the pointer to the root of the copy (reduced) subtree of politics
 */
void PolicyEvaluator::extractAttributesFromSubtree(const TreeNode *root)
{
    const ChildrenSet & children = root->getChildrenSet();

    for (
    std::list<TreeNode *>::const_iterator it = children.begin();
            it != children.end();
            ++it
            ) {
        TreeNode * node = *it;
        if (node->getTypeID() != TreeNode::Policy && node->getTypeID() !=
                TreeNode::PolicySet) {
            //It is not a policy so we may be sure that we have already checked that SubjectId matches
            //Add new node to new tree and extract attributes

            extractAttributesFromRules(node);
        } else { //TreeNode is a Policy or PolicySet
            const Policy * policy =
                dynamic_cast<const Policy *>(node->getElement());
            //We will be needing also the attributes from target
            if (policy) {
                extractTargetAttributes(policy);
            } else {
                LogError(" extractAttributesFromSubtree policy=NULL");
            }
            //Enter recursion
            extractAttributesFromSubtree(node);
        }
    }
}

bool PolicyEvaluator::extractAttributesFromRules(const TreeNode *root)
{
    Assert(
            root->getTypeID() == TreeNode::Rule
                    &&
                    "Tree structure, extracting attributes from node that is not a rule");
    Rule * rule = dynamic_cast<Rule *>(root->getElement());
            Assert(rule != NULL);
    //Get attributes from rule
    rule->getAttributes(&m_attributeSet);

    //[CR] consider returned value, because its added only to eliminate errors
    return true;
}

Effect PolicyEvaluator::evaluatePolicies(const TreeNode * root)
{
    if (root == NULL) {
        LogInfo("Error: policy tree doesn't exist. "
                "Probably xml file is missing");
        return Deny;
    }

    if (m_attributeSet.empty()) {
        LogInfo("Warning: evaluatePolicies: attribute set was empty");
    }
    m_combiner->setAttributeSet(&m_attributeSet);
    return m_combiner->combinePolicies(root);
}

int PolicyEvaluator::updatePolicy(const char* newPolicy)
{
    ConfigurationManager* configMgr = ConfigurationManager::getInstance();

    if (NULL == configMgr) {
        LogError("ACE fatal error: failed to create configuration manager");
        return POLICY_PARSING_ERROR;
    }

    int result = POLICY_PARSING_SUCCESS;
    if (newPolicy == NULL) {
        LogError("Policy Update: incorrect policy name");
        return POLICY_FILE_ERROR;
    }
    LogDebug("Starting update policy: " << newPolicy);

    Parser parser;
    TreeNode *backup = m_root;

    m_root = parser.parse(
            newPolicy,
            configMgr->getFullPathToCurrentPolicyXMLSchema());

    if (NULL == m_root) {
        m_root = backup;
        LogError("Policy Update: corrupted policy file");
        result = POLICY_PARSING_ERROR;
    } else {
        m_currentPolicyFile = newPolicy;
        backup->releaseResources();
        LogInfo("Policy Update: successful.");
        try {
            AceDAO::resetDatabase();
        } catch (AceDAO::Exception::DatabaseError &e) {
        }
    }
    return result;
}

std::string PolicyEvaluator::getCurrentPolicy(){
    return m_currentPolicyFile;
}

const char * toString(Validity validity)
{
    switch (validity) {
    case Validity::ONCE:
        return "Once";
        break;
    case Validity::SESSION:
        return "Session";
    case Validity::ALWAYS:
        return "Always";
    default:
        return "WRONG VALIDITY";
    }
}

const char * toString(Verdict verdict)
{
    switch (verdict) {
    case Verdict::VERDICT_PERMIT:
        return "Permit";
    case Verdict::VERDICT_DENY:
        return "Deny";
    case Verdict::VERDICT_INAPPLICABLE:
        return "Inapplicable";
    case Verdict::VERDICT_UNKNOWN:
        return "Unknown";
    case Verdict::VERDICT_UNDETERMINED:
        return "Undetermined";
    case Verdict::VERDICT_ERROR:
        return "Error";
    case Verdict::VERDICT_ASYNC:
        return "Async";
    default:
        return "Wrong verdict value";
    }
}
