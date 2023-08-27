#pragma once
#include "transport_catalogue.h"

namespace transport_catalogue {
	namespace stat_reader {

		std::ostringstream LoadOutputQueries(std::stringstream& stream, TransportCatalogue& transport_catalogue);

		void PutStopInfo(const std::string& stop_name, TransportCatalogue& transport_catalogue, std::ostringstream& stream);

		void PutBusInfo(const std::string& bus_name, TransportCatalogue& transport_catalogue, std::ostringstream& stream);
	}
}