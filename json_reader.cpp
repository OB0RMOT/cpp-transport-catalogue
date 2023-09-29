#include "json_reader.h"
#include "svg.h"

using namespace std;
using namespace json;

void JsonReader::FillTransportCatalogue(transport_catalogue::TransportCatalogue& tcat)
{
	JsonReader::LoadStopRequests(tcat);
	JsonReader::LoadStopDistancesRequests(tcat);
	JsonReader::LoadBusRequests(tcat);
}

renderer::MapRenderer JsonReader::LoadRenderSettings()
{
	return renderer::MapRenderer(requests_.AsDict().at("render_settings"s).AsDict());
}

void JsonReader::LoadStopDistancesRequests(transport_catalogue::TransportCatalogue& tcat)
{
	for (Node request : requests_.AsDict().at("base_requests"s).AsArray())
	{
		Dict map = request.AsDict();
		if (map.at("type"s).AsString() == "Stop"s)
		{
			for (auto m : map.at("road_distances"s).AsDict())
			{
				tcat.AddStopDistances(map.at("name"s).AsString(), m.first, m.second.AsInt());
			}
		}
	}
}

void JsonReader::LoadStopRequests(transport_catalogue::TransportCatalogue& tcat)
{
	for (Node request : requests_.AsDict().at("base_requests"s).AsArray())
	{
		Dict map = request.AsDict();
		if (map.at("type"s).AsString() == "Stop"s)
		{
			string name = map.at("name"s).AsString();
			tcat.AddStop(name, { map.at("latitude"s).AsDouble(), map.at("longitude"s).AsDouble() });
		}
	}
}

void JsonReader::LoadBusRequests(transport_catalogue::TransportCatalogue& tcat)
{
	for (Node request : requests_.AsDict().at("base_requests"s).AsArray())
	{
		Dict map = request.AsDict();
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

void JsonReader::ProcessStatRequests(RequestHandler& request_handler, std::ostream& out)
{
    Node n = requests_.AsDict().at("stat_requests"s);
	Builder document{};
	document.StartArray();
	for (Node request : n.AsArray())
	{
		Dict map = request.AsDict();
		if (map.at("type"s).AsString() == "Map"s)
		{
			ostringstream osstr;
			request_handler.RenderMap().Render(osstr);
			document.StartDict()
						.Key("map"s).Value(osstr.str())
						.Key("request_id"s).Value(map.at("id"s).AsInt())
					.EndDict();
		}
		if (map.at("type"s).AsString() == "Stop"s)
		{
			document.StartDict().Key("request_id"s).Value(map.at("id"s).AsInt());
			string name = map.at("name"s).AsString();
			auto [buses, stop_find] = request_handler.GetStopStat(name);
				if (!stop_find)
				{
					document.Key("error_message"s).Value("not found"s).EndDict();
					continue;
				}
			Array a;
			for (const string& s : buses)
			{
				a.push_back(s);
			}
			document.Key("buses"s).Value(a).EndDict();
		}
		if (map.at("type"s).AsString() == "Bus"s)
		{
			document.StartDict().Key("request_id"s).Value(map.at("id"s).AsInt());
			string name = map.at("name"s).AsString();
			domain::BusInfo bus_info = request_handler.GetBusStat(name);
				if (bus_info.stops_on_route == 0)
				{
					document.Key("error_message"s).Value("not found"s).EndDict();
					continue;
				}
			document.Key("stop_count"s).Value(bus_info.stops_on_route)
					.Key("unique_stop_count"s).Value(bus_info.unique_stops)
					.Key("route_length"s).Value(bus_info.route_lenght)
					.Key("curvature"s).Value(bus_info.curvature).EndDict();
		}
	}
	document.EndArray();
	json::Print(Document(document.Build()), out);
}

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */