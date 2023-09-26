#include "svg.h"

namespace svg {

using namespace std::literals;



void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(move(obj));
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
    RenderContext ctx(out, 2, 2);
    for (auto& obj : objects_)
    {
        obj->Render(ctx);
    }
    out << "</svg>"sv;
}

// Задаёт координаты опорной точки (атрибуты x и y)
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t font_size) {
    font_size_ = font_size;
    return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}

//<text fill = "yellow" stroke = "yellow" stroke - width = "3" stroke - linecap = "round" stroke - linejoin = "round" x = "10" y = "100" dx = "0" dy = "0" font - size = "12" font - family = "Verdana">Happy New Year!< / text>
//<text fill = "red" x = "10" y = "100" dx = "0" dy = "0" font - size = "12" font - family = "Verdana">Happy New Year!< / text>
//<text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    
    out << "<text "sv << "x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" font-size=\"" << font_size_ << "\"";
    RenderAttrs(out);
    if (!font_family_.empty())
    {
        out << " font-family=\"" << font_family_ << "\"";
    }
    if (!font_weight_.empty())
    {
        out << " font-weight=\"" << font_weight_ << "\"";
    }
    out << ">";
    for (char c : data_)
    {
        if      (c == '<')  { out << "&lt;"sv; }
        else if (c == '>')  { out << "&gt;"sv; }
        else if (c == '"')  { out << "&quot;"sv; }
        else if (c == '&')  { out << "&amp;"sv; }
        else if (c == '\'') { out << "&apos;"sv; }
        else                { out << c; }
    }
    
    out << "</text>\n"sv;
}

Polyline& Polyline::AddPoint(Point point) {
    polyline_.push_back(point);
    return *this;
}

//<polyline points="20,40 22.9389,45.9549 29.5106,46.9098" />
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    int i = 0;
    for (auto p : polyline_)
    {
        if (i > 0)
        {
            out << " "sv;
        }
        out << p.x << ","sv << p.y;
        i++;
    }
    
    out << "\"";
    RenderAttrs(out); 
    out << "/>\n"sv;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

//<circle cx="30" cy="70" r="20" fill="rgb(240,240,240)" stroke="black"/>
void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << "/>\n"sv;
}

std::ostream& operator<<(std::ostream& out, const svg::StrokeLineCap& line_cap) {
    if (line_cap == svg::StrokeLineCap::BUTT)
    {
        out << "butt"sv;
    }
    if (line_cap == svg::StrokeLineCap::ROUND)
    {
        out << "round"sv;
    }
    if (line_cap == svg::StrokeLineCap::SQUARE)
    {
        out << "square"sv;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const svg::StrokeLineJoin& line_join) {
    if (line_join == svg::StrokeLineJoin::ARCS)
    {
        out << "arcs"sv;
    }
    if (line_join == svg::StrokeLineJoin::BEVEL)
    {
        out << "bevel"sv;
    }
    if (line_join == svg::StrokeLineJoin::MITER)
    {
        out << "miter"sv;
    }
    if (line_join == svg::StrokeLineJoin::MITER_CLIP)
    {
        out << "miter-clip"sv;
    }
    if (line_join == svg::StrokeLineJoin::ROUND)
    {
        out << "round"sv;
    }
    return out;
}

}  // namespace svg