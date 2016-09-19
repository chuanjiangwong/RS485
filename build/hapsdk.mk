

# This variable holds path to example code which
# illustrates features exposed by SDK
#b-examples-path-y := app/test

include build/sdk_common.mk



global-cflags-y +=

######### SDK Version
SDK_VERSION_INTERNAL ?= 1.0.r4
SDK_VERSION :=\"$(SDK_VERSION_INTERNAL)$(EXTRA_SDK_VERSION)\"
global-cflags-y += -DSDK_VERSION=$(SDK_VERSION) -Isdk/include

# Default defconfig
b-defconfig-y := defconfig
# Path for searching SDK configuration files
b-sdkconfig-dir-y := sdk/config


include build/config.mk
