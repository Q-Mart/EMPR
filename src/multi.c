#include "multi.h"
#include "network.h"
void any_to_multi(){
    //lcd_send_str(0, "Multi Mode");
    network_send(MULTI);
}
