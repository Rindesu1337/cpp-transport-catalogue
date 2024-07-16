#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */



namespace render {
    
    void MapRender::RenderBusLine(svg::Document& doc, const std::vector<svg::Point>& stops_point, size_t color) const {

        svg::Polyline polyline;

        for (const auto& geo_coord: stops_point) {
            polyline.AddPoint(geo_coord)
                             .SetStrokeWidth(rs_.line_width)
                             .SetStrokeColor(rs_.color_palette[color])
                             .SetFillColor(svg::NoneColor)
                             .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                             .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);  
        }


        doc.Add(std::move(polyline));
    }

    void MapRender::RenderBusName(svg::Document& doc, const svg::Point& point, const std::string& text, size_t color) const {
        svg::Text str_und;

        str_und.SetPosition(point)
            .SetOffset({rs_.bus_label_offset.at(0), rs_.bus_label_offset.at(1)})
            .SetFontSize(static_cast<uint32_t>(rs_.bus_label_font_size))
            .SetFontFamily("Verdana"s)
            .SetFontWeight("bold"s)
            .SetData(text)
            .SetFillColor(rs_.underlayer_color)
            .SetStrokeColor(rs_.underlayer_color)
            .SetStrokeWidth(rs_.underlayer_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        
        doc.Add(std::move(str_und));

        svg::Text str;

        str.SetPosition(point)
            .SetOffset({rs_.bus_label_offset.at(0), rs_.bus_label_offset.at(1)})
            .SetFontSize(static_cast<uint32_t>(rs_.bus_label_font_size))
            .SetFontFamily("Verdana"s)
            .SetFontWeight("bold"s)
            .SetData(text)
            .SetFillColor(rs_.color_palette[color]);
        
        doc.Add(std::move(str));
    }

    void MapRender::RenderStopCircle(svg::Document& doc, const svg::Point& point) const {
        svg::Circle circle;

        circle.SetCenter(point)
                .SetRadius(rs_.stop_radius)
                .SetFillColor("white"s);
        doc.Add(circle);
    }

    void MapRender::RenderStopName(svg::Document& doc, const svg::Point& point, const std::string& text) const {
        svg::Text str_und;

        str_und.SetPosition(point)
            .SetOffset({rs_.stop_label_offset.at(0), rs_.stop_label_offset.at(1)})
            .SetFontSize(static_cast<uint32_t>(rs_.stop_label_font_size))
            .SetFontFamily("Verdana"s)
            .SetData(text)
            .SetFillColor(rs_.underlayer_color)
            .SetStrokeColor(rs_.underlayer_color)
            .SetStrokeWidth(rs_.underlayer_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        
        doc.Add(std::move(str_und));

        svg::Text str;

        str.SetPosition(point)
            .SetOffset({rs_.stop_label_offset.at(0), rs_.stop_label_offset.at(1)})
            .SetFontSize(static_cast<uint32_t>(rs_.stop_label_font_size))
            .SetFontFamily("Verdana"s)
            .SetData(text)
            .SetFillColor("black"s);
        
        doc.Add(std::move(str));
    }

    size_t MapRender::GetColorPaletteSize() const{
        return rs_.color_palette.size();
    }

    double MapRender::GetWidth() const {
        return rs_.width;
    }
    double MapRender::GetHeight() const {
        return rs_.height;
    }
    double MapRender::GetPadding() const{
        return rs_.padding;
    }
}