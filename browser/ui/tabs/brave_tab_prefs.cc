/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/tabs/brave_tab_prefs.h"

#include <algorithm>

#include "base/feature_list.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/ui/tabs/features.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace brave_tabs {

namespace {

int GetScaleFromLocalState(const char* pref_name) {
  if (!g_browser_process || !g_browser_process->local_state()) {
    return kBrowserChromeScaleDefault;
  }

  return std::clamp(g_browser_process->local_state()->GetInteger(pref_name),
                    kBrowserChromeScaleMin, kBrowserChromeScaleMax);
}

int ScaleMetric(int value, int scale) {
  if (value == 0) {
    return 0;
  }

  const int scaled = (value * scale + 50) / kBrowserChromeScaleDefault;
  return std::max(1, scaled);
}

}  // namespace

void RegisterBraveProfilePrefs(PrefRegistrySimple* registry) {
  registry->RegisterIntegerPref(kTabHoverMode, TabHoverMode::CARD);
  registry->RegisterBooleanPref(kVerticalTabsEnabled, false);
  registry->RegisterBooleanPref(kVerticalTabsCollapsed, false);
  registry->RegisterBooleanPref(kVerticalTabsExpandedStatePerWindow, false);
#if BUILDFLAG(IS_WIN)
  // On Windows, we show window title by default
  // https://github.com/brave/brave-browser/issues/30027
  registry->RegisterBooleanPref(kVerticalTabsShowTitleOnWindow, true);
#else
  registry->RegisterBooleanPref(kVerticalTabsShowTitleOnWindow, false);
#endif

  if (base::FeatureList::IsEnabled(tabs::kBraveVerticalTabHideCompletely)) {
    registry->RegisterBooleanPref(kVerticalTabsHideCompletelyWhenCollapsed,
                                  false);
  }

  registry->RegisterBooleanPref(kVerticalTabsFloatingEnabled, true);
  registry->RegisterIntegerPref(kVerticalTabsExpandedWidth, 220);
  registry->RegisterBooleanPref(kVerticalTabsOnRight, false);
  registry->RegisterBooleanPref(kVerticalTabsShowScrollbar, false);
  registry->RegisterBooleanPref(kShowHorizontalTabScrollButtons, false);

  registry->RegisterBooleanPref(kSharedPinnedTab, false);

  if (base::FeatureList::IsEnabled(tabs::kBraveTreeTab)) {
    registry->RegisterBooleanPref(kTreeTabsEnabled, false);
  }

  registry->RegisterBooleanPref(kAlwaysHideTabCloseButton, false);
  registry->RegisterBooleanPref(kMiddleClickCloseTabEnabled, true);
  registry->RegisterIntegerPref(kTabMinWidthMode,
                                static_cast<int>(TabMinWidthMode::kMinimum));
  registry->RegisterBooleanPref(kScrollableHorizontalTabStrip, false);
}

void MigrateBraveProfilePrefs(PrefService* prefs) {
  if (auto* pref = prefs->FindPreference(kVerticalTabsShowScrollbar);
      pref && pref->IsDefaultValue() &&
      base::FeatureList::IsEnabled(tabs::kBraveVerticalTabScrollBar)) {
    prefs->SetBoolean(kVerticalTabsShowScrollbar, true);
  }
}

void RegisterLocalStatePrefs(PrefRegistrySimple* registry) {
  registry->RegisterBooleanPref(kCompactHorizontalTabs, false);
  registry->RegisterIntegerPref(kBrowserChromeScale,
                                kBrowserChromeScaleDefault);
  registry->RegisterIntegerPref(kHorizontalTabScale,
                                kBrowserChromeScaleDefault);
  registry->RegisterIntegerPref(kVerticalTabScale,
                                kBrowserChromeScaleDefault);
  registry->RegisterIntegerPref(kToolbarScale, kBrowserChromeScaleDefault);
  registry->RegisterIntegerPref(kAddressBarScale,
                                kBrowserChromeScaleDefault);
}

int GetBrowserChromeScale() {
  return GetScaleFromLocalState(kBrowserChromeScale);
}

int GetHorizontalTabScale() {
  return GetScaleFromLocalState(kHorizontalTabScale);
}

int GetVerticalTabScale() {
  return GetScaleFromLocalState(kVerticalTabScale);
}

int GetToolbarScale() {
  return GetScaleFromLocalState(kToolbarScale);
}

int GetAddressBarScale() {
  return GetScaleFromLocalState(kAddressBarScale);
}

int ScaleBrowserChromeMetric(int value) {
  return ScaleMetric(value, GetBrowserChromeScale());
}

int ScaleHorizontalTabMetric(int value) {
  return ScaleMetric(value, GetHorizontalTabScale());
}

int ScaleVerticalTabMetric(int value) {
  return ScaleMetric(value, GetVerticalTabScale());
}

int ScaleToolbarMetric(int value) {
  return ScaleMetric(value, GetToolbarScale());
}

int ScaleAddressBarMetric(int value) {
  return ScaleMetric(value, GetAddressBarScale());
}

bool AreTooltipsEnabled(PrefService* prefs) {
  return prefs->GetInteger(kTabHoverMode) == TabHoverMode::TOOLTIP;
}

bool AreCardPreviewsEnabled(PrefService* prefs) {
  return prefs->GetInteger(kTabHoverMode) == TabHoverMode::CARD_WITH_PREVIEW;
}

bool IsScrollableHorizontalTabStripEnabled(const PrefService* prefs) {
  return base::FeatureList::IsEnabled(tabs::kBraveScrollableTabStrip) &&
         prefs->GetBoolean(kScrollableHorizontalTabStrip);
}

}  // namespace brave_tabs
