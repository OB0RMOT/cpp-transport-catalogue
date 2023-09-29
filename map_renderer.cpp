#include "map_renderer.h"
using namespace std;
using namespace json;
using namespace domain;
using namespace svg;
using namespace geo;
namespace renderer {

    vector<svg::Color> GetColorPalette(json::Node n)
    {
        const Array& color_palette = n.AsArray();
        std::vector<svg::Color> cp;
        for (const Node& node : color_palette)
        {
            if (node.IsArray())
            {
                const Array& arr = node.AsArray();
                if (arr.size() == 3)
                {
                    Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
                    cp.push_back(rgb_colors);
                }
                else if (arr.size() == 4)
                {
                    Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
                    cp.push_back(rgba_colors);
                }
            }
            else if (node.IsString())
            {
                cp.push_back(node.AsString());
            }
        }
        return cp;
    }

    svg::Color GetUnderlayerColor(json::Node node)
    {
        svg::Color underlayer_color;
        if (node.IsArray())
        {
            const Array& arr = node.AsArray();
            if (arr.size() == 3)
            {
                Rgb rgb_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
                underlayer_color = rgb_colors;
            }
            else if (arr.size() == 4)
            {
                Rgba rgba_colors(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
                underlayer_color = rgba_colors;
            }
        }
        else if (node.IsString())
        {
            underlayer_color = node.AsString();
        }
        return underlayer_color;
    }

    MapRenderer::MapRenderer(const Node& render_settings)
    {
        if (render_settings.IsNull()) return;
        const Dict& settings_map = render_settings.AsDict();
        width_ = settings_map.at("width"s).AsDouble();
        height_ = settings_map.at("height"s).AsDouble();
        padding_ = settings_map.at("padding"s).AsDouble();
        stop_radius_ = settings_map.at("stop_radius"s).AsDouble();
        line_width_ = settings_map.at("line_width"s).AsDouble();
        bus_label_font_size_ = settings_map.at("bus_label_font_size"s).AsInt();
        const Array& bus_label_offset = settings_map.at("bus_label_offset"s).AsArray();
        bus_label_offset_ = { bus_label_offset[0].AsDouble(),
                              bus_label_offset[1].AsDouble() };
        stop_label_font_size_ = settings_map.at("stop_label_font_size"s).AsInt();
        const Array& stop_label_offset = settings_map.at("stop_label_offset"s).AsArray();
        stop_label_offset_ = { stop_label_offset[0].AsDouble(),
                               stop_label_offset[1].AsDouble() };
        underlayer_color_ = GetUnderlayerColor(settings_map.at("underlayer_color"s));
        underlayer_width_ = settings_map.at("underlayer_width"s).AsDouble();
        color_palette_ = GetColorPalette(settings_map.at("color_palette"s));
    }

    vector<Polyline> MapRenderer::GetBusLines(const map<string, Bus>& buses, const SphereProjector& sp) const
    {
        vector<Polyline> result;
        size_t color_index = 0;
        for (const auto& [bus_name, bus] : buses)
        {
            if (bus.stops.empty()) { continue; }
            Polyline line;
            for (const Stop* stop : bus.stops)
            {
                line.AddPoint(sp(stop->coordinates));
            }
            if (!bus.is_circle)
            {
                for (int i = bus.stops.size() - 2; i >= 0; --i)
                {
                    line.AddPoint(sp(bus.stops[i]->coordinates));
                }
            }
            line.SetFillColor(NoneColor);
            line.SetStrokeColor(color_palette_[color_index]);
            line.SetStrokeWidth(line_width_);
            line.SetStrokeLineCap(StrokeLineCap::ROUND);
            line.SetStrokeLineJoin(StrokeLineJoin::ROUND);
            if (color_index < (color_palette_.size() - 1))
            {
                ++color_index;
            }
            else { color_index = 0; }
            result.push_back(line);
        }
        return result;
    }

    vector<Text> MapRenderer::GetBusLabels(const map<string, Bus>& buses, const SphereProjector& sp) const
    {
        vector<Text> result;
        unsigned color_num = 0;
        for (auto& [bus_name, bus] : buses)
        {
            if (bus.stops.size() == 0) continue;
            Text text;
            text.SetData(bus.name)
                .SetFillColor(color_palette_[color_num])
                .SetFontFamily("Verdana"s)
                .SetFontSize(bus_label_font_size_)
                .SetFontWeight("bold"s)
                .SetOffset(bus_label_offset_)
                .SetPosition(sp(bus.stops[0]->coordinates));
            Text text_underlayer;
            text_underlayer.SetData(bus.name)
                .SetFillColor(underlayer_color_)
                .SetStrokeColor(underlayer_color_)
                .SetFontFamily("Verdana"s)
                .SetFontSize(bus_label_font_size_)
                .SetFontWeight("bold"s)
                .SetOffset(bus_label_offset_)
                .SetStrokeWidth(underlayer_width_)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                .SetPosition(sp(bus.stops[0]->coordinates));
            if (color_num < (color_palette_.size() - 1))
            {
                ++color_num;
            }
            else color_num = 0;
            result.push_back(text_underlayer);
            result.push_back(text);
            if (bus.is_circle == false && bus.stops[0] != bus.stops[bus.stops.size() - 1])
            {
                svg::Text text2 = text;
                svg::Text text2_underlayer = text_underlayer;
                text2.SetPosition(sp(bus.stops[bus.stops.size() - 1]->coordinates));
                text2_underlayer.SetPosition(sp(bus.stops[bus.stops.size() - 1]->coordinates));
                result.push_back(text2_underlayer);
                result.push_back(text2);
            }
        }
        return result;
    }

    vector<Text> MapRenderer::GetStopLabels(const map<string, Stop>& stops, const SphereProjector& sp) const
    {
        vector<Text> result;
        for (auto& [stop_name, stop] : stops)
        {
            Text text;
            text.SetPosition(sp(stop.coordinates))
                .SetOffset(stop_label_offset_)
                .SetFontSize(stop_label_font_size_)
                .SetFontFamily("Verdana"s)
                .SetData(stop.name)
                .SetFillColor("black"s);
            Text text_underlayer;
            text_underlayer.SetPosition(sp(stop.coordinates))
                .SetOffset(stop_label_offset_)
                .SetFontSize(stop_label_font_size_)
                .SetFontFamily("Verdana"s)
                .SetData(stop.name)
                .SetFillColor(underlayer_color_)
                .SetStrokeColor(underlayer_color_)
                .SetStrokeWidth(underlayer_width_)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            result.push_back(text_underlayer);
            result.push_back(text);
        }
        return result;
    }

    vector<Circle> MapRenderer::GetStopCircles(const map<string, Stop>& stops, const SphereProjector& sp) const
    {
        vector<Circle> result;
        for (auto& [stop_name, stop] : stops)
        {
            Circle circle;
            circle.SetCenter(sp(stop.coordinates))
                .SetRadius(stop_radius_)
                .SetFillColor("white"s);
            result.push_back(circle);
        }
        return result;
    }

    svg::Document MapRenderer::RenderSvgDocument(const map<string, Bus>& buses) const
    {
        map<string, Stop> all_stops;
        vector<Coordinates> all_coords;
        svg::Document result;
        for (const auto& [bus_name, bus_ptr] : buses)
        {
            if (bus_ptr.stops.size() == 0) continue;
            for (const Stop* stop : bus_ptr.stops)
            {
                all_stops[stop->name] = *stop;
                all_coords.push_back(stop->coordinates);
            }
        }
        SphereProjector sp(all_coords.begin(), all_coords.end(), width_, height_, padding_);
        for (const auto& line : GetBusLines(buses, sp))
        {
            result.Add(line);
        }
        for (const auto& text : GetBusLabels(buses, sp))
        {
            result.Add(text);
        }
        for (const auto& circle : GetStopCircles(all_stops, sp))
        {
            result.Add(circle);
        }
        for (const auto& text : GetStopLabels(all_stops, sp))
        {
            result.Add(text);
        }

        return result;
    }

    bool IsZero(double value)
    {
        return std::abs(value) < EPSILON;
    }
    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point SphereProjector::operator()(geo::Coordinates coords) const
    {
        return
        {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }
}