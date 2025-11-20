/*
 * 30.lab_resource_manager.c
 *
 *  Created on: Nov 13, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/resmgr.h>
#include <secpol/secpol.h>
#include <errno.h>

#define DEVICE_NAME "/dev/example"

static void options(int argc, char* argv[]);

static int io_open(resmgr_context_t* ctp, io_open_t* msg, RESMGR_HANDLE_T* handle, void* extra);
static int io_read(resmgr_context_t* ctp, io_read_t* msg, RESMGR_OCB_T* ocb);
static int io_write(resmgr_context_t* ctp, io_write_t* msg, RESMGR_OCB_T* ocb);

static resmgr_connect_funcs_t connect_funcs;
static resmgr_io_funcs_t io_funcs;

static dispatch_t* dpp;
static dispatch_context_t* ctp;
static iofunc_attr_t ioattr;
static int optv;

int qnx_lab_resource_manager(int argc, char** argv, char** envp) {
  printf("example starting...\n");
  options(argc, argv);

  dpp = dispatch_create_channel(-1, DISPATCH_FLAG_NOLOCK);
  if (dpp == NULL) {
    fprintf(stderr, "dispatch_create_channel failed\n");
    exit(EXIT_FAILURE);
  }

  iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
  connect_funcs.open = io_open;
  io_funcs.read = io_read;
  io_funcs.write = io_write;

  iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);
  int ret = secpol_resmgr_attach(NULL, dpp, NULL,
                                 DEVICE_NAME, _FTYPE_ANY, 0,
                                 &connect_funcs,
                                 &io_funcs, &ioattr, NULL);
  if (ret == -1) {
    fprintf(stderr, "secpol_resmgr_attach failed\n");
    exit(EXIT_FAILURE);
  }

  ctp = dispatch_context_alloc(dpp);
  if (ctp == NULL) {
    fprintf(stderr, "dispatch_context_alloc failed\n");
    exit(EXIT_FAILURE);
  }

  secpol_transition_type(NULL, NULL, 0);

  while (1) {
    if (dispatch_block(ctp) == NULL) {
      fprintf(stderr, "dispatch_block failed\n");
      exit(EXIT_FAILURE);
    }

    if (optv > 1) {
      if (ctp->message_context.rcvid == 0) {
        printf("pulse code: %d\n", ctp->message_context.msg->pulse.code);
      }
      else {
        printf("message type: %d\n", ctp->message_context.msg->type);
      }
    }

    dispatch_handler(ctp);
  }
  return EXIT_SUCCESS;
}

static int io_open(resmgr_context_t* ctp, io_open_t* msg, RESMGR_HANDLE_T* handle, void* extra) {
  if (optv) {
    printf("> io_open\n");
  }

  return iofunc_open_default(ctp, msg, handle, extra);
}

static int io_read(resmgr_context_t* ctp, io_read_t* msg, RESMGR_OCB_T* ocb) {
  char data[] = "hello";
  size_t nb;
  if (optv) {
    printf("> io_read\n");
  }

  int status;
  if ((status = iofunc_read_verify(ctp, msg, ocb, NULL)) != EOK) {
    return status;
  }

  if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE) {
    return ENOSYS;
  }

  nb = sizeof(data);
  nb = min(nb, msg->i.nbytes);
  MsgReply(ctp->rcvid, nb, data, nb);
  if (nb > 0) {
    ocb->attr->flags |= IOFUNC_ATTR_ATIME;
  }

  return _RESMGR_NOREPLY;
}

static int io_write(resmgr_context_t* ctp, io_write_t* msg, RESMGR_OCB_T* ocb) {
  if (optv) {
    printf("> io_write\n");
  }

  int status;
  if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK) {
    return status;
  }

  if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE) {
    return ENOSYS;
  }

  if (msg->i.nbytes == 0) {
    MsgReply(ctp->rcvid, 0, NULL, 0);
    return _RESMGR_NOREPLY;
  }

  char* buf = (char*)malloc(msg->i.nbytes);
  if (!buf) {
    return ENOMEM;
  }

  int nb = resmgr_msgget(ctp, buf, msg->i.nbytes, sizeof(*msg));
  if (nb == -1) {
    free(buf);
    return errno;
  }

  status = write(STDOUT_FILENO, buf, nb);
  free(buf);

  if (status == -1) {
    return errno;
  }

  MsgReply(ctp->rcvid, nb, NULL, 0);
  if (nb > 0) {
    ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;
  }

  return _RESMGR_NOREPLY;
}

static void options(int argc, char* argv[]) {
  optv = 0;

  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        optv++;
        break;
    }
  }
}
