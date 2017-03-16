/* simple.c
Simple libftdi usage example
This program is distributed under the GPL, version 2
*/
#include <stdio.h>
#include <stdlib.h>
#include <ftdi.h>
int main(void)
{
int ret, f;
struct ftdi_context *ftdi;
//struct ftdi_version_info version;
if ((ftdi = ftdi_new()) == 0)
{
fprintf(stderr, "ftdi_new failed\n");
return EXIT_FAILURE;
}
//version = ftdi_get_library_version();
//printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
//version.version_str, version.major, version.minor, version.micro,
//version.snapshot_str);
if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6001)) < 0)
{
fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
ftdi_free(ftdi);
return EXIT_FAILURE;
}
// Read out FTDIChip-ID of R type chips
if (ftdi->type == TYPE_R)
{
unsigned int chipid;
printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
printf("FTDI chipid: %X\n", chipid);
}


    // Set baudrate
    f = ftdi_set_baudrate(ftdi, 9600);
    if (f < 0)
    {
        fprintf(stderr, "unable to set baudrate: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }



    f = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
    if (f < 0)
    {
        fprintf(stderr, "unable to set line parameters: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }


f = ftdi_write_data(ftdi, ">O000\r",6);
    if (f < 0)
    {
        fprintf(stderr, "unable to write: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }





if ((ret = ftdi_usb_close(ftdi)) < 0)
{
fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
ftdi_free(ftdi);
return EXIT_FAILURE;
}
ftdi_free(ftdi);
return EXIT_SUCCESS;
}
