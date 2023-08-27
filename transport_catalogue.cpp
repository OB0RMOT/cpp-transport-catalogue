#include "transport_catalogue.h"
using namespace std;

namespace transport_catalogue {
	namespace detail {
		// функция чтения и разбиения запросов одной секции на вектор строк.
		std::vector<std::string> StreamSectionToVector(const int& queries_count, std::stringstream& stream)
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

	void TransportCatalogue::AddBus(const string& bus_name, bool is_circle, list<string>& stops_names) {
		Bus bus;
		bus.name = bus_name;
		bus.is_circle = is_circle;
		vector<Stop*> stops;
		for (auto it = stops_names.begin(); it != stops_names.end(); ++it)
		{
			Stop* stop = stops_map_.at(*it);
			stops.push_back(stop);
			bus.stops.push_back(stop);
		}
		buses_[bus_name] = bus;
		for (Stop* stop : stops)
		{
			stops_to_buses_[stop].push_back(&buses_.at(bus_name));
		}
	}

	void TransportCatalogue::AddStopDistances(const std::string& first_stop_name, const std::string& second_stop_name, const int& distance)
	{
		distances_.insert({ {stops_map_.at(first_stop_name), stops_map_.at(second_stop_name)}, distance });
	}

	void TransportCatalogue::AddStop(const Stop& stop) {
		stops_.push_back(stop);
		stops_map_[string_view(stops_.back().name)] = &stops_.back();
		stops_to_buses_[&stops_.back()];
	}

	Bus* TransportCatalogue::FindBus(string_view name) {
		if (!buses_.count(string(name)))
		{
			return nullptr;
		}
		return &buses_.at(string(name));
	}

	Stop* TransportCatalogue::FindStop(string_view name) {
		return stops_map_.at(name);
	}

	BusInfo TransportCatalogue::GetBusInfo(string_view name) {
		if (buses_.count(string(name)) == 0)
		{
			static BusInfo empty;
			return empty;
		}

		Bus bus = buses_.at(string(name));

		int stops_on_route = static_cast<int>(bus.stops.size());
		unordered_set<string> unique_stops_names;
		int distance = 0;
		double coordinates_distance = 0;
		for (auto it = bus.stops.begin(); it != bus.stops.end();)
		{
			auto first_stop = *it;
			unique_stops_names.insert(first_stop->name);
			it++;
			
			if (it != bus.stops.end())
			{
				auto second_stop = *it;
				coordinates_distance += geo::ComputeDistance(first_stop->coordinates, second_stop->coordinates);
				if (distances_.count({ first_stop, second_stop }))
				{
					int distance_to_next_stop = distances_.at({ first_stop, second_stop });
					distance += distance_to_next_stop;
				}
				else
				{
					distance += distances_.at({ second_stop, first_stop });
				}
			}
		}
		if (!bus.is_circle)
		{
			stops_on_route = stops_on_route * 2 - 1;
			for (int i = bus.stops.size() - 1; i >= 0; --i)
			{
				auto first_stop = bus.stops[i];
				unique_stops_names.insert(first_stop->name);
				if (i != 0)
				{
					auto second_stop = bus.stops[i - 1];
					coordinates_distance += geo::ComputeDistance(first_stop->coordinates, second_stop->coordinates);
					if (distances_.count({ first_stop, second_stop }))
					{
						int distance_to_next_stop = distances_.at({ first_stop, second_stop });
						distance += distance_to_next_stop;
					}
					else
					{
						distance += distances_.at({ second_stop, first_stop });
					}
				}
			}
		}

		return { stops_on_route, static_cast<int>(unique_stops_names.size()), distance, distance / coordinates_distance };
	}

	tuple<set<string>, bool> TransportCatalogue::GetStopInfo(const string& name)
	{
		if (stops_map_.count(name) == 0)
		{
			return { {}, false };
		}
		set<string> result;
		Stop* stop = stops_map_.at(name);
		for (Bus* bus : stops_to_buses_.at(stop))
		{
			result.insert(bus->name);
		}
		return { result, true };
	}
}