$(MAKECMDGOALS) : run_make

.PHONY: run_make

current_dir := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SPARKDIR :=  $(realpath $(current_dir)/../platform/spark/device-os)

ifndef APP
	APP=brewblox
endif

ifeq ($(APP), brewblox)
	APP_MAKEFILE := app/brewblox-particle/build.mk
	APPNAME:=brewblox
else
	APP_MAKEFILE := app/$(APP)/build.mk
	APPNAME:=$(APP)
endif

APPDIR := $(realpath $(current_dir)/..) # repo root
WARNINGS_AS_ERRORS ?=n
USE_PRINTF_FLOAT ?=n

# using both APP and APPDIR causes problems in Particle's makefiles
override undefine APP

run_make:
	@echo "Building app $(APPNAME) from $(APP_MAKEFILE)"
	@$(MAKE) -s -C $(SPARKDIR)/main \
	APPDIR=$(APPDIR) \
	USER_MAKEFILE=$(APP_MAKEFILE) \
	USE_PRINTF_FLOAT=$(USE_PRINTF_FLOAT) \
	WARNINGS_AS_ERRORS=$(WARNINGS_AS_ERRORS) \
	TARGET_FILE=$(APPNAME)-$(PLATFORM) \
	TARGET_DIR=$(current_dir)/target/$(APPNAME)-$(PLATFORM) \
	BUILD_PATH_BASE=$(current_dir)/target \
	PARTICLE_DEVELOP=y \
	$(MAKEOVERRIDES) $(MAKECMDGOALS)
