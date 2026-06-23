# Brave UI/UX Architecture Guide

This document maps the major user-facing UI surfaces in Brave Core and explains
how to customize them at the source-code level.

It is written for developers who are new to the codebase and want to make
significant UI or UX changes safely.

## 1. Architecture Overview

Brave desktop UI is split across two primary systems:

1. Native Chromium Views/C++ UI for browser chrome and transient desktop UI.
2. WebUI surfaces for settings, NTP, internal pages, feature pages, and some
   panels.

At a high level, the layering is:

- Native shell and browser chrome: `browser/ui/` and `browser/ui/views/`
- WebUI C++ controllers and registration: `browser/ui/webui/`
- WebUI frontend resources:
  - Settings-specific desktop resources: `browser/resources/`
  - Reusable feature UIs: `components/`
- Chromium replacement hooks: `chromium_src/`

The key architectural relationship is:

- Upstream Chromium provides the base browser framework, Views widgets, WebUI
  infrastructure, and many default pages.
- Brave either:
  - extends upstream C++ classes in Brave-owned directories,
  - replaces upstream registration/configuration,
  - overrides upstream source files via `chromium_src/`, or
  - injects frontend changes into Chromium WebUI using Polymer or Lit override
    utilities.

The central WebUI replacement point is
`chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc`, where Brave
removes upstream configs for Settings, History, and private NTP, then
registers Brave equivalents.

### Chromium vs Brave Customization Model

- `browser/ui/` and `browser/ui/views/` contain Brave-owned native desktop UI.
- `browser/ui/webui/` contains Brave C++ WebUI controllers and handlers.
- `browser/resources/` contains Brave desktop WebUI frontend code, especially
  Settings integrations and overrides.
- `components/*_ui/` contains larger reusable feature frontends, usually React
  based.
- `chromium_src/` contains selective upstream overrides and patch shims.

### Frontend Frameworks and Build Technology

Frontend technologies in use:

- Polymer / Chromium Web Components for Settings and many Chromium-derived
  pages
- TypeScript throughout WebUI code
- React for larger Brave feature UIs such as NTP and Wallet
- Redux or store-driven state in some complex React surfaces
- Mojo for typed IPC on complex WebUIs
- `chrome.send` / `RegisterMessageCallback` for simpler settings and legacy
  flows
- Webpack and Brave’s WebUI preprocessing pipeline for frontend builds
- Storybook for component-level UI iteration

The main docs for this are:

- `docs/webui_frontend.md`
- `docs/webui_overriding.md`
- `browser/resources/settings/BUILD.gn`
- `browser/resources/settings/settings.gni`

## 2. UI Component Inventory

### Browser Window, Frame, and Layout

Primary directories:

- `browser/ui/views/frame/`
- `browser/ui/browser_window/`

What lives here:

- Browser frame and root window
- Platform-specific frame views
- Content layout coordination
- Vertical tabs and split-view integration hooks

Suggested modification points:

- Frame appearance and root layout: `browser/ui/views/frame/`
- Content composition and advanced layouts: `browser/ui/views/split_view/`

This layer is mostly native Views/C++. If you want to redesign the top-level
desktop shell, this is where that work starts.

### Tabs and Tab Strip

Primary directories:

- `browser/ui/tabs/`
- `browser/ui/views/tabs/`

What lives here:

- Tab model and tab-state behavior
- Views for tab strip, tab rendering, hover cards, vertical tabs
- Tree tabs and tab placement coordination

Suggested modification points:

- Tab preferences and persistent behavior: `browser/ui/tabs/`
- Rendering and layout: `browser/ui/views/tabs/`

### Toolbar, App Menu, and Browser Actions

Primary directories:

- `browser/ui/toolbar/`
- `browser/ui/views/toolbar/`
- `browser/ui/views/brave_actions/`

Key file:

- `browser/ui/toolbar/brave_app_menu_model.cc`

What lives here:

- App menu composition
- Toolbar button layout and feature buttons
- Brave-specific buttons like Wallet, VPN, AI Chat, and Sidebar entries

Data and rendering flow:

- Command availability is decided by command controllers and feature state.
- Menu model builds items and submenus.
- Views layer renders actual controls.

Suggested modification points:

- Add or reorder app menu items: `browser/ui/toolbar/brave_app_menu_model.cc`
- Toolbar UI placement: `browser/ui/views/toolbar/`

### Omnibox, Address Bar, and Page Actions

Primary directories:

- `browser/ui/omnibox/`
- `browser/ui/views/location_bar/`
- `browser/ui/views/page_action/`

What lives here:

- Address bar layout and styling
- Action icons and badges
- Shields integration and onion indicators

This is native Views-based chrome. Styling and composition changes are here;
search logic itself is often deeper in Chromium or Brave omnibox integrations.

### Sidebar and Side Panel

Primary directories:

- `browser/ui/sidebar/`
- `browser/ui/views/sidebar/`
- `browser/ui/side_panel/`
- `browser/ui/views/side_panel/`
- Shared sidebar services: `components/sidebar/`

What lives here:

- Brave sidebar container and item rendering
- Sidebar service, model, and controller
- Side panel integrations for feature surfaces

Suggested modification points:

- Sidebar item list and behavior: `browser/ui/views/sidebar/`
- Shared model and service rules: `components/sidebar/`

### Context Menus

Primary directories:

- `browser/ui/views/renderer_context_menu/`
- `browser/ui/tabs/`

Representative file:

- `browser/ui/views/renderer_context_menu/brave_render_view_context_menu_views.cc`

What lives here:

- Renderer and page context menu Views wrapper
- Cleanup and final shaping on top of upstream menu initialization
- Tab-specific context menus in tab menu model code

This surface is lighter Brave glue than Settings or NTP. For basic
additions and removals, you often need to trace both the menu model
population and the Brave wrapper.

### Permission Prompts, Page Info, and Bubbles

Primary directories:

- `browser/ui/views/permission_bubble/`
- `browser/ui/views/permissions/`
- `browser/ui/page_info/`
- `browser/ui/views/page_info/`

Representative file:

- `browser/ui/views/permission_bubble/brave_wallet_permission_prompt_impl.cc`

What lives here:

- Brave-specific permission prompt implementations
- Anchored bubbles
- Page info and Shields dialog integration

This area mixes Brave-specific prompts with upstream permission infrastructure.
If you want to change generic site permission prompts globally, expect upstream
Chromium involvement.

### Built-in Dialogs and Overlays

Primary directories:

- `browser/ui/views/`
- notable subdirectories:
  - `browser/ui/views/overlay/`
  - `browser/ui/views/infobars/`
  - `browser/ui/views/page_info/`

Representative file:

- `browser/ui/views/crash_report_permission_ask_dialog_view.cc`

What lives here:

- Constrained modal dialogs
- Infobars
- Help bubbles
- Overlays and feature-specific transient UI

### Theme and Design System

Primary directories:

- `browser/ui/color/`
- `browser/ui/darker_theme/`

What lives here:

- Brave color IDs and mixers
- Private and Tor theme variants
- Color-provider integration with Chromium UI system

Suggested modification points:

- Color IDs and theme registration: `browser/ui/color/`
- Darker theme transforms: `browser/ui/darker_theme/`

### WebUI Controllers and Internal Pages

Primary directory:

- `browser/ui/webui/`

Major surfaces under it:

- `browser/ui/webui/brave_settings_ui.cc`
- `browser/ui/webui/new_tab_page/`
- `browser/ui/webui/history/`
- `browser/ui/webui/brave_wallet/`
- `browser/ui/webui/brave_rewards/`
- `browser/ui/webui/brave_welcome_page/`
- `browser/ui/webui/ai_chat/`
- `browser/ui/webui/brave_shields/`

This directory is the main backend and controller map for Brave WebUI.

## 3. Settings System Analysis

Settings is the most important UI subsystem if your goal is broad UI and UX
customization.

### Core Settings Architecture

Entry points:

- WebUI registration:
  `chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc`
- Settings config class: `browser/ui/webui/brave_settings_ui.h`
- Settings controller: `browser/ui/webui/brave_settings_ui.cc`
- Upstream override injection:
  `chromium_src/chrome/browser/ui/webui/settings/settings_ui.cc`

What happens:

1. Chromium’s default Settings WebUI is removed from the config map.
2. Brave registers `BraveSettingsUIConfig`.
3. `BraveSettingsUI` extends Chromium’s `SettingsUI`.
4. Brave adds additional handlers and binds extra Mojo interfaces.
5. Frontend resources are merged into Chromium’s Settings frontend build.
6. Brave injects additional routes, pages, style overrides, and template
   replacements.

### Frontend Locations

Main Settings frontend tree:

- `browser/resources/settings/`

Important files:

- Module bootstrap: `browser/resources/settings/br/index.ts`
- Route injection: `browser/resources/settings/brave_routes.ts`
- Page visibility rules: `browser/resources/settings/br/page_visibility.ts`
- Main page template modification:
  `browser/resources/settings/br/settings_main.ts`
- Left navigation and styling:
  `browser/resources/settings/br/settings_menu.ts`
- Top-level settings shell override:
  `browser/resources/settings/br/settings_ui.ts`
- Basic-page imports and section composition:
  `browser/resources/settings/br/basic_page.ts`

### How Settings Are Registered, Routed, Rendered, and Persisted

#### Registered

Settings is registered through `BraveSettingsUIConfig` and the WebUI config map
replacement in `chrome_web_ui_configs.cc`.

#### Routed

Brave adds settings routes in `browser/resources/settings/brave_routes.ts`.

Examples include:

- `/getStarted`
- `/origin`
- `/shields`
- `/extensions`
- `/braveSync`
- `/web3`
- `/newTab`
- `/leo-ai`
- `/braveContent`

This is the main place to add new top-level or child routes.

#### Rendered

Brave modifies Chromium’s `settings-main` template in
`browser/resources/settings/br/settings_main.ts` using
`RegisterPolymerTemplateModifications`.

That file appends new `dom-if` page containers into Chromium’s view switcher
and can also replace upstream slots entirely.

#### Persisted

There are three common persistence patterns.

1. Direct pref binding in Polymer templates and components.
2. Browser proxy plus `chrome.send` and a C++ message handler.
3. Mojo-bound services for more complex settings surfaces.

### Persistence Pattern 1: Direct Pref Binding

Representative files:

- `browser/resources/settings/brave_appearance_page/browser_ui.html`
- `browser/resources/settings/brave_appearance_page/browser_ui.ts`

These use `pref="{{prefs....}}"` or `setPrefValue(...)` and are the easiest
path when the value is already exposed through Chromium’s pref plumbing.

### Persistence Pattern 2: Browser Proxy and Message Handler

Representative files:

- `browser/resources/settings/brave_new_tab_page/brave_new_tab_browser_proxy.ts`
- `browser/resources/settings/brave_new_tab_page/brave_new_tab_page.ts`
- `browser/ui/webui/settings/brave_appearance_handler.cc`

Flow:

- Component calls a browser proxy method.
- Proxy uses `sendWithPromise`.
- C++ `RegisterMessageCallback` resolves it.
- Handler may also `FireWebUIListener` for push updates.

### Persistence Pattern 3: Mojo-Bound Services

`BraveSettingsUI` also binds Mojo interfaces in
`browser/ui/webui/brave_settings_ui.h`. Use this when the settings surface is
more complex or already backed by a reusable service interface.

### How to Add New Settings Pages, Sections, and Controls

Typical path:

1. Add a route in `browser/resources/settings/brave_routes.ts`.
2. Add a page visibility flag in `browser/resources/settings/br/page_visibility.ts`
   if needed.
3. Create a frontend page directory under `browser/resources/settings/`.
4. Import the page or index from `browser/resources/settings/br/basic_page.ts`
   or another relevant container.
5. Inject the page container in `browser/resources/settings/br/settings_main.ts`.
6. Add a navigation item in `browser/resources/settings/br/settings_menu.ts`
   if it should be visible in the left navigation.
7. Add message handler registration in `browser/ui/webui/brave_settings_ui.cc`
   if custom backend logic is needed.
8. Add build and resource entries in `browser/resources/settings/BUILD.gn`
   and related `sources.gni` if required.

### How to Modify or Replace Existing Settings Pages

Three levels of intervention:

- Low impact:
  - change styles,
  - insert or remove rows,
  - change labels,
  - hide or show sections.
- Medium impact:
  - replace menu organization,
  - move routes between sections,
  - override templates and swap page containers.
- High impact:
  - replace whole upstream page implementations,
  - rebuild backend handlers and UI bindings,
  - change underlying pref or feature plumbing.

Brave already does all three.

## 4. Customization Guide

### What Is Easy to Modify

Usually safe and localized:

- Settings layout and styles in `browser/resources/settings/br/`
- Settings controls that map directly to existing prefs
- Settings navigation order and styling
- App menu entries in `browser/ui/toolbar/brave_app_menu_model.cc`
- Dialog and Views styling in Brave-owned dialog files
- React WebUI feature surfaces like NTP and Wallet

### What Requires Deeper Architectural Changes

Higher cost changes:

- Core browser chrome layout
- Generic permission prompt flows across Chromium
- Omnibox or address bar behavior beyond presentation
- Tab strip model, selection, and drag behavior
- Large upstream WebUI replacement without using Brave’s targeted override
  strategies

### Recommended Extension Points

Best-supported seams in this codebase:

- `chromium_src/` for narrow upstream replacement
- `browser/ui/webui/*` for per-page WebUI controller logic
- `browser/resources/settings/br/*` for Settings shell and template injection
- `browser/resources/settings/brave_*` for standalone settings features
- `components/*_ui` for large React-based feature UIs
- `browser/ui/views/*` for native desktop chrome and dialogs
- `browser/ui/color/*` for native theming

### Risks, Dependencies, and Maintenance Considerations

Main risks:

- Upstream Chromium changes can break override assumptions, especially template
  injections and route rewrites.
- Polymer and Lit override code is more fragile than fully Brave-owned pages.
- Large native chrome changes can collide with platform-specific frame and
  layout behavior.
- Mojo contract changes require coordinated C++ and generated JS or TS updates.
- Settings changes often span route graph, menu, page visibility, frontend
  resources, and backend handler registration.

The repo docs explicitly recommend minimal `chromium_src` overrides and moving
larger logic into Brave-owned targets where possible.

## 5. Practical Examples

### Modify an Existing Settings Control

Pattern:

- A pref-bound toggle, dropdown, or input in
  `browser/resources/settings/brave_appearance_page/browser_ui.html`
- Custom input handling in
  `browser/resources/settings/brave_appearance_page/browser_ui.ts`

Use this when the UI maps directly to a pref and no special backend
orchestration is required.

### Add a New Settings Subpage with Backend Behavior

Pattern:

- Add a route in `browser/resources/settings/brave_routes.ts`
- Create a browser proxy like
  `browser/resources/settings/brave_new_tab_page/brave_new_tab_browser_proxy.ts`
- Add a page component like
  `browser/resources/settings/brave_new_tab_page/brave_new_tab_page.ts`
- Register a C++ handler like
  `browser/ui/webui/settings/brave_appearance_handler.cc`

Use this when you need async data, dynamic visibility, event-driven updates, or
nontrivial persistence and business logic.

### Restyle the Entire Settings UI

Key files:

- `browser/resources/settings/br/settings_ui.ts`
- `browser/resources/settings/br/settings_menu.ts`
- `browser/resources/settings/br/settings_main.ts`

These already implement a substantial redesign over Chromium and are the
cleanest existing blueprint for a major settings overhaul.

### Replace a Chromium WebUI Page with a Brave-Specific Page

Pattern:

- Remove upstream config in
  `chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc`
- Add a Brave `*UIConfig`
- Create or extend a backend controller under `browser/ui/webui/`
- Provide frontend resources in `browser/resources/` or `components/`

Brave does this for Settings and History.

### Customize the New Tab Page

Core files:

- Backend controller: `browser/ui/webui/new_tab_page/brave_new_tab_ui.cc`
- Frontend entry: `components/brave_new_tab_ui/brave_new_tab.tsx`
- Frontend tree: `components/brave_new_tab_ui/`

Flow:

- C++ creates the data source and binds handlers and interfaces.
- React app boots, dispatches init, and wires API events into the store.
- Store-driven UI updates render the NTP.

This is a strong candidate for major redesign because it is already a React app
rather than a narrow Chromium Polymer patch.

### Customize Wallet or Other Feature Pages

Representative page:

- `browser/ui/webui/brave_wallet/wallet_page/wallet_page_ui.cc`
- Frontend components: `components/brave_wallet_ui/`

These are complex Mojo-backed feature UIs. If you want large-scale product-page
redesigns, these are more self-contained than core browser chrome.

## 6. Developer Workflow

### Build and Run

From `README.md` and `package.json`:

- Full build: `npm run build`
- Debug build: `npm run build -- Debug`
- Release build: `npm run build Release`
- Run browser: `npm start [Release|Component|Static|Debug]`
- Generate GN or build config: `npm run gn`
- GN check: `npm run gn_check`

### Hot Reload and Frontend Iteration

Relevant scripts in `package.json`:

- Storybook dev: `npm run storybook`
- WebUI webpack build: `npm run web-ui`
- Unit tests: `npm run test-unit`
- Generate TS config: `npm run web-ui-gen-tsconfig`

Per `docs/webui_frontend.md`, Storybook is the intended fast iteration path for
componentized WebUI work and supports hot reload.

Practical distinction:

- For React feature surfaces and component libraries: use Storybook and Jest.
- For Chromium-derived settings pages and native UI: you will usually still
  need full browser runs to validate integration.

### Testing and Validation

Relevant docs:

- `docs/running_test_suites.md`
- `docs/webui_testing.md`

Key commands:

- JS or TS unit tests: `npm run test-unit -- [path or test filter]`
- C++ unit or browser tests:
  `npm run test -- [suite] --filter="Fixture.Test"`

Example suites:

- `brave_unit_tests`
- `brave_browser_tests`
- `browser_tests`

Validation strategy by UI type:

- Settings frontend logic: Jest where possible, browser or manual validation
  for route integration
- React WebUI features: Jest plus Storybook plus manual browser run
- Native Views UI: `brave_browser_tests` or `browser_tests` where coverage
  exists, plus manual validation
- `chromium_src` template overrides: watch for snapshot or update tests
  referenced in `docs/webui_overriding.md`

## 7. Quick Reference by UI Area

### Settings

- Backend: `browser/ui/webui/brave_settings_ui.cc`
- Frontend shell: `browser/resources/settings/br/`
- Feature pages: `browser/resources/settings/brave_*`
- Main routing hook: `browser/resources/settings/brave_routes.ts`
- Best modification points:
  - `br/settings_ui.ts`
  - `br/settings_menu.ts`
  - `br/settings_main.ts`
  - `brave_routes.ts`

### Browser Chrome

- Native Views root: `browser/ui/views/`
- Toolbar: `browser/ui/views/toolbar/`
- Tabs: `browser/ui/views/tabs/`
- Address bar: `browser/ui/views/location_bar/`
- Menus: `browser/ui/toolbar/`
- Best modification points:
  - `browser/ui/toolbar/brave_app_menu_model.cc`
  - `browser/ui/views/toolbar/`
  - `browser/ui/views/tabs/`

### New Tab Page

- Backend controller: `browser/ui/webui/new_tab_page/`
- Frontend: `components/brave_new_tab_ui/`
- Best modification points:
  - `brave_new_tab_ui.cc`
  - `brave_new_tab.tsx`
  - store and reducers under `components/brave_new_tab_ui/`

### Brave Feature Pages

- Wallet: `browser/ui/webui/brave_wallet/` and `components/brave_wallet_ui/`
- Rewards: `browser/ui/webui/brave_rewards/` and
  `components/brave_rewards/resources/`
- AI Chat: `browser/ui/webui/ai_chat/`
- Welcome pages: `browser/ui/webui/brave_welcome_page/`

### Context Menus and Prompts

- Renderer context menu: `browser/ui/views/renderer_context_menu/`
- Permission prompts: `browser/ui/views/permission_bubble/`
- Page info and Shields: `browser/ui/views/page_info/`

### Themes and Native Styling

- Color system: `browser/ui/color/`
- Darker theme transforms: `browser/ui/darker_theme/`

## 8. Recommended Starting Map

If your goal is broad UI or UX customization, start in this order:

1. Settings shell and navigation:
   - `browser/resources/settings/br/settings_ui.ts`
   - `browser/resources/settings/br/settings_menu.ts`
   - `browser/resources/settings/br/settings_main.ts`
2. Settings routing and visibility:
   - `browser/resources/settings/brave_routes.ts`
   - `browser/resources/settings/br/page_visibility.ts`
3. Settings backend controller:
   - `browser/ui/webui/brave_settings_ui.cc`
4. NTP:
   - `browser/ui/webui/new_tab_page/brave_new_tab_ui.cc`
   - `components/brave_new_tab_ui/`
5. Native browser chrome:
   - `browser/ui/views/`
   - `browser/ui/toolbar/`
   - `browser/ui/tabs/`
6. WebUI registration and replacement:
   - `chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc`

## 9. Summary

The most important structural fact in Brave UI development is that not all UI
surfaces are built the same way:

- Browser chrome is mostly native Views/C++.
- Settings is a Chromium WebUI heavily reshaped by Brave template, route, and
  style overrides.
- NTP and some major feature pages are more self-contained React or Mojo-backed
  applications.
- Narrow upstream customization is usually done through `chromium_src/`, but
  larger work is easier to maintain in Brave-owned code.

For most significant UI work, the safest approach is:

1. Find whether the surface is native Views, Chromium WebUI override, or a
   self-contained Brave WebUI.
2. Work at the highest Brave-owned layer possible.
3. Use `chromium_src/` only for the minimal upstream hook or replacement.
4. Validate with the narrowest relevant frontend or browser test, then run the
   full browser for integration checks.
