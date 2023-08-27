#pragma once
#include "transport_catalogue.h"
#include "stat_reader.h"

namespace transport_catalogue {
	namespace input_reader {

		//using stosd = std::tuple<std::pair<std::string, std::string>, int>;

		using stob = std::tuple<std::string, bool, std::list<std::string>>; // StringToBus

		// функция чтения и разбора запросов из потока.
		std::vector<std::pair<char, std::string>> LoadQueries(std::istream& stream, TransportCatalogue& transport_catalogue);

		// функция преобразования строки в остановку.
		Stop StringToStop(std::string& str);

		void StringToStopDistances(std::string& str, TransportCatalogue& transport_catalogue);

		// функция преобразования строки в маршрут.
		stob StringToBus(std::string& str, size_t& colon);

		// функция чтения и разбиения запросов одной секции на вектор строк.
		std::vector<std::string> StreamSectionToVector(const int& queries_count, std::istream& stream);
	}
}