#include <iostream>
#include <windows.h>
#include <string>
#include <stdexcept>

using namespace std;

/*
==========================================================================================================================================
                                  ДОМАШНЄ ЗАВДАННЯ 35 тема: обробка виняткових ситуацій.
==========================================================================================================================================
*/

class Starship
{
private:
    string name;
    int maxLoad;
    int load;
    int fuel;

public:
    Starship(const string& name, int maxLoad, int fuel) : name(name), maxLoad(maxLoad), load(0), fuel(fuel)
    {
        if (maxLoad <= 0)
        {
            throw invalid_argument("Вантажопідйомність має бути додатною");   // об'єкт не буде створено
        }
    }

    string getName() const
    {
        return name;
    }

    /*
      1. Що таке виняток? Чому loadCargo повідомляє про помилку винятком, а не повертає false?
         Як інакше конструктор міг би повідомити, що корабель створити не можна?
    */
    void loadCargo(const string& item, int weight)
    {
        if (weight <= 0)
        {
            throw invalid_argument("Вага вантажу має бути додатною");
        }
        if (load + weight > maxLoad)
        {
            throw runtime_error("Перевантаження: " + item + " не влазить");
        }
        load += weight;
        cout << "  Завантажено: " << item << " (" << weight << " т), разом " << load << "/" << maxLoad << " т" << endl;
    }

    void launch()
    {
        if (fuel < 50)
        {
            throw runtime_error("Недостатньо пального для старту");
        }
        fuel -= 50;
        cout << name << ": Старт! На борту " << load << " т, залишок пального " << fuel << "%" << endl;
    }
};

class Airlock
{
private:
    string shipName;

public:
    explicit Airlock(const string& shipName) : shipName(shipName)
    {
        cout << "  [шлюз " << shipName << " відкрито]" << endl;
    }

    ~Airlock()
    {
        cout << "  [шлюз " << shipName << " закрито]" << endl;
    }
};

// ==================== ЧАСТИНА 2 ====================

void loadManifest(Starship& ship, const string items[], const int weights[], int count)
{
    Airlock airlock(ship.getName());      // локальний об'єкт: деструктор спрацює за будь-якого виходу з функції
    for (int i = 0; i < count; i++)
    {
        ship.loadCargo(items[i], weights[i]);   // try тут немає: виняток вийде в main, шлюз закриється по дорозі
    }
}

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "===== ЧАСТИНА 1 =====" << endl;
    try
    {
        Starship ghost("Привид", 0, 100);
    }
    catch (const invalid_argument& e)
    {
        cout << "Корабель не створено: " << e.what() << endl;
    }

    Starship dream("Мрія", 100, 60);
    //  2. Як працюють try, throw і catch? У кроці 2 частини 1: що встигло виконатися і чому програма не повернулася в try?
    try
    {
        dream.loadCargo("Продукти", 30);
        dream.loadCargo("Вода", 40);
        dream.loadCargo("Руда", 50);      // перевантаження — старт нижче не виконається
        dream.launch();
    }
    catch (const invalid_argument& e)
    {
        cout << "Некоректні дані: " << e.what() << endl;
    }
    catch (const runtime_error& e)
    {
        cout << "Помилка польоту: " << e.what() << endl;
    }
    cout << "Рейс продовжується" << endl;

    try
    {
        dream.launch();                   // 60% — вистачає
        dream.launch();                   // 10% — ні
    }
    /*
      3. Чому винятки ловлять за константним посиланням (const exception& e), і чому такий catch спіймав
         і invalid_argument, і runtime_error?
    */
    catch (const exception& e)            // один catch для всіх стандартних винятків
    {
        cout << "Помилка: " << e.what() << endl;
    }

    cout << endl << "===== ЧАСТИНА 2 =====" << endl;
    Starship hauler("Вантажник-7", 120, 100);

    string mailItems[] = { "Пошта", "Запчастини" };
    int mailWeights[] = { 10, 20 };
    loadManifest(hauler, mailItems, mailWeights, 2);   // помилок немає — шлюз закривається після циклу

    string heavyItems[] = { "Генератор", "Броня", "Супутник" };
    int heavyWeights[] = { 50, 40, 30 };
    //  4. Що таке розкрутка стека? Чому шлюз закрився, хоча loadManifest завершилася винятком?
    try
    {
        loadManifest(hauler, heavyItems, heavyWeights, 3);   // виняток — шлюз однаково закривається
    }
    catch (const exception& e)
    {
        cout << "Центр управління: " << e.what() << endl;
    }

    return 0;
}

/*

  1. Що таке виняток? Чому loadCargo повідомляє про помилку винятком, а не повертає false?
     Як інакше конструктор міг би повідомити, що корабель створити не можна?
  2. Як працюють try, throw і catch? У кроці 2 частини 1: що встигло виконатися і чому програма не повернулася в try?
  3. Чому винятки ловлять за константним посиланням (const exception& e), і чому такий catch спіймав
     і invalid_argument, і runtime_error?
  4. Що таке розкрутка стека? Чому шлюз закрився, хоча loadManifest завершилася винятком?
 
  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ» (еталон для розбору)

  1. 

  2. 

  3. 

  4. 

*/
