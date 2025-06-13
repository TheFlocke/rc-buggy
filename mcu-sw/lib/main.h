//
// Created by Philipp Winterberg on 13.06.25.
//

#ifndef MAIN_H
#define MAIN_H

class main {



public:
    void sensorTask(void *xTaskParameters);
    void bleTask(void *xTaskParameters);

};

extern constexpr int MEAS_DUR;

#endif //MAIN_H
