#include "input_reader.h"

using namespace std::literals;
using namespace std;

namespace transport_catalogue::input_reader {

	// функция чтения и разбора запросов из потока.
	vector<pair<char, string>> LoadQueries(std::istream& stream, TransportCatalogue& transport_catalogue) {

		string tmp = ""s;					// строка для текущего запроса.
		vector<pair<char, string>> output_queries; // вектор запросов на вывод (тип запроса и сам запрос).

		while (getline(stream, tmp))				// пока поток не пустой.
		{
			if (tmp == ""s)
			{
				break; ////
			}
			// получение строки запроса.
			if (!isdigit(tmp[0]))			// ожидание строки с кол-вом запросов N.
			{
				continue;
			}
			vector<string> queries = move(StreamSectionToVector(stoi(tmp), stream)); // создаем вектор запросов и инициализируем одной секцией (секция - это N запросов после получения кол-ва запросов N).

			for (string query : queries)										
			{
				if (query[0] == 'S')											// если запрос на добавление или получение маршрута:
				{
													// поиск двоиточия.
					if (query.find(':') != NPOS)									// если двоеточие найдено, то запрос на добавление маршрута.
					{
						transport_catalogue.AddStop(StringToStop(query));
					}
				}
			}
			for (string query : queries)										// перебор вектора запросов.
			{
				if (query[0] == 'S' && count(query.begin(), query.end(), ',') > 1)
				{
					StringToStopDistances(query, transport_catalogue);
				}
			}
			for (string query : queries)										// перебор вектора запросов.
			{
				if (query[0] == 'B')											// если запрос на добавление или получение маршрута:
				{
					auto colon = query.find(':');								// поиск двоиточия.
					if (colon != NPOS)									// если двоеточие найдено, то запрос на добавление маршрута.
					{
						auto [name, is_circle, stops] = StringToBus(query, colon);	// получение и декомпозиция кортежа маршрута в переменные.
						transport_catalogue.AddBus(name, is_circle, stops);			// добавление маршрута в базу.
					}
				}
			}
			for (string query : queries)
			{
				auto colon = query.find(':');
				if (colon == NPOS)
				{
					if (query[0] == 'S')
					{
						output_queries.push_back({ 'S', query.erase(0, query.find(' ') + 1) });
					}
					else if (query[0] == 'B')
					{
						output_queries.push_back({ 'B', query.erase(0, query.find(' ') + 1) });
					}
				}
			}
		}
		return output_queries;
	}

	// функция преобразования строки в остановку.
	Stop StringToStop(string& str)
	{
		auto colon = str.find(':');					// сохранение результата поиска двоеточия.
		Stop stop;
		stop.name = str;							// копирование строки в структуру stop для дальнейшей обработки.
		stop.name.erase(colon);						// удаление всех символов справа от двоеточия, включая само двоеточие.
		auto space = stop.name.find_first_of(' ');  // поиск пробела.
		stop.name.erase(0, space + 1);				// удаление с начала строки до начала имени.

		string coordinate = str;
		coordinate.erase(0, colon + 2);				// удаление с начала строки до двоеточия включительно.
		coordinate.erase(coordinate.find(','));		// удаление элементов с запятой до конца строки.
		stop.coordinates.lat = stod(coordinate);	// привидение string к double (широта).

		coordinate = str;
		coordinate.erase(0, coordinate.find_first_of(',') + 2);
		auto comma = coordinate.find(',');
		if (comma != NPOS)
		{
			coordinate.erase(comma);
		}
		
		stop.coordinates.lng = stod(coordinate);			// привидение string к double (долгота).

		return stop;
	}

	// функция преобразования строки в маршрут.
	stob StringToBus(string& str, size_t& colon)
	{
		string bus_name = str;
		bus_name.erase(colon);						// удаление элементов начиная с двоеточия и до конца.
		auto space = bus_name.find_first_of(' ');	// поиск пробела.
		bus_name.erase(0, ++space);					// удаление от начала до первого пробела включительно.

		char separator;
		bool is_circle;
		list<string> stops;
		if (str.find('>') != NPOS)	// если найден символ '>',
		{
			is_circle = true;				// маршрут кольцевой.
			separator = '>';
		}
		else								// иначе маршрут последовательный.
		{
			is_circle = false;
			separator = '-';
		}

		str.erase(0, colon + 2);						// удаление с начала строки до пробела включительно (до начала имени первой остановки).
		while (str.find(separator) != NPOS)		// пока в строке присутствует разделитель,
		{
			auto begin_pos = str.find_last_of(separator) + 2;				// поиск позиции первого символа имени остановки.
			auto end_pos = str.size();										// получение конца строки.
			stops.push_front(str.substr(begin_pos, end_pos - begin_pos));	// добавление остановки в список остановок.
			str.erase(begin_pos - 3);										// удаление добавленной остановки.
		}
		stops.push_front(str);							// добавление оставшейся остановки.

		return tuple{ bus_name, is_circle, stops };
	}

	void StringToStopDistances(string& str, TransportCatalogue& transport_catalogue)
	{
		string first_stop_name = str;
		first_stop_name.erase(0, first_stop_name.find_first_of(' ') + 1);
		first_stop_name.erase(first_stop_name.find(':'));
		
		string distances = str;
		distances.erase(0, distances.find_first_of(',') + 2);
		distances.erase(0, distances.find_first_of(',') + 2);
		string distance;
		while (true)
		{
			distance = distances;
			auto comma = distance.find(',');
			if (comma != NPOS)
			{
				distance.erase(comma);
				string second_stop_name = distance;
				second_stop_name.erase(0, second_stop_name.find_first_of('m') + 5);
				distance.erase(distance.find_first_of('m'));
				transport_catalogue.AddStopDistances(first_stop_name, second_stop_name, stoi(distance));
				distances.erase(0, distances.find_first_of(',') + 2);
			}
			else
			{
				string second_stop_name = distance;
				second_stop_name.erase(0, second_stop_name.find_first_of('m') + 5);
				distance.erase(distance.find_first_of('m'));
				transport_catalogue.AddStopDistances(first_stop_name, second_stop_name, stoi(distance));

				break;
			}
		}
	}

	// функция чтения и разбиения запросов одной секции на вектор строк.
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