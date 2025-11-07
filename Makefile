CHIP?=E3k
DRIVER_NAME?=cx4
TARGET_ARCH?=x86_64
DEBUG?=0
VIDEO_ONLY_FPGA?=0
RUN_HW_NULL?=0
HW_NULL?=0
CONFIG_DRM_CX4?=m
ifeq ("$(M)", "")
CHECK_GCC_VERSION?=0
else
CHECK_GCC_VERSION?=1
endif

ccflags-y := -D__LINUX__ -DKERNEL_BUILD
ccflags-y += -Wno-undef -Wno-unused -Wno-missing-braces -Wno-overflow -Wno-error=implicit-function-declaration -Wno-missing-prototypes -Wno-missing-declarations
#ccflags-y += -DZX_MUTEX_TRACK

ifeq ($(CHECK_GCC_VERSION), 1)

KERNEL_UNAME?=$(shell uname -r)
KERNEL_MODLIB:=/lib/modules/$(KERNEL_UNAME)
KERNEL_SOURCES:=$(shell test -d $(KERNEL_MODLIB)/source && echo $(KERNEL_MODLIB)/source || echo $(KERNEL_MODLIB)/build)
KERNEL_COMPILE_H=$(KERNEL_SOURCES)/include/generated/compile.h
KERNEL_COMPILE_H_EXIT=$(shell if [ -f $(KERNEL_COMPILE_H) ]; then echo 1; else echo 0; fi)

ifeq ($(KERNEL_COMPILE_H_EXIT), 1)

KERNEL_BUILT_GCC_STRING=$(shell cat ${KERNEL_COMPILE_H} | grep LINUX_COMPILER | cut -f 2 -d '"')
KERNEL_BUILT_GCC_VERSION=$(shell echo "${KERNEL_BUILT_GCC_STRING}" | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n 1)
ifeq ("$(KERNEL_BUILT_GCC_VERSION)", "")
KERNEL_BUILT_GCC_VERSION=$(shell echo "${KERNEL_BUILT_GCC_STRING}" | grep -o '[0-9]\+\.[0-9]\+' | head -n 1)
endif

SYSTEM_GCC_VERSION=$(shell $(CC) -v 2>&1 | awk 'END{print}' | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -n 1)
ifeq ("$(SYSTEM_GCC_VERSION)", "")
SYSTEM_GCC_VERSION=$(shell $(CC) -v 2>&1 | awk 'END{print}' | grep -o '[0-9]\+\.[0-9]\+' | head -n 1)
endif

ifneq ("$(KERNEL_BUILT_GCC_VERSION)", "$(SYSTEM_GCC_VERSION)")
$(warning "Kernel Built GCC Version ($(KERNEL_BUILT_GCC_VERSION)) Are Differ From System GCC Version($(SYSTEM_GCC_VERSION))!!")
$(warning "System GCC Version Must Match To Kernel Built GCC Version!!")
$(warning "Please Check GCC Version!!")
endif

else
$(warning "$(KERNEL_COMPILE_H) not exist,can not do gcc version check,skip")
endif

endif

ifeq ($(DEBUG), 1)
	DEBUG_FLAGS_LINUX = -ggdb3 -O2 -D_DEBUG_ -DDEBUG_LINUX
	DEBUG_FLAGS_CORE = -ggdb3 -O2 -D_DEBUG_ -DDEBUG_CORE
else
	DEBUG_FLAGS_LINUX += -O2 -fno-strict-aliasing -DRELEASE_LINUX
	DEBUG_FLAGS_CORE += -O2 -fno-strict-aliasing -DRELEASE_CORE
endif

ccflags-y += -DZX_TRACE_EVENT

ifeq ($(VIDEO_ONLY_FPGA), 1)
ccflags-y += -DVIDEO_ONLY_FPGA
endif

ifeq ($(RUN_HW_NULL), 1)
ccflags-y += -DZX_HW_NULL
else
ccflags-y += -DZX_PCIE_BUS
endif

ccflags-y += -I$(src)

ifeq ("$(M)", "")
ifeq ("$(O)", "")
GFGPU_FULL_PATH=$(src)
else
GFGPU_FULL_PATH=$(srctree)/$(src)
endif
else
GFGPU_FULL_PATH=$(src)
endif

include $(GFGPU_FULL_PATH)/core/Makefile
include $(GFGPU_FULL_PATH)/cbios/cbios.mk
include $(GFGPU_FULL_PATH)/linux/Makefile
obj-$(CONFIG_DRM_CX4) := $(DRIVER_NAME).o
