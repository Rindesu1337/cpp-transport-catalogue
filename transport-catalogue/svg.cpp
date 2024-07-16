#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

std::ostream& operator<<(std::ostream& out, svg::StrokeLineCap line_cap) {
    using namespace std::literals;
    if (line_cap == StrokeLineCap::BUTT) { out << "butt"sv;}
    else if (line_cap == StrokeLineCap::ROUND) { out << "round"sv;}
    else if (line_cap == StrokeLineCap::SQUARE) { out << "square"sv;}

    return out;
} 

std::ostream& operator<<(std::ostream& out,  StrokeLineJoin line_join)  {
    using namespace std::literals;
    if (line_join == StrokeLineJoin::ARCS) { out << "arcs"sv; }
    else if (line_join == StrokeLineJoin ::BEVEL) { out << "bevel"sv; }
    else if (line_join == StrokeLineJoin ::MITER) { out << "miter"sv; }
    else if (line_join == StrokeLineJoin ::MITER_CLIP) { out << "miter-clip"sv; }
    else if (line_join == StrokeLineJoin ::ROUND) { out << "round"sv; }

    return out;
}  

std::ostream& operator<<(std::ostream& out,  Color color) {
    std::ostringstream strm;
    std::visit(ColorPrint{strm}, color);

    out << strm.str();

    return out;
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

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    if (points_.empty()) {
        out << "<polyline points=\""sv << "\" "sv << "/>"sv ;
        return;
    }

    bool is_first = true;
    out << "<polyline points=\""sv;
    for (const auto& point : points_) {
        if (!is_first) {
            out << " "sv;
        } else { is_first = false; }
        out << point.x << ","sv << point.y;
    }
    out << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv ;
}

Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        // Словарь замен: символ => строка замены
        std::unordered_map<char, std::string> replacements = {
            {'<', "&lt;"},
            {'>', "&gt;"},
            {'\'', "&apos;"},
            {'&', "&amp;"}
        };

        // Результирующая строка
        std::string result;

        // Проход по каждому символу исходной строки
        for (char c : data) {
            // Проверка, есть ли символ в словаре замен
            if (replacements.find(c) != replacements.end()) {
                // Если есть, добавляем строку замены
                result += replacements[c];
            } else {
                // Если нет, добавляем сам символ
                result += c;
            }
        }

        data_ = result;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;

        out << "<text "sv;
        RenderAttrs(context.out);
        out  << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << size_ << "\""sv;
        if (!font_family_.empty()) { out << " font-family=\""sv << font_family_ << "\""sv; }
        if (!font_weight_.empty()) {out <<" font-weight=\""sv << font_weight_ << "\""; }
        out << ">"sv;
        if (!data_.empty()) {
            out << data_;
        }     
        out << "</text>"sv;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        RenderContext ctx(out, 2, 2);
        for (auto& obj : objects_) {
            obj->Render(ctx);
        }

        out << "</svg>"sv;
    }

}  // namespace svg