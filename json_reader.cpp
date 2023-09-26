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

void JsonReader::ProcessStatRequests(RequestHandler& request_handler, std::ostream& out)
{
    Node n = requests_.AsMap().at("stat_requests"s);
	Array arr;
	for (Node request : n.AsArray())
	{
		Dict map = request.AsMap();
		if (map.at("type"s).AsString() == "Map"s)
		{
			Dict dict;
			ostringstream osstr;
			request_handler.RenderMap().Render(osstr);
			dict.insert({ "map"s, osstr.str() });
			dict.insert({ "request_id"s, map.at("id"s) });
			arr.push_back(dict);
		}
		if (map.at("type"s).AsString() == "Stop"s)
		{
			Dict dict;
			dict.insert({ "request_id"s, map.at("id"s) });
			string name = map.at("name"s).AsString();
			auto [buses, stop_find] = request_handler.GetStopStat(name);
				if (!stop_find)
				{
					dict.insert({ "error_message"s, "not found"s });////
					arr.push_back(dict);
					continue;
				}
			Array a;
			for (const string& s : buses)
			{
				a.push_back(s);
			}
			dict.insert({ "buses"s, a });
			arr.push_back(dict);
		}
		if (map.at("type"s).AsString() == "Bus"s)
		{
			Dict dict;
			dict.insert({ "request_id"s, map.at("id"s) });
			string name = map.at("name"s).AsString();
			domain::BusInfo bus_info = request_handler.GetBusStat(name);
				if (bus_info.stops_on_route == 0)
				{
					dict.insert({ "error_message"s, "not found"s });
					arr.push_back(dict);
					continue;
				}
			dict.insert({ "stop_count"s, bus_info.stops_on_route });
			dict.insert({ "unique_stop_count"s, bus_info.unique_stops });
			dict.insert({ "route_length"s, bus_info.route_lenght });
			dict.insert({ "curvature"s, bus_info.curvature });
			arr.push_back(dict);
		}
	}
	json::Print(Document(arr), out);
}

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */