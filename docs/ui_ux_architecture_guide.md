# Brave UI/UX Architecture Guide

This document is a source-level map of Brave’s desktop UI and a practical
playbook for modifying it.

Use it for two things:

1. Understanding where a given piece of UI lives.
2. Deciding which file to edit first for a specific customization.

## Quick Start

If you only need the shortest route to the right part of the codebase, start
here.

| If you want to change... | Start here | Usually also check |
| --- | --- | --- |
| Settings shell, nav, layout | `browser/resources/settings/br/settings_ui.ts` | `br/settings_menu.ts`, `br/settings_main.ts` |
| Add a settings page | `browser/resources/settings/brave_routes.ts` | `br/basic_page.ts`, `br/settings_main.ts`, `browser/ui/webui/brave_settings_ui.cc` |
| Settings control bound to a pref | `browser/resources/settings/brave_*/*.html` | paired `.ts` file, pref registration/owner |
| Toolbar or app menu | `browser/ui/toolbar/brave_app_menu_model.cc` | `browser/ui/views/toolbar/`, command controller files |
| Tabs or tab strip | `browser/ui/views/tabs/` | `browser/ui/tabs/`, frame/layout code |
| Address bar / omnibox presentation | `browser/ui/views/location_bar/` | `browser/ui/omnibox/`, page action UI |
| Sidebar / side panel | `browser/ui/views/sidebar/` | `browser/ui/sidebar/`, `components/sidebar/` |
| NTP | `browser/ui/webui/new_tab_page/brave_new_tab_ui.cc` | `components/brave_new_tab_ui/` |
| Wallet / Rewards / AI Chat page UI | corresponding `browser/ui/webui/...` controller | corresponding `components/*_ui/` or `components/.../resources/` |
| Dialog, bubble, infobar | `browser/ui/views/` subdirectory for that surface | page info, permission, or overlay integration |
| Native theme/colors | `browser/ui/color/` | `browser/ui/darker_theme/` |
| Replace an upstream WebUI | `chromium_src/chrome/browser/ui/webui/` | Brave-owned controller/resources target |

## 1. High-Level Architecture

Brave desktop UI is split into two main systems.

| Layer | Purpose | Primary locations |
| --- | --- | --- |
| Native desktop UI | Browser chrome, windows, tabs, toolbar, menus, dialogs, bubbles | `browser/ui/`, `browser/ui/views/` |
| WebUI | Settings, NTP, internal pages, feature pages, panels | `browser/ui/webui/`, `browser/resources/`, `components/*_ui/` |

Brave’s customization model sits on top of Chromium:

| Layer | Role in Brave |
| --- | --- |
| Upstream Chromium | Base Views framework, WebUI infrastructure, default pages, many core models |
| `browser/ui/` | Brave-owned native UI logic, models, controllers |
| `browser/ui/views/` | Brave-owned native desktop presentation |
| `browser/ui/webui/` | Brave C++ WebUI controllers, handlers, Mojo bindings |
| `browser/resources/` | Brave desktop WebUI frontend code, especially Settings overrides |
| `components/*_ui/` | Reusable, larger feature frontends, often React-based |
| `chromium_src/` | Targeted upstream overrides and replacement hooks |

### How Chromium and Brave Interact

Brave typically avoids rewriting all upstream UI from scratch. Instead it uses
three strategies:

| Strategy | When it is used | Typical location |
| --- | --- | --- |
| Extend upstream classes | Native chrome tweaks and feature additions | `browser/ui/`, `browser/ui/views/` |
| Replace or register a Brave WebUI | Full-page WebUIs like Settings or History | `browser/ui/webui/`, `chromium_src/chrome/browser/ui/webui/` |
| Patch or inject frontend changes | Chromium Settings and other existing WebUI | `browser/resources/settings/br/`, `chromium_src/.../resources/...` |

The central replacement hook is
`chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc`, which removes
upstream configs and inserts Brave-owned configs for pages such as Settings and
History.

### Frontend Technology Stack

| Technology | Where it appears | Typical use |
| --- | --- | --- |
| Polymer / Chromium Web Components | Settings and many Chromium-derived WebUI | UI structure, pref-bound controls, shell integration |
| TypeScript | Most WebUI frontend code | UI logic, proxies, glue code |
| React | NTP, Wallet, some larger feature UIs | Complex application-style surfaces |
| Redux or store-driven state | NTP, Wallet, similar complex UIs | Stateful feature pages |
| Mojo | NTP, Wallet, History embeddings, richer feature pages | Typed IPC between JS and C++ |
| `chrome.send` + message callbacks | Settings and simpler WebUI | Lightweight request/response backend calls |
| Storybook | Componentized WebUI work | Fast visual iteration |
| Webpack + GN preprocessing | WebUI build pipeline | Resource generation and bundling |

Relevant background docs:

- `docs/webui_frontend.md`
- `docs/webui_overriding.md`
- `browser/resources/settings/BUILD.gn`
- `browser/resources/settings/settings.gni`

## 2. Architecture Decision Guide

Before editing anything, identify what type of surface you are changing.

| UI type | Primary code style | Best first question |
| --- | --- | --- |
| Browser chrome | Native Views/C++ | Is this window chrome, a toolbar control, tab UI, menu, or dialog? |
| Settings | Chromium Settings WebUI plus Brave overrides | Is this a route, menu item, page container, pref-bound control, or handler? |
| NTP / Wallet / Rewards | Feature-specific WebUI app | Is this frontend-only, controller-side, or Mojo contract work? |
| Prompt / bubble / dialog | Native Views/C++ | Is this Brave-specific or a generic Chromium prompt? |
| Upstream page tweak | Mixed | Can this be a narrow override instead of a full replacement? |

Use this routing rule:

1. If it is browser chrome, start in `browser/ui/views/`.
2. If it is a full page at a `chrome://` URL, start in `browser/ui/webui/`.
3. If it is Settings, start in `browser/resources/settings/`.
4. If it is a complex feature app, look for a paired controller in
   `browser/ui/webui/` and frontend in `components/*_ui/`.
5. If it is an upstream page that Brave lightly reshapes, inspect
   `chromium_src/` and existing override helpers before replacing anything.

## 3. Surface Inventory

### Browser Chrome and Native Desktop UI

| Surface | Primary locations | Backend/control entry | Render entry | What to edit first |
| --- | --- | --- | --- | --- |
| Window frame / shell | `browser/ui/views/frame/`, `browser/ui/browser_window/` | browser window setup and layout managers | frame views and root view | frame view or layout manager |
| Tabs / tab strip | `browser/ui/tabs/`, `browser/ui/views/tabs/` | tab model, prefs, placement logic | tab strip and tab views | `browser/ui/views/tabs/` for appearance, `browser/ui/tabs/` for behavior |
| Toolbar | `browser/ui/toolbar/`, `browser/ui/views/toolbar/` | command and menu model logic | toolbar views and buttons | toolbar views or app menu model |
| Omnibox / address bar | `browser/ui/omnibox/`, `browser/ui/views/location_bar/` | omnibox integration and action logic | location bar and page action views | location bar view for visual changes |
| Sidebar | `browser/ui/sidebar/`, `browser/ui/views/sidebar/`, `components/sidebar/` | sidebar controller/service/model | sidebar container/item views | sidebar views for layout, service/model for capabilities |
| Side panel | `browser/ui/side_panel/`, `browser/ui/views/side_panel/` | side panel coordinator | panel header/container views | panel coordinator or header view |
| Context menus | `browser/ui/views/renderer_context_menu/`, `browser/ui/tabs/` | menu setup and command population | menu Views wrapper | menu model or wrapper cleanup |
| Permission prompts / bubbles | `browser/ui/views/permission_bubble/`, `browser/ui/views/page_info/` | prompt implementation | anchored bubble UI | prompt impl for Brave-specific prompts |
| Dialogs / overlays / infobars | `browser/ui/views/` subdirectories | feature-specific controller | dialog/overlay/infobar view | specific dialog view |
| Theme/colors | `browser/ui/color/`, `browser/ui/darker_theme/` | color provider setup | native theme application | color IDs and mixer registration |

### WebUI and Internal Pages

| Surface | Controller location | Frontend location | IPC pattern | Best modification point |
| --- | --- | --- | --- | --- |
| Settings | `browser/ui/webui/brave_settings_ui.cc` | `browser/resources/settings/` | message callbacks plus some Mojo | `br/` shell overrides and `brave_*` feature pages |
| NTP | `browser/ui/webui/new_tab_page/` | `components/brave_new_tab_ui/` | Mojo plus legacy handlers | React app and controller pair |
| History | `browser/ui/webui/history/` | Chromium history resources plus Brave integration | Mojo | Brave history UI/controller and injected frontend |
| Wallet | `browser/ui/webui/brave_wallet/` | `components/brave_wallet_ui/` | Mojo | page controller plus wallet UI frontend |
| Rewards | `browser/ui/webui/brave_rewards/` | `components/brave_rewards/resources/` | Mojo and feature-specific handlers | page controller plus rewards frontend |
| AI Chat | `browser/ui/webui/ai_chat/` | feature frontend under components/resources | Mojo | feature controller and frontend pair |
| Welcome / onboarding | `browser/ui/webui/brave_welcome_page/` | corresponding frontend resources | mixed | page controller and page resources |
| Shields page/panel | `browser/ui/webui/brave_shields/` | page or panel frontend | mixed | controller or panel frontend |

## 4. Settings System Deep Dive

Settings is the most important UI subsystem if you want broad UI/UX
customization.

### Core Settings Entry Points

| Responsibility | File |
| --- | --- |
| Replace upstream Settings WebUI config | `chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc` |
| Define Brave Settings config | `browser/ui/webui/brave_settings_ui.h` |
| Register Brave handlers and bindings | `browser/ui/webui/brave_settings_ui.cc` |
| Override upstream SettingsUI wiring | `chromium_src/chrome/browser/ui/webui/settings/settings_ui.cc` |
| Frontend bootstrap imports | `browser/resources/settings/br/index.ts` |
| Route injection | `browser/resources/settings/brave_routes.ts` |
| Page visibility model | `browser/resources/settings/br/page_visibility.ts` |
| Main page container injection | `browser/resources/settings/br/settings_main.ts` |
| Left navigation styling and item injection | `browser/resources/settings/br/settings_menu.ts` |
| Top-level shell styling and behavior | `browser/resources/settings/br/settings_ui.ts` |

### Settings Flow in One Table

| Step | What happens | Main file |
| --- | --- | --- |
| Registration | Brave removes Chromium Settings config and installs its own | `chrome_web_ui_configs.cc` |
| Controller setup | `BraveSettingsUI` extends `SettingsUI` and adds handlers | `brave_settings_ui.cc` |
| Build merge | Brave frontend resources are preprocessed into Chromium’s Settings bundle | `browser/resources/settings/BUILD.gn` |
| Routing | Brave adds sections and child routes | `brave_routes.ts` |
| Visibility | Brave decides which sections appear | `br/page_visibility.ts` |
| Page composition | Brave injects and replaces page containers | `br/settings_main.ts` |
| Navigation | Brave restyles and rewrites left-nav items | `br/settings_menu.ts` |
| Persistence | Pref binding, browser proxy calls, or Mojo | page component + handler / interface |

### Settings Persistence Patterns

| Pattern | Use it when | Frontend example | Backend example |
| --- | --- | --- | --- |
| Direct pref binding | Control maps directly to an existing pref | `browser/resources/settings/brave_appearance_page/browser_ui.html` | no custom handler required beyond pref plumbing |
| Browser proxy + message handler | Need async data or imperative calls without a full Mojo contract | `browser/resources/settings/brave_new_tab_page/brave_new_tab_browser_proxy.ts` | `browser/ui/webui/settings/brave_appearance_handler.cc` |
| Mojo | Surface is richer, shared, typed, or already service-backed | feature-specific Mojo frontend | `browser/ui/webui/brave_settings_ui.h` bindings and owned handlers |

### Settings Modification Map

| If you want to... | Edit first | Then inspect |
| --- | --- | --- |
| Add a top-level settings section | `browser/resources/settings/brave_routes.ts` | `br/settings_menu.ts`, `br/settings_main.ts`, `br/page_visibility.ts` |
| Add a child subpage | `browser/resources/settings/brave_routes.ts` | parent page index, page container import, handler/proxy |
| Show or hide a section | `browser/resources/settings/br/page_visibility.ts` | shell or page container logic |
| Change settings left-nav structure | `browser/resources/settings/br/settings_menu.ts` | route definitions and page visibility |
| Replace shell layout | `browser/resources/settings/br/settings_ui.ts` | `br/settings_main.ts` |
| Insert page containers | `browser/resources/settings/br/settings_main.ts` | route definitions and imports |
| Add a pref-bound row | page HTML under `browser/resources/settings/brave_*` or `br/` | paired `.ts` file and pref owner |
| Add imperative backend logic | browser proxy file | matching handler in `browser/ui/webui/settings/` |
| Add typed feature logic | Mojo interface and bindings | handler/controller class |

## 5. Modification Playbook

### Change Complexity Guide

| Change type | Typical difficulty | Why |
| --- | --- | --- |
| Change labels, styles, spacing, order inside a Brave-owned page | Low | Usually local to frontend templates or CSS |
| Add a pref-bound control | Low to medium | Requires pref ownership but not usually new IPC |
| Add a settings page or sidebar item | Medium | Touches routing, composition, maybe backend registration |
| Add toolbar button or app menu entry | Medium | Involves commands, visibility, and native layout |
| Redesign NTP or Wallet page | Medium to high | App-style surface, often isolated but stateful |
| Replace generic Chromium prompt or chrome-wide behavior | High | Crosses Chromium boundaries and upstream ownership |
| Replace major upstream page architecture | High | More brittle against upstream churn |

### Recommended Extension Points

| Extension point | Best for | Avoid using it for |
| --- | --- | --- |
| `browser/ui/views/` | Native desktop presentation changes | frontend-only WebUI work |
| `browser/ui/webui/` | C++ page controllers, handlers, Mojo bindings | purely visual frontend tweaks |
| `browser/resources/settings/br/` | Settings shell and upstream Settings overrides | feature-isolated standalone apps |
| `browser/resources/settings/brave_*` | Brave settings feature pages | full app-style UIs unrelated to settings |
| `components/*_ui/` | Reusable React-heavy feature frontends | tiny one-off settings controls |
| `chromium_src/` | Minimal upstream hooks and replacements | large feature logic if a Brave-owned layer can contain it |

### Main Maintenance Risks

| Risk | Where it shows up most |
| --- | --- |
| Upstream DOM/template changes break overrides | `br/settings_main.ts`, Polymer/Lit injection points, manglers |
| Upstream route or component refactors | Settings overrides and `chromium_src` patches |
| Platform-specific native layout regressions | frame, toolbar, tab strip, dialogs |
| Cross-layer drift between C++, TS, and Mojo contracts | complex WebUI surfaces |
| Broad changes touching pref ownership and UI wiring | settings and feature toggles |

## 6. Developer Workflow

### Build and Run Commands

| Task | Command |
| --- | --- |
| Full build | `npm run build` |
| Debug build | `npm run build -- Debug` |
| Release build | `npm run build Release` |
| Run browser | `npm start [Release|Component|Static|Debug]` |
| GN generation | `npm run gn` |
| GN check | `npm run gn_check` |

### Frontend Iteration Workflow

| Task | Command | Best for |
| --- | --- | --- |
| Storybook dev server | `npm run storybook` | componentized WebUI and fast visual iteration |
| Frontend bundle build | `npm run web-ui` | WebUI bundling checks |
| Generate TS config | `npm run web-ui-gen-tsconfig` | editor/build support |
| Jest unit tests | `npm run test-unit` | JS/TS component logic and React UI |

### Test and Validation Workflow

| Surface | First validation | Broader validation |
| --- | --- | --- |
| Settings frontend | `npm run test-unit -- [path/filter]` when tests exist | full browser run |
| React WebUI feature page | Jest + Storybook | manual browser integration run |
| Native Views UI | targeted browser/unit test if present | full browser run |
| `chromium_src` template override | focused unit/snapshot/update test if available | full browser run |

Relevant docs:

- `docs/running_test_suites.md`
- `docs/webui_testing.md`
- `docs/webui_overriding.md`

## 7. Appendix: How to Modify X

### A. Add a New Settings Page

Use this when you want a new top-level or child page in `chrome://settings`.

| Step | Edit |
| --- | --- |
| 1 | Add the route in `browser/resources/settings/brave_routes.ts` |
| 2 | Add visibility logic in `browser/resources/settings/br/page_visibility.ts` if conditional |
| 3 | Create the page frontend under `browser/resources/settings/brave_<feature>/` |
| 4 | Import the page/index in `browser/resources/settings/br/basic_page.ts` or the appropriate page container |
| 5 | Inject the page container in `browser/resources/settings/br/settings_main.ts` |
| 6 | Add a nav item in `browser/resources/settings/br/settings_menu.ts` if the page should appear in left nav |
| 7 | Register a handler in `browser/ui/webui/brave_settings_ui.cc` if the page needs backend calls |
| 8 | Add build/resource entries in `browser/resources/settings/BUILD.gn` and `sources.gni` if needed |

Validate with:

- focused Jest tests if the page has frontend logic
- manual run of `chrome://settings/...`

### B. Add a New Settings Control Backed by a Pref

Use this when the UI only needs to read or write an existing pref.

| Step | Edit |
| --- | --- |
| 1 | Add the row to the relevant `.html` template under `browser/resources/settings/` |
| 2 | Bind the row to `prefs...` or call `setPrefValue(...)` in the paired `.ts` file |
| 3 | Verify the pref already exists and is exposed in the Settings context |

Good examples:

- `browser/resources/settings/brave_appearance_page/browser_ui.html`
- `browser/resources/settings/brave_appearance_page/browser_ui.ts`

### C. Add a Settings Control That Needs Backend Logic

Use this when the UI needs computed data, policy checks, or imperative backend
actions.

| Step | Edit |
| --- | --- |
| 1 | Add or extend a browser proxy under `browser/resources/settings/` |
| 2 | Add UI calls in the paired page component |
| 3 | Register a message callback handler in `browser/ui/webui/settings/` |
| 4 | Add the handler to `browser/ui/webui/brave_settings_ui.cc` |

Good example path:

- proxy: `browser/resources/settings/brave_new_tab_page/brave_new_tab_browser_proxy.ts`
- page: `browser/resources/settings/brave_new_tab_page/brave_new_tab_page.ts`
- handler: `browser/ui/webui/settings/brave_appearance_handler.cc`

### D. Restyle the Entire Settings Experience

Use this when you want to change the layout, nav, spacing, shell behavior, or
page composition rather than a single page.

| Focus | File |
| --- | --- |
| Shell structure and scrolling behavior | `browser/resources/settings/br/settings_ui.ts` |
| Left-nav items, icons, and selection styling | `browser/resources/settings/br/settings_menu.ts` |
| Main page container injection and section placement | `browser/resources/settings/br/settings_main.ts` |

This is the main trio for Settings UX redesigns.

### E. Add or Reorder App Menu Items

| Step | Edit |
| --- | --- |
| 1 | Update `browser/ui/toolbar/brave_app_menu_model.cc` |
| 2 | Confirm the command ID exists and is enabled in the command controller path |
| 3 | Adjust icon or submenu wiring if needed |

If the item is not showing up, inspect command enable state before changing UI
layout again.

### F. Add a Toolbar Button

| Step | Edit |
| --- | --- |
| 1 | Find the relevant toolbar container under `browser/ui/views/toolbar/` |
| 2 | Add or register the button view |
| 3 | Wire visibility, command, or feature gating |
| 4 | Update layout logic if position matters |

Also check:

- command controller or feature flags
- icon assets / theme colors

### G. Redesign Tabs or Tab Strip Layout

| Focus | Start in |
| --- | --- |
| Visual rendering | `browser/ui/views/tabs/` |
| Persistent behavior, prefs, placement rules | `browser/ui/tabs/` |
| Shell/layout side effects | `browser/ui/views/frame/`, `browser/ui/views/split_view/` |

### H. Customize the NTP

| Step | Edit |
| --- | --- |
| 1 | Inspect controller and bindings in `browser/ui/webui/new_tab_page/brave_new_tab_ui.cc` |
| 2 | Modify React app in `components/brave_new_tab_ui/` |
| 3 | Adjust store/actions/reducers for stateful changes |
| 4 | Update Mojo or handlers only if the frontend needs new data or actions |

Use Storybook or targeted frontend tests where available, then validate in the
browser.

### I. Add a New Brave WebUI Page Outside Settings

Use this when the destination is a standalone `chrome://...` page rather than a
Settings section.

| Step | Edit |
| --- | --- |
| 1 | Add or find a WebUI controller under `browser/ui/webui/<feature>/` |
| 2 | Register a `*UIConfig` or controller factory path |
| 3 | Add resources/frontend under `browser/resources/` or `components/*_ui/` |
| 4 | Bind Mojo or message handlers |
| 5 | Register or replace config in `chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc` if needed |

### J. Replace an Upstream WebUI Safely

Preferred order:

1. Add the smallest `chromium_src` hook that gives Brave control.
2. Move substantial logic into a Brave-owned target.
3. Keep override files narrow so upstream updates are easier to rebase.

This follows the guidance in `docs/webui_overriding.md`.

## 8. Recommended Reading Order

For a first serious UI modification pass, read these in order:

1. `chromium_src/chrome/browser/ui/webui/chrome_web_ui_configs.cc`
2. `browser/ui/webui/brave_settings_ui.cc`
3. `browser/resources/settings/brave_routes.ts`
4. `browser/resources/settings/br/settings_ui.ts`
5. `browser/resources/settings/br/settings_menu.ts`
6. `browser/resources/settings/br/settings_main.ts`
7. one representative native surface under `browser/ui/views/`
8. one representative feature page under `browser/ui/webui/` plus its frontend

## 9. Summary

The key structural facts are:

| Fact | Why it matters |
| --- | --- |
| Browser chrome is mostly native Views/C++ | Major chrome redesigns start in `browser/ui/views/` |
| Settings is Chromium Settings plus Brave overrides | Route, menu, shell, and handler changes are split across multiple files |
| NTP and several feature pages are more app-like | React/Mojo changes can be more self-contained |
| `chromium_src/` is powerful but costly | Keep upstream overrides minimal when possible |
| The safest edits start at the highest Brave-owned layer | Reduces maintenance burden against Chromium churn |
