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
let bleServiceContainer;
let bleServiceList;
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
let deviceName ='CRBK';
let SERVICE_UUID         = "5eaf1079-e806-47a9-a1ec-d815bea94805";
let CHARACTERISTIC_CMD   = "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3";
let CHARACTERISTIC_STATE = "bd6fbfde-385d-480f-b5eb-64d60cc7be9a";

//Global variables to Handle Bluetooth
let bleServer;
let bleService;
let cmdCharacteristic;
let stateCharacteristic;

// orientation information
let T;
let X;
let Y;
let orientationFrame;
let orientationCenter;
let direction=0;
let invertDirection=false;
let speed=0;
let running =false;
let led1=false;
let led2=false;

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
	bleServiceList = document.getElementById('bleServiceList');
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
	buttonLeft.addEventListener('click', () => writeCmd("left"));
	buttonRight.addEventListener('click', () => writeCmd("right"));
	buttonUp.addEventListener('click', () => writeCmd("up"));
	buttonDown.addEventListener('click', () => writeCmd("down"));
	buttonStop.addEventListener('click', () => writeCmd("stop"));
	orientationFrame.addEventListener('click', () => toggleStart());

	if (!navigator.bluetooth) {
		errorMessageContainer.innerHTML = "Web Bluetooth API ist für diesen Browser nicht verfügbar!";
	}

	// Connect Button (search for BLE Devices only if BLE is available)
	connectButton.addEventListener('click', (event) => {
		if (navigator.bluetooth){
			connectToDevice();
		}
	});

	// Disconnect Button
	disconnectButton.addEventListener('click', disconnectDevice);
	
	window.addEventListener("deviceorientation", handleOrientation, true);
}


function toggleLED1() {
    led1=!led1;
	if(led1) {
		writeCmdBlocked("led1-on");
        switch1.getElementsByTagName("i")[0].classList.remove("off");
        switch1.getElementsByTagName("i")[0].classList.add("on");
	} else {
		writeCmdBlocked("led1-off");
        switch1.getElementsByTagName("i")[0].classList.remove("on");
        switch1.getElementsByTagName("i")[0].classList.add("off");
	}
}

function toggleLED2() {
    led2=!led2;
	if(led2) {
		writeCmdBlocked("led2-on");
        switch2.getElementsByTagName("i")[0].classList.remove("off");
        switch2.getElementsByTagName("i")[0].classList.add("on");
	} else {
		writeCmdBlocked("led2-off");
        switch2.getElementsByTagName("i")[0].classList.remove("on");
        switch2.getElementsByTagName("i")[0].classList.add("off");
	}
}

function toggleStart() {
	if(running) {
		running = false;
		orientationFrame.classList.remove("running");
		orientationFrame.classList.add("stopped");
		writeCmdBlocked("0:0");
	} else {
		running = true;
		orientationFrame.classList.add("running");
		orientationFrame.classList.remove("stopped");		
		writeCmdBlocked(`${direction}:${speed}`);
	}
}
function handleOrientation(event) {
    let newSpeed = speed;
    let newDirection=direction;
    if(screen.orientation.angle!=0) {
        // Y <-90 || Y>0 -> speed =0
        // Y 0 ... -90 -> 255 -> 0
        // -80° -> speed = -255
        // -60° -> speed = 0
        // -40° -> speed = 255
        let Y = event.gamma+60; // -20 ... 0 ... 20 -> -255 ... 0 ... 255
        newSpeed = Math.round(Y*255/20);
        // -30 .. 30° 
        newDirection=Math.round(event.beta*90/30);
    } else {
        // Y <-90 || Y>0 -> speed =0
        // Y 0 ... -90 -> 255 -> 0
        // 80° -> speed = -255
        // 60° -> speed = 0
        // 40° -> speed = 255
        let Y = event.beta-60; // -20 ... 0 ... 20 -> -255 ... 0 ... 255
        newSpeed = -Math.round(Y*255/20);
        // -30 .. 30° 
        newDirection=Math.round(event.gamma*90/30);
    }
    
    if(newSpeed < -255) newSpeed = -255;
    if(newSpeed > 255) newSpeed = 255;
    if(newDirection<-90) newDirection=-90;
    if(newDirection> 90) newDirection=90;    
    
    if(invertDirection)
        newDirection=-newDirection;
    if(newDirection!=direction || newSpeed!=speed) {
        speed=newSpeed;
        direction=newDirection;
        if(running)
            writeCmd(`${direction}:${speed}`);
        else
            writeCmd("0:0");
    }
        
    let orientation_maxX = orientationFrame.clientWidth - orientationCenter.clientWidth;
    let orientation_maxY = orientationFrame.clientHeight - orientationCenter.clientHeight;

    orientationCenter.style.left = `${(orientation_maxX * (direction+90)) / 180}px`;
    orientationCenter.style.top = `${orientation_maxY/2-(orientation_maxY * speed) / 510}px`; 
    orientation.innerHTML=`dir=${direction}, speed=${speed} (bga: ${Math.round(event.beta)}, ${Math.round(event.gamma)}, ${Math.round(event.alpha)})`;
}

async function registerServiceWorker() { 
	try {
		if ('serviceWorker' in navigator) {
			let registration = await navigator.serviceWorker.register('./sw.js');
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
		
		infoMessageContainer.innerHTML = "retrieve char STATE "+CHARACTERISTIC_STATE;
		stateCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_STATE);
		
		bleCharState.innerHTML = "State Characteristik OK: "+stateCharacteristic.uuid;
		bleCharState.classList.remove("error");
		bleCharState.classList.add("info");
		
		stateCharacteristic.addEventListener('characteristicvaluechanged', handleCharacteristicChange);
		await stateCharacteristic.startNotifications();
		stateCharacteristic.readValue();
		
		infoMessageContainer.innerHTML = "erfolgreich verbunden";
        document.getElementById('connection').innerHTML='Verbunden mit ' + device.name;

		bleStateContainer.innerHTML = 'Verbunden mit ' + device.name;
		bleStateContainer.classList.remove("error");
		bleStateContainer.classList.add("info");
		
		disconnectButton.style.display = "block";
		connectButton.style.display = "none";

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
	const newValueReceived = new TextDecoder().decode(event.target.value);
	retrievedValue.innerHTML = newValueReceived;
	retrievedTimestamp.innerHTML = getDateTime();
	orientationDevice.innerHTML = newValueReceived;
}

let sending = false;

async function writeCmd(value) {
    if(sending) return "busy";
    sending=true;
    let sent="failed";
	if (bleServer && bleServer.connected) {
		const textEncoder = new TextEncoder();
		const uint8Array = textEncoder.encode(value);
		sentTimestamp.innerHTML = getDateTime();
		try {
			await cmdCharacteristic.writeValueWithoutResponse(uint8Array);
			latestValueSent.innerHTML = value;
            sent="ok";
		} catch(error) {
			console.error("Error writing to the CMD characteristic: ", error);
		};
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

function writeCmdBlocked(value){
    let sent="failed";
	if (bleServer && bleServer.connected) {
		const textEncoder = new TextEncoder();
		const uint8Array = textEncoder.encode(value);
		sentTimestamp.innerHTML = getDateTime();
		try {
			cmdCharacteristic.writeValueWithResponse(uint8Array);
			latestValueSent.innerHTML = value;
            sent="ok";
		} catch(error) {
			console.error("Error writing to the CMD characteristic: ", error);
		};
	} else {
		console.error ("Bluetooth is not connected. Cannot write to characteristic.")
		onDisconnected();
        sent="disconnected"
	}
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


