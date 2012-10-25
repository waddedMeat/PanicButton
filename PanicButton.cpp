#include <iostream>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

using namespace std;

#define VENDOR_ID   0x04f3
#define PRODUCT_ID  0x04a0
#define EXECUTABLE  "/etc/panic-button/panic-script"

int main() {
	libusb_device_handle *dev_handle; //a device handle
	libusb_context *ctx = NULL; //a libusb session

	// init device
	int r; //for return values

	r = libusb_init(&ctx); //initialize the library for the session we just declared

	if(r < 0) {
		cout<<"Init Error "<<r<<endl; //there was an error
		return 1;
	}

	// grab the device
	dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);

	if(dev_handle == NULL) {
		cout<<"Cannot open device"<<endl;
		return 1;
	}
	else {
		cout<<"Device Opened"<<endl;
	}

	// free device from the kernel if it has already been claimed
	if(libusb_kernel_driver_active(dev_handle, 0) == 1) {
		cout<<"Kernel Driver Active"<<endl;
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout<<"Kernel Driver Detached!"<<endl;
	}

	// claim the device
	r = libusb_claim_interface(dev_handle, 0);
	if(r < 0) {
		cout<<"Cannot Claim Interface"<<endl;
		return 1;
	}
	cout<<"Claimed Interface"<<endl;

	// start listening
	cout<<"Waiting..."<<endl;

	unsigned char * data = new unsigned char[4];
	int actual_length;

	do
	{
		// the device is retarded and sends 6 responses with one press of the button
        for (int i=0; i<6; i++) {
            r = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_IN), data, sizeof(data), &actual_length, 0);
        }

		if (r == 0 && actual_length == sizeof(data)) {
			cout <<"Executing Script"<<endl;
			system(EXECUTABLE);
			cout<<"Waiting..."<<endl;
		} else {
			cout<<"error"<<endl;
			break;
		}
	}
	while (1);

	// release the interfact
	r = libusb_release_interface(dev_handle, 0);
	if(r!=0) {
		cout<<"Cannot Release Interface"<<endl;
		return 1;
	}
	cout<<"Released Interface"<<endl;

	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the session

	delete[] data; //delete the allocated memory for data
	return 0;
}
