#include "./DatabaseManager.h"
#include "../exceptions/DatabaseException.h"
#include "../exceptions/InvalidInputException.h"
#include "../exceptions/NotFoundException.h"
#include <iostream>

DatabaseManager *DatabaseManager::instance = nullptr;

DatabaseManager::DatabaseManager() : connection(nullptr)
{
    const char *conninfo = "dbname= user= password= host= port=";
    connection = PQconnectdb(conninfo);
    if (PQstatus(connection) != CONNECTION_OK)
    {
        std::string msg = PQerrorMessage(connection);
        PQfinish(connection);
        connection = nullptr;
        throw DatabaseException("Ошибка подключения: " + msg);
    }
    std::cout << "\033[1;31mСоздан DatabaseManager и подключен к PostgreSQL\033[0m\n";
}

DatabaseManager::~DatabaseManager()
{
    if (connection)
    {
        PQfinish(connection);
        connection = nullptr;
    }
    std::cout << "\033[1;31mУдален DatabaseManager, соединение закрыто\033[0m\n";
}

DatabaseManager *DatabaseManager::getInstance()
{
    if (!instance)
    {
        instance = new DatabaseManager();
    }
    return instance;
}

void DatabaseManager::destroyInstance()
{
    delete instance;
    instance = nullptr;
}

void DatabaseManager::checkResult(PGresult *res, const std::string &context)
{
    if (!res)
    {
        std::string err = PQerrorMessage(connection);
        throw DatabaseException(context + ": " + err);
    }
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
    {
        std::string err = PQresultErrorMessage(res);
        PQclear(res);
        throw DatabaseException(context + ": " + err);
    }
    PQclear(res);
}

void DatabaseManager::execute(const std::string &sql)
{
    PGresult *res = PQexec(connection, sql.c_str());
    checkResult(res, "Ошибка выполнения запроса");
}

void DatabaseManager::executeWithCallback(const std::string &sql,
                                          const std::function<void(const std::vector<std::string> &)> &callback)
{
    PGresult *res = PQexec(connection, sql.c_str());
    if (!res)
    {
        std::string err = PQerrorMessage(connection);
        throw DatabaseException("Ошибка выполнения SELECT: " + err);
    }
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::string err = PQresultErrorMessage(res);
        PQclear(res);
        throw DatabaseException("Ошибка выполнения SELECT: " + err);
    }
    int rows = PQntuples(res);
    int cols = PQnfields(res);
    for (int i = 0; i < rows; ++i)
    {
        std::vector<std::string> row;
        for (int j = 0; j < cols; ++j)
        {
            char *val = PQgetvalue(res, i, j);
            row.push_back(val ? val : "");
        }
        callback(row);
    }
    PQclear(res);
}

void DatabaseManager::beginTransaction()
{
    execute("BEGIN");
}

void DatabaseManager::commitTransaction()
{
    execute("COMMIT");
}

void DatabaseManager::rollbackTransaction()
{
    execute("ROLLBACK");
}

void DatabaseManager::initializeSchema()
{
    beginTransaction();
    try
    {
        execute(
            "CREATE TABLE IF NOT EXISTS students ("
            "id SERIAL PRIMARY KEY,"
            "name VARCHAR(255) NOT NULL,"
            "\"group\" VARCHAR(255) NOT NULL,"
            "date_of_birth VARCHAR(10) NOT NULL,"
            "average_score DOUBLE PRECISION NOT NULL DEFAULT 0)");
        commitTransaction();
    }
    catch (...)
    {
        rollbackTransaction();
        throw;
    }
}