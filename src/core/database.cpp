
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <string>

#include <geek/core-database.h>

using namespace std;
using namespace Geek::Core;

string GET_TABLES_SQL = "SELECT name FROM sqlite_master WHERE type='table'";

Database::Database(string path)
{
    m_path = path;
    m_db = NULL;
    m_open = false;
    m_inTransaction = 0;

    sqlite3_initialize();
}

Database::~Database()
{
    close();

    //sqlite3_shutdown();
}

bool Database::open()
{
    if (m_open)
    {
        return true;
    }

    // Check that the parent directory exists
    size_t pos = m_path.rfind('/');
    if (pos != m_path.npos)
    {
        string dir = m_path.substr(0, pos);

        int res;
        res = access(dir.c_str(), R_OK | X_OK | W_OK);
        if (res == -1)
        {
            if (errno == ENOENT)
            {
                res = mkdir(dir.c_str(), 0755);
                if (res == -1)
                {
                    printf("Database::open: Failed to create parent dir\n");
                    return false;
                }
            }
            else
            {
                printf("Database::open: Failed to check parent directory\n");
                return false;
            }
        }
    }

    int res;
    res = sqlite3_open(m_path.c_str(), &m_db);
    if (res)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        return false;
    }

    m_open = true;

    return true;
}

bool Database::close()
{
    if (!m_open)
    {
        return true;
    }
    sqlite3_close(m_db);

    m_open = false;

    return true;
}

bool Database::startTransaction()
{
    m_inTransaction++;
    if (m_inTransaction > 1)
    {
        return true;
    }
    char* errmsg;
    sqlite3_exec(m_db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);
    return true;
}

bool Database::endTransaction()
{
    m_inTransaction--;
    if (m_inTransaction > 0)
    {
        return true;

    }
    char* errmsg;
    sqlite3_exec(m_db, "COMMIT", NULL, NULL, &errmsg);

    m_inTransaction = false;

    return true;
}

bool Database::checkSchema(vector<Table> schema)
{
    bool created = false;
    set<string> tables = getTables();
    set<string>::iterator it;

    vector<Table>::iterator tableIt;

    for (tableIt = schema.begin(); tableIt != schema.end(); tableIt++)
    {
        string table = tableIt->name;
        it = tables.find(table);
        if (it == tables.end())
        {
            string createSql = "CREATE TABLE " + table + " (";
string primaryKeys = "";

            bool comma = false;
            set<Column>::iterator columnIt;
            for (
                columnIt = tableIt->columns.begin();
                columnIt != tableIt->columns.end();
                columnIt++)
            {
                if (comma)
                {
                    createSql += ", ";
                }
                comma = true;
                createSql += columnIt->name;
                if (columnIt->type.length() > 0)
                {
                    createSql += " " + columnIt->type;
                }
                if (columnIt->isPrimary)
                {
                    if (primaryKeys.length() > 0)
                    {
                        primaryKeys += ", ";
                    }
                    primaryKeys += columnIt->name;
                }
            }

            if (primaryKeys.length() > 0)
            {
                createSql += ", PRIMARY KEY (" + primaryKeys + ")";
            }

            createSql += ")";
/*
            printf("Database::checkSchema:  -> %s\n", createSql.c_str());
*/

            execute(createSql);
            created = true;
        }
        else
        {
            // Table already exists
            // Check columns all exist
            set<string> columns = getColumns(table);
            set<Column>::iterator columnIt;
            for (
                columnIt = tableIt->columns.begin();
                columnIt != tableIt->columns.end();
                columnIt++)
            {
                set<string>::iterator it;
                string column = columnIt->name;
                it = columns.find(column);
                if (it == columns.end())
                {
/*
                    printf(
                        "Database::checkSchema: Column missing: %s\n",
                        column.c_str());
*/
                    string alterSql =
                        "ALTER TABLE " + table + " ADD COLUMN " + column;
/*
                    printf("Database::checkSchema:  -> %s\n", alterSql.c_str());
*/
                    execute(alterSql);
                    created = true;
                }
            }
        }
    }

    return created;
}

PreparedStatement* Database::prepareStatement(string sql)
{
    PreparedStatement* ps;
    int res;

    sqlite3_stmt* stmt;
    res = sqlite3_prepare_v2(m_db, sql.c_str(), sql.length(), &stmt, NULL);
    if (res)
    {
        printf(
            "Database::prepareStatement: Prepare Error: res=%d, msg=%s\n",
            res,
            sqlite3_errmsg(m_db));
        return NULL;
    }

    ps = new PreparedStatement(this, stmt);

    return ps;
}

ResultSet Database::executeQuery(string query)
{
    vector<string> noargs;
    return executeQuery(query, noargs);
}

ResultSet Database::executeQuery(string query, vector<string> args)
{
    ResultSet resultSet;
    int res;

    if (!m_open)
    {
        open();
    }

    sqlite3_stmt* stmt;
    res = sqlite3_prepare_v2(m_db, query.c_str(), query.length(), &stmt, NULL);
    if (res)
    {
        printf(
            "Database::open: Prepare Error: res=%d, msg=%s\n",
            res,
            sqlite3_errmsg(m_db));
        return resultSet;
    }

    vector<string>::iterator argIt;
    int arg = 1;
    for (argIt = args.begin(); argIt != args.end(); argIt++)
    {
        sqlite3_bind_text(
            stmt,
            arg++,
            argIt->c_str(),
            argIt->length(),
            SQLITE_STATIC);
    }

    while (true)
    {
        int s;
        s = sqlite3_step(stmt);

        if (s == SQLITE_ROW)
        {
            Row row;

            int count = sqlite3_column_count(stmt);
            int c;
            for (c = 0; c < count; c++)
            {
                const char* name;
                name = sqlite3_column_name(stmt, c);

                if (resultSet.rows.size() == 0)
                {
                    resultSet.columns.insert(string(name));
                }

                const unsigned char* value;
                value = sqlite3_column_text(stmt, 0);
                if (value == NULL)
                {
                    value = (const unsigned char*)"";
                }
                row.columns.insert(
                    make_pair(string(name),
                    string((char*)value)));
#if 0
                printf("Database::executeQuery: %s=%s\n", name, value);
#endif
            }
            resultSet.rows.push_back(row);
        }
        else if (s == SQLITE_DONE)
        {
            break;
        }
        else
        {
            printf(
                "Database::open: Error: res=%d, msg=%s\n",
                s,
                sqlite3_errmsg(m_db));
            break;
        }
    }
    sqlite3_finalize(stmt);
    return resultSet;
}

bool Database::execute(string query)
{
    vector<string> noargs;
    return execute(query, noargs);
}

bool Database::execute(string query, vector<string> args)
{
    int res;

    if (!m_open)
    {
        open();
    }

    sqlite3_stmt* stmt;
    res = sqlite3_prepare_v2(m_db, query.c_str(), query.length(), &stmt, NULL);
    if (res)
    {
        printf(
            "Database::execute: Error: res=%d, msg=%s\n",
            res,
            sqlite3_errmsg(m_db));
        return false;
    }

    vector<string>::iterator argIt;
    int arg = 1;
    for (argIt = args.begin(); argIt != args.end(); argIt++)
    {
        sqlite3_bind_text(
            stmt,
            arg++,
            argIt->c_str(),
            argIt->length(),
            SQLITE_STATIC);
    }

    res = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (res != SQLITE_DONE)
    {
        printf(
            "Database::execute: Error: res=%d, msg=%s\n",
            res,
            sqlite3_errmsg(m_db));
        return false;
    }

    return true;
}

set<string> Database::getTables()
{
    set<string> tables;
    ResultSet results;

    results = executeQuery(GET_TABLES_SQL);
    vector<Row>::iterator rowIt;
    for (rowIt = results.rows.begin(); rowIt != results.rows.end(); rowIt++)
    {
        map<string, string>::iterator it = rowIt->columns.find("name");
        if (it != rowIt->columns.end())
        {
            tables.insert(it->second);
        }
    }
    return tables;
}

set<string> Database::getColumns(string table)
{
    set<string> columns;

    PreparedStatement* ps = prepareStatement("SELECT * FROM " + table + " LIMIT 1");

    ps->step();
    int columnCount = ps->getColumnCount();
    int i;
    for (i = 0; i < columnCount; i++)
    {
        columns.insert(ps->getColumnName(i));
    }
    delete ps;

    return columns;
}

int64_t Database::getLastInsertId()
{
    return sqlite3_last_insert_rowid(m_db);
}

PreparedStatement::PreparedStatement(Database* db, sqlite3_stmt* stmt)
{
    m_db = db;
    m_stmt = stmt;
}

PreparedStatement::~PreparedStatement()
{
    sqlite3_finalize(m_stmt);
}

bool PreparedStatement::bindString(int i, const char* str, int length)
{
    sqlite3_bind_text(m_stmt, i, str, length, SQLITE_TRANSIENT);
    return true;
}

bool PreparedStatement::bindString(int i, string str)
{
    sqlite3_bind_text(m_stmt, i, str.c_str(), str.length(), SQLITE_TRANSIENT);
    return true;
}

bool PreparedStatement::bindInt64(int i, int64_t value)
{
    sqlite3_bind_int64(m_stmt, i, value);
    return true;
}

bool PreparedStatement::bindDouble(int i, double value)
{
    sqlite3_bind_double(m_stmt, i, value);
    return true;
}

bool PreparedStatement::bindBlob(int i, void* data, int length)
{
    sqlite3_bind_blob(m_stmt, i, data, length, SQLITE_TRANSIENT);
    return true;
}

bool PreparedStatement::bindNull(int i)
{
    sqlite3_bind_null(m_stmt, i);
    return true;
}

int PreparedStatement::getColumnCount()
{
   return sqlite3_column_count(m_stmt);
}

string PreparedStatement::getColumnName(int i)
{
   return string((char*)sqlite3_column_name(m_stmt, i));
}

int PreparedStatement::getInt(int i)
{
   return sqlite3_column_int(m_stmt, i);
}

int64_t PreparedStatement::getInt64(int i)
{
   return sqlite3_column_int64(m_stmt, i);
}

double PreparedStatement::getDouble(int i)
{
   return sqlite3_column_double(m_stmt, i);
}

string PreparedStatement::getString(int i)
{
    const void* str;
    str = sqlite3_column_text(m_stmt, i);
    if (str == NULL)
    {
        str = "";
    }
    return string((char*)str);
}

bool PreparedStatement::getBlob(int i, const void** data, uint32_t* length)
{
    *data = sqlite3_column_blob(m_stmt, i);
    *length = sqlite3_column_bytes(m_stmt, i);
   return true;
}


bool PreparedStatement::execute()
{
    int res;
    bool result;
    res = sqlite3_step(m_stmt);

    m_error = res;

    if (res != SQLITE_DONE)
    {
        printf(
            "PreparedStatement::execute: Prepare Error: res=%d, msg=%s\n",
            m_error,
            sqlite3_errmsg(m_db->getDB()));
        result = false;
    }
    else
    {
        result = true;
    }
    sqlite3_reset(m_stmt);

    return result;
}

bool PreparedStatement::executeQuery()
{
    // There's not actually anything to do
    return true;
}

bool PreparedStatement::step()
{
    int res;
    res = sqlite3_step(m_stmt);

    if (res == SQLITE_ROW)
    {
        return true;
    }
    else if (res == SQLITE_DONE)
    {
        return false;
    }
    else
    {
        printf(
            "PreparedStatement::executeQuery: Prepare Error: res=%d, msg=%s\n",
            res,
            sqlite3_errmsg(m_db->getDB()));
    }

    return false;
}

bool PreparedStatement::reset()
{
    sqlite3_reset(m_stmt);
    return true;
}


