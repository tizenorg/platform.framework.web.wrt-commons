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
#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <dpl/db/thread_database_support.h>
#include <generator_dpl_orm_test.h>
#include <sstream>

const char* PATH_DB = "/opt/share/wrt/wrt-commons/tests/db/dpl_orm_test.db";

//utils

#define TEST_REPETITION 16

class SmartAttach
{
public:

    SmartAttach(bool autoattach = true) :
        m_interface(PATH_DB,
                    DPL::DB::SqlConnection::Flag::UseLucene),
        m_autoattach(autoattach)
    {
        if (m_autoattach) {
            m_interface.AttachToThread(DPL::DB::SqlConnection::Flag::RW);
        }
    }

    ~SmartAttach()
    {
        if (m_autoattach) {
            m_interface.DetachFromThread();
        }
    }

    DPL::DB::ThreadDatabaseSupport* get()
    {
        return &m_interface;
    }
private:
    DPL::DB::ThreadDatabaseSupport m_interface;
    bool m_autoattach;
};

template<typename ContainerType1, typename ContainerType2>
bool ContainerContentsEqual(const ContainerType1& container1, const ContainerType2& container2)
{
    using namespace DPL::DB::ORM::dpl_orm_test::TestTableInsert;
    typedef std::set<typename ContainerType1::value_type> Set1;
    typedef std::set<typename ContainerType2::value_type> Set2;
    Set1 set1(container1.begin(), container1.end());
    Set2 set2(container2.begin(), container2.end());

    for (typename Set1::iterator it = set1.begin();
            it != set1.end();
            it++)
    {
        LogDebug("Set1 element: " << *it);
    }

    for (typename Set2::iterator it = set2.begin(); it != set2.end(); it++)
    {
        LogDebug("Set2 element: " << *it);
    }

    return set1 == set2;
}

template<typename T>
std::list<T> makeList(const T& a, const T& b)
{
    std::list<T> list;
    list.push_back(a);
    list.push_back(b);
    return list;
}

//tests

RUNNER_TEST_GROUP_INIT(DPL)

RUNNER_TEST(ORM_SelectSingleValue)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;
    //Getting each column
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(8));
        int result;
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptInt>()) == 6, "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(8));
        DPL::String result;
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptText>()) == L"seven", "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(8));
        int result;
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt>()) == 8, "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(8));
        int result;
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 9, "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(8));
        DPL::String result;
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnText>()) == L"ten", "Got " << result);
    }

    //Where on each column
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnOptInt>(6));
        int result;
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptInt>()) == 6, "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnOptText>(DPL::String(L"seven")));
        DPL::String result;
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptText>()) == L"seven", "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(8));
        int result;
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt>()) == 8, "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt2>(9));
        int result;
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 9, "Got " << result);
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnText>(L"ten"));
        DPL::String result;
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnText>()) == L"ten", "Got " << result);
    }
}

RUNNER_TEST(ORM_SelectSingleRow)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(3));
        TestTable::Row result = select.GetSingleRow();
        TestTable::Row expected;
        expected.Set_ColumnOptInt(1);
        expected.Set_ColumnOptText(DPL::String(L"two"));
        expected.Set_ColumnInt(3);
        expected.Set_ColumnInt2(4);
        expected.Set_ColumnText(L"five");
        RUNNER_ASSERT_MSG(result == expected, "Got " << result);
    }

    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnOptText>(DPL::String(L"seven")));
        TestTable::Row result = select.GetSingleRow();
        TestTable::Row expected;
        expected.Set_ColumnOptInt(6);
        expected.Set_ColumnOptText(DPL::String(L"seven"));
        expected.Set_ColumnInt(8);
        expected.Set_ColumnInt2(9);
        expected.Set_ColumnText(L"ten");
        RUNNER_ASSERT_MSG(result == expected, "Got " << result);
    }
}

RUNNER_TEST(ORM_SelectRowList)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;
    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(3));
        std::list<TestTable::Row> result = select.GetRowList();
        RUNNER_ASSERT_MSG(result.size() == 1, "Got " << result.size());

        TestTable::Row expected;
        expected.Set_ColumnOptInt(1);
        expected.Set_ColumnOptText(DPL::String(L"two"));
        expected.Set_ColumnInt(3);
        expected.Set_ColumnInt2(4);
        expected.Set_ColumnText(L"five");
        RUNNER_ASSERT_MSG(*(result.begin()) == expected, "Got " << *(result.begin()) );
    }

    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnOptText>(DPL::String(L"seven")));
        std::list<TestTable::Row> result = select.GetRowList();
        RUNNER_ASSERT_MSG(result.size() == 1, "Got " << result.size());

        TestTable::Row expected;
        expected.Set_ColumnOptInt(6);
        expected.Set_ColumnOptText(DPL::String(L"seven"));
        expected.Set_ColumnInt(8);
        expected.Set_ColumnInt2(9);
        expected.Set_ColumnText(L"ten");
        RUNNER_ASSERT_MSG(*(result.begin()) == expected, "Got " << *(result.begin()) );
    }

    {
        TestTable::Select select(interface.get());
        select.Where(Equals<TestTable::ColumnInt>(99));
        std::list<TestTable::Row> result = select.GetRowList();

        TestTable::Row expected1;
        expected1.Set_ColumnInt(99);
        expected1.Set_ColumnInt2(11);
        expected1.Set_ColumnText(L"twelve");

        TestTable::Row expected2;
        expected2.Set_ColumnInt(99);
        expected2.Set_ColumnInt2(13);
        expected2.Set_ColumnText(L"fourteen");

        RUNNER_ASSERT(ContainerContentsEqual(makeList(expected1, expected2), result));
    }
}

RUNNER_TEST(ORM_SelectValueList)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;
    //Getting each column
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnOptInt>(DPL::Optional<int>::Null));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnInt>(),
                      makeList(99, 99)));
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnOptInt>(DPL::Optional<int>::Null));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnInt2>(),
                      makeList(11, 13)));
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnOptInt>(DPL::Optional<int>::Null));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnText>(),
                      makeList(DPL::String(L"twelve"), DPL::String(L"fourteen"))));
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnOptInt>(DPL::Optional<int>::Null));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnOptText>(),
                      makeList(DPL::Optional<DPL::String>::Null,DPL::Optional<DPL::String>::Null)));
    }

    //Where on each column
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnOptInt>(DPL::Optional<int>::Null));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnInt2>(),
                      makeList(11, 13)));
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnOptText>(DPL::Optional<DPL::String>::Null));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnInt2>(),
                      makeList(11, 13)));
    }
    {
        TestTable::Select select(interface.get());
        select.Where(Is<TestTable::ColumnInt>(99));
        RUNNER_ASSERT(ContainerContentsEqual(select.GetValueList<TestTable::ColumnInt2>(),
                      makeList(11, 13)));
    }
}

RUNNER_TEST(ORM_MultipleCalls)
{
    for (int j = 0 ; j < TEST_REPETITION ; j++ )
    {
        for (int i = 0 ; i < TEST_REPETITION ; i++ )
            ORM_SelectSingleValue();

        for (int i = 0 ; i < TEST_REPETITION ; i++ )
            ORM_SelectSingleRow();

        for (int i = 0 ; i < TEST_REPETITION ; i++ )
            ORM_SelectRowList();

        for (int i = 0 ; i < TEST_REPETITION ; i++ )
            ORM_SelectValueList();
    }
}

RUNNER_TEST(ORM_Insert)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;

    TestTableInsert::Select select1(interface.get());
    std::list<int> resultList = select1.GetValueList<TestTableInsert::ColumnInt>();
    RUNNER_ASSERT_MSG(resultList.size() == 0, "Returned list has wrong size: " << resultList.size());
    std::list<TestTableInsert::Row> list;

    TestTableInsert::Insert insert(interface.get());
    TestTableInsert::Row row;
    row.Set_ColumnOptInt(1);
    row.Set_ColumnInt2(2);
    row.Set_ColumnText(L"three");
    insert.Values(row);
    insert.Execute();

    row.Set_ColumnInt(99);
    list.push_back(row);
    {
        TestTableInsert::Select select2(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select2.GetRowList(), list), "Returned list doesn't match.");
    }

    TestTableInsert::Insert insert2(interface.get());
    TestTableInsert::Row row2;
    row2.Set_ColumnInt(4);
    row2.Set_ColumnInt2(5);
    row2.Set_ColumnText(L"six");
    insert2.Values(row2);
    insert2.Execute();

    list.push_back(row2);
    {
        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    TestTableInsert::Insert insert3(interface.get());
    TestTableInsert::Row row3;
    row3.Set_ColumnOptInt(1);
    row3.Set_ColumnInt2(7);
    row3.Set_ColumnText(L"eight");
    insert3.Values(row3);
    insert3.Execute();

    row3.Set_ColumnInt(99);
    list.push_back(row3);
    {
        TestTableInsert::Select select3(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select3.GetRowList(), list), "Returned list doesn't match.");
    }

    TestTableInsert::Insert insert4(interface.get());
    TestTableInsert::Row row4;
    row4.Set_ColumnOptInt(9);
    row4.Set_ColumnInt2(10);
    row4.Set_ColumnText(L"eleven");
    insert4.Values(row4);
    insert4.Execute();

    row4.Set_ColumnInt(99);
    list.push_back(row4);
    {
        TestTableInsert::Select select4(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select4.GetRowList(), list), "Returned list doesn't match.");
    }

    // restore original table state
    {
        TestTableInsert::Delete del(interface.get());
        del.Execute();

        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT(select.GetRowList().size() == 0);
    }
}

RUNNER_TEST(ORM_MultipleBindInsert)
{
    for ( int i = 0 ; i < TEST_REPETITION ; i++ )
    {
        ORM_Insert();
    }
}

RUNNER_TEST(ORM_Delete)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;
    TestTableDelete::Select selectStart(interface.get());
    selectStart.OrderBy("ColumnInt2 ASC");
    std::list<TestTableDelete::Row> list = selectStart.GetRowList();
    std::list<TestTableDelete::Row> originalList = list;

    std::vector<TestTableDelete::Row> vector(list.begin(), list.end());
    RUNNER_ASSERT_MSG(list.size() == 4, "Returned list has wrong size: " << list.size());

    typedef DPL::String S;

    //no-act deletes
    {
        TestTableDelete::Delete del(interface.get());
        del.Where(And(Equals<TestTableDelete::ColumnOptInt>(1), Equals<TestTableDelete::ColumnOptText>(S(L"seven"))));
        del.Execute();

        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    {
        TestTableDelete::Delete del(interface.get());
        del.Where(And(Equals<TestTableDelete::ColumnOptInt>(6), Equals<TestTableDelete::ColumnOptText>(S(L"two"))));
        del.Execute();

        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    {
        TestTableDelete::Delete del(interface.get());
        del.Where(Equals<TestTableDelete::ColumnInt2>(10));
        del.Execute();

        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    //act deletes
    {
        list.remove(vector[1]);

        TestTableDelete::Delete del(interface.get());
        del.Where(And(Equals<TestTableDelete::ColumnOptInt>(6), Equals<TestTableDelete::ColumnText>(L"ten")));
        del.Execute();

        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    {
        list.remove(vector[2]);
        list.remove(vector[3]);

        TestTableDelete::Delete del(interface.get());
        del.Where(Is<TestTableDelete::ColumnOptText>(DPL::Optional<DPL::String>::Null));
        del.Execute();

        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    {
        TestTableDelete::Delete del(interface.get());
        del.Execute();

        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(select.GetRowList().size() == 0, "Returned list is not empty");
    }

    // Restore original table state
    // This also tests if multiple different binds for Insert are working properly
    for (std::list<TestTableDelete::Row>::iterator i = originalList.begin(); i != originalList.end(); i++)
    {
        TestTableDelete::Insert insert(interface.get());
        insert.Values(*i);
        insert.Execute();
    }

    {
        TestTableDelete::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), originalList), "Returned list doesn't match.");
    }

}

RUNNER_TEST(ORM_MultipleBindDelete)
{
    for ( int i = 0 ; i < TEST_REPETITION ; i++ )
    {
        ORM_Delete();
    }
}

RUNNER_TEST(ORM_MultipleBindWhere)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;
    {
        TestTable::Select select(interface.get());
        int result;
        select.Where(Equals<TestTable::ColumnInt>(8));
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptInt>()) == 6, "Got " << result);

        select.Where(Equals<TestTable::ColumnInt>(3));
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptInt>()) == 1, "Got " << result);

        select.Where(Equals<TestTable::ColumnInt>(8));
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptInt>()) == 6, "Got " << result);

        select.Where(Equals<TestTable::ColumnInt>(3));
        RUNNER_ASSERT_MSG((result = *select.GetSingleValue<TestTable::ColumnOptInt>()) == 1, "Got " << result);
    }

    {
        TestTable::Select select(interface.get());
        int result;
        select.Where(And(Equals<TestTable::ColumnInt>(99),
                         Equals<TestTable::ColumnText>(L"fourteen")));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 13, "Got " << result);

        select.Where(And(Equals<TestTable::ColumnInt>(99),
                         Equals<TestTable::ColumnText>(L"twelve")));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 11, "Got " << result);

        select.Where(And(Equals<TestTable::ColumnInt>(99),
                         Equals<TestTable::ColumnText>(L"fourteen")));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 13, "Got " << result);

        select.Where(And(Equals<TestTable::ColumnInt>(99),
                         Equals<TestTable::ColumnText>(L"twelve")));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 11, "Got " << result);
    }

    {
        TestTable::Select select(interface.get());
        int result;
        select.Where(And(Equals<TestTable::ColumnText>(L"fourteen"),
                         Equals<TestTable::ColumnInt>(99)));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 13, "Got " << result);

        select.Where(And(Equals<TestTable::ColumnText>(L"twelve"),
                         Equals<TestTable::ColumnInt>(99)));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 11, "Got " << result);

        select.Where(And(Equals<TestTable::ColumnText>(L"fourteen"),
                         Equals<TestTable::ColumnInt>(99)));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 13, "Got " << result);

        select.Where(And(Equals<TestTable::ColumnText>(L"twelve"),
                         Equals<TestTable::ColumnInt>(99)));
        RUNNER_ASSERT_MSG((result = select.GetSingleValue<TestTable::ColumnInt2>()) == 11, "Got " << result);

    }

}

RUNNER_TEST(ORM_Update)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;

    std::list<TestTableInsert::Row> list;

    TestTableInsert::Delete del(interface.get());
    del.Execute();

    // INSERT
    {
        TestTableInsert::Insert insert(interface.get());
        TestTableInsert::Row row;
        row.Set_ColumnOptInt(5);
        row.Set_ColumnInt2(2);
        row.Set_ColumnText(L"two");
        insert.Values(row);
        insert.Execute();

        row.Set_ColumnInt(99);
        list.push_back(row);
    }
    {
        TestTableInsert::Insert insert(interface.get());
        TestTableInsert::Row row;
        row.Set_ColumnOptInt(1);
        row.Set_ColumnInt2(2);
        row.Set_ColumnText(L"three");
        insert.Values(row);
        insert.Execute();

        row.Set_ColumnInt(99);
        list.push_back(row);
    }
    {
        TestTableInsert::Insert insert(interface.get());
        TestTableInsert::Row row;
        row.Set_ColumnOptInt(2);
        row.Set_ColumnInt2(3);
        row.Set_ColumnText(L"three");
        insert.Values(row);
        insert.Execute();

        row.Set_ColumnInt(99);
        list.push_back(row);

        // CHECK
        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }
    {
        // UPDATE - no rows
        TestTableInsert::Update update(interface.get());
        TestTableInsert::Row row;
        row.Set_ColumnInt2(4);
        row.Set_ColumnText(L"four");
        update.Values(row);
        update.Where(Equals<TestTableInsert::ColumnInt2>(12));
        update.Execute();

        // CHECK
        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }
    {
        // UPDATE - one row
        TestTableInsert::Update update(interface.get());
        TestTableInsert::Row row;
        row.Set_ColumnInt2(2);
        row.Set_ColumnText(L"four");
        update.Values(row);
        update.Where(Equals<TestTableInsert::ColumnInt2>(3));
        update.Execute();

        list.back().Set_ColumnInt2(2);
        list.back().Set_ColumnText(L"four");

        // CHECK
        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    {
        // UPDATE - multiple rows
        TestTableInsert::Update update(interface.get());
        TestTableInsert::Row row;
        row.Set_ColumnText(L"dup");
        update.Values(row);
        update.Where(Equals<TestTableInsert::ColumnInt2>(2));
        update.Execute();

        FOREACH(it, list)
        {
            it->Set_ColumnText(L"dup");
        }

        // CHECK
        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT_MSG(ContainerContentsEqual(select.GetRowList(), list), "Returned list doesn't match.");
    }

    // restore original table state
    {
        TestTableInsert::Delete del2(interface.get());
        del2.Execute();

        TestTableInsert::Select select(interface.get());
        RUNNER_ASSERT(select.GetRowList().size() == 0);
    }
}

RUNNER_TEST(ORM_MultipleBindUpdate)
{
    for ( int i = 0 ; i < TEST_REPETITION ; i++ )
    {
        ORM_Update();
    }
}

RUNNER_TEST(ORM_transactions)
{
    SmartAttach interface;
    DPL::DB::ORM::dpl_orm_test::ScopedTransaction transaction(interface.get());
}

RUNNER_TEST(ORM_MultiAttach)
{
    SmartAttach interface(false);
    RUNNER_ASSERT_MSG(!interface.get()->IsAttached(), "Is attached, but shouldn't be.");
    interface.get()->AttachToThread();
    RUNNER_ASSERT_MSG(interface.get()->IsAttached(), "Isn't attached, but should be.");
    interface.get()->AttachToThread();
    RUNNER_ASSERT_MSG(interface.get()->IsAttached(), "Isn't attached, but should be.");
    interface.get()->DetachFromThread();
    RUNNER_ASSERT_MSG(interface.get()->IsAttached(), "Isn't attached, but should be.");
    interface.get()->DetachFromThread();
    RUNNER_ASSERT_MSG(!interface.get()->IsAttached(), "Is attached, but shouldn't be.");
}

RUNNER_TEST(ORM_Join)
{
    SmartAttach interface;
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::dpl_orm_test;

    typedef DPL::TypeListDecl<TestTableJoin1::TestText, TestTableJoin2::TestText2,
            DPL::TypeListGuard>::Type JoinColumns;

    /* Test for correct join:
     * 5 ids from first table matches 5 ids from second table thus join result
     * contains 5 rows */
    TestTableJoin1::Select select(interface.get());
    select.Join<JoinColumns>(Equal<TestTableJoin1::TestID, TestTableJoin2::TestID>());
    std::list<CustomRow<JoinColumns>> rowlist =
            select.GetCustomRowList<JoinColumns, CustomRow<JoinColumns>>();

    RUNNER_ASSERT_MSG(rowlist.size() == 5, "Invalid number of rows fetched: " << rowlist.size());

    std::string text;
    std::ostringstream oss;
    int cnt = 0;
    FOREACH(rowit, rowlist)
    {
        cnt++;

        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin1::TestText>());
        oss << "text val " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from first column: "
                << text << " expected: " << oss.str());
        oss.str(std::string());

        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin2::TestText2>());
        oss << "text2 " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from second column: "
                << text << " expected: " << oss.str());
        oss.str(std::string());
    }
    /* Test for empty join:
     * None of number values from first table matches ids from second table
     * - join result should be empty */
    TestTableJoin1::Select select2(interface.get());
    select2.Join<JoinColumns>(Equal<TestTableJoin1::TestNumber, TestTableJoin2::TestID>());
    rowlist = select2.GetCustomRowList<JoinColumns, CustomRow<JoinColumns>>();

    RUNNER_ASSERT_MSG(rowlist.empty(), "Result should be empty but it is not!");

    /* Test for "converted" join:
     * - join made with int column and text column as keys
     * - expected 5 matching rows (one row of 6 should not be matched)*/
    TestTableJoin1::Select select3(interface.get());
    select3.Join<JoinColumns>(Equal<TestTableJoin1::TestID, TestTableJoin2::TestText1>());
    rowlist = select3.GetCustomRowList<JoinColumns, CustomRow<JoinColumns>>();
    RUNNER_ASSERT_MSG(rowlist.size() == 5, "Expected 5 rows while received: " << rowlist.size());
    cnt = 0;
    FOREACH(rowit, rowlist)
    {
        cnt++;
        // look at last two insertions into TestTableJoin2
        // for this skip understanding
        if(cnt == 5) {
            cnt = 6;
        }
        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin1::TestText>());
        oss << "text val " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from first column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());

        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin2::TestText2>());
        oss << "text2 " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from second column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());
    }

    /* Test for join with non-unique nullable columns given as keys*/
    typedef DPL::TypeListDecl<TestTableJoin1::TestText, TestTableJoin3::TestText33,
            DPL::TypeListGuard>::Type JoinTables2;
    TestTableJoin1::Select select4(interface.get());
    select4.Join<JoinTables2>(Equal<TestTableJoin1::TestNumber, TestTableJoin3::Value3>());
    std::list<CustomRow<JoinTables2>> rowlist2 = select4.GetCustomRowList<JoinTables2, CustomRow<JoinTables2>>();
    RUNNER_ASSERT_MSG(rowlist2.size() == 4, "Expected 4 rows while received: " << rowlist.size());
    cnt = 0;
    DPL::Optional<DPL::String> optext;
    FOREACH(rowit, rowlist2)
    {
        cnt++;

        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin1::TestText>());
        // values expected in subsequent (1,2,3,4) iterations: 1 1 2 2
        oss << "text val " << (1+(int)(cnt/3));
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from first column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());

        optext = (*rowit).GetColumnData<TestTableJoin3::TestText33>();
        text = DPL::ToUTF8String(*optext);
        oss << "test " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from second column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());
    }

    /* Test for join made on three tables:
     * - 3 text columns selected for join
     * - Equal made for TestID of (table1 and table2) and (table1 and table3) */
    typedef DPL::TypeListDecl<TestTableJoin1::TestText, TestTableJoin2::TestText2,
            TestTableJoin3::TestText33, DPL::TypeListGuard>::Type Join3Tables;
    TestTableJoin1::Select select5(interface.get());
    select5.Join<Join3Tables>(Equal<TestTableJoin1::TestID, TestTableJoin2::TestID>());
    select5.Join<Join3Tables>(Equal<TestTableJoin1::TestID, TestTableJoin3::TestID>());
    std::list<CustomRow<Join3Tables>> rowlist3tab = select5.GetCustomRowList<Join3Tables, CustomRow<Join3Tables>>();
    RUNNER_ASSERT_MSG(rowlist3tab.size() == 3, "Expected 3 rows while received: " << rowlist3tab.size());
    cnt = 0;
    FOREACH(rowit, rowlist3tab)
    {
        cnt++;

        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin1::TestText>());
        oss << "text val " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from first column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());

        text = DPL::ToUTF8String((*rowit).GetColumnData<TestTableJoin2::TestText2>());
        oss << "text2 " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from first column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());

        optext = (*rowit).GetColumnData<TestTableJoin3::TestText33>();
        text = DPL::ToUTF8String(*optext);
        oss << "test " << cnt;
        RUNNER_ASSERT_MSG(text.compare(oss.str()) == 0, "Invalid value from second column: "
                << text << " expected: " << oss.str() << " iteration: " <<cnt);
        oss.str(std::string());
    }
}
