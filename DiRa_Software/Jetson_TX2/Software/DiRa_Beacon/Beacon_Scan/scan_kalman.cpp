#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "SimpleKalmanFilter.h"
#include "SimpleKalmanFilter.cpp"


//Set kalman parameters
SimpleKalmanFilter simpleKalmanFilter(1, 1, 0.01);

//Set addr beacon
char mac_addr_beacon[18]= {"AC:23:3F:22:B7:6A"};

int8_t rssi;
double rssi_loc;

int device , status;

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam){
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}
// Set BLE scan parameters.
void set_BLE_scan_parameters(int device ,int status){

	le_set_scan_parameters_cp scan_params_cp;
	memset(&scan_params_cp, 0, sizeof(scan_params_cp));
	scan_params_cp.type 			= 0x00; 
	scan_params_cp.interval 		= htobs(0x0010);
	scan_params_cp.window 			= htobs(0x0010);
	scan_params_cp.own_bdaddr_type 	= 0x00; // Public Device Address (default).
	scan_params_cp.filter 			= 0x00; // Accept all.

	struct hci_request scan_params_rq = ble_hci_request(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE, &status, &scan_params_cp);

	if ( hci_send_req(device, &scan_params_rq, 1000) < 0 ) {
		hci_close_dev(device);
		perror("Failed to set scan parameters data.");
	}

}
// Set BLE events report mask.
void set_BLE_events_report_mask(int device ,int status){
	le_set_event_mask_cp event_mask_cp;
	memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));
	int i = 0;
	for ( i = 0 ; i < 8 ; i++ ) event_mask_cp.mask[i] = 0xFF;

	struct hci_request set_mask_rq = ble_hci_request(OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE, &status, &event_mask_cp);

	if ( hci_send_req(device, &set_mask_rq, 1000) < 0 ) {
		hci_close_dev(device);
		perror("Failed to set event mask.");

	}

}
// Enable scanning.
le_set_scan_enable_cp scan_cp;
void enable_scanning(int device ,int status){

	//le_set_scan_enable_cp scan_cp;
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable 		= 0x01;	// Enable flag.
	scan_cp.filter_dup 	= 0x00; // Filtering disabled.

	struct hci_request enable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);

	if ( hci_send_req(device, &enable_adv_rq, 1000) < 0 ) {
		hci_close_dev(device);
		perror("Failed to enable scan.");
	}
}
// Disable scanning.
void disable_scanning(int device ,int status){
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = 0x00;	// Disable flag.

	struct hci_request disable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);

	if ( hci_send_req(device, &disable_adv_rq, 1000) < 0 ) {
		hci_close_dev(device);
		perror("Failed to disable scan.");

	}

	hci_close_dev(device);
}
// Get Results.
void get_Results(int device ,int status){
	struct hci_filter nf;
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if ( setsockopt(device, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0 ) {
		hci_close_dev(device);
		perror("Could not set socket options\n");

	}
}


int main(int argc, char **argv){


	// Get HCI device.
	device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) { 
		perror("Failed to open HCI device.");
		return 0; 
	}

	// Set BLE scan parameters.
	set_BLE_scan_parameters(device,status);


	// Set BLE events report mask.
    set_BLE_events_report_mask(device,status);
	
	// Enable scanning.
	enable_scanning(device,status);
	

	// Get Results.
	get_Results( device ,status);

	printf("Scanning....\n");

	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	le_advertising_info * info;

	int len;

   while(1){
           
        len = read(device, buf, sizeof(buf));
		if ( len >= HCI_EVENT_HDR_SIZE ) {
			meta_event = (evt_le_meta_event*)(buf+HCI_EVENT_HDR_SIZE+1);
			if ( meta_event->subevent == EVT_LE_ADVERTISING_REPORT ) {
				uint8_t reports_count = meta_event->data[0];
				void * offset = meta_event->data + 1;

				while ( reports_count-- ) {
				
					info = (le_advertising_info *)offset;
					 char addr[18];
					 ba2str(&(info->bdaddr), addr);

					char a=0;
					for(int i=0 ; i<18 ;i++){
						if(addr[i] == mac_addr_beacon[i]) a++;
						if(i == 17 ){
							if(a==18) rssi = (int8_t)info->data[info->length];
							
						}

					}
                    rssi_loc = simpleKalmanFilter.updateEstimate((float)rssi);
                    printf("RSSI_kalman= %d\n",rssi);

                
                offset = info->data + info->length + 2;
				}
			}
		}
        
    }

	// Disable scanning.
    disable_scanning(device ,status);
    return 0;
}