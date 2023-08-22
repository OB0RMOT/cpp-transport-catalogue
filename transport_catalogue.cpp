#include "transport_catalogue.h"
using namespace std;
 
namespace transport_catalogue {

	void TransportCatalogue::AddBus(const string& bus_name, bool is_circle, list<string>& stops_names) {
		Bus bus;
		bus.name = bus_name;
		bus.is_circle = is_circle;
		vector<Stop*> stops;
		for (auto it = stops_names.begin(); it != stops_names.end();) // ������������ �� ������ ��������� ��������.
		{

			auto old_it = it;						
			Stop* stop = stops_map_.at(*old_it);	// ��������� ��������� �� ��������� �� �����.				
			it++;
			if (it == stops_names.end())			
			{
				bus.end_stop = *old_it;				
				bus.stops.push_back({ stop , 0 });	// �.�. it == end, ��������� ��������� ����, ����������� ���������� = 0.
			}
			else
			{
				bus.stops.push_back({ stop, ComputeDistance(stop->coordinates, stops_map_.at(*it)->coordinates) }); // ���������� ��������� �� ��������� � ������� �������������� ���������� �� ���������.
			}

		}
		buses_[bus_name] = bus;
		for (Stop* stop : stops)
		{
			stops_to_buses_[stop].push_back(&buses_.at(bus_name)); // ���������� �������� �������� � ����������, ����� ������� �������� ���� �������.
		}
	}

	void TransportCatalogue::AddStopDistances(const string& query)
	{
		string stop_name = query;
		stop_name.erase(0, stop_name.find_first_of(' ') + 1);			// �������� ������� Stop � ������� ����� ����.
		stop_name.erase(stop_name.find(':'));							// �������� ������ ����� ������, ������� � :, � stop_name �������� ������ ��� ���������.
		Stop* stop = stops_map_.at(stop_name);
		string distances = query;
		distances.erase(0, distances.find_first_of(',') + 2);	
		distances.erase(0, distances.find_first_of(',') + 2);			// �������� ����� ����� ������ �� ������� ������� ������� (��������� �� �������� ���������).
		string distance;
		while (true)
		{
			distance = distances;
			auto comma = distance.find(',');
			if (comma != NPOS)											// ���� ������� �������, ������ � ������ ������ ����� ���������.
			{
				distance.erase(comma);
				string name = distance;
				name.erase(0, name.find_first_of('m') + 5);				// �������� ����� ����� ������ �� ������ ����� �������� ���������.
				distance.erase(distance.find_first_of('m'));			// �������� ������ ������ ������� � m, �������� ������ ���������.
				stop->distances.insert({ stops_map_.at(name)->name, stoi(distance) }); // ���������� string_view �� ��� ��������� � ��������� �� ���.
				distances.erase(0, distances.find_first_of(',') + 2);	// �������� ����������� ��������� � ��������� �� ���.
			}
			else														// ������� �� �������, ������ �������� ��������� ��������� �� �������� ���������, ��������� �� � ������� �� �����.
			{
				string name = distance;
				name.erase(0, name.find_first_of('m') + 5);
				distance.erase(distance.find_first_of('m'));
				stop->distances.insert({ stops_map_.at(name)->name, stoi(distance) });

				break;
			}
		}
	}

	void TransportCatalogue::AddStop(const Stop& stop) {
		stops_.push_back(stop);
		stops_map_[string_view(stops_.back().name)] = &stops_.back();
		stops_to_buses_[&stops_.back()];
	}

	Bus TransportCatalogue::FindBus(string_view name) {
		if (!buses_.count(string(name)))	// ���� �������� � ������������� ������ ����, ���������� ������ ���������.
		{
			static Bus empty_bus;
			return empty_bus;
		}
		return buses_.at(string(name));
	}

	Stop TransportCatalogue::FindStop(string_view name) {
		return *stops_map_.at(name);
	}

	BusInfo TransportCatalogue::GetBusInfo(string_view name) {
		if (buses_.count(string(name)) == 0)	// ���� �������� � ������������� ������ ����, ���������� ������ ���������.
		{
			static BusInfo empty;
			return empty;
		}

		Bus bus = buses_.at(string(name));

		int stops_on_route = static_cast<int>(bus.stops.size());	// ��������� ���-�� ���� ��������� ��������.
		unordered_set<string> unique_stops_names;					// ��������� ��� ���������� ���������.
		int length = 0;												// ����������� ����� ��������.
		double geographical_length = 0;								// �������������� ����� ��������.
		for (auto it = bus.stops.begin(); it != bus.stops.end();)	// ������� ��������� ��������.
		{
			auto [stop, stop_distance] = *it;
			unique_stops_names.insert(stop->name);
			geographical_length += stop_distance;
			it++;

			if (it != bus.stops.end())
			{
				auto [next_stop, _] = *it;

				if (stop->distances.count(next_stop->name))			// ���� ���������� �� ��������� ��������� �������, �������� � �������� ���.
				{
					int distance_to_next_stop = stop->distances.at(next_stop->name);
					length += distance_to_next_stop;
				}
				else												// ����� ����� ���������� �� ��������� ��������� �� ������.
				{
					length += next_stop->distances.at(stop->name);
				}
			}
		}
		if (!bus.is_circle)		// ���� ������� ����������������, ����� �������� �� ����������, �� ��� � �����, �.�. ���������������� ������� ��������� � ���� �� ��������.
		{
			stops_on_route = stops_on_route * 2 - 1;
			for (int i = bus.stops.size() - 1; i >= 0; --i)
			{
				auto [stop, stop_distance] = bus.stops[i];
				unique_stops_names.insert(stop->name);
				geographical_length += stop_distance;
				if (i != 0)
				{
					auto [next_stop, _] = bus.stops[i - 1];
					if (stop->distances.count(next_stop->name))
					{
						int distance_to_next_stop = stop->distances.at(next_stop->name);
						length += distance_to_next_stop;
					}
					else
					{
						length += next_stop->distances.at(stop->name);
					}
				}
			}
		}

		return { stops_on_route, static_cast<int>(unique_stops_names.size()), length, length / geographical_length }; // ��������� curvature �������� ����������� ����� �������� �� ��������������.
	}

	tuple<set<string>, bool> TransportCatalogue::GetStopInfo(const string& name)
	{
		if (stops_map_.count(name) == 0)	// ���� ������������� ��������� ��� � ����, ���������� ������ ������.
		{
			return { {}, false };			// false �����, ����� � ������� ������ ���������� ������, ����� ������������� �� ������������ ���������.
		}
		set<string> result;
		Stop* stop = stops_map_.at(name);
		for (const Bus* bus : stops_to_buses_.at(stop))
		{
			result.insert(bus->name);
		}
		return { result, true };
	}

}