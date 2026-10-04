/* =========================================================
   ESP32 SMART HOME
   APP.JS
========================================================= */
const ESP32 = "";

let currentStatus = null;
let previousStatus = null;
let previousConnectionState = null;

/* =========================================================
   API HELPER
========================================================= */

async function apiRequest(endpoint, method = "GET", data = null) {
    try {
        const options = {
            method: method,
            headers: {
                "Content-Type": "application/json"
            }
        };

        if (data !== null) {
            options.body = JSON.stringify(data);
        }

        const response = await fetch(
            ESP32 + endpoint,
            options
        );

        if (!response.ok) {
            throw new Error(
                `HTTP ${response.status}`
            );
        }

        return await response.json();

    } catch (error) {

        console.error(
            "API Error:",
            error
        );

        setConnection(false);

        return null;
    }
}


/* =========================================================
   GET STATUS
========================================================= */

async function updateStatus() {

    const data =
        await apiRequest("/api/status");

    if (!data)
        return;

    currentStatus = data;

    setConnection(true);

    logStateChanges(data);

    updateSensors(data);
    updateSensorHealth(data);
    updateDevices(data);
    updateMode(data);
    updateAlarm(data);
    updateSystem(data);
}
/* =========================================================
   SYSTEM PAGE UPDATE
========================================================= */


function updateSystem(data) {

    /* ---------------------------------
       ESP32 CONNECTION
    --------------------------------- */

    setText(
        "systemConnection",
        "ONLINE"
    );


    /* ---------------------------------
       UPTIME
    --------------------------------- */

    if (data.uptime !== undefined) {

        const totalSeconds =
            Math.floor(data.uptime / 1000);

        const days =
            Math.floor(totalSeconds / 86400);

        const hours =
            Math.floor(
                (totalSeconds % 86400) / 3600
            );

        const minutes =
            Math.floor(
                (totalSeconds % 3600) / 60
            );

        const seconds =
            totalSeconds % 60;

        let uptimeText = "";

        if (days > 0)
            uptimeText += `${days}d `;

        uptimeText +=
            `${String(hours).padStart(2, "0")}:` +
            `${String(minutes).padStart(2, "0")}:` +
            `${String(seconds).padStart(2, "0")}`;

        setText(
            "systemUptime",
            uptimeText
        );
    }


    /* ---------------------------------
       FREE MEMORY
    --------------------------------- */

    if (data.free_heap !== undefined) {

        const memoryKB =
            (data.free_heap / 1024).toFixed(1);

        setText(
            "systemMemory",
            `${memoryKB} KB`
        );
    }


    /* ---------------------------------
       FIRMWARE
    --------------------------------- */

    if (data.firmware !== undefined) {

        setText(
            "systemFirmware",
            data.firmware
        );
    }


    /* ---------------------------------
       WI-FI / ACCESS POINT
    --------------------------------- */

    if (data.wifi_ssid !== undefined) {

        setText(
            "wifiStatus",
            data.wifi_ssid
        );
    }


    /* ---------------------------------
       IP ADDRESS
    --------------------------------- */

    if (data.ip !== undefined) {

        setText(
            "ipAddress",
            data.ip
        );
    }


    /* ---------------------------------
       SIGNAL
    --------------------------------- */

    /*
       ESP32 is operating as its own Access Point.
       There is no normal router Wi-Fi RSSI
       value being reported by our API.
    */

    setText(
        "wifiSignal",
        "N/A (AP)"
    );


    /* ---------------------------------
       LAST UPDATE
    --------------------------------- */

    setText(
        "lastUpdate",
        new Date().toLocaleTimeString(
            [],
            {
                hour: "2-digit",
                minute: "2-digit",
                second: "2-digit"
            }
        )
    );


    /* ---------------------------------
       API
    --------------------------------- */

    setText(
        "apiStatus",
        "ONLINE"
    );
}

/* =========================================================
   CONNECTION STATUS
========================================================= */
function setConnection(online) {

    const container =
        document.querySelector(".connection");

    const dot =
        document.querySelector(".connection-dot");

    const text =
        document.getElementById("connectionText");

    if (!container || !dot || !text)
        return;

    if (online) {

        dot.style.background = "#43e6a1";
        dot.style.color = "#43e6a1";

        text.textContent = "ONLINE";

    } else {

        dot.style.background = "#ff5577";
        dot.style.color = "#ff5577";

        text.textContent = "OFFLINE";
    }

    // Log only when connection state changes
    if (previousConnectionState !== online) {

        if (online) {
            addLog("ESP32 → ONLINE");
        } else {
            addLog("ESP32 → OFFLINE");
        }

        previousConnectionState = online;
    }
}
/* =========================================================
   SMART EVENT LOGGING
========================================================= */

function logStateChanges(data) {

    // First successful status response:
    // store it as the baseline without generating fake events.
    if (previousStatus === null) {

        previousStatus = {
            mode: data.mode,
            led1: data.led1,
            led2: data.led2,
            led3: data.led3,
            fan: data.fan,
            buzzer: data.buzzer,
            alarm_type: data.alarm_type,
            alarm_silenced: data.alarm_silenced
        };

        return;
    }


    /* ---------------------------------
       MODE
    --------------------------------- */

    if (data.mode !== previousStatus.mode) {

        addLog(
            `MODE → ${String(data.mode).toUpperCase()}`
        );
    }


    /* ---------------------------------
       LIGHTS
    --------------------------------- */

    const lightsChanged =
        data.led1 !== previousStatus.led1 ||
        data.led2 !== previousStatus.led2 ||
        data.led3 !== previousStatus.led3;

    if (lightsChanged) {

        const allOn =
            data.led1 &&
            data.led2 &&
            data.led3;

        const allOff =
            !data.led1 &&
            !data.led2 &&
            !data.led3;

        if (allOn) {

            addLog("LIGHTS → ALL ON");

        } else if (allOff) {

            addLog("LIGHTS → ALL OFF");

        } else {

            addLog("LIGHTS → CHANGED");
        }
    }


    /* ---------------------------------
       FAN
    --------------------------------- */

    if (data.fan !== previousStatus.fan) {

        addLog(
            `FAN → ${data.fan ? "ON" : "OFF"}`
        );
    }


    /* ---------------------------------
       BUZZER
    --------------------------------- */

    /*
       Buzzer can rapidly pulse during an alarm.
       Do not log every single ON/OFF pulse.
       Alarm events are logged separately below.
    */

    if (
        data.buzzer !== previousStatus.buzzer &&
        data.alarm_type === "NONE" &&
        previousStatus.alarm_type === "NONE"
    ) {

        addLog(
            `BUZZER → ${data.buzzer ? "ON" : "OFF"}`
        );
    }


    /* ---------------------------------
       ALARM
    --------------------------------- */

    const previousAlarm =
        previousStatus.alarm_type || null;

    const currentAlarm =
        data.alarm_type || null;


    if (currentAlarm !== previousAlarm) {

        if (currentAlarm === "FLAME") {

            addLog("ALARM → FIRE DETECTED");

        } else if (currentAlarm === "PIR") {

            addLog("ALARM → MOTION DETECTED");

        } else if (previousAlarm) {

            addLog("ALARM → CLEARED");
        }
    }


    /* ---------------------------------
       ALARM SILENCE
    --------------------------------- */

    if (
        data.alarm_silenced === true &&
        previousStatus.alarm_silenced !== true &&
        currentAlarm
    ) {

        addLog("ALARM → SILENCED");
    }


    /* ---------------------------------
       SAVE CURRENT STATE
    --------------------------------- */

    previousStatus = {
        mode: data.mode,
        led1: data.led1,
        led2: data.led2,
        led3: data.led3,
        fan: data.fan,
        buzzer: data.buzzer,
        alarm_type: data.alarm_type,
        alarm_silenced: data.alarm_silenced
    };
}

/* =========================================================
   SENSOR UPDATE
========================================================= */

function updateSensors(data) {

    setText(
        "temperature",
        data.temperature === null
            ? "--"
            : `${data.temperature.toFixed(1)}°C`
    );

    setText(
        "humidity",
        data.humidity === null
            ? "--"
            : `${data.humidity.toFixed(1)}%`
    );

    setText(
        "motion",
        data.motion
            ? "DETECTED"
            : "CLEAR"
    );

    setText(
        "light",
        data.dark
            ? "DARK"
            : "BRIGHT"
    );

    setText(
        "flame",
        data.flame
            ? "FIRE"
            : "SAFE"
    );
}
/* =========================================================
   SENSOR HEALTH UPDATE
========================================================= */

function updateSensorHealth(data) {

    setText(
        "pirHealth",
        data.pir_status || "--"
    );

    setText(
        "ldrHealth",
        data.ldr_status || "--"
    );

    setText(
        "flameHealth",
        data.flame_status || "--"
    );

    setText(
        "dhtHealth",
        data.dht_status || "--"
    );
}


/* =========================================================
   DEVICE UPDATE
========================================================= */

function updateDevices(data) {

    // LIGHT 1
    setToggle("led1", data.led1);
    setText("led1Status", data.led1 ? "ON" : "OFF");


    // LIGHT 2
    setToggle("led2", data.led2);
    setText("led2Status", data.led2 ? "ON" : "OFF");


    // LIGHT 3
    setToggle("led3", data.led3);
    setText("led3Status", data.led3 ? "ON" : "OFF");


    // FAN
    setToggle("fan", data.fan);
    setText("fanStatus", data.fan ? "ON" : "OFF");


    // BUZZER
    setToggle("buzzer", data.buzzer);
    setText("buzzerStatus", data.buzzer ? "ON" : "OFF");
}

/* =========================================================
   MODE UPDATE
========================================================= */

function updateMode(data) {

    const mode =
        data.mode.toUpperCase();

    document
        .querySelectorAll(".mode-button")
        .forEach(button => {

            button.classList.remove(
                "active"
            );

            if (
                button.dataset.mode === mode
            ) {

                button.classList.add(
                    "active"
                );
            }
        });

        setText(
            "currentMode",
            mode
        );
}


/* =========================================================
   ALARM UPDATE
========================================================= */

function updateAlarm(data) {

    const alarmBox =
        document.querySelector(".alarm-status");

    const alertBanner =
        document.querySelector(".alert-banner");

    const alarmSymbol =
        document.querySelector(".alarm-symbol");

    const alarmTitle =
        document.querySelector(".alarm-info strong");

    const alarmDescription =
        document.querySelector(".alarm-info span");

    const silenceButton =
        document.querySelector(".silence-button");

    if (!alarmBox)
        return;


    /* -----------------------------------------
       FIRE
    ----------------------------------------- */

    if (data.alarm_type === "FLAME") {

        alarmBox.classList.add("active");

        if (alertBanner)
            alertBanner.classList.remove("hidden");

        if (alarmSymbol)
            alarmSymbol.textContent = "🔥";

        if (alarmTitle)
            alarmTitle.textContent = "FIRE ALARM";

        if (alarmDescription)
            alarmDescription.textContent =
                data.alarm_silenced
                    ? "ALARM SILENCED"
                    : "FIRE DETECTED";

        if (silenceButton) {
            silenceButton.disabled = Boolean(data.alarm_silenced);
            silenceButton.textContent =
                data.alarm_silenced
                    ? "🔇 SILENCED"
                    : "🔇 SILENCE ALARM";
        }

        return;
    }


    /* -----------------------------------------
       PIR
    ----------------------------------------- */

    if (data.alarm_type === "PIR") {

        alarmBox.classList.add("active");

        if (alertBanner)
            alertBanner.classList.remove("hidden");

        if (alarmSymbol)
            alarmSymbol.textContent = "⚠";

        if (alarmTitle)
            alarmTitle.textContent = "SECURITY ALERT";

        if (alarmDescription)
            alarmDescription.textContent =
                data.alarm_silenced
                    ? "ALARM SILENCED"
                    : "MOTION DETECTED";

        if (silenceButton) {
            silenceButton.disabled = Boolean(data.alarm_silenced);
            silenceButton.textContent =
                data.alarm_silenced
                    ? "🔇 SILENCED"
                    : "🔇 SILENCE ALARM";
        }

        return;
    }


    /* -----------------------------------------
       NO ALARM
    ----------------------------------------- */

    alarmBox.classList.remove("active");

    if (alertBanner)
        alertBanner.classList.add("hidden");

    if (alarmSymbol)
        alarmSymbol.textContent = "✓";

    if (alarmTitle)
        alarmTitle.textContent = "SYSTEM SAFE";

    if (alarmDescription)
        alarmDescription.textContent =
            "No active alarms";

    if (silenceButton) {
        silenceButton.disabled = true;
        silenceButton.textContent = "🔇 SILENCE ALARM";
    }
}

/* =========================================================
   MODE CONTROL
========================================================= */

async function setMode(mode) {

    const result =
        await apiRequest(
            "/api/mode",
            "POST",
            {
                mode: mode
            }
        );

    if (result && result.success) {

        await updateStatus();
    }
}


/* =========================================================
   LIGHT CONTROL
========================================================= */

async function setLights() {

    if (!currentStatus)
        return;

    const data = {

        led1:
            !currentStatus.led1,

        led2:
            !currentStatus.led2,

        led3:
            !currentStatus.led3
    };

    await apiRequest(
        "/api/lights",
        "POST",
        data
    );

    await updateStatus();
}


/* =========================================================
   INDIVIDUAL LIGHT
========================================================= */

async function setLight(number) {

    if (!currentStatus)
        return;

    const key =
        `led${number}`;

    const data = {};

    data[key] =
        !currentStatus[key];

    await apiRequest(
        "/api/lights",
        "POST",
        data
    );

    await updateStatus();
}


/* =========================================================
   FAN
========================================================= */

async function setFan() {

    if (!currentStatus)
        return;

    await apiRequest(
        "/api/fan",
        "POST",
        {
            fan:
                !currentStatus.fan
        }
    );

    await updateStatus();
}


/* =========================================================
   BUZZER
========================================================= */

async function setBuzzer() {

    if (!currentStatus)
        return;

    await apiRequest(
        "/api/buzzer",
        "POST",
        {
            buzzer:
                !currentStatus.buzzer
        }
    );

    await updateStatus();
}


/* =========================================================
   SILENCE ALARM
========================================================= */

async function silenceAlarm() {

    await apiRequest(
        "/api/alarm/silence",
        "POST"
    );

    await updateStatus();
}


/* =========================================================
   UI HELPERS
========================================================= */

function setText(
    id,
    value
) {

    const element =
        document.getElementById(id);

    if (element)
        element.textContent = value;
}


function setToggle(
    id,
    state
) {

    const toggle =
        document.querySelector(
            `[data-device="${id}"]`
        );

    if (!toggle)
        return;

    toggle.classList.toggle(
        "active",
        Boolean(state)
    );
}


/* =========================================================
   BUTTON EVENTS
========================================================= */

document.addEventListener(
    "DOMContentLoaded",
    () => {

        /* ---------------------------------
           MODE BUTTONS
        --------------------------------- */

        document
            .querySelectorAll(
                ".mode-button"
            )
            .forEach(button => {

                button.addEventListener(
                    "click",
                    () => {

                        const mode =
                            button.dataset.mode;

                        if (mode)
                            setMode(mode);
                    }
                );
            });


        /* ---------------------------------
           DEVICE BUTTONS
        --------------------------------- */

        document
            .querySelectorAll(
                "[data-device]"
            )
            .forEach(button => {

                button.addEventListener(
                    "click",
                    () => {

                        const device =
                            button.dataset.device;

                        if (
                            device === "led1"
                        )
                            setLight(1);

                        else if (
                            device === "led2"
                        )
                            setLight(2);

                        else if (
                            device === "led3"
                        )
                            setLight(3);

                        else if (
                            device === "fan"
                        )
                            setFan();

                        else if (
                            device === "buzzer"
                        )
                            setBuzzer();
                    }
                );
            });


        /* ---------------------------------
           SILENCE
        --------------------------------- */

        const silence =
            document.querySelector(
                ".silence-button"
            );

        if (silence) {

            silence.addEventListener(
                "click",
                silenceAlarm
            );
        }


        /* ---------------------------------
           INITIAL STATUS
        --------------------------------- */

        updateStatus();


        /* ---------------------------------
           LIVE UPDATE
        --------------------------------- */

        setInterval(
            updateStatus,
            250
        );
    }
);
/* =========================================================
   PAGE NAVIGATION
========================================================= */

const pageButtons =
    document.querySelectorAll(".page-nav-button");

const dashboardPage =
    document.getElementById("dashboardPage");

const systemPage =
    document.getElementById("systemPage");
const logsPage =
    document.getElementById("logsPage");


pageButtons.forEach(button => {

    button.addEventListener("click", () => {

        const page =
            button.dataset.page;

        pageButtons.forEach(btn => {
            btn.classList.remove("active");
        });

        button.classList.add("active");


        if (page === "dashboard") {

            dashboardPage.classList.remove("hidden");
            systemPage.classList.add("hidden");
            logsPage.classList.add("hidden");
        
        }
        
        else if (page === "system") {
        
            dashboardPage.classList.add("hidden");
            systemPage.classList.remove("hidden");
            logsPage.classList.add("hidden");
        
        }
        
        else if (page === "logs") {
        
            dashboardPage.classList.add("hidden");
            systemPage.classList.add("hidden");
            logsPage.classList.remove("hidden");
        
        }

    });

});
/* =========================================================
   SYSTEM LOGS
========================================================= */

const logList =
    document.getElementById("logList");

const logEmpty =
    document.getElementById("logEmpty");

const clearLogs =
    document.getElementById("clearLogs");


    function addLog(message) {

        if (!logList)
            return;
    
        const currentEmpty =
            document.getElementById("logEmpty");
    
        if (currentEmpty) {
            currentEmpty.remove();
        }
    
        const entry =
            document.createElement("div");
    
        entry.className = "log-entry";
    
        const time =
            new Date().toLocaleTimeString(
                [],
                {
                    hour: "2-digit",
                    minute: "2-digit",
                    second: "2-digit"
                }
            );
    
        entry.innerHTML = `
            <span class="log-time">${time}</span>
            <span class="log-message">${message}</span>
        `;
    
        logList.prepend(entry);
    }

clearLogs.addEventListener("click", () => {

    logList.innerHTML = `
        <div class="log-empty" id="logEmpty">
            NO EVENTS RECORDED
        </div>
    `;

});
/* =========================================================
   THEME TOGGLE
========================================================= */

const themeToggle =
    document.getElementById("themeToggle");


function applyTheme(theme) {

    if (theme === "light") {

        document.body.classList.add("light-mode");

    } else {

        document.body.classList.remove("light-mode");
    }
}


function toggleTheme() {

    const isLight =
        document.body.classList.contains("light-mode");

    const newTheme =
        isLight ? "dark" : "light";

    applyTheme(newTheme);

    localStorage.setItem(
        "smartHomeTheme",
        newTheme
    );
}


if (themeToggle) {

    themeToggle.addEventListener(
        "click",
        toggleTheme
    );

    themeToggle.addEventListener(
        "keydown",
        (event) => {

            if (
                event.key === "Enter" ||
                event.key === " "
            ) {

                event.preventDefault();

                toggleTheme();
            }
        }
    );
}


/* -----------------------------------------
   LOAD SAVED THEME
----------------------------------------- */

const savedTheme =
    localStorage.getItem("smartHomeTheme");

applyTheme(
    savedTheme === "light"
        ? "light"
        : "dark"
);
