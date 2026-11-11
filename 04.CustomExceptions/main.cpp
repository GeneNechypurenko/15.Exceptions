#include <iostream>
#include <string>
#include <stdexcept>
#include <windows.h>

using namespace std;

/*
==========================================================================================================================================
                                           Власна ієрархія класів винятків. Порядок catch
==========================================================================================================================================

  Власний виняток — звичайний клас, успадкований від стандартного (зазвичай runtime_error або logic_error).
  Текст передається в конструктор базового класу, тож what() працює без додаткового коду:
      class AcademyError : public runtime_error
      {
      public:
          explicit AcademyError(const string& message) : runtime_error(message) {}
      };
  Нащадки можуть мати власні поля з деталями помилки:  InvalidGradeError зберігає саму неправильну оцінку.

  Навіщо: помилку можна ловити на будь-якому рівні — конкретну, будь-яку помилку своєї програми або будь-яку взагалі.

  ПОРЯДОК catch: catch базового класу ловить і всіх нащадків, тому порядок — ВІД КОНКРЕТНОГО ДО ЗАГАЛЬНОГО:
      catch (const InvalidGradeError& e)  // 1) найконкретніший
      catch (const AcademyError& e)       // 2) його базовий клас
      catch (const exception& e)          // 3) усі стандартні винятки
  Якщо поставити базовий клас першим, нижчі catch ніколи не спрацюють (Visual Studio про це попереджає).

*/

// Базовий клас для всіх помилок нашої програми
class AcademyError : public runtime_error
{
public:
    explicit AcademyError(const string& message) : runtime_error(message)   // текст зберігає runtime_error
    {}
};

// Конкретна помилка з власним полем
class InvalidGradeError : public AcademyError
{
private:
    int grade;

public:
    explicit InvalidGradeError(int grade)
        : AcademyError("Оцінки " + to_string(grade) + " не існує (шкала 1-12)"), grade(grade)
    {}

    int getGrade() const
    {
        return grade;
    }
};

class NotAdmittedError : public AcademyError
{
public:
    explicit NotAdmittedError(const string& name)
        : AcademyError(name + ": не допущено до іспиту (середній бал нижчий за 7)")
    {}
};

class Student
{
private:
    string name;
    double gradeSum;
    int gradeCount;

public:
    Student(const string& name) : name(name), gradeSum(0), gradeCount(0)
    {}

    void addGrade(int grade)
    {
        if (grade < 1 || grade > 12)
        {
            throw InvalidGradeError(grade);   // власний виняток
        }
        gradeSum += grade;
        gradeCount++;
    }

    void admitToExam() const
    {
        if (gradeCount == 0 || gradeSum / gradeCount < 7)
        {
            throw NotAdmittedError(name);
        }
        cout << name << ": допущено до іспиту" << endl;
    }
};

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Student olena("Олена Коваль");
    olena.addGrade(6);
    olena.addGrade(7);

    cout << "1. Спрацьовує найконкретніший відповідний catch" << endl;
    try
    {
        olena.addGrade(15);                   // InvalidGradeError
    }
    catch (const InvalidGradeError& e)        // спрацьовує: доступне власне поле
    {
        cout << "Неправильна оцінка " << e.getGrade() << ": " << e.what() << endl;
    }
    catch (const AcademyError& e)
    {
        cout << "Помилка академії: " << e.what() << endl;
    }

    cout << endl << "2. Базовий клас ловить нащадка" << endl;
    try
    {
        olena.admitToExam();                  // середній бал 6.5 — NotAdmittedError
    }
    catch (const InvalidGradeError& e)        // пропускається: це не InvalidGradeError
    {
        cout << "Неправильна оцінка " << e.getGrade() << endl;
    }
    catch (const AcademyError& e)             // спрацьовує: NotAdmittedError Є AcademyError
    {
        cout << "Помилка академії: " << e.what() << endl;
    }
    catch (const exception& e)                // будь-яка інша стандартна помилка
    {
        cout << "Інша помилка: " << e.what() << endl;
    }

    return 0;
}

/*
==========================================================================================================================================
                                                  ІНСАЙТИ / ЛАЙФХАКИ / BEST PRACTICES
==========================================================================================================================================
  
  1. ЛАЙФХАК:
     Не знаєте, від якого класу успадкувати свій виняток, — успадковуйте від runtime_error.
     Від exception напряму не успадковуйте: у нього немає конструктора, який приймає текст.

  2. Успадковуйте власні винятки від стандартних. Тоді один catch (const exception& e) ловить і помилки бібліотеки,
     і помилки вашої програми.

  3. Власний клас винятку потрібен, коли помилку треба ловити окремо або передати з нею додаткові дані.
     Якщо досить тексту — беріть стандартний.

  4. Ієрархія винятків — те саме успадкування і той самий поліморфізм, що й у будь-яких класах:
     catch (const AcademyError& e) працює як параметр-посилання на базовий клас.

==========================================================================================================================================
                                                              ПРАКТИКА
==========================================================================================================================================

  Завдання 4 (продовжуємо Wallet).
    - Клас NotEnoughGoldError : public runtime_error:
        поле int missing (скільки бракує); explicit-конструктор NotEnoughGoldError(int missing) з текстом «Не вистачає золота»;
        метод int getMissing() const.
    - spend замість runtime_error кидає NotEnoughGoldError(amount - gold).
  У main:
    - гаманець на 100; в одному try: spend(-5);
    - в іншому try: spend(250);
    - в обох try однакові catch: NotEnoughGoldError — «Бракує <getMissing()> золота», потім exception — «Помилка: » і what().
    - Поміняйте два catch місцями. Що змінилося у виводі? Що пише Visual Studio?
*/
