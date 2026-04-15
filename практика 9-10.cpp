#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

const int SIZE = 20;
const int MAX_DEST_LEN = 50;
enum FlightType {
    CHARTER,
    TRANSIT,
    LAYOVER
};

struct Time {
    int hours;
    int minutes;
};


struct Flight {
    char destination[MAX_DEST_LEN];
    Time departure;
    Time arrival;
    int flightTimeMinutes;
    double ticketPrice;
    FlightType type;
};

Flight* initFlights();
const char* flightTypeToStr(FlightType t);
void printFlight(const Flight& f);
void printAllFlights(Flight flights[], int n, const char* title);
bool endsWith(const char* str, const char* suffix);
bool isGradAndTransit(const Flight& f);
int filterFlights(Flight src[], int srcSize, Flight dst[], bool (*filter)(const Flight&));
void swapFlights(Flight& a, Flight& b);
bool compareByFlightTime(const Flight& a, const Flight& b);
void bubbleSortFlights(Flight arr[], int n, bool (*compare)(const Flight&, const Flight&));
void findFlightByDestination(Flight flights[], int n, const char* dest);
void print5NightCharter(Flight flights[], int n);
bool modifyFlight(Flight flights[], int n, const char* dest, double newPrice, int newFlightTime);
bool isLayoverCheaperThan(const Flight& f, double maxPrice);
void showFiltered(Flight flights[], int n, const char* filterName);
void writePricesToTextFile(const char* filename);
void readAndUpdatePrices(Flight flights[], int n, const char* filename);
void saveFlightsToBinary(const char* filename, Flight flights[], int n);
void loadFlightsFromBinary(const char* filename, Flight flights[], int n);

Flight* initFlights() {
    Flight* flights = new Flight[SIZE];

    flights[0] = { "Москва",       {10, 30}, {12, 45}, 135, 6500.0,  CHARTER };
    flights[1] = { "Петербург",    {14, 15}, {16, 20}, 125, 4800.0,  TRANSIT };
    flights[2] = { "Калининград",  {8, 0},   {10, 10}, 130, 7200.0,  TRANSIT };
    flights[3] = { "Волгоград",    {22, 10}, {0, 30},  140, 5300.0,  CHARTER };
    flights[4] = { "Краснодар",    {5, 45},  {7, 55},  130, 6100.0,  LAYOVER };
    flights[5] = { "Екатеринбург", {13, 20}, {15, 40}, 140, 8900.0,  CHARTER };
    flights[6] = { "Новосибирск",  {18, 30}, {21, 10}, 160, 10500.0, TRANSIT };
    flights[7] = { "Омск",         {9, 15},  {11, 30}, 135, 7800.0,  LAYOVER };
    flights[8] = { "Челябинск",    {16, 40}, {18, 55}, 135, 6900.0,  CHARTER };
    flights[9] = { "Казань",       {12, 0},  {13, 45}, 105, 5200.0,  TRANSIT };
    flights[10] = { "Сочи",         {21, 50}, {23, 55}, 125, 9800.0,  LAYOVER };
    flights[11] = { "Уфа",          {7, 20},  {9, 30},  130, 6700.0,  TRANSIT };
    flights[12] = { "Самара",       {11, 10}, {13, 25}, 135, 7100.0,  CHARTER };
    flights[13] = { "Ростов-на-Дону",{15, 5}, {17, 0},  115, 6000.0,  LAYOVER };
    flights[14] = { "Красноярск",   {2, 30},  {5, 10},  160, 11200.0, CHARTER };
    flights[15] = { "Владивосток",  {19, 45}, {22, 50}, 185, 14800.0, TRANSIT };
    flights[16] = { "Иркутск",      {6, 10},  {8, 40},  150, 9500.0,  LAYOVER };
    flights[17] = { "Хабаровск",    {4, 0},   {6, 50},  170, 12200.0, CHARTER };
    flights[18] = { "Тюмень",       {17, 20}, {19, 5},  105, 6400.0,  TRANSIT };
    flights[19] = { "Симферополь",  {10, 45}, {12, 30}, 105, 7500.0,  LAYOVER };

    return flights;
}

const char* flightTypeToStr(FlightType t) {
    switch (t) {
    case CHARTER: return "Чартер";
    case TRANSIT: return "Транзит";
    case LAYOVER: return "Стыковка";
    default:      return "Неизвестно";
    }
}

void printFlight(const Flight& f) {
    printf(" %-18s | %02d:%02d - %02d:%02d | %3d мин | %8.2f руб | %s\n",
        f.destination,
        f.departure.hours, f.departure.minutes,
        f.arrival.hours, f.arrival.minutes,
        f.flightTimeMinutes,
        f.ticketPrice,
        flightTypeToStr(f.type));
}

void printAllFlights(Flight flights[], int n, const char* title) {
    printf("\n=================== %s ===================\n", title);
    printf(" %-18s |   Отправление -> Прибытие   | Время |   Цена    | Тип\n");
    printf("----------------------------------------------------------------------\n");
    for (int i = 0; i < n; ++i) {
        printFlight(flights[i]);
    }
}

// Проверка, оканчивается ли строка на заданный суффикс
bool endsWith(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    size_t lenStr = strlen(str);
    size_t lenSuffix = strlen(suffix);
    if (lenSuffix > lenStr) return false;
    return strcmp(str + lenStr - lenSuffix, suffix) == 0;
}

// Фильтр: пункт назначения оканчивается на "град" и рейс транзитный
bool isGradAndTransit(const Flight& f) {
    return endsWith(f.destination, "град") && f.type == TRANSIT;
}

// Универсальная функция фильтрации
int filterFlights(Flight src[], int srcSize, Flight dst[], bool (*filter)(const Flight&)) {
    int cnt = 0;
    for (int i = 0; i < srcSize; ++i) {
        if (filter(src[i])) {
            dst[cnt++] = src[i];
        }
    }
    return cnt;
}

// Обмен двух структур Flight
void swapFlights(Flight& a, Flight& b) {
    Flight temp = a;
    a = b;
    b = temp;
}

// Сравнение для сортировки по возрастанию времени полёта
bool compareByFlightTime(const Flight& a, const Flight& b) {
    return a.flightTimeMinutes > b.flightTimeMinutes;
}

// Сортировка пузырьком
void bubbleSortFlights(Flight arr[], int n, bool (*compare)(const Flight&, const Flight&)) {
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (compare(arr[j], arr[j + 1])) {
                swapFlights(arr[j], arr[j + 1]);
            }
        }
    }
}

// Поиск рейса по пункту назначения и вывод информации
void findFlightByDestination(Flight flights[], int n, const char* dest) {
    for (int i = 0; i < n; ++i) {
        if (strcmp(flights[i].destination, dest) == 0) {
            printf("\n=== Найден рейс: %s ===\n", dest);
            printFlight(flights[i]);
            return;
        }
    }
    printf("\nРейс в пункт назначения \"%s\" не найден.\n", dest);
}

// Вывод 5 чартерных рейсов, вылетающих ночью (00:00 – 05:59)
void print5NightCharter(Flight flights[], int n) {
    printf("\n=== Чартерные рейсы, вылетающие ночью (первые 5) ===\n");
    printf(" %-18s |   Отправление -> Прибытие   | Время |   Цена    | Тип\n");
    printf("----------------------------------------------------------------------\n");
    int count = 0;
    for (int i = 0; i < n && count < 5; ++i) {
        if (flights[i].type == CHARTER && flights[i].departure.hours >= 0 && flights[i].departure.hours < 6) {
            printFlight(flights[i]);
            ++count;
        }
    }
    if (count == 0) {
        printf("Нет подходящих рейсов.\n");
    }
}

// Изменение рейса по пункту назначения (цена и время полёта)
bool modifyFlight(Flight flights[], int n, const char* dest, double newPrice, int newFlightTime) {
    for (int i = 0; i < n; ++i) {
        if (strcmp(flights[i].destination, dest) == 0) {
            flights[i].ticketPrice = newPrice;
            flights[i].flightTimeMinutes = newFlightTime;
            printf("\nРейс \"%s\" изменён: новая цена %.2f руб, время полёта %d мин.\n", dest, newPrice, newFlightTime);
            return true;
        }
    }
    printf("\nРейс \"%s\" не найден для изменения.\n", dest);
    return false;
}

// Фильтр: стыковочный рейс и стоимость меньше maxPrice
bool isLayoverCheaperThan(const Flight& f, double maxPrice) {
    return f.type == LAYOVER && f.ticketPrice < maxPrice;
}

// Обёрточная функция для вывода отфильтрованных данных
void showFiltered(Flight flights[], int n, const char* filterName) {
    printf("\n=== Отфильтровано: %s (%d рейсов) ===\n", filterName, n);
    if (n == 0) {
        printf("Нет рейсов, удовлетворяющих условию.\n");
        return;
    }
    printf(" %-18s |   Отправление -> Прибытие   | Время |   Цена    | Тип\n");
    printf("----------------------------------------------------------------------\n");
    for (int i = 0; i < n; ++i) {
        printFlight(flights[i]);
    }
}

// Создание текстового файла с ценами (пункт назначения – цена)
void writePricesToTextFile(const char* filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Ошибка создания текстового файла " << filename << endl;
        return;
    }
    out << "Москва 7000.50\n";
    out << "Петербург 5000.00\n";
    out << "Калининград 7500.00\n";
    out << "Волгоград 5400.00\n";
    out << "Краснодар 6200.00\n";
    out << "Екатеринбург 9000.00\n";
    out << "Новосибирск 10800.00\n";
    out << "Омск 7900.00\n";
    out << "Челябинск 7000.00\n";
    out << "Казань 5300.00\n";
    out << "Сочи 9900.00\n";
    out << "Уфа 6800.00\n";
    out << "Самара 7200.00\n";
    out << "Ростов-на-Дону 6100.00\n";
    out << "Красноярск 11400.00\n";
    out << "Владивосток 15000.00\n";
    out << "Иркутск 9600.00\n";
    out << "Хабаровск 12300.00\n";
    out << "Тюмень 6500.00\n";
    out << "Симферополь 7600.00\n";
    out.close();
    cout << "Текстовый файл " << filename << " создан.\n";
}

void readAndUpdatePrices(Flight flights[], int n, const char* filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cout << "Ошибка открытия текстового файла " << filename << endl;
        return;
    }
    char dest[MAX_DEST_LEN];
    double price;
    int updated = 0;
    while (in >> dest >> price) {
        for (int i = 0; dest[i] != '\0'; ++i) {
            if (dest[i] == '_') dest[i] = ' ';
        }
        for (int i = 0; i < n; ++i) {
            if (strcmp(flights[i].destination, dest) == 0) {
                flights[i].ticketPrice = price;
                ++updated;
                break;
            }
        }
    }
    in.close();
    cout << "Обновлено цен для " << updated << " рейсов из файла " << filename << ".\n";
}

// Запись массива рейсов в бинарный файл
void saveFlightsToBinary(const char* filename, Flight flights[], int n) {
    ofstream out(filename, ios::binary | ios::out);
    if (!out) {
        cout << "Ошибка записи в бинарный файл " << filename << endl;
        return;
    }
    out.write(reinterpret_cast<const char*>(flights), sizeof(Flight) * n);
    out.close();
    cout << "Данные сохранены в бинарный файл " << filename << ".\n";
}
void loadFlightsFromBinary(const char* filename, Flight flights[], int n) {
    ifstream in(filename, ios::binary | ios::in);
    if (!in) {
        cout << "Ошибка чтения из бинарного файла " << filename << endl;
        return;
    }
    in.read(reinterpret_cast<char*>(flights), sizeof(Flight) * n);
    in.close();
    cout << "Данные загружены из бинарного файла " << filename << ".\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    Flight* flights = initFlights();
    //Вывод всех рейсов
    printAllFlights(flights, SIZE, "ИСХОДНЫЙ МАССИВ (20 РЕЙСОВ)");
    //Фильтрация: пункт назначения оканчивается на "град" и тип транзитный
    Flight gradTransit[SIZE];
    int gradCount = filterFlights(flights, SIZE, gradTransit, isGradAndTransit);
    printAllFlights(gradTransit, gradCount, "РЕЙСЫ С ОКОНЧАНИЕМ \"град\" И ТИПОМ \"ТРАНЗИТ\"");
    //Сортировка отфильтрованного массива по времени полёта (пузырёк)
    bubbleSortFlights(gradTransit, gradCount, compareByFlightTime);
    printAllFlights(gradTransit, gradCount, "ОТСОРТИРОВАНО ПО ВРЕМЕНИ ПОЛЁТА (возрастание)");
    //Поиск конкретного рейса по пункту назначения
    findFlightByDestination(flights, SIZE, "Калининград");
    findFlightByDestination(flights, SIZE, "Лондон");   // несуществующий
    print5NightCharter(flights, SIZE);
    //Изменение рейса (например, для "Москва")
    modifyFlight(flights, SIZE, "Москва", 6999.99, 140);
    findFlightByDestination(flights, SIZE, "Москва"); // проверка
    //Фильтрация стыковочных рейсов со стоимостью менее N (N = 10000)
    Flight cheapLayover[SIZE];
    const double maxPrice = 10000.0;
    int cheapCount = 0;
    for (int i = 0; i < SIZE; ++i) {
        if (isLayoverCheaperThan(flights[i], maxPrice)) {
            cheapLayover[cheapCount++] = flights[i];
        }
    }
    showFiltered(cheapLayover, cheapCount, "СТЫКОВОЧНЫЕ РЕЙСЫ ДЕШЕВЛЕ 10000 РУБ");
    cout << "\n=================== РАБОТА С ТЕКСТОВЫМ ФАЙЛОМ ===================\n";
    //Создание текстового файла с ценами
    writePricesToTextFile("prices.txt");

    readAndUpdatePrices(flights, SIZE, "prices.txt");
    printAllFlights(flights, SIZE, "ПОСЛЕ ОБНОВЛЕНИЯ ЦЕН ИЗ ТЕКСТОВОГО ФАЙЛА");
    cout << "\n=================== РАБОТА С БИНАРНЫМ ФАЙЛОМ ===================\n";
    //Запись в бинарный файл
    saveFlightsToBinary("flights.bin", flights, SIZE);

    Flight* loadedFlights = new Flight[SIZE];
    loadFlightsFromBinary("flights.bin", loadedFlights, SIZE);
    // Проверка прочитанных данных
    printAllFlights(loadedFlights, SIZE, "ПРОЧИТАНО ИЗ БИНАРНОГО ФАЙЛА");

    // Очистка памяти
    delete[] flights;
    delete[] loadedFlights;

    return 0;
}