#include "stat_reader.h"

using namespace std;

namespace transport_catalogue::stat_reader {

	ostringstream GetInfoOnQueries(vector<pair<char, string>>& output_queries, TransportCatalogue& transport_catalogue)
	{
		ostringstream result;
		for (auto [type, str] : output_queries)
		{
			if (type == 'S')
			{
				stat_reader::PutStopInfo(str, transport_catalogue, result);
			}
			else
			{
				stat_reader::PutBusInfo(str, transport_catalogue, result);
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
