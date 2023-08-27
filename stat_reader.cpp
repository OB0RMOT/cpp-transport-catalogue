#include "stat_reader.h"

using namespace std;

namespace transport_catalogue::stat_reader {

	ostringstream LoadOutputQueries(std::stringstream& stream, TransportCatalogue& transport_catalogue)
	{
		string tmp = ""s;							// строка для текущего запроса.

		ostringstream result;

		while (getline(stream, tmp))				// пока поток не пустой.
		{
			if (tmp == ""s)
			{
				break; ////
			}
			if (!isdigit(tmp[0]))					// ожидание строки с кол-вом запросов N.
			{
				continue;
			}
			vector<string> queries = move(detail::StreamSectionToVector(stoi(tmp), stream)); // создаем вектор запросов и инициализируем одной секцией (секция - это N запросов после получения кол-ва запросов N).
			for (string query : queries)
			{
				auto colon = query.find(':');
				if (colon == NPOS)
				{
					if (query[0] == 'S')
					{
						stat_reader::PutStopInfo(query.erase(0, query.find(' ') + 1), transport_catalogue, result);
					}
					else if (query[0] == 'B')
					{
						stat_reader::PutBusInfo(query.erase(0, query.find(' ') + 1), transport_catalogue, result);
					}
				}
			}
		}
		return result;
	}

	void PutBusInfo(const string& bus_name, TransportCatalogue& transport_catalogue, ostringstream& stream) {
		BusInfo bus_info = transport_catalogue.GetBusInfo(bus_name);

		if (bus_info.stops_on_route == 0)
		{
			stream << "Bus " << bus_name << ": not found" << endl;
		}
		else
		{
			auto [stops_on_route, unique_stops, route_length, curvature] = bus_info;
			stream << "Bus " << bus_name << ": " << stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length, " << curvature << " curvature" << endl;
		}
	}

	void PutStopInfo(const string& stop_name, TransportCatalogue& transport_catalogue, ostringstream& stream) {
		auto [buses, stop_find] = transport_catalogue.GetStopInfo(stop_name);
		if (!stop_find)
		{
			stream << "Stop " << stop_name << ": not found" << endl;
		}
		else if (buses.empty())
		{
			stream << "Stop " << stop_name << ": no buses" << endl;
		}
		else
		{
			stream << "Stop " << stop_name << ": buses";
			for (const string& bus : buses)
			{
				stream << " " << bus;
			}
			stream << endl;
		}
	}
}
