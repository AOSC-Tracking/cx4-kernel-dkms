ccflags-y += \
    -I${GFGPU_FULL_PATH}/core/include \
    -I${GFGPU_FULL_PATH}/core/context \
    -I${GFGPU_FULL_PATH}/core/global \
    -I${GFGPU_FULL_PATH}/core/powermgr \
    -I${GFGPU_FULL_PATH}/core/vidmm \
    -I${GFGPU_FULL_PATH}/core/vidsch \
    -I${GFGPU_FULL_PATH}/core/util \
    -I${GFGPU_FULL_PATH}/core/perfevent \
    -I${GFGPU_FULL_PATH}/shared

CORE_OBJ := \
	kernel_interface.o \
	mock_test.o

UTIL_OBJ := \
	util/util.o \
	util/ring_buffer.o

VIDMM_OBJ := \
    vidmm/vidmm.o \
    vidmm/vidmm_vm.o \
    vidmm/vidmm_allocate.o \
    vidmm/vidmm_paging.o \
    vidmm/vidmm_lock.o

VIDSCH_OBJ := \
	vidsch/vidsch.o \
	vidsch/vidsch_daemon_thread.o \
	vidsch/vidsch_workerthread.o \
	vidsch/vidsch_task.o \
	vidsch/vidsch_batch.o \
	vidsch/vidsch_render.o \
	vidsch/vidsch_submit.o \
	vidsch/vidsch_vm_id.o  \
	vidsch/vidsch_paging.o \
	vidsch/vidsch_vm_update.o \
	vidsch/vidsch_task_queue.o \
	vidsch/vidsch_trace.o \
	vidsch/vidsch_gating.o

PERF_EVENT_OBJ := \
	perfevent/perfevent.o

GLOBAL_OBJ := global/global.o

CONTEXT_OBJ := \
	context/context.o

POWERMGR_OBJ := powermgr/powermgr.o

core-objs := \
	$(UTIL_OBJ) \
	$(SHAREDAREA_OBJ) \
	$(GLOBAL_OBJ) \
	$(CONTEXT_OBJ) \
	$(POWERMGR_OBJ) \
	$(VIDMM_OBJ) \
	$(VIDSCH_OBJ) \
	$(SOFT_FLOAT_OBJ) \
	$(CORE_OBJ) \
	$(PERF_EVENT_OBJ)

$(DRIVER_NAME)-objs += $(addprefix core/, $(core-objs))

include $(GFGPU_FULL_PATH)/core/e3k/Makefile

$(foreach f, $(notdir ${core-objs}), $(eval CFLAGS_$f:=${DEBUG_FLAGS_CORE}))
