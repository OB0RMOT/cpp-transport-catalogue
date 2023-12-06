#pragma once

#include "request_handler.h"
#include "transport_router.h"
#include "json_builder.h"


class JsonReader {
public:
	JsonReader(json::Document document) : requests_(document.GetRoot()){}

	void FillTransportCatalogue(transport_catalogue::TransportCatalogue& tcat);

	void ProcessStatRequests(RequestHandler& request_handler, std::ostream& output);

	renderer::MapRenderer LoadRenderSettings();

	transport_catalogue::Router GetRoutingSettings();

private:
	json::Node requests_;

	void LoadStopDistancesRequests(transport_catalogue::TransportCatalogue& tcat);

	void LoadStopRequests(transport_catalogue::TransportCatalogue& tcat);

	void LoadBusRequests(transport_catalogue::TransportCatalogue& tcat);

	
};

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */