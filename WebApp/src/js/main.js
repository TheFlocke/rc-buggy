export const version="0.0.4412";

let connectButton;
let disconnectButton;
let buttonLeft;
let buttonRight;
let buttonUp;
let buttonStop;
let buttonDown;
let versionDisplay;
let bleStateContainer;
let bleCharState;
let bleCharCmd;
let bleSensorState;
let bleArm;
let bleArmState;
let bleServiceContainer;
let bleServiceList;
let bleSendHistory;
let bleReceiveHistory;
let orientation;
let orientationDevice;
let switch1;
let switch2;

let retrievedValue;
let latestValueSent;
let sentTimestamp;
let retrievedTimestamp;

let errorMessageContainer;
let infoMessageContainer;

//Define BLE Device Specs
let SERVICE_UUID         = "5eaf1079-e806-47a9-a1ec-d815bea94805";
let CHARACTERISTIC_CMD   = "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3";
let CHARACTERISTIC_STATE_CMD = "bd6fbfde-385d-480f-b5eb-64d60cc7be9a";
let CHARACTERISTIC_SENSOR       =  "4a95c0ef-4ee8-420d-8c35-c643678f7b77"
let CHARACTERISTIC_ARM          = "99d69805-8efb-450e-ae78-c4ddba09f7f6"
let CHARACTERISTIC_STATE_ARM    =  "f8765d0c-81b5-4780-85a4-44f0999f5474"


//Global variables to Handle Bluetooth
let bleServer;
let bleService;
let cmdCharacteristic;
let cmdStateCharacteristic;
let sensorCharacteristic;
let armCharacteristic;
let armStateCharacteristic;

// orientation information
let orientationFrame;
let orientationCenter;

window.onload = () => {
    registerServiceWorker();

    connectButton = document.getElementById('connectBleButton');
    disconnectButton = document.getElementById('disconnectBleButton');
    buttonLeft = document.getElementById('buttonLeft');
    buttonRight = document.getElementById('buttonRight');
    buttonUp = document.getElementById('buttonSpeedUp');
    buttonStop = document.getElementById('buttonSpeedStop');
    buttonDown = document.getElementById('buttonSpeedDown');
    retrievedValue = document.getElementById('retrievedValue');
    latestValueSent = document.getElementById('valueSent');
    bleStateContainer = document.getElementById('bleState');
	bleServiceContainer = document.getElementById('bleService');
	bleCharCmd = document.getElementById('bleCharCmd');
	bleCharState = document.getElementById('bleCharState');
	bleSensorState = document.getElementById('bleSensorState');
	bleArm = document.getElementById('bleArm');
	bleArmState = document.getElementById('bleArmState')
	bleServiceList = document.getElementById('bleServiceList');
	bleSendHistory = document.getElementById('sendHistory');
	bleReceiveHistory = document.getElementById('receiveHistory')
    sentTimestamp = document.getElementById('sent_timestamp');
    retrievedTimestamp = document.getElementById('retrieved_timestamp');
	errorMessageContainer = document.getElementById('errors');
	infoMessageContainer = document.getElementById('info');
	versionDisplay = document.getElementById('version');
	orientation = document.getElementById('orientation_info');
	orientationDevice = document.getElementById('orientation_info_device');
	orientationFrame = document.getElementById('orientation_frame');
	orientationCenter = document.getElementById('orientation_center');
    switch1 = document.getElementById("swLED1");
    switch2 = document.getElementById("swLED2");

    document.getElementById('version').innerHTML=version;
	onDisconnected();


	switch1.addEventListener('click', () => toggleLED1());
	switch2.addEventListener('click', () => toggleLED2());

	if (!navigator.bluetooth) {
		errorMessageContainer.innerHTML = "Web Bluetooth API ist für diesen Browser nicht verfügbar!";
	}

	// Connect Button (search for BLE Devices only if BLE is available)
	connectButton.addEventListener('click', () => {
		if (navigator.bluetooth){
			connectToDevice();
		}
	});

	// Disconnect Button
	disconnectButton.addEventListener('click', disconnectDevice);
}

function toggleLED1() {
	document.getElementById('armController').style.visibility = 'hidden';
	document.getElementById('speedController').style.display = 'flex';
}

function toggleLED2() {
    document.getElementById('armController').style.visibility = 'visible';
	document.getElementById('speedController').style.display = 'none';
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
                                    document.getElementById('update').innerHTML="UPDATE";
								} else {
									// no update available
								}
								break;
						}
					};
            }   
		}
	} catch(error) {
			console.error("Error creating service worker: ", error);
			errorMessageContainer.innerHTML="Error creating service worker: " + error;
	}
}

async function connectToDevice(){
	bleServiceContainer.classList.remove("info");
	bleServiceContainer.classList.add("error");
	bleStateContainer.classList.remove("info");
	bleStateContainer.classList.add("error");
	bleCharCmd.classList.remove("info");
	bleCharCmd.classList.add("error");
	bleCharState.classList.remove("info");
	bleCharState.classList.add("error");

	errorMessageContainer.innerHTML = "";
	
	try {
		infoMessageContainer.innerHTML = "select device";
		const device = await navigator.bluetooth.requestDevice({
			filters: [
						{ services: [SERVICE_UUID] },
						{ namePrefix: "CRBK" }
					 ]
		});
			
		device.addEventListener('gattservicedisconnected', onDisconnected);
		
		infoMessageContainer.innerHTML = "connecting GATT server";
		bleServer = await device.gatt.connect();
		
		infoMessageContainer.innerHTML = "retrieve service list";
		let servicelist = await bleServer.getPrimaryServices();
		let html="";
		servicelist.forEach(service=>html+="<li>"+(service.isPrimary ? "Primär" : "Zusatz") +": " + service.uuid+"</li>")
		bleServiceList.innerHTML="<div>services: <ul>"+html+"</ul></div>";
		
		infoMessageContainer.innerHTML = "retrieve service "+SERVICE_UUID;
		bleService = await bleServer.getPrimaryService(SERVICE_UUID);
		
		bleServiceContainer.innerHTML = 'Verbunden mit Service ' + bleService.uuid;
		bleServiceContainer.classList.remove("error");
		bleServiceContainer.classList.add("info");
		
		infoMessageContainer.innerHTML = "retrieve char CMD "+CHARACTERISTIC_CMD;
		cmdCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_CMD);
	
		bleCharCmd.innerHTML = "CMD Characteristik OK: "+cmdCharacteristic.uuid;
		bleCharCmd.classList.remove("error");
		bleCharCmd.classList.add("info");
		
		infoMessageContainer.innerHTML = "retrieve char STATE "+CHARACTERISTIC_STATE_CMD;
		cmdStateCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_STATE_CMD);
		
		bleCharState.innerHTML = "State Characteristik OK: "+cmdStateCharacteristic.uuid;
		bleCharState.classList.remove("error");
		bleCharState.classList.add("info");

		bleSensorState.innerHTML = "retrieve chat SENSOR "+CHARACTERISTIC_SENSOR;
		sensorCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_SENSOR);

		bleSensorState.innerHTML = "Sensor Characteristik OK: "+ sensorCharacteristic.uuid;
		bleSensorState.classList.remove('error');
		bleSensorState.classList.add('info');

		bleArm.innerHTML = "retrieve char ARM "+CHARACTERISTIC_ARM;
		armCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_ARM);

		bleArm.innerHTML = "Arm Characteriskik OK: "+armCharacteristic.uuid;
		bleArm.classList.remove('error');
		bleArm.classList.add('info');


		bleArmState.innerHTML = "retrieve char ARM_STATE "+CHARACTERISTIC_STATE_ARM
		armStateCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_STATE_ARM);

		bleArmState.innerHTML = "Arm_State Characteristik OK: "+armStateCharacteristic.uuid;
		bleArmState.classList.remove("error");
		bleArmState.classList.add("info");


		cmdStateCharacteristic.addEventListener('characteristicvaluechanged', handleCharacteristicChange);
		await cmdStateCharacteristic.startNotifications();
		cmdStateCharacteristic.readValue();

		armStateCharacteristic.addEventListener('characteristicvaluechanged', handleCharacteristicChange)
		await armStateCharacteristic.startNotifications();
		armStateCharacteristic.readValue();

		sensorCharacteristic.addEventListener('characteristicvaluechanged', handleCharacteristicChange)
		await sensorCharacteristic.startNotifications();
		sensorCharacteristic.readValue();
		
		infoMessageContainer.innerHTML = "erfolgreich verbunden";
        document.getElementById('connection').innerHTML='Verbunden mit ' + device.name;

		bleStateContainer.innerHTML = 'Verbunden mit ' + device.name;
		bleStateContainer.classList.remove("error");
		bleStateContainer.classList.add("info");
		
		disconnectButton.style.display = "block";
		connectButton.style.display = "none";

		document.getElementById('status').style.display = "block";



	} catch(error) {
		errorMessageContainer.innerHTML = error;
	}
}

function onDisconnected(event){
	disconnectButton.style.display = "none";
	connectButton.style.display = "block";
	
	bleServiceContainer.classList.remove("info");
	bleServiceContainer.classList.add("error");
	bleServiceContainer.innerHTML = "Kein Service";
	
	bleStateContainer.classList.remove("info");
	bleStateContainer.classList.add("error");
	bleStateContainer.innerHTML = "Nicht verbunden";
	
	bleCharCmd.classList.remove("info");
	bleCharCmd.classList.add("error");
	bleCharCmd.innerHTML = "Kein CMD";
	
	bleCharState.classList.remove("info");
	bleCharState.classList.add("error");
	bleCharState.innerHTML = "Kein State";
	bleServiceList.innerHTML="";
    document.getElementById('connection').innerHTML="nicht verbunden";
}

function handleCharacteristicChange(event){
	console.log(event.target.value)
	const newValueReceived = new TextDecoder().decode(event.target.value);
	retrievedValue.innerHTML = newValueReceived;
	retrievedTimestamp.innerHTML = getDateTime();
	orientationDevice.innerHTML = newValueReceived;

	const div = document.createElement('div');
	const header = document.createElement('h1');
	const text = document.createElement('p');
	header.innerHTML = 'RECEIVE';
	text.innerHTML = newValueReceived;
	div.appendChild(header)
	div.appendChild(text)
	div.classList.add('entry')
	bleReceiveHistory.prepend(div)
	// Remove oldest entry if more than 6 children
	if (bleReceiveHistory.children.length > 6) {
		bleReceiveHistory.removeChild(bleReceiveHistory.lastElementChild);
	}
}

let sending = false;

export async function writeCmd(value) {
    if(sending) return "busy";
    sending=true;
    let sent="failed";
	if (bleServer && bleServer.connected) {
		const textEncoder = new TextEncoder();
		const uint8Array = textEncoder.encode(value);
		sentTimestamp.innerHTML = getDateTime();
		try {
			await cmdCharacteristic.writeValue(uint8Array);
			latestValueSent.innerHTML = value;
			const div = document.createElement('div');
			const header = document.createElement('h1');
			const text = document.createElement('p');
			header.innerHTML = 'CMD';
			text.innerHTML = value;
			div.appendChild(header)
			div.appendChild(text)
			div.classList.add('entry')
			bleSendHistory.prepend(div)
			// Remove oldest entry if more than 6 children
			if (bleSendHistory.children.length > 6) {
				bleSendHistory.removeChild(bleSendHistory.lastElementChild);
			}
            sent="ok";
		} catch(error) {
			console.error("Error writing to the CMD characteristic: ", error);
		}
	} else {
		console.error ("Bluetooth is not connected. Cannot write to characteristic.")
		onDisconnected();
        sent="disconnected"
	}
    // 100ms verzögerung einbauen, um nicht zu häufig zu schicken!
    await new Promise((resolve, reject) => setTimeout(resolve, 100));
    sending=false;
    return sent;
}

export async function writeArmCmd(value) {
	if(sending) return "busy";
	sending=true;
	let sent = "failed";
	if (bleServer && bleServer.connected) {
		const textEncoder = new TextEncoder();
		const uint8Array = textEncoder.encode(value);
		sentTimestamp.innerHTML = getDateTime();
		try {
			await armCharacteristic.writeValue(uint8Array);
			latestValueSent.innerHTML = value;
			const div = document.createElement('div');
			const header = document.createElement('h1');
			const text = document.createElement('p');
			header.innerHTML = 'ARM';
			text.innerHTML = value;
			div.appendChild(header)
			div.appendChild(text)
			div.classList.add('entry')
			bleSendHistory.prepend(div)
			// Remove oldest entry if more than 6 children
			if (bleSendHistory.children.length > 6) {
				bleSendHistory.removeChild(bleSendHistory.lastElementChild);
			}
			sent = "ok";
		} catch (error) {
			console.error("Error writing to ARM characteristic: ", error);
		}
	} else  {
		console.error ("Bluetooth is not connected. Cannot write to characteristic.")
		onDisconnected();
		sent="disconnected"
	}
	// 100ms verzögerung einbauen, um nicht zu häufig zu schicken!
	await new Promise((resolve, reject) => setTimeout(resolve, 100));
	sending=false;
	return sent;
}

function disconnectDevice() {
	console.log("Disconnect Device.");
	if (bleServer && bleServer.connected) {
		if (cmdCharacteristic) {
			cmdCharacteristic.stopNotifications()
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
			console.log("No characteristic found to disconnect.");
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

	let datetime = day + "." + month + "." + year + ", " + hours + ":" + minutes + ":" + seconds;
	return datetime;
}


