/*
 *	The PCI Library -- Internal Stuff
 *
 *	Copyright (c) 1997--2022 Martin Mares <mj@ucw.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL v2+
 *
 *	SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _INTERNAL_H
#define _INTERNAL_H

#include "config.h"

#ifdef PCI_SHARED_LIB
#define PCI_ABI __attribute__((visibility("default")))
// Functions, which are bound to externally visible symbols by the versioning
// mechanism, have to be declared as VERSIONED. Otherwise, GCC with global
// optimizations is happy to optimize them away, leading to linker failures.
#define VERSIONED_ABI __attribute__((used)) PCI_ABI
#ifdef __APPLE__
#define STATIC_ALIAS(_decl, _for) VERSIONED_ABI _decl { return _for; }
#define DEFINE_ALIAS(_decl, _for)
#define SYMBOL_VERSION(_int, _ext)
#else
#define DEFINE_ALIAS(_decl, _for) extern _decl __attribute__((alias(#_for))) VERSIONED_ABI
#ifdef _WIN32
#define STATIC_ALIAS(_decl, _for) VERSIONED_ABI _decl { return _for; }
/* GCC does not support asm .symver directive for Windows targets, so define new external global function symbol as alias to internal symbol */
#define SYMBOL_VERSION(_int, _ext) asm(".globl\t" PCI_STRINGIFY(__MINGW_USYMBOL(_ext)) "\n\t" \
                                       ".def\t"   PCI_STRINGIFY(__MINGW_USYMBOL(_ext)) ";\t.scl\t2;\t.type\t32;\t.endef\n\t" \
                                       ".set\t"   PCI_STRINGIFY(__MINGW_USYMBOL(_ext)) "," PCI_STRINGIFY(__MINGW_USYMBOL(_int)))
#else
#define STATIC_ALIAS(_decl, _for)
#define SYMBOL_VERSION(_int, _ext) asm(".symver " #_int "," #_ext)
#endif
#endif
#else
#define VERSIONED_ABI
#define STATIC_ALIAS(_decl, _for) _decl { return _for; }
#define DEFINE_ALIAS(_decl, _for)
#define SYMBOL_VERSION(_int, _ext)
#endif

#include "pci.h"
#include "sysdep.h"

/* Old 32-bit-only versions of MinGW32 do not define __MINGW_USYMBOL macro */
#ifdef __MINGW32__
#ifndef __MINGW_USYMBOL
#define __MINGW_USYMBOL(sym) _##sym
#endif
#endif

#define _PCI_STRINGIFY(x) #x
#define PCI_STRINGIFY(x) _PCI_STRINGIFY(x)

struct pci_methods {
  char *name;
  char *help;
  void (*config)(struct pci_access *);
  int (*detect)(struct pci_access *);
  void (*init)(struct pci_access *);
  void (*cleanup)(struct pci_access *);
  void (*scan)(struct pci_access *);
  void (*fill_info)(struct pci_dev *, unsigned int flags);
  int (*read)(struct pci_dev *, int pos, byte *buf, int len);
  int (*write)(struct pci_dev *, int pos, byte *buf, int len);
  int (*read_vpd)(struct pci_dev *, int pos, byte *buf, int len);
  void (*init_dev)(struct pci_dev *);
  void (*cleanup_dev)(struct pci_dev *);
};

/* generic.c */
void pci_generic_scan_bus(struct pci_access *, byte *busmap, int domain, int bus);
void pci_generic_scan_domain(struct pci_access *, int domain);
void pci_generic_scan(struct pci_access *);
void pci_generic_fill_info(struct pci_dev *, unsigned int flags);
int pci_generic_block_read(struct pci_dev *, int pos, byte *buf, int len);
int pci_generic_block_write(struct pci_dev *, int pos, byte *buf, int len);

/* emulated.c */
int pci_emulated_read(struct pci_dev *d, int pos, byte *buf, int len);

/* init.c */
void *pci_malloc(struct pci_access *, int);
void pci_mfree(void *);
char *pci_strdup(struct pci_access *a, const char *s);
struct pci_access *pci_clone_access(struct pci_access *a);
int pci_init_internal(struct pci_access *a, int skip_method);

void pci_init_v30(struct pci_access *a) VERSIONED_ABI;
void pci_init_v35(struct pci_access *a) VERSIONED_ABI;

/* access.c */
struct pci_dev *pci_alloc_dev(struct pci_access *);
int pci_link_dev(struct pci_access *, struct pci_dev *);

int pci_fill_info_v30(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v31(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v32(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v33(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v34(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v35(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v38(struct pci_dev *, int flags) VERSIONED_ABI;
int pci_fill_info_v313(struct pci_dev *, int flags) VERSIONED_ABI;

static inline int want_fill(struct pci_dev *d, unsigned want_fields, unsigned int try_fields)
{
  want_fields &= try_fields;
  if ((d->known_fields & want_fields) == want_fields)
    return 0;
  else
    {
      d->known_fields |= try_fields;
      return 1;
    }
}

static inline void clear_fill(struct pci_dev *d, unsigned clear_fields)
{
  d->known_fields &= ~clear_fields;
}

struct pci_property {
  struct pci_property *next;
  u32 key;
  char value[1];
};

char *pci_set_property(struct pci_dev *d, u32 key, char *value);

/* params.c */
struct pci_param *pci_define_param(struct pci_access *acc, char *param, char *val, char *help);
int pci_set_param_internal(struct pci_access *acc, char *param, char *val, int copy);
void pci_free_params(struct pci_access *acc);

/* caps.c */
void pci_scan_caps(struct pci_dev *, unsigned int want_fields);
void pci_free_caps(struct pci_dev *);

extern struct pci_methods pm_intel_conf1, pm_intel_conf2, pm_linux_proc,
	pm_fbsd_device, pm_aix_device, pm_nbsd_libpci, pm_obsd_device,
	pm_dump, pm_linux_sysfs, pm_darwin, pm_sylixos_device, pm_hurd,
	pm_mmio_conf1, pm_mmio_conf1_ext, pm_ecam,
	pm_win32_cfgmgr32, pm_win32_kldbg, pm_win32_sysdbg, pm_aos_expansion,
	pm_rt_thread_smart_dm;

#endif
