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
 * @file        orm_generator.h
 * @author      Bartosz Janiak (b.janiak@samsung.com)
 * @version     1.0
 * @brief       Macro definitions for generating the DPL-ORM table definitions from database definitions.
 */

#ifndef ORM_GENERATOR_DATABASE_NAME
#error You need to define database name in ORM_GENERATOR_DATABASE_NAME define before you include orm_generator.h file
#endif

#include <dpl/db/orm_interface.h>

#define ORM_GENERATOR_DATABASE_NAME_LOCAL <ORM_GENERATOR_DATABASE_NAME>

#ifdef DPL_ORM_GENERATOR_H
#warning orm_generator.h is included multiply times. Make sure it has different ORM_GENERATOR_DATABASE_NAME set.
#endif

#define DPL_ORM_GENERATOR_H


#include <dpl/string.h>
#include <dpl/optional.h>
#include <dpl/type_list.h>
#include <dpl/db/sql_connection.h>
#include <dpl/db/orm.h>
#include <dpl/assert.h>
#include <string>

/*

This is true only when exactly one db is available.

#if (defined DECLARE_COLUMN) || (defined INT) || (defined TINYINT) ||               \
    (defined INTEGER) || (defined BIGINT) || defined(VARCHAR) || defined(TEXT) ||   \
    (defined SQL) || (defined TABLE_CONSTRAINTS) || (defined OPTIONAL) ||           \
    (defined DATABASE_START) || (defined DATABASE_END) || (defined CREATE_TABLE) || \
    (defined COLUMN) || (defined COLUMN_NOT_NULL) || (defined CREATE_TABLE_END)

#error  This file temporarily defines many macros with generic names. To avoid name clash please include \
        this file as early as possible. If this is not possible please report this problem to DPL developers.

#endif
*/

namespace DPL {
namespace DB {
namespace ORM {

// Global macros

#define STRINGIFY(s) _str(s)
#define _str(s) #s
#define DECLARE_COLUMN(FIELD, TYPE) \
    struct FIELD { \
        typedef TYPE ColumnType; \
        static const char* GetColumnName() { return STRINGIFY(FIELD); } \
        static void SetRowField(Row& row, const TYPE& value) { row.Set_##FIELD(value);} \
    };

#define INT         int
#define TINYINT     int
#define INTEGER     int  //TODO: should be long long?
#define BIGINT      int  //TODO: should be long long?
#define VARCHAR(x)  DPL::String
#define TEXT        DPL::String

#define SQL(args...)
#define TABLE_CONSTRAINTS(args...)
#define OPTIONAL(type) DPL::Optional< type >
#define DATABASE_START(db_name)                                 \
    namespace db_name                                           \
    {                                                           \
        class ScopedTransaction                                 \
        {                                                       \
            bool m_commited;                                    \
            IOrmInterface *m_interface;                         \
                                                                \
        public:                                                 \
            ScopedTransaction(IOrmInterface *interface) :       \
                m_commited(false),                              \
                m_interface(interface)                          \
            {                                                   \
                Assert(interface != NULL);                      \
                m_interface->TransactionBegin();                \
            }                                                   \
                                                                \
            ~ScopedTransaction()                                \
            {                                                   \
                if (!m_commited)                                \
                    m_interface->TransactionRollback();         \
            }                                                   \
                                                                \
            void Commit()                                       \
            {                                                   \
                m_interface->TransactionCommit();               \
                m_commited = true;                              \
            }                                                   \
        };

#define DATABASE_END() }

// RowBase ostream operator<< declaration

#define CREATE_TABLE(name) \
    namespace name {                                                            \
        class RowBase;                                                          \
        inline std::ostream& operator<<(std::ostream& ostr, const RowBase& row); \
    }
#define COLUMN_NOT_NULL(name, type, args...)
#define COLUMN(name, type, args...)
#define CREATE_TABLE_END()

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

#undef DATABASE_START
#define DATABASE_START(db_name) namespace db_name {

// RowBase class

#define CREATE_TABLE(name) namespace name { class RowBase {                 \
   public: friend std::ostream& operator<<(std::ostream&, const RowBase&);
#define COLUMN_NOT_NULL(name, type, args...)                                \
        protected: type name; bool m_##name##_set;                          \
        public:  void Set_##name(const type& value) {                       \
                     m_##name##_set = true;                                 \
                     this->name = value;                                     \
        }                                                                   \
        public:  type Get_##name() const {                                  \
                     if ( !m_##name##_set ) {                               \
                        ThrowMsg(Exception::RowFieldNotInitialized,         \
                        "You tried to read a row field that hasn't been set yet."); \
                     }                                                      \
                     return name;                                           \
        }

#define COLUMN(name, type, args...)                                         \
        protected: OPTIONAL(type) name; bool m_##name##_set;                \
        public:  void Set_##name(const OPTIONAL(type)& value) {             \
                     m_##name##_set = true;                                 \
                     this->name = value;                                     \
        }                                                                   \
        public:  OPTIONAL(type) Get_##name() const {                        \
                     if ( !m_##name##_set ) {                               \
                        ThrowMsg(Exception::RowFieldNotInitialized,         \
                        "You tried to read a row field that hasn't been set yet."); \
                     }                                                      \
                     return name;                                           \
        }
#define CREATE_TABLE_END() }; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// RowBase ostream operator<<

#define CREATE_TABLE(name) std::ostream& name::operator<<(std::ostream& ostr, const RowBase& row) { using ::operator<< ; ostr << STRINGIFY(name) << " (";
#define COLUMN_NOT_NULL(name, type, args...) ostr << " '" << row.name << "'" ;
#define COLUMN(name, type, args...)          ostr << " '" << row.name << "'" ;
#define CREATE_TABLE_END() ostr << " )" ; return ostr; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// RowBase2 class (== RowBase + operator==)

#define CREATE_TABLE(name) namespace name { class RowBase2 : public RowBase { \
    public: bool operator==(const RowBase2& row) const { return true
#define COLUMN_NOT_NULL(name, type, args...) && (this->name == row.name)
#define COLUMN(name, type, args...)          && (this->name == row.name)
#define CREATE_TABLE_END() ; } }; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// RowBase3 class (== RowBase2 + operator<)

#define CREATE_TABLE(name) namespace name { class RowBase3 : public RowBase2 { \
    public: bool operator<(const RowBase3& row) const {
#define COLUMN_NOT_NULL(name, type, args...) if (this->name < row.name) { return true; } if (this->name > row.name) { return false; }
#define COLUMN(name, type, args...)          if (this->name < row.name) { return true; } if (this->name > row.name) { return false; }
#define CREATE_TABLE_END() return false; } }; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// RowBase4 class (== RowBase3 + IsSignatureMatching )

#define CREATE_TABLE(name) namespace name { class RowBase4 : public RowBase3 { \
    public: bool IsSignatureMatching(const RowBase4& row) const { return true
#define COLUMN_NOT_NULL(name, type, args...) && (this->m_##name##_set == row.m_##name##_set)
#define COLUMN(name, type, args...)          && (this->m_##name##_set == row.m_##name##_set)
#define CREATE_TABLE_END() ; } }; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// RowBase5 class (== RowBase4 + default constructor)

#define CREATE_TABLE(name) namespace name { class RowBase5 : public RowBase4 { \
    public: RowBase5() {
#define COLUMN_NOT_NULL(name, type, args...) m_##name##_set = false;
#define COLUMN(name, type, args...)          m_##name##_set = false;
#define CREATE_TABLE_END() } }; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// Row class (== RowBase5 + ForEachColumn )

#define CREATE_TABLE(name) namespace name { class Row : public RowBase5 { \
    public: template<typename Visitor>                                    \
    void VisitColumns(Visitor& visitor) const {
#define COLUMN_NOT_NULL(name, type, args...) visitor.Visit(STRINGIFY(name), this->name, this->m_##name##_set);
#define COLUMN(name, type, args...)          visitor.Visit(STRINGIFY(name), this->name, this->m_##name##_set);
#define CREATE_TABLE_END() } }; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// Field structure declarations

#define CREATE_TABLE(name) namespace name {
#define COLUMN_NOT_NULL(name, type, args...) DECLARE_COLUMN(name, type)
#define COLUMN(name, type, args...) DECLARE_COLUMN(name, OPTIONAL(type))
#define CREATE_TABLE_END() }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// ColumnList typedef

#define CREATE_TABLE(name) namespace name { typedef DPL::TypeListDecl<
#define COLUMN_NOT_NULL(name, type, args...) name,
#define COLUMN(name, type, args...) name,
#define CREATE_TABLE_END() DPL::TypeListGuard>::Type ColumnList; }

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// TableDefinition struct

#define CREATE_TABLE(table_name)                                            \
    namespace table_name {                                                  \
        struct TableDefinition {                                            \
            typedef table_name::ColumnList ColumnList;                      \
            typedef table_name::Row Row;                                    \
            static const char* GetName() { return STRINGIFY(table_name); }  \
            static DPL::DB::SqlConnection::DataCommand *AllocTableDataCommand( \
                const std::string &statement,                               \
                IOrmInterface *interface)                                   \
            {                                                               \
                Assert(interface != NULL);                                  \
                return interface->AllocDataCommand(statement);              \
            }                                                               \
            static void FreeTableDataCommand(                               \
                DPL::DB::SqlConnection::DataCommand *command,               \
                IOrmInterface *interface)                                   \
            {                                                               \
                Assert(interface != NULL);                                  \
                interface->FreeDataCommand(command);                        \
            }                                                               \
            static DPL::DB::SqlConnection::RowID GetLastInsertRowID(        \
                IOrmInterface *interface)                                   \
            {                                                               \
                Assert(interface != NULL);                                  \
                return interface->GetLastInsertRowID();                     \
            }                                                               \
        };                                                                  \
    }

#define COLUMN_NOT_NULL(name, type, args...)
#define COLUMN(name, type, args...)
#define CREATE_TABLE_END()

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END

// Query typedefs

#define CREATE_TABLE(name) \
    namespace name { \
        typedef Select<TableDefinition> Select; \
        typedef Insert<TableDefinition> Insert; \
        typedef Delete<TableDefinition> Delete; \
        typedef Update<TableDefinition> Update; \
    }
#define COLUMN_NOT_NULL(name, type, args...)
#define COLUMN(name, type, args...)
#define CREATE_TABLE_END()

#include ORM_GENERATOR_DATABASE_NAME_LOCAL

#undef CREATE_TABLE
#undef COLUMN_NOT_NULL
#undef COLUMN
#undef CREATE_TABLE_END


// Global undefs
#undef INT
#undef TINYINT
#undef INTEGER
#undef BIGINT
#undef VARCHAR
#undef TEXT

#undef SQL
#undef TABLE_CONSTRAINTS
#undef OPTIONAL
#undef DATABASE_START
#undef DATABASE_END

} //namespace ORM
} //namespace DB
} //namespace DPL

#undef ORM_GENERATOR_DATABASE_NAME
#undef ORM_GENERATOR_DATABASE_NAME_LOCAL
