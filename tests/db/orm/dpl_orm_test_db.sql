



CREATE TABLE TestTableInsert (
    ColumnOptInt INT ,
    ColumnOptText TEXT ,
    ColumnInt INT DEFAULT 99 not null,
    ColumnInt2 INT not null,
    ColumnText TEXT not null,
CHECK(1) );

CREATE TABLE TestTableDelete (
    ColumnOptInt INT ,
    ColumnOptText TEXT ,
    ColumnInt INT DEFAULT 99 not null,
    ColumnInt2 INT not null,
    ColumnText TEXT not null,
CHECK(1) );

INSERT INTO TestTableDelete VALUES(1, "two", 3, 4, "five"); INSERT INTO TestTableDelete VALUES(6, "seven", 8, 9, "ten"); INSERT INTO TestTableDelete (ColumnInt2, ColumnText) VALUES(11, "twelve"); INSERT INTO TestTableDelete (ColumnInt2, ColumnText) VALUES(13, "fourteen");






CREATE TABLE TestTable (
    ColumnOptInt INT ,
    ColumnOptText TEXT ,
    ColumnInt INT DEFAULT 99 not null,
    ColumnInt2 INT not null,
    ColumnText TEXT not null,
CHECK(1) );

INSERT INTO TestTable VALUES(1, "two", 3, 4, "five"); INSERT INTO TestTable VALUES(6, "seven", 8, 9, "ten"); INSERT INTO TestTable (ColumnInt2, ColumnText) VALUES(11, "twelve"); INSERT INTO TestTable (ColumnInt2, ColumnText) VALUES(13, "fourteen");


