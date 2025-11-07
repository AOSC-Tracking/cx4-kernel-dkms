BIN_TYPE ?= $(shell uname -m |sed -e s/i.86/i386/)
LINUXDIR ?= $(shell pwd)
DRM_VER=$(shell sed -n 's/^RHEL_DRM_VERSION = \(.*\)/\1/p' $(LINUXDIR)/Makefile)
YHQILINOS=$(shell sed -n 's/^export KBUILD_BUILD_USER=//p' $(LINUXDIR)/Makefile)
DRM_SUBVERSION=$(shell sed -n 's/^EXTRAVERSION = \(-\)//p' $(LINUXDIR)/Makefile)
EXTRA_LINUX_FLAGS=

ifneq ($(DRIVER_NAME), zxe3k)
ccflags-y += -DBUILD_VENDOR=1
else
ccflags-y += -DBUILD_VENDOR=0
endif

ifneq (,$(DRM_VER))
DRM_PATCH=$(shell sed -n 's/^RHEL_DRM_PATCHLEVEL = \(.*\)/\1/p' $(LINUXDIR)/Makefile)
DRM_SUBLEVEL=$(shell sed -n 's/^RHEL_DRM_SUBLEVEL = \(.*\)/\1/p' $(LINUXDIR)/Makefile)
DRM_CODE=$(shell expr $(DRM_VER) \* 65536 + 0$(DRM_PATCH) \* 256 + 0$(DRM_SUBLEVEL))
ifneq ($(VERSION).$(PATCHLEVEL).$(SUBLEVEL), $(DRM_VER).$(DRM_PATCH).$(DRM_SUBLEVEL))
    EXTRA_LINUX_FLAGS = -include drm/drm_backport.h
endif
ccflags-y += -DDRM_VERSION_CODE=$(DRM_CODE)
endif

ifeq ($(BIN_TYPE)_$(YHQILINOS), aarch64_YHKYLIN-OS)
    DISTRIB_ID=$(shell sed -n 's/^DISTRIB_ID=//p' /etc/lsb-release)
    DISTRIB_RELEASE=$(shell sed -n 's/^DISTRIB_RELEASE=//p' /etc/lsb-release)
    KYLIN_RELEASE=$(shell sed -n 's/^DISTRIB_KYLIN_RELEASE=//p' /etc/lsb-release)
    ifeq ($(DISTRIB_RELEASE), V10)
        DISTRIB_ID_RELEASE=$(DISTRIB_ID)_$(DISTRIB_RELEASE)
    else
        DISTRIB_ID_RELEASE=$(DISTRIB_ID)_$(KYLIN_RELEASE)
    endif

    ifneq (,$(filter $(DISTRIB_ID_RELEASE)_$(BIN_TYPE), Kylin_V10_aarch64 Kylin_4.0.2-SP4_aarch64))
         ccflags-y += -DPHYTIUM_2000
    else
         DRM_VER=4
         DRM_PATCH=9
         DRM_SUBLEVEL=0
         DRM_CODE=$(shell expr $(DRM_VER) \* 65536 + 0$(DRM_PATCH) \* 256 + 0$(DRM_SUBLEVEL))
         ccflags-y += -DDRM_VERSION_CODE=$(DRM_CODE) -DYHQILIN
    endif
endif

ifneq ($(BIN_TYPE)_$(YHQILINOS), aarch64_YHKYLIN-OS)
    ifeq ($(YHQILINOS), YHKYLIN-OS)
        ccflags-y += -DYHQILIN
        KERNEL_VERSION_NUM := $(shell echo $(KERNELRELEASE) | cut -d - -f1)
        ifeq (,$(DRM_VER))
            ifeq ($(KERNEL_VERSION_NUM), 4.4.131)
                DRM_VER=4
                DRM_PATCH=9
                DRM_SUBLEVEL=0
                DRM_CODE=$(shell expr $(DRM_VER) \* 65536 + 0$(DRM_PATCH) \* 256 + 0$(DRM_SUBLEVEL))
                ccflags-y += -DDRM_VERSION_CODE=$(DRM_CODE) -DDRM_SUBVERSION_CODE=$(DRM_SUBVERSION)
            endif
        endif
    endif
endif

ifeq ($(BIN_TYPE), mips64)
	ccflags-y += -mips64r2 -D__mips64__ -mlong-calls
endif

ifeq ($(BIN_TYPE), aarch64)
	ccflags-y += -mstrict-align
endif

ccflgs-y += -fms-extensions

ccflags-y += \
	-I$(GFGPU_FULL_PATH)/shared \
	-I$(GFGPU_FULL_PATH)/core/include \
	-I$(GFGPU_FULL_PATH)/cbios \
	-I$(GFGPU_FULL_PATH)/linux

linux-objs:= \
	os_interface.o  \
	zx.o \
	zx_driver.o \
	zx_pcie.o \
	../shared/os_shared.o \
	zx_debugfs.o \
	zx_sysfs.o \
	zx_ioctl.o \
	zx_gem.o \
	zx_fence.o \
	zxgfx_trace_events.o \
	zx_drm_syncobj.o \
	zx_dma_fence_array.o \
	zx_atomic.o \
	zx_connector.o \
	zx_crtc.o \
	zx_disp.o \
	zx_drmfb.o \
	zx_encoder.o \
	zx_irq.o \
	zx_plane.o \
	zx_fbdev.o \
	zx_cbios.o \
	zx_params.o \
	zx_capture_drv.o \
	zx_vip.o \
	zx_wb.o \
	zx_i2c.o \
	e3k/zx_irq_e3k.o \
	zx_backlight.o \
	zx_sink.o

ifeq ($(ZX_CMODEL_DRIVER), 1)
	linux-objs += \
		cmodel/zx_cmodel_driver.o \
		cmodel/zx_cmodel_mmio.o
else ifeq ($(HW_NULL), 1)
	linux-objs += zx_hw_null.o
else
	linux-objs += zx_pcie.o
endif

$(DRIVER_NAME)-objs += $(addprefix linux/, $(linux-objs))

$(foreach f, $(notdir ${linux-objs}), $(eval CFLAGS_$f:=${DEBUG_FLAGS_LINUX}))

ifneq ($(EXTRA_LINUX_FLAGS),)
    $(foreach f, $(notdir ${linux-objs}), $(eval CFLAGS_$f:=${EXTRA_LINUX_FLAGS}))
endif
