#include "transport_catalogue.h"
using namespace std;

namespace transport_catalogue {

	void TransportCatalogue::AddBus(const string& bus_name, bool is_circle, list<string>& stops_names) {
		Bus bus;
		bus.name = bus_name;
		bus.is_circle = is_circle;
		vector<Stop*> stops;
		for (auto it = stops_names.begin(); it != stops_names.end();) // итерирование по именам остановок маршрута.
		{

			auto old_it = it;						
			Stop* stop = stops_map_.at(*old_it);	// получение указателя на остановку по имени.				
			it++;
			if (it == stops_names.end())			
			{
				bus.end_stop = *old_it;				
				bus.stops.push_back({ stop , 0 });	// т.к. it == end, следующей остановки нету, добавляется расстояние = 0.
			}
			else
			{
				bus.stops.push_back({ stop, ComputeDistance(stop->coordinates, stops_map_.at(*it)->coordinates) }); // добавление указатель на остановку и считаем географическое расстояние до следующей.
			}

		}
		buses_[bus_name] = bus;
		for (Stop* stop : stops)
		{
			stops_to_buses_[stop].push_back(&buses_.at(bus_name)); // добавление названия маршрута к остановкам, через которые проходит этот маршрут.
		}
	}

	void TransportCatalogue::AddStopDistances(const string& query)
	{
		string stop_name = query;
		stop_name.erase(0, stop_name.find_first_of(' ') + 1);			// удаление запроса Stop и пробела после него.
		stop_name.erase(stop_name.find(':'));							// удаление правой части строки, начиная с :, в stop_name остается только имя остановки.
		Stop* stop = stops_map_.at(stop_name);
		string distances = query;
		distances.erase(0, distances.find_first_of(',') + 2);	
		distances.erase(0, distances.find_first_of(',') + 2);			// удаление левой части строки до первого нужного символа (дистанция до соседней остановки).
		string distance;
		while (true)
		{
			distance = distances;
			auto comma = distance.find(',');
			if (comma != NPOS)											// если запятая найдена, значит в строке больше одной дистанции.
			{
				distance.erase(comma);
				string name = distance;
				name.erase(0, name.find_first_of('m') + 5);				// удаление левой части строки до начала имени соседней остановки.
				distance.erase(distance.find_first_of('m'));			// удаление правой строки начиная с m, остается только дистанция.
				stop->distances.insert({ stops_map_.at(name)->name, stoi(distance) }); // добавление string_view на имя остановки и дистанция до нее.
				distances.erase(0, distances.find_first_of(',') + 2);	// удаление добавленной остановки и дистанции до нее.
			}
			else														// запятая не найдена, значит осталасб последняя дистанция до соседней остановки, добавляем ее и выходим из цикла.
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
		if (!buses_.count(string(name)))	// если маршрута с запрашиваемым именем нету, возвращаем пустую структуру.
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
		if (buses_.count(string(name)) == 0)	// если маршрута с запрашиваемым именем нету, возвращаем пустую структуру.
		{
			static BusInfo empty;
			return empty;
		}

		Bus bus = buses_.at(string(name));

		int stops_on_route = static_cast<int>(bus.stops.size());	// получение кол-ва всех остановок маршрута.
		unordered_set<string> unique_stops_names;					// коллекция для уникальных остановок.
		int length = 0;												// фактическая длина маршрута.
		double geographical_length = 0;								// географическая длина маршрута.
		for (auto it = bus.stops.begin(); it != bus.stops.end();)	// перебор остановок маршрута.
		{
			auto [stop, stop_distance] = *it;
			unique_stops_names.insert(stop->name);
			geographical_length += stop_distance;
			it++;

			if (it != bus.stops.end())
			{
				auto [next_stop, _] = *it;

				if (stop->distances.count(next_stop->name))			// если расстояние до следующей остановки указано, получаем и доавляем его.
				{
					int distance_to_next_stop = stop->distances.at(next_stop->name);
					length += distance_to_next_stop;
				}
				else												// иначе берем расстояние от следующей остановки до данной.
				{
					length += next_stop->distances.at(stop->name);
				}
			}
		}
		if (!bus.is_circle)		// если маршрут последовательный, снова проходим по остановкам, но уже с конца, т.к. последовательный маршрут заносится в базу на половину.
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

		return { stops_on_route, static_cast<int>(unique_stops_names.size()), length, length / geographical_length }; // вычисляем curvature делением фактической длины маршрута на географическую.
	}

	tuple<set<string>, bool> TransportCatalogue::GetStopInfo(const string& name)
	{
		if (stops_map_.count(name) == 0)	// если запрашиваемой остановки нет в базе, возвращаем пустой кортеж.
		{
			return { {}, false };			// false нужен, чтобы в функции вывода обработать случай, когда запрашивается не существующая остановка.
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