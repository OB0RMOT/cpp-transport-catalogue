#include "request_handler.h"

using namespace std;
using namespace json;

svg::Document RequestHandler::RenderMap() const
{
	return renderer_.RenderSvgDocument(db_.GetBuses());
}

std::tuple<std::set<std::string>, bool> RequestHandler::GetStopStat(const std::string& stop_name) const
{
	return db_.GetStopInfo(stop_name);
}

domain::BusInfo RequestHandler::GetBusStat(const std::string& bus_name) const
{
	return db_.GetBusInfo(bus_name);
}



/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */