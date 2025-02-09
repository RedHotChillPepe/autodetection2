#****************************************************************************
#
# Makefile for devsdk
#
# This is a GNU make (gmake) makefile
#****************************************************************************



export CROSS_COMPILE?=aarch64-v01c01-linux-musl-
	
export ARCH_UPPER?=3519dv5
GSF_ARCH  := GSF3519DV5_ARCH

export CC:=$(CROSS_COMPILE)gcc
export AR:=$(CROSS_COMPILE)ar
export TP:=$(CROSS_COMPILE)strip
export LD:=$(CROSS_COMPILE)gcc

DEBUG_CFLAGS     := -g
RELEASE_CFLAGS   := -Wno-unknown-pragmas -O3

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

LDFLAGS          := 

DEBUG_LDFLAGS    := -shared  -g 
RELEASE_LDFLAGS  := -shared

ifeq (YES, ${DEBUG})
	CFLAGS       += ${DEBUG_CFLAGS}
	CXXFLAGS     += ${DEBUG_CXXFLAGS}
	LDFLAGS      += ${DEBUG_LDFLAGS}
else
	CFLAGS       += ${RELEASE_CFLAGS}
	CXXFLAGS     += ${RELEASE_CXXFLAGS}
	LDFLAGS      += ${RELEASE_LDFLAGS}
endif

ifeq ($(GSF_VPSS_YUV_SUPP), 1)
	CFLAGS += -DGSF_VPSS_YUV_SUPP
endif

ifeq ($(GSF_OSD_FREETYPE_SUPP), 1)
	CFLAGS += -DGSF_OSD_FREETYPE_SUPP
endif
CFLAGS += -D$(GSF_ARCH)
#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************
CFLAGS   += ${DEFS}
CXXFLAGS += ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************

STALIB  := ./lib/static/libdevsdk.a
DYNLIB  := ./lib/shared/libdevsdk.so
DEMO    := test3

all: ${STALIB} ${DYNLIB} ${DEMO}

#****************************************************************************
# Source files
#****************************************************************************

SRCS:= $(shell ls src/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRCS))

DEMO_SRCS := ./demo/test.c
DEMO_OBJS := $(patsubst %.c, %.o, $(DEMO_SRCS))

#****************************************************************************
# Include paths
#****************************************************************************
INCS := -I./demo
INCS := -I./inc -I./src -I./hisi
INCS += -I./hisi/$(ARCH)/inc
INCS += -I./inc/freetype
INCS += -I./inc/fontlib

EXTRA_LIBS := -lpthread -ldl -lm



	EXTRA_LIBS += -L./hisi/3519dv5/3519dv5-lib\
			 -lprotobuf-c -lsvp_acl -lhi_mpi_km -lhi_mpi -lhi_mpi_ae -lhi_mpi_isp -lot_mpi_isp -lhi_mpi_awb -ldehaze -ldrc -lldci -lhi_mpi_ive -lhi_mpi_sysmem\
			 -lextend_stats -lbnr -lcalcflicker -lhi_ivs_md -lvoice_engine -lsecurec -lhi_tde -lhi_mpi_cipher -lhi_mpi_fisheye_calibrate \
				 -ldnvqe -lupvqe -lir_auto -lacs -lhi_mpi_otp

ifeq ($(GSF_CUSTOMER_SPEC_RU08), 0)
ifeq ($(GSF_OSD_FREETYPE_SUPP), 1)
	EXTRA_LIBS += ./hisi/3519dv5/3519dv5-lib/libfreetype.so.6
	EXTRA_LIBS += ./hisi/3519dv5/3519dv5-lib/libz.so.1
endif
endif

EXTRA_LIBS += -lgsffont -lgsfbase


#****************************************************************************
# Output
#****************************************************************************

${STALIB}: ${OBJS}
#	@echo "src: $(SRCS)"
#	@echo "obj: $(OBJS)"
	@mkdir -p ./lib/static/
	${AR} rs ${STALIB} ${OBJS}
${DYNLIB}: ${OBJS}
	@mkdir -p ./lib/shared/
	${LD} ${LDFLAGS} -o ${DYNLIB} ${OBJS}
ifneq (YES, ${DEBUG})
	$(TP) --remove-section=.note --remove-section=.comment $(DYNLIB)
endif
${DEMO}: 
	${CC} -o $@ $(DEMO_SRCS) $(INCS) $(STALIB) ${EXTRA_LIBS}
ifneq (YES, ${DEBUG})
	$(TP) --remove-section=.note --remove-section=.comment $(DEMO)
endif

#****************************************************************************
# common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	${CXX} -fPIC -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -fPIC -c ${CFLAGS} ${INCS} $< -o $@	

clean:
	rm -f ${DEMO} ${DEMO_OBJ} ${OBJS}		
	

