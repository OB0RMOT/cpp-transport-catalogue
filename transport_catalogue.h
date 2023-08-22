#pragma once
#include "geo.h"
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <deque>
#include <list>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <unordered_set>
 
namespace transport_catalogue {

	#define NPOS string::npos

	// структура для получения информации о маршруте.
	struct BusInfo {
		int stops_on_route = 0;
		int unique_stops = 0;
		int route_lenght = 0;
		double curvature = 0;
	};

	// структура остановки.
	struct Stop {
		std::string name;
		geo::Coordinates coordinates;
		std::unordered_map<std::string_view, int> distances; // имена соседних остановок и фактическое расстояние до них.
	};

	// структура маршрута.
	struct Bus {
		std::string name;
		bool is_circle;
		std::vector<std::tuple<const Stop*, double>> stops; // остановки маршрута и расстояние до следующей остановки. В последней остановке расстояние = 0.

		std::string_view end_stop;
	};

	class TransportCatalogue {
	public:
		// добавление маршрута.
		void AddBus(const std::string& bus_name, bool is_circle, std::list<std::string>& stops_names);

		// добавление расстояний соседних остановок дял каждой остановки.
		void AddStopDistances(const std::string& query);

		// добавление остановки.
		void AddStop(const Stop& stop);

		// поиск маршрута по имени.
		Bus FindBus(std::string_view name);

		// поиск остановки по имени.
		Stop FindStop(std::string_view name);

		// получение информации о маршруте.
		BusInfo GetBusInfo(std::string_view name);

		// получение информации о остановке.
		std::tuple<std::set<std::string>, bool> GetStopInfo(const std::string& name);

	private:
		std::deque<Stop> stops_; // все остановки.
		std::unordered_map<std::string_view, Stop*> stops_map_; // имена остановок и указатели на них. Контейнер для быстрого поиска остановок по имени.
		std::unordered_map<Stop*, std::vector<Bus*>> stops_to_buses_; // остановки и маршруты, проходящие через остановки.
		std::unordered_map<std::string, Bus> buses_; // все маршруты.
	};
}