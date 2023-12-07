#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport_catalogue {
class Router {
public:
    Router(std::pair<int, double> settings, const TransportCatalogue& catalogue) {
        bus_wait_time_ = settings.first;
        bus_velocity_ = settings.second;
        BuildGraph(catalogue);
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }

    const graph::DirectedWeightedGraph<double>& GetGraph() const;
    const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
private:
    int bus_wait_time_ = 0;
    double bus_velocity_ = 0.0;
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::map<std::string, graph::VertexId> stop_ids_;
    
    const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& catalogue);
};
} // namespace transport_catalogue