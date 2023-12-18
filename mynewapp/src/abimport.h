/* Import (extern) header for application - AppBuilder 2.03  */

#include "abdefine.h"

extern ApWindowLink_t WinBase;
extern ApWidget_t AbWidgets[ 63 ];


#ifdef __cplusplus
extern "C" {
#endif
int WinBaseSetup( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int BtnForceActivate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
int TimerRefreshActivate( PtWidget_t *widget, ApInfo_t *data, PtCallbackInfo_t *cbinfo );
#ifdef __cplusplus
}
#endif
