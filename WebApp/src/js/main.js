export const version = "0.5";

let connectButton;
let disconnectButton;
let versionDisplay;
// BLE
// Global variables to Handle Bluetooth
let bleServer;
let bleStateContainer;
let bleServiceContainer;
let bleServiceList;
// Services
let bleServiceCmd;
let bleServiceSensor;
let bleServices = [];
// Characteristics
let bleCharCmd;
let bleCharCmdState;
let bleCharSensorTemp;
let bleCharSensorPressure;
let bleCharSensorHumidity;
let bleCharSensorIAQ;
let bleCharSensorCO2;
let bleCharSensorVOC;
// arrays for Char
let bleCharsCmd = [];
let bleCharsSensor = [];
let bleCharsList = [];
// BLE History
// Sent
let bleSendHistory;
// Received
let bleReceiveHistory;
// Pages
let page0;
let page1;
let page2;

// For Info box
let retrievedValue;
let latestValueSent;
let sentTimestamp;
let retrievedTimestamp;

let errorMessageContainer;
let infoMessageContainer;

// Sent wait time in ms
let waitTime = 15;


// Define BLE Device Specs
// Services
let UUID_SERVICE_CMD = "5eaf1079-e806-47a9-a1ec-d815bea94805";
let UUID_SERVICE_SENSOR = "8da7a992-e263-4b78-abf2-bdb94808895c";
const UUIDS_SERVICE = [
    UUID_SERVICE_CMD,
    UUID_SERVICE_SENSOR
];
// Chars
let UUID_CHAR_CMD = "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3";
let UUID_CHAR_CMD_STATE = "bd6fbfde-385d-480f-b5eb-64d60cc7be9a";
let UUID_CHAR_SENSOR_TEMP = "24c53354-de00-42ac-926c-31f805e5d2f5";
let UUID_CHAR_SENSOR_PRESSURE = "545343fb-93a0-4415-9e2b-6a4c8e2835c4";
let UUID_CHAR_SENSOR_HUMIDITY = "618c8e95-e436-4a86-9d7d-17c3db9992d0";
let UUID_CHAR_SENSOR_IAQ = "aa4ce7cf-fff0-4d54-b4ec-fb24920b35c1";
let UUID_CHAR_SENSOR_CO2 = "3b9e4e45-42f2-4892-8d63-35f4a4bc8093";
let UUID_CHAR_SENSOR_VOC = "988b016d-91a0-4830-ae55-650ef2bb9c8d";
const UUIDS_CHAR_CMD = [
    UUID_CHAR_CMD,
    UUID_CHAR_CMD_STATE
];
const UUIDS_CHAR_SENSOR = [
    UUID_CHAR_SENSOR_TEMP,
    UUID_CHAR_SENSOR_PRESSURE,
    UUID_CHAR_SENSOR_HUMIDITY,
    UUID_CHAR_SENSOR_IAQ,
    UUID_CHAR_SENSOR_CO2,
    UUID_CHAR_SENSOR_VOC
];
const UUIDS_CHAR_LIST = [
    UUIDS_CHAR_CMD,
    UUIDS_CHAR_SENSOR
];
// How many Characteristics are existing (numbers)
const bleCharNumCmd = 2;
const bleCharNumSensor = 6;
const bleCharNum = [
    bleCharNumCmd,
    bleCharNumSensor
];
// also for Services
const bleServiceNum = 2;


// Values of each char
// CMD
let valCmd;
let valCmdState;
// Sensor
let valSensorTemp;
let valSensorPressure;
let valSensorHumidity;
let valSensorIAQ;
let valSensorCO2;
let valSensorVOC;
let valuesCmd = [
    valCmd,
    valCmdState
];
let valuesSensor = [
    valSensorTemp,
    valSensorPressure,
    valSensorHumidity,
    valSensorIAQ,
    valSensorCO2,
    valSensorVOC
];
const valuesList = [
    valuesCmd,
    valuesSensor
];
// Names for Logging (to make code more efficient)
const serviceNames = [
    String("bleServiceCmd"),
    String("bleServiceSensor")
];
const charNamesCmd = [
    String("bleCharCmd"),
    String("bleCharCmdState")
];
const charNamesSensor = [
    String("bleCharSensorTemp"),
    String("bleCharSensorPressure"),
    String("bleCharSensorHumidity"),
    String("bleCharSensorIAQ"),
    String("bleCharSensorCO2"),
    String("bleCharSensorVOC")
];
const charNamesList = [
    charNamesCmd,
    charNamesSensor,
]

// for sending the last value
// CMD
let sendingDrive = false;
let pendingDriveValue = null;
// Arm
let sendingArm = false;
let pendingArmValue = null;


window.onload = () => {
    registerServiceWorker();

    connectButton = document.getElementById('connectBleButton');
    disconnectButton = document.getElementById('disconnectBleButton');
    bleStateContainer = document.getElementById('bleState');
    bleServiceContainer = document.getElementById('bleService');
    bleServiceList = document.getElementById('bleServiceList');
    // CMD
    bleCharCmd = document.getElementById('bleCharCmd');
    bleCharCmdState = document.getElementById('bleCharCmdState');
    // Sensor
    bleCharSensorTemp = document.getElementById('bleCharSensorTemp');
    bleCharSensorPressure = document.getElementById('bleCharSensorPressure');
    bleCharSensorHumidity = document.getElementById('bleCharSensorHumidity');
    bleCharSensorIAQ = document.getElementById('bleCharSensorIAQ');
    bleCharSensorCO2 = document.getElementById('bleCharSensorCO2');
    bleCharSensorVOC = document.getElementById('bleCharSensorVOC');
    // History (on main page)
    // Values
    // Sent
    bleSendHistory = document.getElementById('sendHistory');
    // Received
    bleReceiveHistory = document.getElementById('receiveHistory');
    // Info boxes (Top left)
    // Sent
    latestValueSent = document.getElementById('valueSent');
    sentTimestamp = document.getElementById('sent_timestamp');
    // retrieved
    retrievedValue = document.getElementById('retrievedValue');
    retrievedTimestamp = document.getElementById('retrieved_timestamp');

    errorMessageContainer = document.getElementById('errors');
    infoMessageContainer = document.getElementById('info');
    versionDisplay = document.getElementById('version');
    // init arrays after vars are assigned so they are undifined
    bleCharsCmd = [
        bleCharCmd,
        bleCharCmdState
    ];
    bleCharsSensor = [
        bleCharSensorTemp,
        bleCharSensorPressure,
        bleCharSensorHumidity,
        bleCharSensorIAQ,
        bleCharSensorCO2,
        bleCharSensorVOC
    ];
    bleCharsList = [
        bleCharsCmd,
        bleCharsSensor
    ];
    bleServices = [
        bleServiceCmd,
        bleServiceSensor
    ];

    // Pages
    page0 = document.getElementById("swPage0");
    page1 = document.getElementById("swPage1");
    page2 = document.getElementById("swPage2");

    document.getElementById('version').innerHTML = version;
    onDisconnected();


    swPage0.addEventListener('click', () => togglePage0());
    swPage1.addEventListener('click', () => togglePage1());
    swPage2.addEventListener('click', () => togglePage2());

    if (!navigator.bluetooth) {
        errorMessageContainer.innerHTML = "Web Bluetooth API is for this Browser not available!";
    }

    // Connect Button (search for BLE Devices only if BLE is available)
    connectButton.addEventListener('click', () => {
        if (navigator.bluetooth) {
            connectToDevice();
        }
    });

    // Disconnect Button
    disconnectButton.addEventListener('click', disconnectDevice);
}

function togglePage0() {
    document.getElementById('armController').style.visibility = 'hidden';
    document.getElementById('sliderContainer').style.display = 'flex';
    document.getElementById('speedController').style.visibility = 'visible';
    document.getElementById('settings').style.visibility = 'hidden';
}

function togglePage1() {
    document.getElementById('sliderContainer').style.display = 'none';
    document.getElementById('armController').style.visibility = 'visible';
    document.getElementById('speedController').style.visibility = 'hidden';
    document.getElementById('settings').style.visibility = 'hidden';
}

function togglePage2() {
    document.getElementById('sliderContainer').style.display = 'none';
    document.getElementById('speedController').style.visibility = 'hidden';
    document.getElementById('armController').style.visibility = 'hidden';
    document.getElementById('settings').style.visibility = 'visible';
    document.getElementById('settings').style.display = 'block';
}

async function registerServiceWorker() {
    try {
        if ('serviceWorker' in navigator) {
            let registration = await navigator.serviceWorker.register('./src/js/sw.js');
            registration.onupdatefound = () => {
                const installingWorker = registration.installing;
                installingWorker.onstatechange = () => {
                    switch (installingWorker.state) {
                        case 'installed':
                            if (navigator.serviceWorker.controller) {
                                // new update available
                                document.getElementById('update').innerHTML = "UPDATE";
                            } else {
                                // no update available
                            }
                            break;
                    }
                };
            }
        }
    } catch (error) {
        console.error("Error creating service worker: ", error);
        errorMessageContainer.innerHTML = "Error creating service worker: " + error;
    }
}

async function connectToDevice() {
    bleServiceContainer.classList.remove("info");
    bleServiceContainer.classList.add("error");
    bleStateContainer.classList.remove("info");
    bleStateContainer.classList.add("error");
    // CMD
    // Drive
    bleCharCmd.classList.remove("info");
    bleCharCmd.classList.add("error");
    bleCharCmdState.classList.remove("info");
    bleCharCmdState.classList.add("error");

    errorMessageContainer.innerHTML = "";

    try {
        infoMessageContainer.innerHTML = "select device";
        const device = await navigator.bluetooth.requestDevice({
            filters: [
                {services: [UUID_SERVICE_CMD]}
            ],
            optionalServices: [UUID_SERVICE_SENSOR]
        });

        device.addEventListener('gattservicedisconnected', onDisconnected);

        infoMessageContainer.innerHTML = "connecting GATT server";
        bleServer = await device.gatt.connect();

        infoMessageContainer.innerHTML = "retrieve service list";
        let servicelist = await bleServer.getPrimaryServices();
        let html = "";
        servicelist.forEach(service => html += "<li>" + (service.isPrimary ? "Primär" : "Zusatz") + ": " + service.uuid + "</li>")
        bleServiceList.innerHTML = "<div>services: <ul>" + html + "</ul></div>";

        // Connect to Services
        // try to connect to CMD and Sensor Service (in parallel for more efficency)
        infoMessageContainer.innerHTML = "retrieving services...";
        const servicePromises = Array.from({length: bleServiceNum}, (_, id) => (async () => {
            bleServices[id] = bleServer.getPrimaryService(UUIDS_SERVICE[id]);
            bleServiceContainer.innerHTML = "Connected to " + serviceNames[id];
        })());
        await Promise.all(servicePromises);
        infoMessageContainer.innerHTML = "All services retrieved and Connected successfully.";
        bleServiceContainer.classList.remove("error");
        bleServiceContainer.classList.add("info");

        // Load Characteristics
        // First Load Services sequentially and inside the service load the Characteristics parallel
        // Drive
        infoMessageContainer.innerHTML = "retrieving CMD characteristics...";
        for (let idx = 0; idx < 2; idx++) {
            const charPromises = Array.from({length: bleCharNum[idx]}, (_, id) => (async () => {
                    valuesList[idx][id] = await bleServices[idx].getCharacteristic(UUIDS_CHAR_LIST[idx][id]);
                    bleCharsList[idx][id].innerHTML = "char " + charNamesList[idx][id] + " OK:" + valuesList[idx][id];
                    bleCharsList[idx][id].classList.remove("error");
                    bleCharsList[idx][id].classList.add("info");
            })());
            await Promise.all(charPromises); // Wait for all 8 characteristics to be retrieved for this service
        }
        infoMessageContainer.innerHTML = "finished retrieving characteristics...";


        // Listeners for BLE Notify
        // CMD
        // Drive_State
        valCmdState.addEventListener('characteristicvaluechanged', handleCmdCharChange);
        await valCmdState.startNotifications();
        valCmdState.readValue();


        // Sensor
        const valPromises = Array.from({length: bleCharNum[1]}, (_, id) => (async () => {
            valuesList[1][id].addEventListener("characteristicvaluechanged", handleSensorCharChange);
            await valuesList[1][id].startNotifications();
            valuesList[1][id].readValue();
        })());
        await Promise.all(valPromises)


        infoMessageContainer.innerHTML = "successfully connected";
        document.getElementById('connection').innerHTML = 'Connected with ' + device.name;

        bleStateContainer.innerHTML = 'Connected with ' + device.name;
        bleStateContainer.classList.remove("error");
        bleStateContainer.classList.add("info");

        disconnectButton.style.display = "block";
        connectButton.style.display = "none";

        document.getElementById('status').style.display = "block";


    } catch (error) {
        errorMessageContainer.innerHTML = error;
    }
}

async function onDisconnected() {
    disconnectButton.style.display = "none";
    connectButton.style.display = "block";

    bleServiceContainer.classList.remove("info");
    bleServiceContainer.classList.add("error");
    bleServiceContainer.innerHTML = "No Services";

    bleStateContainer.classList.remove("info");
    bleStateContainer.classList.add("error");
    bleStateContainer.innerHTML = "Not Connected";

    // Char
    for (let idx = 0; idx < 2; idx++) {
        const charPromises = Array.from({length: bleCharNum[idx]}, (_, id) => (async () => {
            bleCharsList[idx][id].classList.remove("info");
            bleCharsList[idx][id].classList.add("error");
            bleCharsList[idx][id].innerHTML = "Kein " + charNamesList[idx][id];
        })());
        await Promise.all(charPromises); // Wait for all 8 characteristics to be retrieved for this service
    }

    bleServiceList.innerHTML = "";
    document.getElementById('connection').innerHTML = "not connected";
}

function handleCmdCharChange(event) {
    console.log(event.target.value)
    const newValueReceived = new TextDecoder().decode(event.target.value);
    retrievedValue.innerHTML = newValueReceived;
    retrievedTimestamp.innerHTML = getDateTime();

    const div = document.createElement('div');
    const header = document.createElement('h1');
    const text = document.createElement('p');
    header.innerHTML = 'RECEIVED CMD STATE';
    text.innerHTML = newValueReceived;
    div.appendChild(header)
    div.appendChild(text)
    div.classList.add('entry')
    bleReceiveHistory.prepend(div)
    if (bleReceiveHistory.children.length > 1) {
        bleReceiveHistory.removeChild(bleReceiveHistory.lastElementChild);
    }
}

function handleSensorCharChange(event) {
    console.log(event.target.value)
    retrievedValue.innerHTML = new TextDecoder().decode(event.target.value);
    retrievedTimestamp.innerHTML = getDateTime();
}

let sending = false;

export async function writeCmdDrive(value) {
    if (sendingDrive) {
        // Overwrite with the latest value if already busy
        pendingDriveValue = value;
        return "busy";
    }
    sendingDrive = true;
    let sent = "failed";
    if (bleServer && bleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await valCmd.writeValue(uint8Array);
            latestValueSent.innerHTML = value;
            const div = document.createElement('div');
            const header = document.createElement('h1');
            const text = document.createElement('p');
            header.innerHTML = 'CMD Drive';
            text.innerHTML = value;
            div.appendChild(header)
            div.appendChild(text)
            div.classList.add('entry')
            bleSendHistory.prepend(div)
            if (bleSendHistory.children.length > 1) {
                bleSendHistory.removeChild(bleSendHistory.lastElementChild);
            }
            sent = "ok";
        } catch (error) {
            console.error("Error writing to the CMD characteristic: ", error);
        }
    } else {
        console.error("Bluetooth is not connected. Cannot write to characteristic.")
        onDisconnected();
        sent = "disconnected"
    }
    await new Promise((resolve) => setTimeout(resolve, waitTime));
    sendingDrive = false;

    // Process pending value after completing the current send
    if (pendingDriveValue !== null) {
        const nextValue = pendingDriveValue;
        pendingDriveValue = null; // Reset before recursion
        writeCmdDrive(nextValue); // Send the queued value
    }

    return sent;
}


export async function writeCmdArm(value) {
    if (sendingArm) {
        // Overwrite with the latest value requested
        pendingArmValue = value;
        return "busy";
    }
    sending = true;
    let sent = "failed";
    if (bleServer && bleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await charCmdArm.writeValue(uint8Array);
            latestValueSent.innerHTML = value;
            const div = document.createElement('div');
            const header = document.createElement('h1');
            const text = document.createElement('p');
            header.innerHTML = 'CMD ARM';
            text.innerHTML = value;
            div.appendChild(header)
            div.appendChild(text)
            div.classList.add('entry')
            bleSendHistory.prepend(div)
            if (bleSendHistory.children.length > 1) {
                bleSendHistory.removeChild(bleSendHistory.lastElementChild);
            }
            sent = "ok";
        } catch (error) {
            console.error("Error writing to ARM characteristic: ", error);
        }
    } else {
        console.error("Bluetooth is not connected. Cannot write to characteristic.")
        onDisconnected();
        sent = "disconnected"
    }
    await new Promise((resolve) => setTimeout(resolve, waitTime));
    sending = false;

    // After sending, check if a new value was queued during a busy state
    if (pendingArmValue !== null) {
        const nextValue = pendingArmValue;
        pendingArmValue = null; // Clear before sending to avoid loops
        // Recursively calls to send the latest value
        writeCmdArm(nextValue);
    }

    return sent;
}


// arm function must be added
function disconnectDevice() {
    console.log("Disconnect Device.");
    if (bleServer && bleServer.connected) {
        if (valCmd) {
            valCmd.stopNotifications()
                .then(() => {
                    console.log("Notifications Stopped");
                    return bleServer.disconnect();
                })
                .then(() => {
                    onDisconnected();
                })
                .catch(error => {
                    console.log("An error occurred:", error);
                });
        } else {
            console.log("Characteristic: Cmd Drive not found for disconnect.");
        }
    } else {
        // Throw an error if Bluetooth is not connected
        console.error("Bluetooth is not connected.");
        window.alert("Bluetooth is not connected.")
    }
}

function getDateTime() {
    let currentdate = new Date();
    let day = ("00" + currentdate.getDate()).slice(-2); // Convert day to string and slice
    let month = ("00" + (currentdate.getMonth() + 1)).slice(-2);
    let year = currentdate.getFullYear();
    let hours = ("00" + currentdate.getHours()).slice(-2);
    let minutes = ("00" + currentdate.getMinutes()).slice(-2);
    let seconds = ("00" + currentdate.getSeconds()).slice(-2);

    return day + "." + month + "." + year + ", " + hours + ":" + minutes + ":" + seconds;
}


