/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_STRIP_LAYOUT_HELPER_H_
#define BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_STRIP_LAYOUT_HELPER_H_

#include <optional>
#include <utility>
#include <vector>

#include "chrome/browser/ui/views/tabs/tab_strip_layout_types.h"

namespace gfx {
class Rect;
}  // namespace gfx

class Tab;
class TabStripLayoutHelper;
class TabWidthConstraints;
class TabStripController;
class TabContainer;
class TabSlotView;
class TabStrip;

namespace tabs {

// PROOF-OF-CONCEPT: vertical-tab scale factor. 1.0 = stock, 1.25 = 25% larger.
// Adjust this single constant to resize Brave's vertical tabs. Applied to
// every vertical-tab sizing constant in this header.
inline constexpr double kBraveVerticalTabScale = 1.25;

inline constexpr int kVerticalTabHeight =
    static_cast<int>(32 * kBraveVerticalTabScale);
inline constexpr int kVerticalTabMinWidth = kVerticalTabHeight;
inline constexpr int kVerticalTabsSpacing =
    static_cast<int>(4 * kBraveVerticalTabScale);
inline constexpr int kMarginForVerticalTabContainers = kVerticalTabsSpacing;
inline constexpr int kPinnedUnpinnedSeparatorHeight = 1;

// The base offset per level for vertical tabs in tree tabs
inline constexpr int kBaseOffsetPerLevel = 20;

int GetTabCornerRadius(const Tab& tab);

void CalculatePinnedTabsBoundsInGrid(
    const std::vector<TabWidthConstraints>& tabs,
    std::optional<int> width,
    std::vector<gfx::Rect>* result);

std::pair<std::vector<gfx::Rect>, LayoutDomain> CalculateVerticalTabBounds(
    const std::vector<TabWidthConstraints>& tabs,
    std::optional<int> width,
    bool should_layout_pinned_tabs_in_grid);

std::vector<gfx::Rect> CalculateBoundsForVerticalDraggedViews(
    const std::vector<TabSlotView*>& views,
    TabStrip* tab_strip);

void UpdateInsertionIndexForVerticalTabs(
    const gfx::Rect& dragged_bounds,
    int first_dragged_tab_index,
    int num_dragged_tabs,
    bool dragged_groups,
    int candidate_index,
    TabStripController* tab_strip_controller,
    TabContainer* tab_container,
    int& min_distance,
    int& min_distance_index,
    TabStrip* tab_strip);

}  // namespace tabs

#endif  // BRAVE_BROWSER_UI_VIEWS_TABS_BRAVE_TAB_STRIP_LAYOUT_HELPER_H_
