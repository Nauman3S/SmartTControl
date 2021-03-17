//Updates
//OLED (SSID,AP MODE OR GPRS, IP ADDRESS, TEMP VAL, TEMP VAL FROM CMOTS)
//PI Controller
//GSM
//IMEIs display, dynamic sensor display, internet status notification, server data publish time
//Server->ESP32 Dynamic Freq
//Run button removal
 //SSR relay, logic level shifter



//ref https://simple-circuit.com/arduino-frequency-meter-220-380v-ac/
// Alright great. Let me know how it goes. Btw for the PI controller you would need to chop
//  the sinewave of the AC power. otherwise the harmonics can get really bad. 
//  So just program the PI controller according to that
// I can send a document to explain the mechanism if needed
// and can contribute to analog circuit design


// The ZCD will go on the pcb. It should have an optoisolator to keep the ac and dc side isolated. 
// Also SSR is itself an optisolated triac,
// so once a signal pulse is given at the input, it stays on until that half cycle is over
// So simply one 0.5ms pulse would be enough to turn it on

// Yes, for the system to work with both 60hz and 50hz we need our own optoisolator to
// detect the time of 2 zero crossings so we can know the time duration of a full AC cycle

// Dont need to measure the amplitude of the AC waveform. Just the time at which it goes to zero. 