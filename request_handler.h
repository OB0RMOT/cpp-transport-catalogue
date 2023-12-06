#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"
#include "transport_router.h"

class RequestHandler {
public:
    RequestHandler(const transport_catalogue::TransportCatalogue& db, const transport_catalogue::Router& router, const renderer::MapRenderer& renderer) : db_(db), router_(router), renderer_(renderer){}

    // Возвращает информацию о маршруте (запрос Bus)
    domain::BusInfo GetBusStat(const std::string& bus_name) const;

    std::tuple<std::set<std::string>, bool> GetStopStat(const std::string& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

    const std::optional<graph::Router<double>::RouteInfo> GetRoute(const std::string start_stop, const std::string end_stop) const;

    const graph::DirectedWeightedGraph<double>& GetRouterGraph() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const transport_catalogue::TransportCatalogue& db_;
    const transport_catalogue::Router& router_;
    const renderer::MapRenderer& renderer_;
};

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)