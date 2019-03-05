#ifndef __LIBGEEK_CORE_DATABASE_H_
#define __LIBGEEK_CORE_DATABASE_H_

#include <map>
#include <set>
#include <vector>
#include <string>

#include <stdint.h>

#include <sqlite3.h>

namespace Geek
{
namespace Core
{

struct Column
{
    std::string name;
    std::string type;
    bool isPrimary;

    Column() {}

    Column(std::string _name, std::string _type, bool _isPrimary)
    {
        name = _name;
        type = _type;
        isPrimary = _isPrimary;
    }


    Column(std::string _name, bool _isPrimary)
    {
        name = _name;
        type = "";
        isPrimary = _isPrimary;
    }

    Column(std::string _name)
    {
        name = _name;
        type = "";
        isPrimary = false;
    }

    bool operator <(const Column& rhs) const
    {
        return this->name < rhs.name;
    }
};

struct Table
{
    std::string name;
    std::set<Column> columns;
};

struct Row
{
    std::map<std::string, std::string> columns;

    std::string getValue(std::string column)
    {
        std::map<std::string, std::string>::iterator it;
        it = columns.find(column);
        if (it != columns.end())
        {
            return it->second;
        }
        return "";
    }
};

struct ResultSet
{
    std::set<std::string> columns;
    std::vector<Row> rows;
};

class Database;

class PreparedStatement
{
 private:
    Database* m_db;
    sqlite3_stmt* m_stmt;
    int m_error;

 public:
    PreparedStatement(Database* db, sqlite3_stmt* stmt);
    ~PreparedStatement();

    bool bindString(int i, const char* str, int length);
    bool bindString(int i, std::string str);
    bool bindInt64(int i, int64_t v);
    bool bindDouble(int i, double v);
    bool bindBlob(int i, void* data, int length);
    bool bindNull(int i);

    int getColumnCount();
    std::string getColumnName(int i);

    int getInt(int i);
    int64_t getInt64(int i);
    double getDouble(int i);
    std::string getString(int i);
    bool getBlob(int i, const void** data, uint32_t* length);

    bool execute();
    bool reset();

    bool executeQuery();
    bool step();

    int getLastError() { return m_error; }
};

class Database
{
 private:
    std::string m_path;
    sqlite3* m_db;
    bool m_open;
    int m_inTransaction;

 public:
    Database(std::string path);
    ~Database();

    bool open();
    bool close();

    bool startTransaction();
    bool endTransaction();

    bool checkSchema(std::vector<Table> schema);

    PreparedStatement* prepareStatement(std::string sql);

    ResultSet executeQuery(std::string query);
    ResultSet executeQuery(std::string query, std::vector<std::string> args);

    bool execute(std::string query);
    bool execute(std::string query, std::vector<std::string> args);

    std::set<std::string> getTables();
    std::set<std::string> getColumns(std::string table);

    sqlite3* getDB() { return m_db; }

    int64_t getLastInsertId();
};

};
};

#endif
