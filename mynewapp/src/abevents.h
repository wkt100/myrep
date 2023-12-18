/* Event header for application - AppBuilder 2.03  */

static const ApEventLink_t AbApplLinks[] = {
	{ 3, 0, 0L, 0L, 0L, &WinBase, NULL, NULL, 0, WinBaseSetup, 0, 0, 0, 0, },
	{ 0 }
	};

static const ApEventLink_t AbLinks_WinBase[] = {
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "BtnForce", 2009, BtnForceActivate, 0, 0, 0, 0, },
	{ 8, 0, 0L, 0L, 0L, NULL, NULL, "TimerRefresh", 41002, TimerRefreshActivate, 0, 0, 0, 0, },
	{ 0 }
	};

const char ApOptions[] = AB_OPTIONS;
