export const version = "0.5";
import {setError, setInfo} from './debug.js'

/*
*            !!!IMPORTANT!!!
*   Only these values need to be changed.
*   The other are part of the whole program!
*/
// Define BLE Device Specs
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
export let errorMessageContainer;
export let infoMessageContainer;
// For ble info at the Top
let htmlBleStateContainer;
let htmlBleServiceContainer;
let htmlBleCharContainer;
let connectButton;
let disconnectButton;
let versionDisplay;




// Everything used for BLE
// BLE chars (defined by UUIDS and numbering up top)
let waitTime = 15; // Sent wait time in ms
let bleServer;
let bleCharsCmd = [
];
let bleCharsSensor = [
];
let bleServices = [
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
    // for offline Cache
    registerServiceWorker();

    connectButton = document.getElementById('connectBleButton');
    disconnectButton = document.getElementById('disconnectBleButton');
    htmlBleStateContainer = document.getElementById('htmlBleState');
    htmlBleServiceContainer = document.getElementById('htmlBleService');
    htmlBleCharContainer =document.getElementById('htmlBleChar');
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
        setError("Web Bluetooth API is for this Browser not available!");
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
        setError("Error creating service worker: " + error);
    }
}

async function connectToDevice() {
    htmlBleServiceContainer.classList.remove("info");
    htmlBleServiceContainer.classList.add("error");
    htmlBleStateContainer.classList.remove("info");
    htmlBleStateContainer.classList.add("error");
    htmlBleCharContainer.classList.remove("info");
    htmlBleCharContainer.classList.add("error");

    try {
        setInfo("selecting device...");
        const device = await navigator.bluetooth.requestDevice({
            filters: [
                {services: [UUID_SERVICE_CMD]}
            ],
            optionalServices: [UUID_SERVICE_SENSOR]
        });

        device.addEventListener('gattservicedisconnected', onDisconnected);

        setInfo("Connecting to BLE server...");
        bleServer = await device.gatt.connect();

        setInfo("Retrieve service list...");
        let servicelist = await bleServer.getPrimaryServices();
        let html = "";
        servicelist.forEach(service => html += "<li>" + (service.isPrimary ? "Primär" : "Zusatz") + ": " + service.uuid + "</li>")
        htmlBleServiceContainer.innerHTML = "<div>services: <ul>" + html + "</ul></div>";

        // Connect to Services
        // try to connect to CMD and Sensor Service (in parallel for more efficency)
        setInfo("connecting to services...");

        const servicePromises = Array.from({length: UUIDS_SERVICE.length}, (_, id) => (async () => {
            bleServices[id] = await bleServer.getPrimaryService(UUIDS_SERVICE[id]);
        })());
        await Promise.all(servicePromises);

        setInfo("All services retrieved and Connected successfully.");
        htmlBleServiceContainer.innerHTML = `Connected to ${bleServices.length} Services.`   ;
        htmlBleServiceContainer.classList.remove("error");
        htmlBleServiceContainer.classList.add("info");

        // Load Characteristics
        // First Load Services sequentially and inside the service load the Characteristics parallel
        // Drive
        setInfo("Retrieving CMD characteristics...");
        for (let idx = 0; idx < 2; idx++) {
            const charPromises = Array.from({length: UUIDS_CHAR_LIST[idx].length}, async (_, id) => {
                try {
                    bleCharsList[idx][id] = await bleServices[idx].getCharacteristic(UUIDS_CHAR_LIST[idx][id]);
                } catch (error) {
                    let errorMsg = `Error while loading ${String(id)} out of ${UUIDS_CHAR_LIST[idx].length} characteristics of Service ${String(idx)}`;
                    setError(errorMsg);
                    htmlBleCharContainer.innerHTML = errorMsg;
                    htmlBleCharContainer.classList.add("error");
                }
            });
            await Promise.all(charPromises);
            htmlBleCharContainer.innerHTML = `${bleCharsList[idx].length} chars loaded of ${bleServices.length} Services` ;
            htmlBleCharContainer.classList.remove("error");
            htmlBleCharContainer.classList.add("info");
        }

        setInfo("Finished retrieving characteristics.");

        setInfo("Adding listeners to BLE RX");
        // Listeners for BLE Notify
        // CMD
        // Drive_State
        bleCharsList[0][1].addEventListener('characteristicvaluechanged', handleCharChange);
        await bleCharsList[0][1].startNotifications();
        await bleCharsList[0][1].readValue();


        // Sensor
        const listenerPromises = Array.from({length: UUIDS_CHAR_SENSOR.length}, async (_, id) => {
            try {
                bleCharsList[1][id].addEventListener("characteristicvaluechanged", handleCharChange);
                await bleCharsList[1][id].startNotifications();
                await bleCharsList[1][id].readValue();
            } catch (error) {
                setError(`Error adding Listener for  Characteristic: ${id} for Service: 1`);
            }
        });
        await Promise.all(listenerPromises);


        setInfo("successfully connected");
        document.getElementById('connection').innerHTML = 'Connected with ' + device.name;

        htmlBleStateContainer.innerHTML = 'Connected with ' + device.name;
        htmlBleStateContainer.classList.remove("error");
        htmlBleStateContainer.classList.add("info");

        disconnectButton.style.display = "block";
        connectButton.style.display = "none";

        document.getElementById('status').style.display = "block";


    } catch (error) {
        setError(error);
    }
}

async function onDisconnected() {
   setInfo("BLE Client disconnected.");
    disconnectButton.style.display = "none";
    connectButton.style.display = "block";

    htmlBleServiceContainer.classList.remove("info");
    htmlBleServiceContainer.classList.add("error");
    htmlBleServiceContainer.innerHTML = "No Services";

    htmlBleStateContainer.classList.remove("info");
    htmlBleStateContainer.classList.add("error");
    htmlBleStateContainer.innerHTML = "Not Connected";

    htmlBleCharContainer.classList.remove("info");
    htmlBleCharContainer.classList.add("error");
    htmlBleCharContainer.innerHTML = "Not Characteristics";

    bleServices.innerHTML = "";
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
    if (bleServer && bleServer.connected) {
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
            setError("Error writing to the CMD characteristic: ", error);
        }
    } else {
        setError("Bluetooth is not connected. Cannot write to characteristic.")
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
    if (bleServer && bleServer.connected) {
        if (bleCharCmd) {
            bleCharCmd.stopNotifications()
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
            console.log("Characteristic: Cmd not found for disconnect.");
        }
    } else {
        // Throw an error if Bluetooth is not connected
        setError("Bluetooth is not connected.");
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


