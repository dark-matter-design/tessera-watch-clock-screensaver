// SPDX-License-Identifier: Apache-2.0
#include "watch_clock.h"
#include "geometry.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include "esphome/core/hal.h"

namespace esphome::watch_clock {
namespace ui = tessera::ui;
using tessera::Font;

// The retained point buffers live as long as the saver. Tessera owns its LVGL
// tree and provides burn-in drift for the whole stage, including the numerals.
class WatchSaver : public tessera::Saver {
 public:
  explicit WatchSaver(const tessera::Plugin *plugin) : plugin_(plugin) {}
  void show(const tessera::SaverContext &c) override {
    parent_ = c.parent; width_ = c.width; height_ = c.height;
    face_ = lv_obj_create(parent_);
    lv_obj_remove_style_all(face_);
    lv_obj_remove_flag(face_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(face_, LV_OBJ_FLAG_SCROLLABLE);
    for (auto &label : numerals_) label = ui::label(face_, Font::DIGITS, theme::INK);
    date_ = ui::label(face_, Font::VALUE, theme::INK);
    hour_ = line(face_, 1, theme::CLOCK_HAND);
    minute_ = line(face_, 1, theme::CLOCK_HAND);
    ring_ = ui::block(face_, theme::CAMERA_PAGE);
    second_ = line(face_, 1, theme::CLOCK_HAND);
    pivot_ = ui::block(face_, theme::CLOCK_HAND);
    on_options(c.options);
  }

  void on_options(JsonObjectConst options) override {
    seconds_ = options["seconds"] | true;
    date_enabled_ = options["date"] | true;
    const bool inverted = options["invert"] | false;
    const char *layout = options["layout"] | "original";
    const char *alignment = options["alignment"] | "centre";
    int align = 0;
    if (!std::strcmp(alignment, "left")) align = 1;
    if (!std::strcmp(alignment, "right")) align = 2;
    if (!std::strcmp(alignment, "top")) align = 3;
    if (!std::strcmp(alignment, "bottom")) align = 4;
    // Reserve room for Tessera's millimetre drift at all scales and alignments.
    margin_ = ui::mm(2);
    const int margin = margin_;
    face_height_ = height_ - 2 * margin;
    face_width_ = width_ - 2 * margin;
    art_ = geometry::layout(std::max(1, width_ - 2 * margin), std::max(1, height_ - 2 * margin),
                            options["scale"] | 100.0f, !std::strcmp(layout, "adaptive"), align);
    cx_ = art_.x + art_.width / 2; cy_ = art_.y + art_.height / 2;
    const char *colour = options["numerals"] | "green";
    theme::Role role = theme::MARK_GREEN;
    if (!std::strcmp(colour, "white")) role = theme::QR_LIGHT;
    if (!std::strcmp(colour, "blue")) role = theme::MARK_BLUE;
    if (!std::strcmp(colour, "purple")) role = theme::MARK_PURPLE;
    if (!std::strcmp(colour, "amber")) role = theme::MARK_AMBER;
    const theme::Role background = inverted ? role : theme::CAMERA_PAGE;
    const theme::Role ink = inverted ? theme::QR_DARK : role;
    const theme::Role hand_ink = inverted && !std::strcmp(colour, "white") ? theme::QR_DARK : theme::CLOCK_HAND;
    lv_obj_set_style_bg_color(parent_, ui::color(background), 0);
    lv_obj_set_style_bg_opa(parent_, LV_OPA_COVER, 0);
    lv_obj_set_size(face_, face_width_, face_height_);
    lv_obj_set_pos(face_, margin, margin);
    const char *digits[] = {"12", "3", "6", "9"};
    const int inset = std::max(1, static_cast<int>(8 * art_.unit));
    numeral_font_ = Font::DIGITS;
    for (auto font : {Font::DIGITS, Font::VALUE, Font::HEADLINE, Font::TITLE}) {
      numeral_font_ = font;
      if (ui::line_height(font) <= art_.height * .42f && ui::text_width("12", font) <= art_.width * .48f) break;
    }
    const int height = ui::line_height(numeral_font_);
    for (int i = 0; i < 4; ++i) {
      auto *label = numerals_[i];
      ui::set_text(label, digits[i]); ui::set_font(label, numeral_font_); ui::set_color(label, ink);
      const int width = ui::text_width(digits[i], numeral_font_);
      lv_obj_set_size(label, width, height);
      const int x = i == 1 ? art_.x + art_.width - inset - width : i == 3 ? art_.x + inset : cx_ - width / 2;
      const int y = i == 0 ? art_.y + inset : i == 2 ? art_.y + art_.height - inset - height : cy_ - height / 2;
      lv_obj_set_pos(label, x, y);
    }
    const char *position = options["date_position"] | "bottom_left";
    date_right_ = std::strstr(position, "right") != nullptr;
    date_top_ = std::strstr(position, "top") != nullptr;
    const char *format = options["date_format"] | "weekday";
    date_format_ = !std::strcmp(format, "month") ? 1 : !std::strcmp(format, "numeric") ? 2 : 0;
    const char *date_size = options["date_size"] | "large";
    date_size_ = !std::strcmp(date_size, "small") ? 0 : !std::strcmp(date_size, "medium") ? 1 : 2;
    utc_ = !std::strcmp(options["timezone"] | "screen", "utc");
    const char *motion = options["motion"] | "smooth";
    interval_ = !std::strcmp(motion, "tick") ? 0 : !std::strcmp(motion, "fluid") ? 16 : 33;
    ui::set_color(date_, ink);
    date_text_[0] = 0;
    for (auto *part : {hour_, minute_}) {
      lv_obj_set_style_line_width(part, std::max(1, static_cast<int>(6.2f * art_.unit)), 0);
      lv_obj_set_style_line_color(part, ui::color(hand_ink), 0);
    }
    ui::set_fill(ring_, tessera::Tone::NORMAL, background);
    const int diameter = std::max(3, static_cast<int>(9.2f * art_.unit));
    lv_obj_set_size(ring_, diameter, diameter);
    lv_obj_set_pos(ring_, static_cast<int>(cx_ - diameter / 2.0f), static_cast<int>(cy_ - diameter / 2.0f));
    lv_obj_set_style_radius(ring_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(ring_, std::max(1, static_cast<int>(1.2f * art_.unit)), 0);
    lv_obj_set_style_border_color(ring_, ui::color(hand_ink), 0);
    lv_obj_set_style_line_width(second_, std::max(1, static_cast<int>(.85f * art_.unit)), 0);
    lv_obj_set_style_line_color(second_, ui::color(inverted ? theme::QR_DARK : hand_ink), 0);
    ui::set_fill(pivot_, tessera::Tone::NORMAL, hand_ink);
    const int dot = std::max(2, static_cast<int>(3.6f * art_.unit));
    lv_obj_set_size(pivot_, dot, dot);
    lv_obj_set_pos(pivot_, static_cast<int>(cx_ - dot / 2.0f), static_cast<int>(cy_ - dot / 2.0f));
    lv_obj_set_style_radius(pivot_, LV_RADIUS_CIRCLE, 0);
    on_tick(tessera::epoch());
  }

  uint32_t frame_interval_ms() const override { return interval_; }
  void on_frame(uint32_t now_ms) override { paint_time(tessera::epoch(), now_ms, true); }
  void on_tick(uint32_t epoch) override { paint_time(epoch, esphome::millis(), interval_ != 0); }

 private:
  void paint_time(uint32_t epoch, uint32_t now_ms, bool smooth) {
    const auto drift = tessera::drift(epoch);
    lv_obj_set_pos(face_, margin_ + drift.x, margin_ + drift.y);
    if (!epoch) {
      phase_ = {};
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
    const auto a = geometry::angles(now.hour, now.minute, now.second + (smooth ? phase_.fraction(epoch, now_ms) : 0));
    update_hand(hour_, hour_points_, 48, 9, a.hour);
    update_hand(minute_, minute_points_, 82, 10, a.minute);
    if (seconds_) update_hand(second_, second_points_, 91, 13, a.second);
    if (date_enabled_) paint_date(now);
    else visible(date_, false);
  }

  static lv_obj_t *line(lv_obj_t *parent, float width, theme::Role role) {
    auto *part = lv_line_create(parent);
    lv_obj_remove_flag(part, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_line_width(part, std::max(1, static_cast<int>(std::lround(width))), 0);
    lv_obj_set_style_line_color(part, ui::color(role), 0);
    lv_obj_set_style_line_rounded(part, true, 0);
    return part;
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
    // The corner must clear the native 12/6 label's full box, including its font's line height.
    const int numeral_width = ui::text_width(date_top_ ? "12" : "6", numeral_font_);
    const int corner = std::max(1, (static_cast<int>(art_.width) - numeral_width) / 2 - inset - ui::px(4));
    const Font fonts[] = {Font::VALUE, Font::HEADLINE, Font::TITLE, Font::BODY};
    const int first = date_size_ == 0 ? 2 : date_size_ == 1 ? 1 : 0;
    Font font = Font::BODY;
    for (int i = first; i < 4; ++i) {
      if (ui::text_width(text, fonts[i]) <= corner) { font = fonts[i]; break; }
    }
    ui::set_font(date_, font);
    const int width = std::min(room, ui::text_width(text, font));
    int x = static_cast<int>(art_.x) + (date_right_ ? static_cast<int>(art_.width) - inset - width : inset);
    const int line_height = ui::line_height(font);
    int y = static_cast<int>(art_.y) + (date_top_ ? inset : static_cast<int>(art_.height) - inset - line_height);
    if (date_format_ == 2 || width > corner) {
      // At a small clock scale a long numeric date can outgrow every built-in
      // font's corner. Use the free space outside the clock instead of its 6.
      const int outside = date_top_ ? static_cast<int>(art_.y) - line_height - ui::px(6)
                                   : static_cast<int>(art_.y + art_.height) + ui::px(6);
      const int opposite = date_top_ ? static_cast<int>(art_.y + art_.height) + ui::px(6)
                                    : static_cast<int>(art_.y) - line_height - ui::px(6);
      if (outside >= 0 && outside + line_height <= face_height_) {
        y = outside;
      } else if (opposite >= 0 && opposite + line_height <= face_height_) {
        y = opposite;
      } else if (width > corner) {
        // Full-height clocks have no free row. Use spare horizontal margin,
        // moving away from the numeral instead of truncating the date.
        x += (date_right_ ? 1 : -1) * (width - corner + ui::px(2));
        x = std::clamp(x, 0, std::max(0, face_width_ - width));
      }
    }
    lv_obj_set_width(date_, std::max(1, width));
    lv_obj_set_pos(date_, x, y);
    visible(date_, true);
  }
  const tessera::Plugin *plugin_;
  lv_obj_t *parent_{};
  int width_ = 0, height_ = 0;
  Font numeral_font_ = Font::DIGITS;
  uint32_t interval_ = 33;
  geometry::SecondPhase phase_{};
  geometry::Layout art_{};
  float cx_ = 0, cy_ = 0;
  bool seconds_ = true, date_enabled_ = true, date_right_ = false, date_top_ = false, utc_ = false;
  int date_format_ = 0;
  int date_size_ = 2, margin_ = 0, face_height_ = 0, face_width_ = 0;
  char date_text_[64]{};
  lv_obj_t *numerals_[4]{};
  lv_point_precise_t hour_points_[2]{}, minute_points_[2]{}, second_points_[2]{};
  lv_obj_t *face_{}, *hour_{}, *minute_{}, *second_{}, *ring_{}, *pivot_{}, *date_{};
};
void WatchClock::setup() {
  add_saver("watch", tessera::SaverStep::CLOCK, [this]() { return new WatchSaver(this); });
}
}  // namespace esphome::watch_clock
