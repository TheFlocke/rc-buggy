export const version = "0.5";
/*
*            !!!IMPORTANT!!!
*   Only these values need to be changed.
*   The other are part of the whole program!
*/
// Define BLE Device Specs
// How many Characteristics are existing (numbers)
const bleCharNumCmd = 2;
const bleCharNumSensor = 6;
// also for Services
const bleServiceNum = 2;
// Services
const UUID_SERVICE_CMD = "5eaf1079-e806-47a9-a1ec-d815bea94805";
const UUID_SERVICE_SENSOR = "8da7a992-e263-4b78-abf2-bdb94808895c";
// Chars
const UUID_CHAR_CMD = "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3";
const UUID_CHAR_CMD_STATE = "bd6fbfde-385d-480f-b5eb-64d60cc7be9a";
const UUID_CHAR_SENSOR_TEMP = "24c53354-de00-42ac-926c-31f805e5d2f5";
const UUID_CHAR_SENSOR_PRESSURE = "545343fb-93a0-4415-9e2b-6a4c8e2835c4";
const UUID_CHAR_SENSOR_HUMIDITY = "618c8e95-e436-4a86-9d7d-17c3db9992d0";
const UUID_CHAR_SENSOR_IAQ = "aa4ce7cf-fff0-4d54-b4ec-fb24920b35c1";
const UUID_CHAR_SENSOR_CO2 = "3b9e4e45-42f2-4892-8d63-35f4a4bc8093";
const UUID_CHAR_SENSOR_VOC = "988b016d-91a0-4830-ae55-650ef2bb9c8d";
// arrays for UUID
// add additional UUIDS in the arrays
const UUIDS_SERVICE = [
    UUID_SERVICE_CMD,
    UUID_SERVICE_SENSOR
];
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
// add additional Services in this array
const bleCharNum = [
    bleCharNumCmd,
    bleCharNumSensor
];



// values that are used for HTML DOM
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
// For ble info at the Top
let bleStateContainer;
let bleServiceContainer;
let connectButton;
let disconnectButton;
let versionDisplay;
// also for BLE info (main page middle info)
let waitTime = 15; // Sent wait time in ms
let htmlBleServer;
// Services
let htmlBleServiceCmd;
let htmlBleServiceSensor;
// Characteristics
let htmlBleCharCmd;
let htmlBleCharCmdState;
let htmlBleCharSensorTemp;
let htmlBleCharSensorPressure;
let htmlBleCharSensorHumidity;
let htmlBleCharSensorIAQ;
let htmlBleCharSensorCO2;
let htmlBleCharSensorVOC;
// arrays
let htmlBleServices = [];
let htmlBleCharsCmd = [];
let htmlBleCharsSensor = [];
let htmlBleCharsList = [];



// Everything used for BLE
// BLE chars (defined by UUIDS and numbering up top)
let bleCharsCmd = [
];
let bleCharsSensor = [
];
const bleCharsList = [
    bleCharsCmd,
    bleCharsSensor
];

// for sending the last value to make BLE more burst type ==> efficent
// CMD
let sending = false;
let pendingValue = null;



window.onload = () => {
    registerServiceWorker();

    connectButton = document.getElementById('connectBleButton');
    disconnectButton = document.getElementById('disconnectBleButton');
    bleStateContainer = document.getElementById('bleState');
    bleServiceContainer = document.getElementById('bleService');
    htmlBleServices = document.getElementById('bleServiceList');
    // CMD
    htmlBleCharCmd = document.getElementById('bleCharCmd');
    htmlBleCharCmdState = document.getElementById('bleCharCmdState');
    // Sensor
    htmlBleCharSensorTemp = document.getElementById('bleCharSensorTemp');
    htmlBleCharSensorPressure = document.getElementById('bleCharSensorPressure');
    htmlBleCharSensorHumidity = document.getElementById('bleCharSensorHumidity');
    htmlBleCharSensorIAQ = document.getElementById('bleCharSensorIAQ');
    htmlBleCharSensorCO2 = document.getElementById('bleCharSensorCO2');
    htmlBleCharSensorVOC = document.getElementById('bleCharSensorVOC');
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
    // init arrays after vars are assigned so they are undefined
    htmlBleCharsCmd = [
        htmlBleCharCmd,
        htmlBleCharCmdState
    ];
    htmlBleCharsSensor = [
        htmlBleCharSensorTemp,
        htmlBleCharSensorPressure,
        htmlBleCharSensorHumidity,
        htmlBleCharSensorIAQ,
        htmlBleCharSensorCO2,
        htmlBleCharSensorVOC
    ];
    htmlBleCharsList = [
        htmlBleCharsCmd,
        htmlBleCharsSensor
    ];
    htmlBleServices = [
        htmlBleServiceCmd,
        htmlBleServiceSensor
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
    htmlBleCharCmd.classList.remove("info");
    htmlBleCharCmd.classList.add("error");
    htmlBleCharCmdState.classList.remove("info");
    htmlBleCharCmdState.classList.add("error");

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
        htmlBleServer = await device.gatt.connect();

        infoMessageContainer.innerHTML = "retrieve service list";
        let servicelist = await htmlBleServer.getPrimaryServices();
        let html = "";
        servicelist.forEach(service => html += "<li>" + (service.isPrimary ? "Primär" : "Zusatz") + ": " + service.uuid + "</li>")
        htmlBleServices.innerHTML = "<div>services: <ul>" + html + "</ul></div>";

        // Connect to Services
        // try to connect to CMD and Sensor Service (in parallel for more efficency)
        infoMessageContainer.innerHTML = "retrieving services...";

        const servicePromises = Array.from({length: bleServiceNum}, (_, id) => (async () => {
            htmlBleServices[id] = await htmlBleServer.getPrimaryService(UUIDS_SERVICE[id]);
            bleServiceContainer.innerHTML = "Connected to ";
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
            const charPromises = Array.from({length: bleCharNum[idx]}, async (_, id) => {
                try {
                    const characteristic = await htmlBleServices[idx].getCharacteristic(UUIDS_CHAR_LIST[idx][id]);
                    bleCharsList[idx][id] = characteristic;
                  //  htmlBleCharsList[idx][id].innerHTML = "char " + charNamesList[idx][id] + " OK:" + characteristic.uuid;
                    htmlBleCharsList[idx][id].classList.remove("error");
                    htmlBleCharsList[idx][id].classList.add("info");
                } catch (error) {
                  //  htmlBleCharsList[idx][id].innerHTML = "Error loading " + charNamesList[idx][id];
                    htmlBleCharsList[idx][id].classList.add("error");
                }
            });
            await Promise.all(charPromises);
        }

        infoMessageContainer.innerHTML = "finished retrieving characteristics...";


        // Listeners for BLE Notify
        // CMD
        // Drive_State
        bleCharsList[0][1].addEventListener('characteristicvaluechanged', handleCharChange);
        await bleCharsList[0][1].startNotifications();
        await bleCharsList[0][1].readValue();


        // Sensor
        const listenerPromises = Array.from({length: bleCharNum[1]}, async (_, id) => {
            try {
                bleCharsList[1][id].addEventListener("characteristicvaluechanged", handleCharChange);
                await bleCharsList[1][id].startNotifications();
                await bleCharsList[1][id].readValue();
            } catch (error) {
               // valuesList[1][id].innerHTML = "Error adding Listener " + charNamesList[1][id];
                bleCharsList[1][id].classList.add("error");
            }
        });
        await Promise.all(listenerPromises);


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
            htmlBleCharsList[idx][id].classList.remove("info");
            htmlBleCharsList[idx][id].classList.add("error");
          //  htmlBleCharsList[idx][id].innerHTML = "Kein " + charNamesList[idx][id];
        })());
        await Promise.all(charPromises); // Wait for all 8 characteristics to be retrieved for this service
    }

    htmlBleServices.innerHTML = "";
    document.getElementById('connection').innerHTML = "not connected";
}

function handleCharChange(event) {
    console.log(event.target.value)
    retrievedValue.innerHTML = new TextDecoder().decode(event.target.value);
    retrievedTimestamp.innerHTML = getDateTime();
}

export async function writeCmd(value) {
    if (sending) {
        // Overwrite with the latest value if already busy
        pendingValue = value;
        return "busy";
    }
    sending = true;
    let sent = "failed";
    if (htmlBleServer && htmlBleServer.connected) {
        const textEncoder = new TextEncoder();
        const uint8Array = textEncoder.encode(value);
        sentTimestamp.innerHTML = getDateTime();
        try {
            await bleCharsList[0][0].writeValue(uint8Array);
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
    sending = false;

    // Process pending value after completing the current send
    if (pendingValue !== null) {
        const nextValue = pendingValue;
        pendingValue = null; // Reset before recursion
        writeCmd(nextValue); // Send the queued value
    }

    return sent;
}


function disconnectDevice() {
    console.log("Disconnect Device.");
    if (htmlBleServer && htmlBleServer.connected) {
        if (bleCharCmd) {
            bleCharCmd.stopNotifications()
                .then(() => {
                    console.log("Notifications Stopped");
                    return htmlBleServer.disconnect();
                })
                .then(() => {
                    onDisconnected();
                })
                .catch(error => {
                    console.log("An error occurred:", error);
                });
        } else {
            console.log("Characteristic: Cmd not found for disconnect.");
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


