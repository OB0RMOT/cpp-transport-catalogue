#include <iostream>
#include <sstream>

#include "input_reader.h"

using namespace std;

int main()
{
    transport_catalogue::TransportCatalogue transport_catalogue;

    std::stringstream input_stream; // Создаем stringstream

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line == ""s)
        {
            break; ////
        }
        input_stream << line << '\n'; // Записываем ввод в stringstream
    }

    transport_catalogue::input_reader::LoadInputQueries(input_stream, transport_catalogue);
    cout << transport_catalogue::stat_reader::LoadOutputQueries(input_stream, transport_catalogue).str();
}