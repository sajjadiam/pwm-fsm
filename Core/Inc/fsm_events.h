#ifndef __FSM_EVENTS_H__
#define __FSM_EVENTS_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	Evt_None = 0,

	// ---- دستورات کنترلی ----
	Evt_StartCommand,
	Evt_StopCommand,

	// ---- اتمام مراحل ----
	Evt_InitComplete,
	Evt_SoftStartDone,
	Evt_SoftStopDone,
	Evt_ResonanceFound,

	// ---- کنترل حلقه توان و رزونانس ----
	Evt_TargetPowerReached,
	Evt_ResonanceLost,

	// ---- دستورات فرعی ----
	Evt_TuningCommand,
	Evt_TuningComplete,

	// ---- خطاهای نرم قابل بازیابی ----
	Evt_OverPowerDetected,
	Evt_OverTempDetected,
	Evt_UnderVoltageDetected,
	Evt_OverCurrentDetected,

	// ---- خطاهای سخت غیرقابل بازیابی ----
	Evt_ShortCircuitDetected,
	Evt_HardwareFault,
	Evt_ArcDetected,
	Evt_GridFaultDetected,
	
	// ---- سیستم ----
	Evt_FaultCleared,
	Evt_End
}PWM_Event_t;

#ifdef __cplusplus
}
#endif

#endif //__FSM_EVENTS_H__