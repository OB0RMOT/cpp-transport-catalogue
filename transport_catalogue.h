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
#include <sstream>
#include <iostream>

namespace transport_catalogue {
	namespace detail {
		// функция чтения и разбиения запросов одной секции на вектор строк.
		std::vector<std::string> StreamSectionToVector(const int& queries_count, std::stringstream& stream);
	}

	#define NPOS string::npos

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
	};

	// структура маршрута.
	struct Bus {
		std::string name;
		bool is_circle;
		std::vector<Stop*> stops; // остановки маршрута и расстояние до следующей остановки. В последней остановке расстояние = 0.
	};

	class TransportCatalogue {
	public:
		// добавление маршрута.
		void AddBus(const std::string& bus_name, bool is_circle, std::list<std::string>& stops_names);


		void AddStopDistances(const std::string& first_stop_name, const std::string& second_stop_name, const int& distance);

		// добавление остановки.
		void AddStop(const Stop& stop);

		// поиск маршрута по имени.
		Bus* FindBus(std::string_view name);

		// поиск остановки по имени.
		Stop* FindStop(std::string_view name);

		// получение информации о маршруте.
		BusInfo GetBusInfo(std::string_view name);

		std::tuple<std::set<std::string>, bool> GetStopInfo(const std::string& name);

	private:
		struct StopPointerPairHasher {
			std::size_t operator()(const std::pair<Stop*, Stop*>& key) const {
				std::size_t h1 = std::hash<std::string>()(key.first->name);
				std::size_t h2 = std::hash<std::string>()(key.second->name);
				// Производим побитовую операцию XOR между хешами указателей
				return h1 ^ h2;
			}
		};

		struct StopPointerHasher {
			std::size_t operator()(const Stop* key) const {
				std::size_t h = std::hash<std::string>()(key->name);
				return h;
			}
		};


		std::deque<Stop> stops_; // все остановки.
		std::unordered_map<std::string_view, Stop*> stops_map_; //
		std::unordered_map<Stop*, std::vector<Bus*>, StopPointerHasher> stops_to_buses_;
		std::unordered_map<std::string, Bus> buses_; // все маршруты.
		std::unordered_map<std::pair<Stop*, Stop*>, int, StopPointerPairHasher> distances_;
	};
}