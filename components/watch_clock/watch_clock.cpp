// SPDX-License-Identifier: Apache-2.0
#include "watch_clock.h"
#include "geometry.h"
#include "numerals.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace esphome::watch_clock {
namespace ui = tessera::ui;
using tessera::Font;

// The retained point buffers live as long as the saver. Tessera owns its LVGL
// tree and provides burn-in drift for the whole stage, including the numerals.
class WatchSaver : public tessera::Saver {
 public:
  explicit WatchSaver(const tessera::Plugin *plugin) : plugin_(plugin) {}
  void show(const tessera::SaverContext &c) override {
    seconds_ = c.options["seconds"] | true;
    date_enabled_ = c.options["date"] | true;
    const char *layout = c.options["layout"] | "original";
    const char *alignment = c.options["alignment"] | "centre";
    int align = 0;
    if (!std::strcmp(alignment, "left")) align = 1;
    if (!std::strcmp(alignment, "right")) align = 2;
    if (!std::strcmp(alignment, "top")) align = 3;
    if (!std::strcmp(alignment, "bottom")) align = 4;
    // Reserve room for Tessera's millimetre drift at all scales and alignments.
    const int margin = ui::mm(2);
    art_ = geometry::layout(std::max(1, c.width - 2 * margin), std::max(1, c.height - 2 * margin),
                            c.options["scale"] | 100.0f, !std::strcmp(layout, "adaptive"), align);
    art_.x += margin; art_.y += margin;
    cx_ = art_.x + art_.width / 2; cy_ = art_.y + art_.height / 2;
    const char *colour = c.options["numerals"] | "green";
    theme::Role role = theme::MARK_GREEN;
    if (!std::strcmp(colour, "white")) role = theme::INK_SOFT;
    if (!std::strcmp(colour, "blue")) role = theme::MARK_BLUE;
    if (!std::strcmp(colour, "purple")) role = theme::MARK_PURPLE;
    if (!std::strcmp(colour, "amber")) role = theme::MARK_AMBER;
    numeral(c.parent, geometry::NUMERAL_0, points_0_, 10 + art_.dx, 2, role);
    numeral(c.parent, geometry::NUMERAL_1, points_1_, 10 + art_.dx, 2, role);
    numeral(c.parent, geometry::NUMERAL_2, points_2_, 2, -13 + art_.dy, role);
    numeral(c.parent, geometry::NUMERAL_3, points_3_, 14 + 2 * art_.dx, -13 + art_.dy, role);
    numeral(c.parent, geometry::NUMERAL_4, points_4_, 10 + art_.dx, -4 + 2 * art_.dy, role);
    const char *position = c.options["date_position"] | "bottom_left";
    date_right_ = std::strstr(position, "right") != nullptr;
    date_top_ = std::strstr(position, "top") != nullptr;
    const char *format = c.options["date_format"] | "weekday";
    date_format_ = !std::strcmp(format, "month") ? 1 : !std::strcmp(format, "numeric") ? 2 : 0;
    utc_ = !std::strcmp(c.options["timezone"] | "screen", "utc");
    date_ = ui::label(c.parent, Font::BODY, theme::INK_SOFT);
    lv_obj_add_flag(date_, LV_OBJ_FLAG_HIDDEN);
    hour_ = line(c.parent, 6.2f * art_.unit, theme::INK);
    minute_ = line(c.parent, 6.2f * art_.unit, theme::INK);
    ring_ = ui::block(c.parent, theme::CAMERA_PAGE);
    const int diameter = std::max(3, static_cast<int>(9.2f * art_.unit));
    lv_obj_set_size(ring_, diameter, diameter);
    lv_obj_set_pos(ring_, static_cast<int>(cx_ - diameter / 2.0f), static_cast<int>(cy_ - diameter / 2.0f));
    lv_obj_set_style_radius(ring_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(ring_, std::max(1, static_cast<int>(1.2f * art_.unit)), 0);
    lv_obj_set_style_border_color(ring_, ui::color(theme::INK), 0);
    second_ = line(c.parent, .85f * art_.unit, theme::INK_SOFT);
    pivot_ = ui::block(c.parent, theme::INK);
    const int dot = std::max(2, static_cast<int>(3.6f * art_.unit));
    lv_obj_set_size(pivot_, dot, dot);
    lv_obj_set_pos(pivot_, static_cast<int>(cx_ - dot / 2.0f), static_cast<int>(cy_ - dot / 2.0f));
    lv_obj_set_style_radius(pivot_, LV_RADIUS_CIRCLE, 0);
    set_hands_visible(false);
  }

  void on_tick(uint32_t epoch) override {
    if (!epoch) {
      set_hands_visible(false); visible(date_, false);
      return;
    }
    set_hands_visible(true);
    auto now = tessera::local_time(epoch);
    if (utc_) {
      const time_t timestamp = epoch;
      struct tm utc_time{};
      gmtime_r(&timestamp, &utc_time);
      now.year = utc_time.tm_year + 1900; now.month = utc_time.tm_mon + 1;
      now.day = utc_time.tm_mday; now.weekday = utc_time.tm_wday + 1;
      now.hour = utc_time.tm_hour; now.minute = utc_time.tm_min; now.second = utc_time.tm_sec;
    }
    const auto a = geometry::angles(now.hour, now.minute, now.second);
    update_hand(hour_, hour_points_, 48, 9, a.hour);
    update_hand(minute_, minute_points_, 82, 10, a.minute);
    if (seconds_) update_hand(second_, second_points_, 91, 13, a.second);
    if (date_enabled_) paint_date(now);
  }

 private:
  static lv_obj_t *line(lv_obj_t *parent, float width, theme::Role role) {
    auto *part = lv_line_create(parent);
    lv_obj_remove_flag(part, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_line_width(part, std::max(1, static_cast<int>(std::lround(width))), 0);
    lv_obj_set_style_line_color(part, ui::color(role), 0);
    lv_obj_set_style_line_rounded(part, true, 0);
    return part;
  }
  template<size_t N>
  void numeral(lv_obj_t *parent, const geometry::Point (&source)[N], lv_point_precise_t (&points)[N],
               float x, float y, theme::Role role) {
    int left = 100000, top = 100000;
    for (size_t i = 0; i < N; ++i) {
      points[i].x = std::lround(art_.x + (source[i].x + x) * art_.unit);
      points[i].y = std::lround(art_.y + (source[i].y + y) * art_.unit);
      left = std::min(left, static_cast<int>(points[i].x));
      top = std::min(top, static_cast<int>(points[i].y));
    }
    for (auto &p : points) {
      p.x -= left; p.y -= top;
    }
    auto *part = line(parent, 6.4f * art_.unit, role);
    lv_line_set_points(part, points, N);
    lv_obj_set_pos(part, left, top);
  }
  void update_hand(lv_obj_t *part, lv_point_precise_t (&points)[2], float length, float tail, float angle) {
    const auto end = geometry::hand(cx_, cy_, length * art_.unit, angle);
    const auto start = geometry::hand(cx_, cy_, -tail * art_.unit, angle);
    lv_point_precise_t next[2] = {};
    next[0].x = std::lround(start.x); next[0].y = std::lround(start.y);
    next[1].x = std::lround(end.x); next[1].y = std::lround(end.y);
    const int left = std::min(next[0].x, next[1].x);
    const int top = std::min(next[0].y, next[1].y);
    for (auto &p : next) { p.x -= left; p.y -= top; }
    if (lv_obj_get_x(part) == left && lv_obj_get_y(part) == top &&
        points[0].x == next[0].x && points[0].y == next[0].y &&
        points[1].x == next[1].x && points[1].y == next[1].y) return;
    // Keep the dirty region to the hand's own rectangle, not a rectangle
    // stretching all the way to the top-left corner of the screen.
    lv_obj_invalidate(part);
    std::copy(next, next + 2, points);
    lv_line_set_points(part, points, 2);
    lv_obj_set_pos(part, left, top);
  }
  static void visible(lv_obj_t *part, bool value) {
    if (value && lv_obj_has_flag(part, LV_OBJ_FLAG_HIDDEN)) lv_obj_remove_flag(part, LV_OBJ_FLAG_HIDDEN);
    if (!value && !lv_obj_has_flag(part, LV_OBJ_FLAG_HIDDEN)) lv_obj_add_flag(part, LV_OBJ_FLAG_HIDDEN);
  }
  void set_hands_visible(bool value) {
    visible(hour_, value); visible(minute_, value); visible(second_, value && seconds_);
    visible(ring_, value); visible(pivot_, value);
  }
  void paint_date(const tessera::LocalTime &now) {
    char text[64];
    if (date_format_ == 2) {
      std::snprintf(text, sizeof(text), "%04d-%02d-%02d", now.year, now.month, now.day);
    } else {
      char key[16];
      std::snprintf(key, sizeof(key), date_format_ == 1 ? "month_%d" : "weekday_%d",
                    date_format_ == 1 ? now.month : now.weekday);
      if (date_format_ == 1) std::snprintf(text, sizeof(text), "%d %s", now.day, plugin_->text(key));
      else std::snprintf(text, sizeof(text), "%s %d", plugin_->text(key), now.day);
    }
    visible(date_, true);
    if (!std::strcmp(text, date_text_)) return;
    std::snprintf(date_text_, sizeof(date_text_), "%s", text);
    ui::set_text(date_, text);
    const int inset = static_cast<int>(14 * art_.unit);
    const int room = std::max(1, static_cast<int>(art_.width) - 2 * inset);
    const int width = std::min(room, ui::text_width(text, Font::BODY));
    const int x = static_cast<int>(art_.x) + (date_right_ ? static_cast<int>(art_.width) - inset - width : inset);
    const int y = static_cast<int>(art_.y) + (date_top_ ? inset : static_cast<int>(art_.height) - inset - ui::line_height(Font::BODY));
    lv_obj_set_width(date_, std::max(1, width));
    lv_obj_set_pos(date_, x, y);
    visible(date_, true);
  }
  const tessera::Plugin *plugin_;
  geometry::Layout art_{};
  float cx_ = 0, cy_ = 0;
  bool seconds_ = true, date_enabled_ = true, date_right_ = false, date_top_ = false, utc_ = false;
  int date_format_ = 0;
  char date_text_[64]{};
  lv_point_precise_t points_0_[3]{}, points_1_[26]{}, points_2_[73]{}, points_3_[49]{}, points_4_[61]{};
  lv_point_precise_t hour_points_[2]{}, minute_points_[2]{}, second_points_[2]{};
  lv_obj_t *hour_{}, *minute_{}, *second_{}, *ring_{}, *pivot_{}, *date_{};
};
void WatchClock::setup() {
  add_saver("watch", tessera::SaverStep::CLOCK, [this]() { return new WatchSaver(this); });
}
}  // namespace esphome::watch_clock
