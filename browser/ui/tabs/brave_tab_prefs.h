/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_TABS_BRAVE_TAB_PREFS_H_
#define BRAVE_BROWSER_UI_TABS_BRAVE_TAB_PREFS_H_

class PrefRegistrySimple;
class PrefService;

namespace brave_tabs {

enum TabHoverMode { TOOLTIP = 0, CARD = 1, CARD_WITH_PREVIEW = 2 };

inline constexpr char kTabHoverMode[] = "brave.tabs.hover_mode";

inline constexpr char kVerticalTabsEnabled[] =
    "brave.tabs.vertical_tabs_enabled";
inline constexpr char kVerticalTabsCollapsed[] =
    "brave.tabs.vertical_tabs_collapsed";
inline constexpr char kVerticalTabsExpandedStatePerWindow[] =
    "brave.tabs.vertical_tabs_expanded_state_per_window";
inline constexpr char kVerticalTabsShowTitleOnWindow[] =
    "brave.tabs.vertical_tabs_show_title_on_window";
inline constexpr char kVerticalTabsHideCompletelyWhenCollapsed[] =
    "brave.tabs.vertical_tabs_hide_completely_when_collapsed";
inline constexpr char kVerticalTabsFloatingEnabled[] =
    "brave.tabs.vertical_tabs_floating_enabled";
inline constexpr char kVerticalTabsExpandedWidth[] =
    "brave.tabs.vertical_tabs_expanded_width";
inline constexpr char kVerticalTabsOnRight[] =
    "brave.tabs.vertical_tabs_on_right";
inline constexpr char kVerticalTabsShowScrollbar[] =
    "brave.tabs.vertical_tabs_show_scrollbar";
// When tabs::kBraveScrollableTabStrip is on: toggles visibility of the
// horizontal tab strip scroll buttons only (wheel/trackpad overflow scrolling
// is unchanged).
inline constexpr char kShowHorizontalTabScrollButtons[] =
    "brave.tabs.show_horizontal_tab_scroll_buttons";
inline constexpr char kTreeTabsEnabled[] = "brave.tabs.tree_tabs_enabled";

inline constexpr char kSharedPinnedTab[] = "brave.tabs.shared_pinned_tab";

// A boolean local state pref that, when true, will reduce the height of the
// horizontal tab strip and toolbar. This pref is stored in local state instead
// of profile prefs in order to allow access where a Profile is not available
// in the current Chromium architecture (i.e. `layout_constants.cc`).
inline constexpr char kCompactHorizontalTabs[] =
    "brave.tabs.compact_horizontal_tabs";

// An integer local state pref storing the browser chrome scale as a
// percentage. The value is kept in local state so sizing code in global layout
// helpers can read it without requiring a Profile.
inline constexpr char kBrowserChromeScale[] = "brave.ui.browser_chrome_scale";
inline constexpr char kHorizontalTabScale[] =
    "brave.ui.horizontal_tab_scale";
inline constexpr char kVerticalTabScale[] = "brave.ui.vertical_tab_scale";
inline constexpr char kToolbarScale[] = "brave.ui.toolbar_scale";
inline constexpr char kAddressBarScale[] = "brave.ui.address_bar_scale";
inline constexpr int kBrowserChromeScaleDefault = 100;
inline constexpr int kBrowserChromeScaleMin = 90;
inline constexpr int kBrowserChromeScaleMax = 150;

inline constexpr char kAlwaysHideTabCloseButton[] =
    "brave.tabs.always_hide_tab_close_button";

inline constexpr char kMiddleClickCloseTabEnabled[] =
    "brave.tabs.middle_click_close_tab_enabled";

inline constexpr char kTabMinWidthMode[] = "brave.tabs.min_width_mode";

// When true (and kBraveScrollableTabStrip is enabled), the horizontal tab strip
// scrolls when there are more tabs than fit. When false, tabs shrink instead.
inline constexpr char kScrollableHorizontalTabStrip[] =
    "brave.tabs.scrollable_horizontal_tab_strip";

// Integer values stored in prefs; stable for migration. kDefault is reserved so
// the product default can change without migrating explicit user choices.
enum class TabMinWidthMode {
  kDefault = 0,
  kMinimum = 1,
  kMedium = 2,
  kLarge = 3,
  kFull = 4,
};

void RegisterBraveProfilePrefs(PrefRegistrySimple* registry);
void MigrateBraveProfilePrefs(PrefService* prefs);

void RegisterLocalStatePrefs(PrefRegistrySimple* registry);

int GetBrowserChromeScale();
int GetHorizontalTabScale();
int GetVerticalTabScale();
int GetToolbarScale();
int GetAddressBarScale();
int ScaleBrowserChromeMetric(int value);
int ScaleHorizontalTabMetric(int value);
int ScaleVerticalTabMetric(int value);
int ScaleToolbarMetric(int value);
int ScaleAddressBarMetric(int value);

bool AreTooltipsEnabled(PrefService* prefs);
bool AreCardPreviewsEnabled(PrefService* prefs);

// True when the scrollable horizontal tab strip feature flag is enabled and the
// user pref to use it is on.
bool IsScrollableHorizontalTabStripEnabled(const PrefService* prefs);

}  // namespace brave_tabs

#endif  // BRAVE_BROWSER_UI_TABS_BRAVE_TAB_PREFS_H_
