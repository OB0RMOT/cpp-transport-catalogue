#include "input_reader.h"

using namespace std::literals;
using namespace std;

namespace transport_catalogue::input_reader {

	void LoadQueries(std::istream& stream, TransportCatalogue& transport_catalogue) {

		string tmp = ""s;					

		while (getline(stream, tmp))				
		{
			if (!isdigit(tmp[0]))			// ожидание строки с кол-вом запросов N.
			{
				continue;
			}
			vector<string> queries(move(StreamSectionToVector(stoi(tmp), stream))); // создаем вектор запросов и инициализируем одной секцией (секция - это N запросов после получения кол-ва запросов N).
			for (string query : queries)										
			{
				if (query[0] == 'S')												// если запрос на добавление или получение информации о остановке:
				{
					// поиск двоиточия.
					if (query.find(':') != NPOS)									// если двоеточие найдено, то запрос на добавление остановки.
					{
						transport_catalogue.AddStop(StringToStop(query));			
					}
				}
			}
			for (string query : queries)										
			{
				if (query[0] == 'S' && count(query.begin(), query.end(), ',') > 1)	// если кол-во запятых больше 1, запрос на добавление соседних остановок и дистанций до них.
				{
					transport_catalogue.AddStopDistances(query);					
				}
			}
			for (string query : queries)										
			{
				if (query[0] == 'B')												// если запрос на добавление или получение маршрута:
				{
					auto colon = query.find(':');									// поиск двоиточия.
					if (colon != NPOS)												// если двоеточие найдено, то запрос на добавление маршрута.
					{
						auto [name, is_circle, stops] = StringToBus(query, colon);	// получение и декомпозиция кортежа маршрута в переменные.
						transport_catalogue.AddBus(name, is_circle, stops);			
					}
				}
			}

			for (string query : queries)
			{
				auto colon = query.find(':');								// поиск двоиточия.
				if (colon == NPOS)											// если двоеточия не найдено, запросы на получение информации.
				{
					if (query[0] == 'S')
					{
						// удаляем из строки запрос, оставляя только запрашиваемое имя и вызываем функцию вывода информации.
						transport_catalogue::stat_reader::PutStopInfo(query.erase(0, query.find(' ') + 1), transport_catalogue);
					}
					else if (query[0] == 'B')
					{
						transport_catalogue::stat_reader::PutBusInfo(query.erase(0, query.find(' ') + 1), transport_catalogue);
					}
				}
			}
		}
	}

	Stop StringToStop(string& str)
	{
		auto colon = str.find(':');					
		Stop stop;
		stop.name = str;							
		stop.name.erase(colon);						// удаление всех символов справа от двоеточия, включая само двоеточие.
		auto space = stop.name.find_first_of(' ');  // поиск пробела.
		stop.name.erase(0, space + 1);				// удаление с начала строки до начала имени.

		string coordinate = str;
		coordinate.erase(0, colon + 2);				// удаление с начала строки до двоеточия включительно, так же удаляем пробел после двоеточия.
		coordinate.erase(coordinate.find(','));		// удаление элементов с запятой до конца строки.
		stop.coordinates.lat = stod(coordinate);	// привидение string к double (широта).
													// аналогичная обработка и добавление долготы.
		coordinate = str;
		coordinate.erase(0, coordinate.find_first_of(',') + 2);
		auto comma = coordinate.find(',');
		if (comma != NPOS)
		{
			coordinate.erase(comma);
		}

		stop.coordinates.lng = stod(coordinate);	// привидение string к double (долгота).

		return stop;
	}

	stob StringToBus(string& str, size_t& colon)
	{
		string bus_name = str;
		bus_name.erase(colon);						// удаление элементов начиная с двоеточия и до конца.
		auto space = bus_name.find_first_of(' ');	// поиск пробела.
		bus_name.erase(0, ++space);					// удаление от начала до первого пробела включительно.

		char separator;								// тип разделителя между именами остановок маршрута.
		bool is_circle;
		list<string> stops;
		if (str.find('>') != NPOS)					// если найден символ '>',
		{
			is_circle = true;						// маршрут кольцевой.
			separator = '>';
		}
		else										// иначе маршрут последовательный.
		{
			is_circle = false;
			separator = '-';
		}

		str.erase(0, colon + 2);											// удаление с начала строки до пробела включительно (до начала имени первой остановки).
		while (str.find(separator) != NPOS)									// пока в строке присутствует разделитель,
		{
			auto begin_pos = str.find_last_of(separator) + 2;				// поиск позиции первого символа имени остановки.
			auto end_pos = str.size();										// получение конца строки.
			stops.push_front(str.substr(begin_pos, end_pos - begin_pos));	// добавление остановки в список остановок.
			str.erase(begin_pos - 3);										// удаление добавленной остановки.
		}
		stops.push_front(str);												// добавление оставшейся остановки.

		return tuple{ bus_name, is_circle, stops };
	}

	vector<string> StreamSectionToVector(const int& queries_count, std::istream& stream)
	{
		vector<string> queries(queries_count);	// вектор для секции запросов.
		string query;
		for (int i = 0; i < queries_count; ++i) // перебираем запросы секции.
		{
			getline(stream, query);				// получение текущего запроса.
			queries.push_back(query);			// добавление в вектор запросов.
		}

		return queries;
	}
}