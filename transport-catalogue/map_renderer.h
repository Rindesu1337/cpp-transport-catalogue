#pragma once

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

#include "domain.h"
#include "geo.h"
#include "svg.h"

#include <array>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

namespace render {

struct RenderSettings {
    double width = 0;//ширина и высота изображения в пикселях. Вещественное число в диапазоне от 0 до 100000.
    double height = 0;//ширина и высота изображения в пикселях. Вещественное число в диапазоне от 0 до 100000.
    double padding = 0;//отступ краёв карты от границ SVG-документа. Вещественное число не меньше 0 и меньше min(width, height)/2.

    double line_width = 0; //толщина линий, которыми рисуются автобусные маршруты. Вещественное число в диапазоне от 0 до 100000.

    double stop_radius = 0;//радиус окружностей, которыми обозначаются остановки. Вещественное число в диапазоне от 0 до 100000.

    int bus_label_font_size = 1; //размер текста, которым написаны названия автобусных маршрутов. Целое число в диапазоне от 0 до 100000.

    std::array<double,2> bus_label_offset = {0.0, 0.0}; //смещение надписи с названием маршрута относительно координат конечной остановки на карте. Массив из двух элементов типа double. Задаёт значения свойств dx и dy SVG-элемента <text>. Элементы массива — числа в диапазоне от –100000 до 100000.
    
    int stop_label_font_size = 1; // размер текста, которым отображаются названия остановок. Целое число в диапазоне от 0 до 100000.

    std::array<double, 2> stop_label_offset = {0.0, 0.0};//смещение названия остановки относительно её координат на карте. Массив из двух элементов типа double. Задаёт значения свойств dx и dy SVG-элемента <text>. Числа в диапазоне от –100000 до 100000.

    svg::Color underlayer_color; //цвет подложки под названиями остановок и маршрутов. Формат хранения цвета будет ниже.

    double underlayer_width = 0; //толщина подложки под названиями остановок и маршрутов. Задаёт значение атрибута stroke-width элемента <text>. Вещественное число в диапазоне от 0 до 100000.

    std::vector<svg::Color> color_palette; //цветовая палитра. Непустой массив.

};

inline const double EPSILON = 1e-6;

class SphereProjector {
public:

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

class MapRender {
public: 

    MapRender(RenderSettings  rs)
        : rs_(std::move(rs)){
    } 

    void SetSetting(RenderSettings& rs, std::vector<geo::Coordinates>& all_coor);

    void RenderBusLine(svg::Document& doc,const std::vector<svg::Point>& stops_point, size_t color) const ;

    void RenderBusName(svg::Document& doc, const svg::Point& point, const std::string& text, size_t color) const;

    void RenderStopCircle(svg::Document& doc, const svg::Point& point) const ;

    void RenderStopName(svg::Document& doc, const svg::Point& point, const std::string& text) const;

    size_t GetColorPaletteSize() const;

    double GetWidth() const;
    double GetHeight() const;
    double GetPadding() const;

private:
    RenderSettings rs_;
};

}