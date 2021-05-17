volatile int interruptsEnabled=1;

void pauseInterrupts(){
    interruptsEnabled=0;
}

void resumeInterrupts(){
    interruptsEnabled=1;
}