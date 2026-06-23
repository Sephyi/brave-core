// Copyright (c) 2026 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import "../settings_shared.css.js";
import "../settings_vars.css.js";

import { PolymerElement } from "chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js";
import type { PrefsMixinInterface } from "/shared/settings/prefs/prefs_mixin.js";
import { PrefsMixin } from "/shared/settings/prefs/prefs_mixin.js";

import { getTemplate } from "./browser_ui.html.js";

const SettingsBraveAppearanceBrowserUiElementBase = PrefsMixin(
	PolymerElement,
) as {
	new (): PolymerElement & PrefsMixinInterface;
};

export class SettingsBraveAppearanceBrowserUiElement extends SettingsBraveAppearanceBrowserUiElementBase {
	static get is() {
		return "settings-brave-appearance-browser-ui";
	}

	static get template() {
		return getTemplate();
	}

	static get properties() {
		return {
			scaleOptions_: {
				readOnly: true,
				type: Array,
				value() {
					return [
						{ value: 90, name: "90%" },
						{ value: 100, name: "100%" },
						{ value: 110, name: "110%" },
						{ value: 125, name: "125%" },
						{ value: 140, name: "140%" },
						{ value: 150, name: "150%" },
					];
				},
			},
		};
	}

	private declare scaleOptions_: Array<{ value: number; name: string }>;
}

customElements.define(
	SettingsBraveAppearanceBrowserUiElement.is,
	SettingsBraveAppearanceBrowserUiElement,
);
