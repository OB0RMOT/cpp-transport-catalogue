#pragma once
#include "transport_catalogue.h"
#include <iostream>

namespace transport_catalogue {
	namespace stat_reader {

		// функция вывода информации о остановке.
		void PutStopInfo(const std::string& stop_name, TransportCatalogue& transport_catalogue);

		// функция вывода информации о маршруте.
		void PutBusInfo(const std::string& bus_name, TransportCatalogue& transport_catalogue);
	}
}