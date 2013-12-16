/*
 Copyright (c) 2010 Mathieu Laurendeau <mat.lau@laposte.net>
   License: GPLv3
*/

#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define HCI_REQ_TIMEOUT   1000

/*
 * \brief This function gets the bluetooth device address for a given device number.
 *
 * \param device_number  the device number
 * \param bdaddr     the buffer to store the bluetooth device address
 *
 * \return 0 if successful, -1 otherwise
 */
int get_device_bdaddr(int device_number, char bdaddr[18])
{
  int ret = 0;

  bdaddr_t bda;

  int s = hci_open_dev (device_number);

  if(hci_read_bd_addr(s, &bda, HCI_REQ_TIMEOUT) < 0)
  {
    ret = -1;
  }
  else
  {
    ba2str(&bda, bdaddr);
  }

  close(s);

  return ret;
}

/*
 * \brief This function writes the bluetooth device class for a given device number.
 *
 * \param device_number  the device number
 *
 * \return 0 if successful, -1 otherwise
 */
int write_device_class(int device_number, uint32_t class)
{
  int ret = 0;

  int s = hci_open_dev (device_number);

  if(hci_write_class_of_dev(s, class, HCI_REQ_TIMEOUT) < 0)
  {
    ret = -1;
  }

  close(s);

  return ret;
}

int delete_stored_link_key(int device_number, char* bdaddr)
{
  int ret = 0;

  int s = hci_open_dev (device_number);

  bdaddr_t bda;
  str2ba(bdaddr, &bda);

  if(hci_delete_stored_link_key(s, &bda, 0, HCI_REQ_TIMEOUT) < 0)
  {
    ret = -1;
  }

  close(s);

  return ret;
}

int write_stored_link_key(int device_number, char* bdaddr, unsigned char* key)
{
  int ret = 0;

  int s = hci_open_dev (device_number);

  bdaddr_t bda;
  str2ba(bdaddr, &bda);

  if(hci_write_stored_link_key(s, &bda, key, HCI_REQ_TIMEOUT) < 0)
  {
    ret = -1;
  }

  close(s);

  return ret;
}

int authenticate_link(char* bdaddr)
{
  int ret = 0;

  int err = 0, dd;
  struct hci_conn_info_req *cr = 0;

  // find the connection handle to the specified bluetooth device
  cr = (struct hci_conn_info_req*) malloc(
      sizeof(struct hci_conn_info_req) +
      sizeof(struct hci_conn_info));

  str2ba(bdaddr, &cr->bdaddr);

  cr->type = ACL_LINK;
  dd = hci_open_dev( hci_get_route( &cr->bdaddr ) );
  if( dd < 0 ) {
    ret = -1;
  }
  else
  {
    err = ioctl(dd, HCIGETCONNINFO, (unsigned long) cr );
    if( !err )
    {
      if(hci_authenticate_link(dd, cr->conn_info->handle, HCI_REQ_TIMEOUT) < 0)
      {
        ret = -1;
      }
    }
    else
    {
      ret = -1;
    }
  }

  free(cr);
  close(dd);

  return ret;
}

int encrypt_link(char* bdaddr)
{
  int ret = 0;

  int err = 0, dd;
  struct hci_conn_info_req *cr = 0;

  // find the connection handle to the specified bluetooth device
  cr = (struct hci_conn_info_req*) malloc(
      sizeof(struct hci_conn_info_req) +
      sizeof(struct hci_conn_info));

  str2ba(bdaddr, &cr->bdaddr);

  cr->type = ACL_LINK;
  dd = hci_open_dev( hci_get_route( &cr->bdaddr ) );
  if( dd < 0 ) {
    ret = -1;
  }
  else
  {
    err = ioctl(dd, HCIGETCONNINFO, (unsigned long) cr );
    if( !err )
    {
      if(hci_encrypt_link(dd, cr->conn_info->handle, 0x01, HCI_REQ_TIMEOUT) < 0)
      {
        ret = -1;
      }
    }
    else
    {
      ret = -1;
    }
  }

  free(cr);
  close(dd);

  return ret;
}
