# OldEyes Watchface for Pebble

**OldEyes** is a high-visibility, minimalistic analog/digital hybrid watchface for Pebble smartwatches. Designed for clarity and legibility, it combines an ultra-clean display with a tap-activated weather and environmental data overlay.

---

## 🌟 Key Features

* **High-Legibility Time Display:** Features a large, clear 24-hour digital clock.
* **Perimeter Minute Tracker:** A progress line around the outer edge of the display tracks the current minute (0–59) in real time against 60 precision tick marks.
* **Automatic Day/Night Inversion:** Standard black-on-white high-contrast theme during daylight hours; automatically switches to an inverted white-on-black theme from sunset to sunrise.
* **Health & Battery Info:**
  * Displays today's step count via Pebble Health integration.
  * Live battery level percentage and `CHRG` indicator.
* **On-Demand Weather Card (Shake Overlay):**
  * **Interactive:** Flick or tap your wrist to reveal current weather details, temperature, Air Quality Index (AQI), and location name.
  * **Smart Night Delay:** Instant overlay during the day; delayed by 2 seconds at night to prevent unwanted glaring flashes. Automatically dismisses after 5 seconds.
* **Power & Performance Optimized:**
  * **Suppressed Redraws:** Skips unnecessary repaints while in Quiet Time or while charging to preserve battery life.
  * Static runtime tick calculations pre-computed per platform.

---

## ⌚ Multi-Platform Support

OldEyes includes custom pixel-aligned layouts and font configurations tailored specifically for each Pebble hardware platform and screen resolution:

| Platform | Display Type | Resolution | Features & Layout Adjustments |
| :--- | :--- | :--- | :--- |
| **Aplite / Basalt / Diorite / Flint** | Rectangular | 144 × 168 | Classic layout with top-region hours and bottom-region date & steps. |
| **Emery** | Rectangular | 200 × 228 | Upscaled layout tailored for high-resolution rectangular displays. |
| **Chalk** | Round | 180 × 180 | Circular sweeping arc minute tracker with stacked split display. |
| **Gabbro** | Round | 260 × 260 | High-density circular display with larger font targets. |

---

## 📱 AppMessage Data Protocol

The companion JS application (`src/pkjs/index.js`) communicates using raw numeric key IDs ranging from `10001` to `10006` to pass weather and daylight metrics:

| Key ID | Field | Format / Values | Description |
| :---: | :--- | :--- | :--- |
| `10001` | **Sunrise** | `int32` (UNIX Epoch) | Sunrise time used for theme switching. |
| `10002` | **Sunset** | `int32` (UNIX Epoch) | Sunset time used for theme switching. |
| `10003` | **Temperature** | `String` (`T:<val>`) | Current temperature text (e.g., `T:72°F`). |
| `10004` | **Condition** | `String` (`D:<val>`) | Short weather description (e.g., `D:Partly Cloudy`). |
| `10005` | **Location** | `String` (`L:<val>`) | Location name (supports multi-line word wrapping). |
| `10006` | **AQI** | `String` (`A:<val>`) | Air Quality Index summary. |

---

## 🛠️ Diagnostics & Fallbacks

If no weather data has been received from the phone companion app:
* Tapping the watchface opens an overlay displaying live diagnostics (total AppMessage count and dropped packet logs).
* Built-in string markers (`T:`, `D:`, `L:`, `A:`) ensure reliable tuple parsing even across different phone SDK versions.