// pwm_fsm.c
#include "pwm_fsm.h"

// Static variable to hold the current state
static PWM_State_Machine currentState = PwmStateStandby;

// Forward declaration of state transition table
static const StateTransition_t stateTransitions[] = {
    // currentState,       event,            action,             nextState
    { PwmStateStandby,     Evt_StartCommand,  NULL,              PwmStateInit },
    { PwmStateInit,        Evt_InitComplete,  NULL,              PwmStateSoftStart },
    { PwmStateSoftStart,   Evt_SoftStartDone, NULL,              PwmStateRunning },
    { PwmStateRunning,     Evt_TuningCommand, NULL,              PwmStateTuning },
    { PwmStateRunning,     Evt_OverPowerDetected, NULL,          PwmStateFault },
    { PwmStateRunning,     Evt_OverTempDetected,  NULL,          PwmStateFault },
    { PwmStateRunning,     Evt_UnderVoltageDetected, NULL,       PwmStateFault },
    { PwmStateFault,       Evt_FaultCleared,  NULL,              PwmStateSoftStop },
    { PwmStateSoftStop,    Evt_SoftStopDone,  NULL,              PwmStateStandby },
    { PwmStateTuning,      Evt_TuningComplete,NULL,              PwmStateRunning },
    { PwmStateRunning,     Evt_StopCommand,   NULL,              PwmStateSoftStop },
    // Add more transitions as needed
};

// Number of transitions
static const uint16_t numTransitions = sizeof(stateTransitions) / sizeof(stateTransitions[0]);

void PWM_FSM_Init(void) {
    currentState = PwmStateStandby;
}

void PWM_FSM_HandleEvent(PWM_Event_t event) {
    for (uint16_t i = 0; i < numTransitions; i++) {
        if (stateTransitions[i].currentState == currentState && stateTransitions[i].event == event) {
            if (stateTransitions[i].action != NULL) {
                stateTransitions[i].action();
            }
            currentState = stateTransitions[i].nextState;
            break;
        }
    }
}

PWM_State_Machine PWM_FSM_GetCurrentState(void) {
    return currentState;
}
