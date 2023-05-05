
int SECONDS = 0;  // Seconds from start of DAY, day=[0,16*60*60] night=(16*60*60, 24*60*60)

struct Climate {
    int max_dirt_humidity;
    int min_dirt_humidity;
} CLIMATE;

CLIMATE = {50, 30};


enum State {ON, OFF};

struct Cooler {
    bool status_vent;
    bool status_air_humudity;
    bool status_heat;
} COOLER = {False, False, False};


void timer() {
    SECONDS++;
    if(SECONDS >= 60 * 60 * 24) SECONDS = 0;
    delay(1000);  // 01 second
}


void cooler() {
    if( COOLER.status_vent
        & COOLER.status_air_humidity
        & COOLER.status_heat ) {
        // включить
    } else {
        // выключить
    }
}


void periodic_ventilation() {
    if(2*60*60 < SECONDS & SECONDS < 3*60*60) { COOLER.status_vent = True; return;}
    if(6*60*60 < SECONDS & SECONDS < 7*60*60) { COOLER.status_vent = True; return;}
    COOLER.status_vent = False;
}


void control_temperature() {  // TODO

}


void irrigate() {
    int max_working_time = 10;  // pump is on for 10 seconds
    int min_relaxation_time = 30;  // after irrigation pump if of for 30 seconds at least
    static current_relaxation_time = 0;
    // включить помпу
    current_relaxation_time--;
}


void control_dirt_humidity() {  // TODO
    int humidity = get_humidity();
    if(humidity < CLIMATE.min_humidity) irrigate();
}


void control_air_humidity() {  // TODO
}


void control_light() {  // TODO
}


void setup() {
}


void loop() {
    timer();

    periodic_ventilation();
    control_temperature();
    control_dirt_humidity();
    control_air_humidity();
    control_light();

    cooler();
    irrigate();
}
