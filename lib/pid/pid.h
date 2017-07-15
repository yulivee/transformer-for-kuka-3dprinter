#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#define MOVEP 15 //P Motorspannung pro Schritt - Genauigkeit. Erzeugt Zittern und schießt übers Ziel raus
#define MOVEI 0.5 //I Erhöht Motorspannung bei kleinen Fehlern - lässt die letzten bisschen Fehler verschwinden
#define MOVED 10 //D Rechtzeitig bremsen - dämpft Zittern und rauschießen
#define MOVEMAX 255
#define MOVEIMAX 100   
#define GOVERNOR_FREQ 100	
#define BOUNDS(var, max) if ((var)>(max)) (var)=(max); if ( (var)<-(max)) (var)=-(max);
    
void timer_init();

#ifdef __cplusplus
}
#endif

#endif
