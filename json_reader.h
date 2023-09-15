#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"


class JsonReader {
public:
	JsonReader(json::Document document) : requests_(document.GetRoot()){}

	void FillTransportCatalogue(transport_catalogue::TransportCatalogue& tcat);

	json::Node GetStatRequests() const;

	renderer::MapRenderer LoadRenderSettings();

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