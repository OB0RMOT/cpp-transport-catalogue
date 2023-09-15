#include "json_reader.h"
#include "svg.h"

using namespace std;
using namespace json;

json::Node JsonReader::GetStatRequests() const
{
	return requests_.AsMap().at("stat_requests"s);
}

void JsonReader::FillTransportCatalogue(transport_catalogue::TransportCatalogue& tcat)
{
	JsonReader::LoadStopRequests(tcat);
	JsonReader::LoadStopDistancesRequests(tcat);
	JsonReader::LoadBusRequests(tcat);
}

renderer::MapRenderer JsonReader::LoadRenderSettings()
{
	return renderer::MapRenderer(requests_.AsMap().at("render_settings"s).AsMap());
}

void JsonReader::LoadStopDistancesRequests(transport_catalogue::TransportCatalogue& tcat)
{
	for (Node request : requests_.AsMap().at("base_requests"s).AsArray())
	{
		Dict map = request.AsMap();
		if (map.at("type"s).AsString() == "Stop"s)
		{
			for (auto m : map.at("road_distances"s).AsMap())
			{
				tcat.AddStopDistances(map.at("name"s).AsString(), m.first, m.second.AsInt());
			}
		}
	}
}

void JsonReader::LoadStopRequests(transport_catalogue::TransportCatalogue& tcat)
{
	for (Node request : requests_.AsMap().at("base_requests"s).AsArray())
	{
		Dict map = request.AsMap();
		if (map.at("type"s).AsString() == "Stop"s)
		{
			string name = map.at("name"s).AsString();
			tcat.AddStop(name, { map.at("latitude"s).AsDouble(), map.at("longitude"s).AsDouble() });
		}
	}
}

void JsonReader::LoadBusRequests(transport_catalogue::TransportCatalogue& tcat)
{
	for (Node request : requests_.AsMap().at("base_requests"s).AsArray())
	{
		Dict map = request.AsMap();
		if (map.at("type"s).AsString() == "Bus"s)
		{
			vector<string> stops;
			for (Node n : map.at("stops"s).AsArray())
			{
				stops.push_back(n.AsString());
			}
			tcat.AddBus(map.at("name"s).AsString(), map.at("is_roundtrip"s).AsBool(), stops);
		}
	}
}

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */