#include "../common.h"
#include <iostream>

using namespace odbc_test;

TEST_CASE("bindcol", "[odbc") {
	SQLRETURN ret;
	SQLHANDLE env;
	SQLHANDLE dbc;
	HSTMT hstmt = SQL_NULL_HSTMT;
	/*
	 * NOTE: in the psqlodbc, we assume that SQL_C_LONG actually means a
	 * variable of type SQLINTEGER. They are not the same on platforms where
	 * "long" is a 64-bit integer. That seems a bit bogus, but it's too late
	 * to change that without breaking applications that depend on it.
	 * (on little-endian systems, you won't notice the difference if you reset
	 * the high bits to zero before calling SQLBindCol.)
	 */
	SQLINTEGER longvalue;
	SQLLEN indLongvalue;
	char charvalue[100];
	SQLLEN indCharvalue;
	int rowno = 0;

	// Connect to the database
	CONNECT_TO_DATABASE(ret, env, dbc);

	ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &hstmt);
	ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLAllocHandle (HSTMT)");

	ret = SQLBindCol(hstmt, 1, SQL_C_LONG, &longvalue, sizeof(SQLINTEGER), &indLongvalue);
	ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLBindCol (HSTMT)");

	ret = SQLBindCol(hstmt, 2, SQL_C_CHAR, &charvalue, sizeof(charvalue), &indCharvalue);
	ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLBindCol (HSTMT)");

	ret = SQLExecDirect(hstmt, (SQLCHAR *)
                       "SELECT id, 'foo' || id FROM generate_series(1, 10) id(id)", SQL_NTS);
	ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLExecDirect (HSTMT)");

	SQLINTEGER id = 1;
	SQLINTEGER foo = 1;
	bool incrID = true;
	bool incrFoo = true;
	while (1) {
        ret = SQLFetch(hstmt);
        if (ret == SQL_NO_DATA) {
            break;
        }
        if (ret == SQL_SUCCESS) {
			REQUIRE(longvalue == id);
			REQUIRE(strcmp(charvalue, ("foo" + std::to_string(foo)).c_str()) == 0);
        } else {
            ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLFetch (HSTMT)");
        }

		if (incrID) {
			id++;
		}
		if (incrFoo) {
            foo++;
        }
		rowno++;
		if (rowno == 3) {
			ret = SQLBindCol(hstmt, 2, SQL_C_CHAR, NULL, 0, NULL);
			ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLBindCol (HSTMT)");
			incrFoo = false;
		}
		if (rowno == 5 || rowno == 9) {
			ret = SQLBindCol(hstmt, 2, SQL_C_CHAR, &charvalue, sizeof(charvalue), &indCharvalue);
            ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLBindCol (HSTMT)");
			incrFoo = true;
		}
		if (rowno == 7) {
			ret = SQLFreeStmt(hstmt, SQL_UNBIND);
            ODBC_CHECK(ret, SQL_HANDLE_STMT, hstmt, "SQLFreeStmt (HSTMT)");
			incrID = false;
			incrFoo = false;
		}
    }
}
