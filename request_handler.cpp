#include "request_handler.h"

using namespace std;
using namespace json;

void RequestHandler::ProcessStatRequests(json::Node n, std::ostream& out)
{
	out << "[\n\t";
	int i = 0;
	bool not_found = false;
    Node last = n.AsArray().back();
	for (Node request : n.AsArray())
	{
		if (i > 0 && !not_found)
		{
			out << ",\n\t";
		}
		++i;
		Dict map = request.AsMap();
		
		//{
		//"map": "<?xml version=\"1.0\",
		//"request_id": 11111
		//}
		if (map.at("type").AsString() == "Map")
		{
			out << "{\n\t\t\"map\": ";
			ostringstream osstr;
			renderer_.GetSvgDocument(db_.GetBuses()).Render(osstr);
			//Dict dict;
			string s(osstr.str());
			json::Document doc(s);
			Print(doc, out);
			out << ",\n\t\t\"request_id\": " << map.at("id").AsInt() << "\n}";

		}
		if (map.at("type").AsString() == "Stop")
		{
			out << "{\n\t\t\"request_id\": " << map.at("id").AsInt() << ",\n\t\t";
			string name = map.at("name").AsString();
			auto [buses, stop_find] = db_.GetStopInfo(name);
			if (!stop_find)
			{
				out << "\"error_message\": \"not found\"\n\t}\n";
                if(request != last)
                {
                    out << ",";
                }
				not_found = true;
				continue;
			}
			not_found = false;
			
            if(buses.empty())
            {
                out << "\"buses\": []\n\t}";
            }
            else
            {
                out << "\"buses\": [\n\t\t\t";
                int j = 0;
                for (const string& s : buses)
                {
                    if (j > 0)
                    {
                        out << ",\n\t\t";
                    }
                    ++j;
                    out << "\"" << s << "\"";
                }
                out << "\n\t\t]\n\t}";
            }
		}
		if (map.at("type").AsString() == "Bus")
		{
			out << "{\n\t\t\"request_id\": " << map.at("id").AsInt() << ",\n\t\t";
			string name = map.at("name").AsString();
			domain::BusInfo bus_info = db_.GetBusInfo(name);
			if (bus_info.stops_on_route == 0)
			{
				out << "\"error_message\": \"not found\"\n\t}\n";
                if(request != last)
                {
                    out << ",";
                }
				not_found = true;
				continue;
			}
            not_found = false;
			out << "\"stop_count\": " << bus_info.stops_on_route << ",\n\t\t";
			out << "\"unique_stop_count\": " << bus_info.unique_stops << ",\n\t\t";
			out << "\"route_length\": " << bus_info.route_lenght << ",\n\t\t";
			out << "\"curvature\": " << bus_info.curvature;
			out << "\n\t}";
		}
	}
    
	out << "\n]";
}

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */