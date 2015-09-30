#include <iostream>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

using namespace std;

#define VENDOR_ID   0x04f3
#define PRODUCT_ID  0x04a0

int main(int argc, const char** argv) {
	// exit out if a command isn't provided
	if (argc < 2) {
		cout << "Missing argument" << endl;
		return 1;
	}

	libusb_device_handle *dev_handle; //a device handle
	libusb_context *ctx = NULL; //a libusb session

	// init device
	int r; //for return values

	if ((r = libusb_init(&ctx)) < 0) {
		cout << "Init Error " << r << endl; //there was an error
		return 1;
	}

	if (NULL == (dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID))) {
		cout << "Cannot open device" << endl;
		return 1;
	}

	cout << "Device Opened" << endl;

	// free device from the kernel if it has already been claimed
	if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
		cout << "Kernel Driver Active" << endl;
		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout << "Kernel Driver Detached!" << endl;
	}

	// claim the device
	if ((r = libusb_claim_interface(dev_handle, 0)) < 0) {
		cout << "Cannot Claim Interface" << endl;
		return 1;
	}
	cout << "Claimed Interface" << endl;

	// start listening
	cout << "Waiting..." << endl;

	unsigned char* data = new unsigned char[4];
	int actual_length;

	do {
		// the device is retarded and sends 6 responses with one press of the button
        for (int i=0; i<6; i++) {
            r = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_IN), data, sizeof(data), &actual_length, 0);
        }

		if (r == 0 && actual_length == sizeof(data)) {
			cout << "Executing Command" << endl;
			system(argv[1]);
			cout << "Waiting..." << endl;
		} else {
			cout << "error" << endl;
			break;
		}
	} while (1);

	// release the interface
	if ((r = libusb_release_interface(dev_handle, 0)) != 0) {
		cout << "Cannot Release Interface" << endl;
		return 1;
	}

	cout << "Released Interface" << endl;

	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the session

	delete[] data; //delete the allocated memory for data
	return 0;
}
