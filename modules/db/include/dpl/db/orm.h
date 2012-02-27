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
 * @file        orm.h
 * @author      Bartosz Janiak (b.janiak@samsung.com)
 * @version     1.0
 * @brief       DPL-ORM: Object-relational mapping for sqlite database, written on top of DPL.
 */

#include <cstdlib>
#include <cstdio>
#include <string>
#include <typeinfo>
#include <utility>

#include <dpl/db/sql_connection.h>
#include <dpl/db/orm_interface.h>
#include <dpl/string.h>
#include <dpl/optional.h>
#include <dpl/shared_ptr.h>
#include <dpl/type_list.h>
#include <dpl/assert.h>

#ifndef DPL_ORM_H
#define DPL_ORM_H

namespace DPL {
namespace DB {
namespace ORM {

//TODO move to type utils
#define DPL_CHECK_TYPE_INSTANTIABILITY(type) \
    { \
        type _ignored_; \
        (void)_ignored_; \
    }

typedef size_t ColumnIndex;
typedef size_t ArgumentIndex;
typedef DPL::Optional<DPL::String> OptionalString;
typedef DPL::Optional<int> OptionalInteger;
typedef DPL::DB::SqlConnection::DataCommand DataCommand;

namespace RelationTypes {
    extern const char Equal[];
    extern const char LessThan[];
    extern const char And[];
    extern const char Or[];
    extern const char Is[];
    //TODO define more relation types
}

namespace DataCommandUtils {
    //TODO move to DPL::DataCommand?
    void BindArgument(DataCommand *command, ArgumentIndex index, int argument);
    void BindArgument(DataCommand *command, ArgumentIndex index, const OptionalInteger& argument);
    void BindArgument(DataCommand *command, ArgumentIndex index, const DPL::String& argument);
    void BindArgument(DataCommand *command, ArgumentIndex index, const OptionalString& argument);
}
class Expression {
public:
    virtual ~Expression() {}
    virtual std::string GetString() const = 0;
    virtual ArgumentIndex BindTo(DataCommand *command, ArgumentIndex index) = 0;
};

typedef DPL::SharedPtr<Expression> ExpressionPtr;

template<const char* Operator, typename LeftExpression, typename RightExpression>
class BinaryExpression : public Expression {
protected:
    LeftExpression  m_leftExpression;
    RightExpression m_rightExpression;
    bool            m_outerParenthesis;
public:
    BinaryExpression(const LeftExpression& leftExpression, const RightExpression& rightExpression, bool outerParenthesis = true) :
        m_leftExpression(leftExpression),
        m_rightExpression(rightExpression),
        m_outerParenthesis(outerParenthesis)
    {}

    virtual std::string GetString() const
    {
        return  (m_outerParenthesis ? "( " : " " ) +
                 m_leftExpression.GetString() + " " + Operator + " " + m_rightExpression.GetString() +
                (m_outerParenthesis ? " )" : " " ) ;
    }

    virtual ArgumentIndex BindTo(DataCommand *command, ArgumentIndex index)
    {
        index = m_leftExpression.BindTo(command, index);
        return  m_rightExpression.BindTo(command, index);
    }

    template<typename TableDefinition>
    struct ValidForTable {
        typedef std::pair<typename LeftExpression ::template ValidForTable<TableDefinition>::Yes ,
                          typename RightExpression::template ValidForTable<TableDefinition>::Yes >
                Yes;
    };
};

template<typename LeftExpression, typename RightExpression>
BinaryExpression<RelationTypes::And, LeftExpression, RightExpression>
    And(const LeftExpression& leftExpression, const RightExpression& rightExpression)
{
    return BinaryExpression<RelationTypes::And, LeftExpression, RightExpression>
            (leftExpression, rightExpression);
}

template<typename ArgumentType>
class ExpressionWithArgument : public Expression {
protected:
    ArgumentType argument;

public:
    explicit ExpressionWithArgument(const ArgumentType& _argument) : argument(_argument) {}

    virtual ArgumentIndex BindTo(DataCommand *command, ArgumentIndex index)
    {
        DataCommandUtils::BindArgument(command, index, argument);
        return index + 1;
    }
};


template<typename ColumnData, const char* Relation>
class Compare : public ExpressionWithArgument<typename ColumnData::ColumnType> {
public:
    explicit Compare(typename ColumnData::ColumnType column) :
        ExpressionWithArgument<typename ColumnData::ColumnType>(column)
    {}

    virtual std::string GetString() const
    {
        std::string statement;
        statement += ColumnData::GetColumnName();
        statement += " ";
        statement += Relation;
        statement += " ?";
        return statement;
    }

    template<typename TableDefinition>
    struct ValidForTable {
        typedef typename TableDefinition::ColumnList::template Contains<ColumnData> Yes;
    };
};
#define ORM_DEFINE_COMPARE_EXPRESSION(name, relationType)                      \
    template<typename ColumnData>                                              \
    class name : public Compare<ColumnData, RelationTypes::relationType> {     \
    public:                                                                    \
        name(typename ColumnData::ColumnType column) :                         \
            Compare<ColumnData, RelationTypes::relationType>(column)           \
        {}                                                                     \
    };

ORM_DEFINE_COMPARE_EXPRESSION(Equals, Equal)
ORM_DEFINE_COMPARE_EXPRESSION(Is, Is)

template<typename ColumnType>
ColumnType GetColumnFromCommand(ColumnIndex columnIndex, DataCommand *command);

template<typename ColumnList, typename Row>
class FillRowUtil {
public:
    static void FillRow(Row& row, ColumnIndex columnIndex, DataCommand *command)
    {
        typename ColumnList::Head::ColumnType rowField;
        rowField = GetColumnFromCommand<typename ColumnList::Head::ColumnType>(columnIndex, command);
        ColumnList::Head::SetRowField(row, rowField);
        FillRowUtil<typename ColumnList::Tail, Row>::FillRow(row, columnIndex + 1, command);
    }
};

template<typename Row>
class FillRowUtil<DPL::TypeListGuard, Row> {
public:
    static void FillRow(Row&, ColumnIndex, DataCommand *)
    { /* do nothing, we're past the last element of column list */ }
};

class Exception {
public:
    DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
    DECLARE_EXCEPTION_TYPE(Base, SelectReuseWithDifferentQuerySignature)
    DECLARE_EXCEPTION_TYPE(Base, RowFieldNotInitialized)
    DECLARE_EXCEPTION_TYPE(Base, EmptyUpdateStatement)
};

template<typename TableDefinition>
class Query
{
protected:
    explicit Query(IOrmInterface* interface) :
        m_interface(interface),
        m_command(NULL)
    {
    }

    virtual ~Query()
    {
        if (m_command == NULL)
            return;

        TableDefinition::FreeTableDataCommand(m_command, m_interface);
    }

    IOrmInterface* m_interface;
    DataCommand *m_command;
    std::string m_commandString;
    ArgumentIndex m_bindArgumentIndex;
};

template<typename TableDefinition>
class QueryWithWhereClause : public Query<TableDefinition>
{
protected:
    ExpressionPtr m_whereExpression;

    void Prepare()
    {
        if ( !!m_whereExpression )
        {
            this->m_commandString += " WHERE ";
            this->m_commandString += m_whereExpression->GetString();
        }
    }

    void Bind()
    {
        if ( !!m_whereExpression )
        {
            this->m_bindArgumentIndex = m_whereExpression->BindTo(
                this->m_command, this->m_bindArgumentIndex);
        }
    }

public:
    explicit QueryWithWhereClause(IOrmInterface* interface) :
        Query<TableDefinition>(interface)
    {
    }

    template<typename Expression>
    void Where(const Expression& expression)
    {
        DPL_CHECK_TYPE_INSTANTIABILITY(typename Expression::template ValidForTable<TableDefinition>::Yes);
        if ( !!m_whereExpression && ( typeid(Expression) != typeid(*m_whereExpression) ) )
        {
            std::ostringstream str;
            str << "Current ORM implementation doesn't allow to reuse Select"
                    " instance with different query signature (particularly "
                    "WHERE on different column).\n";
            str << "Query: ";
            str << this->m_commandString;
            ThrowMsg(Exception::SelectReuseWithDifferentQuerySignature,
                str.str());
        }
        //TODO maybe don't make a copy here but just generate the string part of the query.
        m_whereExpression.Reset(new Expression(expression));
    }

};

template<typename TableDefinition>
class Delete : public QueryWithWhereClause<TableDefinition>
{
protected:
    void Prepare()
    {
        if ( !this->m_command)
        {
            this->m_commandString  = "DELETE FROM ";
            this->m_commandString += TableDefinition::GetName();

            QueryWithWhereClause<TableDefinition>::Prepare();

            this->m_command = TableDefinition::AllocTableDataCommand(
                    this->m_commandString.c_str(),
                    Query<TableDefinition>::m_interface);
            LogPedantic("Prepared SQL command " << this->m_commandString);
        }
    }

    void Bind()
    {
        this->m_bindArgumentIndex = 1;
        QueryWithWhereClause<TableDefinition>::Bind();
    }

public:
    explicit Delete(IOrmInterface *interface = NULL) :
        QueryWithWhereClause<TableDefinition>(interface)
    {
    }

    void Execute()
    {
        Prepare();
        Bind();
        this->m_command->Step();
        this->m_command->Reset();
    }
};

template<typename TableDefinition>
class Insert : public Query<TableDefinition>
{
public:
    typedef typename TableDefinition::Row Row;
    typedef DPL::DB::SqlConnection::RowID RowID;

protected:
    DPL::Optional<std::string> m_orClause;
    Row m_row;

    class PrepareVisitor {
    public:
        std::string m_columnNames;
        std::string m_values;

        template<typename ColumnType>
        void Visit(const char* name, const ColumnType&, bool isSet)
        {
            if ( isSet )
            {
                if ( !m_columnNames.empty() )
                {
                    m_columnNames += ", ";
                    m_values += ", ";
                }
                m_columnNames += name;
                m_values += "?";
            }
        }
    };

    void Prepare()
    {
        if ( !this->m_command )
        {
            this->m_commandString = "INSERT ";
            if ( !!m_orClause )
            {
                this->m_commandString += " OR " + *m_orClause + " ";
            }
            this->m_commandString += "INTO ";
            this->m_commandString += TableDefinition::GetName();

            PrepareVisitor visitor;
            m_row.VisitColumns(visitor);

            this->m_commandString += " ( " + visitor.m_columnNames + " ) ";
            this->m_commandString += "VALUES ( " + visitor.m_values + " )";

            LogPedantic("Prepared SQL command " << this->m_commandString);
            this->m_command = TableDefinition::AllocTableDataCommand(
                this->m_commandString.c_str(),
                Query<TableDefinition>::m_interface);
        }
    }

    class BindVisitor {
    private:
        DataCommand *m_command;
        ArgumentIndex m_bindArgumentIndex;
    public:
        BindVisitor(DataCommand *command) :
            m_command(command),
            m_bindArgumentIndex(1)
        {}

        template<typename ColumnType>
        void Visit(const char*, const ColumnType& value, bool isSet)
        {
            if ( isSet )
            {
                DataCommandUtils::BindArgument(m_command, m_bindArgumentIndex, value);
                m_bindArgumentIndex++;
            }
        }
    };

    void Bind()
    {
        BindVisitor visitor(this->m_command);
        m_row.VisitColumns(visitor);
    }

public:
    explicit Insert(
            IOrmInterface* interface = NULL,
            const DPL::Optional<std::string>& orClause = DPL::Optional<std::string>::Null) :
        Query<TableDefinition>(interface),
        m_orClause(orClause)
    {
    }

    void Values(const Row& row)
    {
        if ( this->m_command )
        {
            if ( !row.IsSignatureMatching(m_row) )
            {
                ThrowMsg(Exception::SelectReuseWithDifferentQuerySignature,
                    "Current ORM implementation doesn't allow to reuse Insert instance "
                    "with different query signature.");
            }
        }
        m_row = row;
    }

    RowID Execute()
    {
        Prepare();
        Bind();
        this->m_command->Step();

        RowID result = TableDefinition::GetLastInsertRowID(
            Query<TableDefinition>::m_interface);

        this->m_command->Reset();
        return result;
    }
};

template<typename TableDefinition>
class Select : public QueryWithWhereClause<TableDefinition>
{
public:
    typedef typename TableDefinition::ColumnList       ColumnList;
    typedef typename TableDefinition::Row              Row;

    typedef std::list<Row>                             RowList;
protected:
    DPL::Optional<std::string> m_orderBy;
    bool                       m_distinctResults;

    void Prepare(const char* selectColumnName)
    {
        if ( !this->m_command )
        {
            this->m_commandString  = "SELECT ";
            if (m_distinctResults)
                this->m_commandString += "DISTINCT ";
            this->m_commandString += selectColumnName;
            this->m_commandString += " FROM ";
            this->m_commandString += TableDefinition::GetName();

            QueryWithWhereClause<TableDefinition>::Prepare();

            if ( !m_orderBy.IsNull() )
            {
                this->m_commandString += " ORDER BY " + *m_orderBy;
            }

            this->m_command = TableDefinition::AllocTableDataCommand(
                this->m_commandString.c_str(),
                Query<TableDefinition>::m_interface);

            LogPedantic("Prepared SQL command " << this->m_commandString);
        }
    }

    void Bind()
    {
        this->m_bindArgumentIndex = 1;
        QueryWithWhereClause<TableDefinition>::Bind();
    }

    template<typename ColumnType>
    ColumnType GetColumn(ColumnIndex columnIndex)
    {
        return GetColumnFromCommand<ColumnType>(columnIndex, this->m_command);
    }

    Row GetRow()
    {
        Row row;
        FillRowUtil<ColumnList, Row>::FillRow(row, 0, this->m_command);
        return row;
    }

public:

    explicit Select(IOrmInterface *interface = NULL) :
        QueryWithWhereClause<TableDefinition>(interface),
        m_distinctResults(false)
    {
    }

    void Distinct()
    {
        m_distinctResults = true;
    }

    void OrderBy(const std::string& orderBy)
    {
        m_orderBy = orderBy;
    }

    template<typename ColumnData>
    typename ColumnData::ColumnType GetSingleValue()
    {
        Prepare(ColumnData::GetColumnName());
        Bind();
        this->m_command->Step();

        typename ColumnData::ColumnType result =
            GetColumn<typename ColumnData::ColumnType>(0);

        this->m_command->Reset();
        return result;
    }

    //TODO return range - pair of custom iterators
    template<typename ColumnData>
    std::list<typename ColumnData::ColumnType> GetValueList()
    {
        Prepare(ColumnData::GetColumnName());
        Bind();

        std::list<typename ColumnData::ColumnType> resultList;

        while (this->m_command->Step())
            resultList.push_back(GetColumn<typename ColumnData::ColumnType>(0));

        this->m_command->Reset();
        return resultList;
    }

    Row GetSingleRow()
    {
        Prepare("*");
        Bind();
        this->m_command->Step();

        Row result = GetRow();

        this->m_command->Reset();
        return result;
    }

    //TODO return range - pair of custom iterators
    RowList GetRowList()
    {
        Prepare("*");
        Bind();

        RowList resultList;

        while (this->m_command->Step())
            resultList.push_back(GetRow());

        this->m_command->Reset();
        return resultList;
    }
};

template<typename TableDefinition>
class Update : public QueryWithWhereClause<TableDefinition> {
public:
    typedef typename TableDefinition::Row Row;

protected:
    DPL::Optional<std::string> m_orClause;
    Row m_row;

    class PrepareVisitor {
    public:
        std::string m_setExpressions;

        template<typename ColumnType>
        void Visit(const char* name, const ColumnType&, bool isSet)
        {
            if ( isSet )
            {
                if ( !m_setExpressions.empty() )
                {
                    m_setExpressions += ", ";
                }
                m_setExpressions += name;
                m_setExpressions += " = ";
                m_setExpressions += "?";
            }
        }
    };

    void Prepare()
    {
        if ( !this->m_command )
        {
            this->m_commandString = "UPDATE ";
            if ( !!m_orClause )
            {
                this->m_commandString += " OR " + *m_orClause + " ";
            }
            this->m_commandString += TableDefinition::GetName();
            this->m_commandString += " SET ";

            // got through row columns and values
            PrepareVisitor visitor;
            m_row.VisitColumns(visitor);

            if(visitor.m_setExpressions.empty())
            {
                ThrowMsg(Exception::EmptyUpdateStatement, "No SET expressions in update statement");
            }

            this->m_commandString += visitor.m_setExpressions;

            // where
            QueryWithWhereClause<TableDefinition>::Prepare();

            this->m_command = TableDefinition::AllocTableDataCommand(
                    this->m_commandString.c_str(),
                    Query<TableDefinition>::m_interface);
            LogPedantic("Prepared SQL command " << this->m_commandString);
        }
    }

    class BindVisitor {
    private:
        DataCommand *m_command;

    public:
        ArgumentIndex m_bindArgumentIndex;

        BindVisitor(DataCommand *command) :
            m_command(command),
            m_bindArgumentIndex(1)
        {}

        template<typename ColumnType>
        void Visit(const char*, const ColumnType& value, bool isSet)
        {
            if ( isSet )
            {
                DataCommandUtils::BindArgument(m_command, m_bindArgumentIndex, value);
                m_bindArgumentIndex++;
            }
        }
    };

    void Bind()
    {
        BindVisitor visitor(this->m_command);
        m_row.VisitColumns(visitor);

        this->m_bindArgumentIndex = visitor.m_bindArgumentIndex;
        QueryWithWhereClause<TableDefinition>::Bind();
    }


public:
    explicit Update(IOrmInterface *interface = NULL,
                    const DPL::Optional<std::string>& orClause = DPL::Optional<std::string>::Null) :
        QueryWithWhereClause<TableDefinition>(interface),
        m_orClause(orClause)
    {
    }

    void Values(const Row& row)
    {
        if ( this->m_command )
        {
            if ( !row.IsSignatureMatching(m_row) )
            {
                ThrowMsg(Exception::SelectReuseWithDifferentQuerySignature,
                    "Current ORM implementation doesn't allow to reuse Update instance "
                    "with different query signature.");
            }
        }
        m_row = row;
    }

    void Execute()
    {
        Prepare();
        Bind();
        this->m_command->Step();
        this->m_command->Reset();
    }
};

} //namespace ORM
} //namespace DB
} //namespace DPL

#endif // DPL_ORM_H
