#pragma once
#include "transport_catalogue.h"
#include "stat_reader.h"
#include <sstream>

namespace transport_catalogue {
	namespace input_reader {

		using stob = std::tuple<std::string, bool, std::list<std::string>>; // StringToBus

		// функция чтения и разбора запросов из потока.
		void LoadQueries(std::istream& stream, transport_catalogue::TransportCatalogue& transport_catalogue);

		// функция преобразования строки в остановку.
		transport_catalogue::Stop StringToStop(std::string& str);

		// функция преобразования строки в маршрут.
		stob StringToBus(std::string& str, size_t& colon);

		// функция чтения и разбиения запросов одной секции на вектор строк.
		std::vector<std::string> StreamSectionToVector(const int& queries_count, std::istream& stream);
	}
}