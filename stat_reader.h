#pragma once
#include "transport_catalogue.h"
#include <sstream>

namespace transport_catalogue {
	namespace stat_reader {

		std::ostringstream GetInfoOnQueries(std::vector<std::pair<char, std::string>>& output_queries, TransportCatalogue& transport_catalogue);

		void PutStopInfo(const std::string& stop_name, TransportCatalogue& transport_catalogue, std::ostringstream& stream);

		void PutBusInfo(const std::string& bus_name, TransportCatalogue& transport_catalogue, std::ostringstream& stream);
	}
}