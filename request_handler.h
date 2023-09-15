#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"

#include <optional>

class RequestHandler {
public:
    RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer) : db_(db), renderer_(renderer){}
    // MapRenderer понадобится в следующей части итогового проекта
    //RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    void ProcessStatRequests(json::Node n, std::ostream& output);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<domain::BusInfo> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    const std::unordered_set<domain::Bus>* GetBusesByStop(const std::string_view& stop_name) const;

    // Этот метод будет нужен в следующей части итогового проекта
    //svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const transport_catalogue::TransportCatalogue& db_;
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