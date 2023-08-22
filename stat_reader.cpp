#include "stat_reader.h"

using namespace std;

namespace transport_catalogue::stat_reader {

	void PutBusInfo(const string& bus_name, TransportCatalogue& transport_catalogue) {
		BusInfo bus_info = transport_catalogue.GetBusInfo(bus_name);

		if (bus_info.stops_on_route == 0)
		{
			cout << "Bus " << bus_name << ": not found" << endl;
		}
		else
		{
			auto [stops_on_route, unique_stops, route_length, curvature] = bus_info;
			cout << "Bus " << bus_name << ": " << stops_on_route << " stops on route, " << unique_stops << " unique stops, " << route_length << " route length, " << curvature << " curvature" << endl;
		}
	}

	void PutStopInfo(const string& stop_name, TransportCatalogue& transport_catalogue) {
		auto [buses, stop_find] = transport_catalogue.GetStopInfo(stop_name);
		if (!stop_find)
		{
			cout << "Stop " << stop_name << ": not found" << endl;
		}
		else if (buses.empty())
		{
			cout << "Stop " << stop_name << ": no buses" << endl;
		}
		else
		{
			cout << "Stop " << stop_name << ": buses";
			for (const string& bus : buses)
			{
				cout << " " << bus;
			}
			cout << endl;
		}
	}
}