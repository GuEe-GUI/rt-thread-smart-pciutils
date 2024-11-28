/*
 *  The PCI Library -- Direct Configuration access via RT-Thread Smart DM Ports
 *
 *  Copyright (c) 2024 RT-Thread Development Team
 *
 *  Can be freely distributed and used under the terms of the GNU GPL v2+.
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

static void rt_thread_smart_dm_config(struct pci_access *a)
{
  pci_define_param(a, "rt-thread-smart-dm.path", PCI_PATH_RT_THREAD_SMART_DM,
    "Path to the RT-Thread Smart PCI device");
}

static int rt_thread_smart_dm_detect(struct pci_access *a)
{
  char *name = pci_get_param(a, "rt-thread-smart-dm.path");

  if (access(name, R_OK))
    {
      a->warning("Cannot open %s", name);
      return 0;
    }

  a->debug("...using %s", name);
  return 1;
}

static void rt_thread_smart_dm_init(struct pci_access *a UNUSED)
{

}

static void rt_thread_smart_dm_cleanup(struct pci_access *a UNUSED)
{

}

static void rt_thread_smart_dm_scan(struct pci_access *a)
{

}

static void rt_thread_smart_dm_fill_info(struct pci_dev *d, unsigned int flags)
{

}

static int rt_thread_smart_dm_read(struct pci_dev *d, int pos, byte *buf, int len)
{

}

static int rt_thread_smart_dm_write(struct pci_dev *d, int pos, byte *buf, int len)
{

}

struct pci_methods pm_rt_thread_smart_dm = {
  .name = "rt-thread-smart-dm",
  .help = "RT-Thrad Smart /proc/pci device",
  .config = rt_thread_smart_dm_config,
  .detect = rt_thread_smart_dm_detect,
  .init = rt_thread_smart_dm_init,
  .cleanup = rt_thread_smart_dm_cleanup,
  .scan = rt_thread_smart_dm_scan,
  .fill_info = pci_generic_fill_info,
  .read = rt_thread_smart_dm_read,
  .write = rt_thread_smart_dm_write,
};
