var RETRY_MS = 60 * 1000;
var REFRESH_MS = 15 * 60 * 1000;
var SAFETY_MS = 15 * 1000;

var lastLat = null;
var lastLon = null;
var retryTimer = null;
var refreshTimer = null;
var safetyTimer = null;
var sentAnything = false;
var fallbackSent = false;

var DEG = Math.PI / 180;
var RAD = 180 / Math.PI;

/* Weather, place and air quality are fetched from separate APIs; the watch
 * is sent ONE message once all are available, using raw NUMERIC key IDs
 * because CloudPebble's toolchain never emits the message-key table
 * (symbolic names are dropped by the phone at send time - the watch showed
 * "k: empty" under that scheme). The IDs map to the toolchain auto-assignment
 * base 10000 from package.json messageKeys: sunrise=10001, sunset=10002,
 * weather_temp=10003, weather_desc=10004, place_name=10005, weather_aqi=10006
 * (request=10000 unused). Values keep marker prefixes ("T:", "D:", "L:",
 * "A:") which the watch strips. Sunrise/sunset are plain epoch ints; the
 * temp already carries the degree symbol ("31\u00B0C").
 */
var pending = {
  weather_temp: null,
  weather_desc: null,
  sunrise: null,
  sunset: null,
  place_name: null,
  aqi: null
};

function scheduleRetry() {
  if (retryTimer) {
    return;
  }
  retryTimer = setTimeout(function () {
    retryTimer = null;
    refresh();
  }, RETRY_MS);
}

function wmoToText(code) {
  if (code === 0)              return "Clear";
  if (code === 1 || code === 2) return "Partly Cloudy";
  if (code === 3)              return "Overcast";
  if (code >= 45 && code <= 48) return "Foggy";
  if (code >= 51 && code <= 55) return "Drizzle";
  if (code >= 61 && code <= 65) return "Rain";
  if (code === 66 || code === 67) return "Freezing Rain";
  if (code >= 71 && code <= 75) return "Snow";
  if (code === 77)             return "Snow";
  if (code >= 80 && code <= 82) return "Showers";
  if (code === 85 || code === 86) return "Snow Showers";
  if (code === 95)             return "Thunderstorm";
  if (code >= 96)              return "Storm";
  return "Weather";
}

function toEpoch(isoLocal) {
  return Math.round(Date.parse(isoLocal) / 1000);
}

/* One solar event (rise hr=6, set hr=18) as minutes since local midnight,
 * classic NOAA-style algorithm (zenith 90.833 degrees). Returns null for a
 * polar day/night where cos(H) falls outside [-1, 1]. This is a phone-side
 * fallback only, so a slight zone drift from the exact meridian is fine. */
function solarEvent(N, lngHour, lat, hr) {
  var t = N + ((hr - lngHour) / 24);
  var M = (0.9856 * t) - 3.289;
  var L = M + (1.916 * Math.sin(M * DEG)) +
          (0.020 * Math.sin(2 * M * DEG)) + 282.634;
  L = ((L % 360) + 360) % 360;
  var RA = Math.atan(0.91764 * Math.tan(L * DEG)) * RAD;
  RA = ((RA % 360) + 360) % 360;
  RA += Math.floor(L / 90) * 90 - Math.floor(RA / 90) * 90;
  RA /= 15.0;
  var sinDec = 0.39782 * Math.sin(L * DEG);
  var cosDec = Math.cos(Math.asin(sinDec));
  var cosH = (Math.cos(90.833 * DEG) - sinDec * Math.sin(lat * DEG)) /
             (cosDec * Math.cos(lat * DEG));
  if (cosH > 1 || cosH < -1) {
    return null;
  }
  var acosH = Math.acos(cosH) * RAD;
  var H = (hr === 6) ? (360 - acosH) : acosH;
  H /= 15.0;
  var T = H + RA - (0.06571 * t) - 6.622;
  var ut = T - lngHour;
  var mins = ((ut % 24) + 24) % 24 * 60;
  return Math.round(mins) % 1440;
}

/* Today's sunrise/sunset as minutes since local midnight, or null when
 * either event does not occur (polar day/night). */
function solarRiseSet(lat, lon) {
  var now = new Date();
  var month = now.getMonth() + 1;
  var N = Math.floor((275 * month) / 9) -
          2 * Math.floor((month + 9) / 12) + now.getDate() - 30;
  var lngHour = lon / 15.0;
  var rise = solarEvent(N, lngHour, lat, 6);
  var set = solarEvent(N, lngHour, lat, 18);
  if (rise === null || set === null) {
    return null;
  }
  return { rise: rise, set: set };
}

/* Minutes-since-local-midnight -> epoch seconds for today, using the phone's
 * real timezone offset (a +ve getTimezoneOffset() means behind UTC). */
function localToEpoch(minutes) {
  var now = new Date();
  var dayStart = Math.floor(
      Date.UTC(now.getFullYear(), now.getMonth(), now.getDate()) / 1000);
  return dayStart - now.getTimezoneOffset() * 60 + Math.round(minutes * 60);
}

function sendToWatch(data) {
  Pebble.sendAppMessage(data, function () {
    console.log('OldEyes: data sent to watch');
  }, function (err) {
    console.log('OldEyes: sendAppMessage failed: ' + JSON.stringify(err));
  });
}

/* Last-known payload snapshot for the offline path. Values are stored
 * marker-free ("31°C", "Rain", "Berlin, Germany", "AQI: 42") plus epoch
 * rise/set; the offline caller re-adds the "T:"/"D:"/"L:"/"A:" prefixes. */
function ovCacheWrite(obj) {
  try {
    var keys = ['rise', 'set', 'temp', 'desc', 'place', 'aqi'];
    var out = {};
    for (var i = 0; i < keys.length; i++) {
      if (obj[keys[i]] !== null && obj[keys[i]] !== undefined) {
        out[keys[i]] = obj[keys[i]];
      }
    }
    if (Object.keys(out).length === 0) {
      return;
    }
    localStorage.setItem('ov_cache', JSON.stringify(out));
  } catch (e) {
    console.log('OldEyes: ov_cache write failed');
  }
}

function ovCacheRead() {
  try {
    var raw = localStorage.getItem('ov_cache');
    if (!raw) {
      return null;
    }
    var j = JSON.parse(raw);
    if (!j || typeof j !== 'object') {
      return null;
    }
    return j;
  } catch (e) {
    return null;
  }
}

function maybeSend() {
  if (pending.weather_temp === null || pending.place_name === null ||
      pending.aqi === null) {
    return;
  }
  /* Raw NUMERIC key IDs: CloudPebble's toolchain never emits a message-key
   * table, so symbolic names are dropped by the phone at send time (watch
   * saw "k: empty"). Numeric IDs pass through without a table. The numbers
   * match the toolchain's auto-assignment from package.json messageKeys
   * (request=10000, then sunrise=10001, sunset=10002, weather_temp=10003,
   * weather_desc=10004, place_name=10005, weather_aqi=10006). */
  sendToWatch({
    10001: pending.sunrise,
    10002: pending.sunset,
    10003: 'T:' + pending.weather_temp,
    10004: 'D:' + pending.weather_desc,
    10005: 'L:' + pending.place_name,
    10006: 'A:' + pending.aqi
  });
  ovCacheWrite({
    rise: pending.sunrise,
    set: pending.sunset,
    temp: pending.weather_temp,
    desc: pending.weather_desc,
    place: pending.place_name,
    aqi: pending.aqi
  });
  sentAnything = true;
  pending.weather_temp = null;
  pending.weather_desc = null;
  pending.sunrise = null;
  pending.sunset = null;
  pending.place_name = null;
  pending.aqi = null;
}

/* Offline path: the weather APIs failed (or nothing arrived before the
 * safety window), so send a best-effort snapshot from local caches plus a
 * sunrise/sunset computed on the phone for today. Rise/set priority:
 * computed-today > cached > 6:00/18:00 local (only when we never had
 * coordinates at all; polar locations fall back to the cached pair). Text
 * rows merge whatever fresh values we hold in `pending` over the cache.
 * Delivered via sendToWatch directly (bypassing the maybeSend gate) and
 * only once - the flag latches across every failure branch and the safety
 * timer. */
function offlineFallback() {
  if (fallbackSent) {
    return;
  }
  fallbackSent = true;
  if (safetyTimer) {
    clearTimeout(safetyTimer);
    safetyTimer = null;
  }
  var cache = ovCacheRead() || {};
  var lat = null;
  var lon = null;
  try {
    var lats = localStorage.getItem('ov_lat');
    var lons = localStorage.getItem('ov_lon');
    if (lats !== null && lons !== null) {
      lat = parseFloat(lats);
      lon = parseFloat(lons);
    }
  } catch (e) {}
  var rise = null;
  var set = null;
  var sol = null;
  if (lat !== null && lon !== null && isFinite(lat) && isFinite(lon)) {
    sol = solarRiseSet(lat, lon);
  }
  if (sol) {
    rise = localToEpoch(sol.rise);
    set = localToEpoch(sol.set);
  } else if (typeof cache.rise === 'number' && typeof cache.set === 'number') {
    rise = cache.rise;
    set = cache.set;
  } else {
    rise = localToEpoch(6 * 60);
    set = localToEpoch(18 * 60);
  }
  var temp = pending.weather_temp || cache.temp || '--';
  var desc = pending.weather_desc || cache.desc || 'offline';
  var place = pending.place_name || cache.place || 'offline';
  var aqi = pending.aqi || cache.aqi || 'AQI: --';
  sendToWatch({
    10001: rise,
    10002: set,
    10003: 'T:' + temp,
    10004: 'D:' + desc,
    10005: 'L:' + place,
    10006: 'A:' + aqi
  });
  ovCacheWrite({
    rise: rise,
    set: set,
    temp: temp,
    desc: desc,
    place: place,
    aqi: aqi
  });
  console.log('OldEyes: offline fallback sent');
}

function fetchWeather(lat, lon) {
  var url = 'https://api.open-meteo.com/v1/forecast' +
    '?latitude=' + lat +
    '&longitude=' + lon +
    '&current=temperature_2m,weather_code' +
    '&daily=sunrise,sunset&timezone=auto&forecast_days=1';

  var xhr = new XMLHttpRequest();
  xhr.open('GET', url, true);
  xhr.onload = function () {
    if (xhr.status !== 200) {
      console.log('OldEyes: open-meteo http ' + xhr.status);
      scheduleRetry();
      offlineFallback();
      return;
    }
    try {
      var j = JSON.parse(xhr.responseText);
      pending.weather_temp = Math.round(j.current.temperature_2m) + '\u00B0C';
      pending.weather_desc = wmoToText(j.current.weather_code);
      pending.sunrise = toEpoch(j.daily.sunrise[0]);
      pending.sunset = toEpoch(j.daily.sunset[0]);
      maybeSend();
    } catch (e) {
      console.log('OldEyes: open-meteo parse error: ' + e);
      scheduleRetry();
      offlineFallback();
    }
  };
  xhr.onerror = function () {
    console.log('OldEyes: open-meteo request error');
    scheduleRetry();
    offlineFallback();
  };
  xhr.send();
}

function fetchAqi(lat, lon) {
  var url = 'https://air-quality-api.open-meteo.com/v1/air-quality' +
    '?latitude=' + lat +
    '&longitude=' + lon +
    '&current=us_aqi';

  var xhr = new XMLHttpRequest();
  xhr.open('GET', url, true);
  xhr.onload = function () {
    if (xhr.status !== 200) {
      console.log('OldEyes: air-quality http ' + xhr.status);
      scheduleRetry();
      offlineFallback();
      return;
    }
    try {
      var j = JSON.parse(xhr.responseText);
      if (j && j.current && typeof j.current.us_aqi === 'number') {
        pending.aqi = 'AQI: ' + Math.round(j.current.us_aqi);
      } else {
        console.log('OldEyes: air-quality missing us_aqi');
        scheduleRetry();
        offlineFallback();
        return;
      }
      maybeSend();
    } catch (e) {
      console.log('OldEyes: air-quality parse error: ' + e);
      scheduleRetry();
      offlineFallback();
    }
  };
  xhr.onerror = function () {
    console.log('OldEyes: air-quality request error');
    scheduleRetry();
    offlineFallback();
  };
  xhr.send();
}

function fetchPlace(lat, lon) {
  var url = 'https://api.bigdatacloud.net/data/reverse-geocode-client' +
    '?latitude=' + lat +
    '&longitude=' + lon +
    '&localityLanguage=en';

  var xhr = new XMLHttpRequest();
  xhr.open('GET', url, true);
  xhr.onload = function () {
    var name = null;
    if (xhr.status === 200) {
      try {
        var j = JSON.parse(xhr.responseText);
        var loc = j.locality || j.city || j.principalSubdivision || '';
        var ctry = j.countryName || '';
        name = loc ? (ctry ? loc + ', ' + ctry : loc) : (ctry || null);
      } catch (e) {
        console.log('OldEyes: geocode parse error: ' + e);
      }
    }
    if (!name) {
      name = lat.toFixed(1) + ', ' + lon.toFixed(1);
    }
    pending.place_name = name;
    maybeSend();
  };
  xhr.onerror = function () {
    console.log('OldEyes: geocode request error');
    pending.place_name = lat.toFixed(1) + ', ' + lon.toFixed(1);
    maybeSend();
  };
  xhr.send();
}

function fetchByCoords(lat, lon) {
  lastLat = lat;
  lastLon = lon;
  try {
    localStorage.setItem('ov_lat', String(lat));
    localStorage.setItem('ov_lon', String(lon));
  } catch (e) {}
  console.log('OldEyes: position ' + lat.toFixed(3) + ', ' + lon.toFixed(3));
  fetchWeather(lat, lon);
  fetchAqi(lat, lon);
  fetchPlace(lat, lon);
}

function geoIpFallback() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', 'https://api.bigdatacloud.net/data/ip-geolocation-with-confidence',
           true);
  xhr.onload = function () {
    if (xhr.status === 200) {
      try {
        var j = JSON.parse(xhr.responseText);
        if (j && typeof j.latitude === 'number' && typeof j.longitude === 'number') {
          console.log('OldEyes: geoip ' + j.latitude.toFixed(3) + ', ' + j.longitude.toFixed(3));
          fetchByCoords(j.latitude, j.longitude);
          return;
        }
      } catch (e) {
        console.log('OldEyes: geoip parse error: ' + e);
      }
    }
    scheduleRetry();
    offlineFallback();
  };
  xhr.onerror = function () {
    console.log('OldEyes: geoip request error');
    scheduleRetry();
    offlineFallback();
  };
  xhr.send();
}

function refresh() {
  if (navigator.geolocation &&
      typeof navigator.geolocation.getCurrentPosition === 'function') {
    navigator.geolocation.getCurrentPosition(
      function (pos) {
        fetchByCoords(pos.coords.latitude, pos.coords.longitude);
      },
      function (err) {
        console.log('OldEyes: geolocation error ' + err.code);
        if (lastLat !== null) {
          fetchByCoords(lastLat, lastLon);
        } else {
          geoIpFallback();
        }
      },
      { enableHighAccuracy: false, maximumAge: 10 * 60 * 1000, timeout: 10000 }
    );
  } else {
    console.log('OldEyes: no geolocation API');
    if (lastLat !== null) {
      fetchByCoords(lastLat, lastLon);
    } else {
      geoIpFallback();
    }
  }
}

Pebble.addEventListener('ready', function () {
  if (refreshTimer) {
    clearInterval(refreshTimer);
  }
  refreshTimer = setInterval(function () {
    console.log('OldEyes: 15 min refresh');
    refresh();
  }, REFRESH_MS);
  setTimeout(refresh, 1500);
  if (safetyTimer) {
    clearTimeout(safetyTimer);
  }
  safetyTimer = setTimeout(function () {
    safetyTimer = null;
    if (!sentAnything) {
      console.log('OldEyes: nothing delivered within ' + SAFETY_MS +
                  ' ms, offline fallback');
      offlineFallback();
    }
  }, SAFETY_MS);
});