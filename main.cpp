#include "./database/DatabaseManager.h"
#include "./exceptions/DatabaseException.h"
#include "./exceptions/InvalidInputException.h"
#include "./exceptions/NotFoundException.h"
#include <iostream>
#include <chrono>
#include <conio.h>

DatabaseManager *db = DatabaseManager::getInstance();

void displayList()
{
    bool hasData = false;
    db->executeWithCallback(
        "SELECT * FROM students",
        [&](const std::vector<std::string> &row)
        {
            hasData = true;
            std::cout << "ID=" << row[0] << ": " << row[1] << ", " << row[2] << ", " << row[3] << ", " << row[4] << "\n";
        });
    if (!hasData)
        std::cout << "Нет студентов\n";
}

int getValidatedInt(const std::string &prompt, int min, int max)
{
    int value;
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidInputException("Неверный ввод: нужно целое число");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (value < min || value > max)
    {
        throw InvalidInputException("Значение должно быть от " + std::to_string(min) + " до " + std::to_string(max));
    }
    return value;
}

double getValidatedDouble(const std::string &prompt, double min, double max)
{
    double value;
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw InvalidInputException("Неверный ввод: нужно число");
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (value < min || value > max)
    {
        throw InvalidInputException("Значение должно быть в заданном диапазоне");
    }
    return value;
}

std::string getValidatedString(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (value.empty())
    {
        throw InvalidInputException("Поле не может быть пустым");
    }
    return value;
}

bool isValidDateFormat(const std::string &date)
{
    if (date.size() != 10)
        return false;
    if (date[2] != '.' || date[5] != '.')
        return false;
    for (int i = 0; i < 10; ++i)
    {
        if (i == 2 || i == 5)
            continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }
    int d = std::stoi(date.substr(0, 2));
    int m = std::stoi(date.substr(3, 2));
    int y = std::stoi(date.substr(6, 4));
    if (m < 1 || m > 12)
        return false;
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (leap)
        daysInMonth[1] = 29;
    if (d < 1 || d > daysInMonth[m - 1])
        return false;
    return true;
}

std::string getValidatedDate(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    if (!isValidDateFormat(value))
    {
        throw InvalidInputException("Некорректный формат даты, ожидается ДД.ММ.ГГГГ");
    }
    return value;
}

void addStudent()
{
    std::string name;
    while (true)
    {
        try
        {
            name = getValidatedString("Введите имя:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string group;
    while (true)
    {
        try
        {
            group = getValidatedString("Введите группу:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::string dateOfBirth;
    while (true)
    {
        try
        {
            dateOfBirth = getValidatedDate("Введите дату рождения:\n");
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    double averageScore;
    while (true)
    {
        try
        {
            averageScore = getValidatedDouble("Введите средний балл:\n", 0.0, 5.0);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    db->execute("INSERT INTO students (name, \"group\", date_of_birth, average_score) VALUES ('" + name + "', '" + group + "', '" + dateOfBirth + "', " + std::to_string(averageScore) + ")");
}

void editStudent()
{
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM students", [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found) {
        throw NotFoundException("Студентов для редактирования нет");
    }
    int studentId;
    while (true)
    {
        try
        {
            studentId = getValidatedInt("Введите ID студента для редактирования:\n", 0, 100);
            bool found = false;
            db->executeWithCallback("SELECT 1 FROM students WHERE id=" + std::to_string(studentId), [&](const std::vector<std::string> &)
                                    { found = true; });
            if (!found)
            {
                throw NotFoundException("Студент не найден");
            }
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    std::cout << "1. Имя\n2. Группа\n3. Дата рождения\n4. Средний балл\n";
    int field;
    while (true)
    {
        try
        {
            field = getValidatedInt("Выберите поле для редактирования:\n", 1, 4);
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    if (field == 1)
    {
        std::string name;
        while (true)
        {
            try
            {
                name = getValidatedString("Введите имя:\n");
                break;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << "\n";
            }
        }
        db->execute("UPDATE students SET name='" + name + "' WHERE id=" + std::to_string(studentId));
        return;
    }
    if (field == 2)
    {
        std::string group;
        while (true)
        {
            try
            {
                group = getValidatedString("Введите группу:\n");
                break;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << "\n";
            }
        }
        db->execute("UPDATE students SET \"group\"='" + group + "' WHERE id=" + std::to_string(studentId));
        return;
    }
    if (field == 3)
    {
        std::string dateOfBirth;
        while (true)
        {
            try
            {
                dateOfBirth = getValidatedDate("Введите дату рождения:\n");
                break;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << "\n";
            }
        }
        db->execute("UPDATE students SET date_of_birth='" + dateOfBirth + "' WHERE id=" + std::to_string(studentId));
        return;
    }
    if (field == 4)
    {
        double averageScore;
        while (true)
        {
            try
            {
                averageScore = getValidatedDouble("Введите средний балл:\n", 0.0, 5.0);
                break;
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << "\n";
            }
        }
        db->execute("UPDATE students SET average_score='" + std::to_string(averageScore) + "' WHERE id=" + std::to_string(studentId));
        return;
    }
}

void deleteStudent()
{
    bool found = false;
    db->executeWithCallback("SELECT 1 FROM students", [&](const std::vector<std::string> &)
                            { found = true; });
    if (!found) {
        throw NotFoundException("Студентов для удаления нет");
    }
    int studentId;
    while (true)
    {
        try
        {
            studentId = getValidatedInt("Введите ID студента для удаления:\n", 0, 100);
            bool found = false;
            db->executeWithCallback("SELECT 1 FROM students WHERE id=" + std::to_string(studentId), [&](const std::vector<std::string> &)
                                    { found = true; });
            if (!found)
            {
                throw NotFoundException("Студент не найден");
            }
            break;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    db->execute("DELETE FROM students WHERE id=" + std::to_string(studentId));
}

int main()
{
    db->execute("SET client_min_messages = warning");
    db->initializeSchema();
    while (true)
    {
        try
        {
            std::cout << "=== Список студентов ===\n";
            displayList();
            std::cout << "1. Добавить студента\n2. Редактировать студента\n3. Удалить студента\n0. Выход\n";
            int step;
            while (true)
            {
                try
                {
                    step = getValidatedInt("Выберите действие:\n", 0, 3);
                    break;
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            if (step == 0)
            {
                break;
            }
            else if (step == 1)
            {
                addStudent();
            }
            else if (step == 2)
            {
                editStudent();
            }
            else if (step == 3)
            {
                deleteStudent();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    db->destroyInstance();
    return 0;
}
