rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
src := $(call rwildcard,./,*.cpp)

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lzmq -pthread -lv8

INTELMAC_INCLUDE=-I/usr/local/include							# Intel mac
APPLESILICON_INCLUDE=-I/opt/homebrew/include					# Apple Silicon
UBUNTU_APPLESILICON_INCLUDE=-I/usr/include -I/usr/include/v8	# Apple Silicon Ubuntu VM
UBUNTU_INTEL_INCLUDE=-I/usr/include -I/usr/include/v8			# Intel Ubuntu VM

INTELMAC_LIB=-L/usr/local/lib									# Intel mac
APPLESILICON_LIB=-L/opt/homebrew/lib							# Apple Silicon
UBUNTU_APPLESILICON_LIB=-L/usr/lib/aarch64-linux-gnu			# Apple Silicon Ubuntu VM
UBUNTU_INTEL_LIB=-L/usr/lib/x86_64-linux-gnu					# Intel Ubuntu VM

MACOS_INCLUDE=$(APPLESILICON_INCLUDE)
MACOS_LIB=$(APPLESILICON_LIB)
UBUNTU_INCLUDE=$(UBUNTU_APPLESILICON_INCLUDE)
UBUNTU_LIB=$(UBUNTU_APPLESILICON_LIB)

MACOS_COMPILER=/usr/bin/clang++ -std=c++17
UBUNTU_COMPILER=/usr/bin/g++ -std=c++17

all: server client

uname_s := $(shell uname -s)
server: server.o
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

uname_s := $(shell uname -s)
client: client.o platform.o player.o timeline.o gameObject.o eventManager.o timeListener.o ScriptManager.o v8helpers.o scriptListener.o killPlane.o movingCamera.o movingCameraBounds.o movingPlatform.o movingPlatformVector.o
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

uname_s := $(shell uname -s)
server.o: server.cpp typedefs.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
client.o: client.cpp player.h platform.h timeline.h gameObject.h typedefs.h eventManager.h timeListener.h ScriptManager.h scriptListener.h killPlane.h movingCamera.h movingCameraBounds.h movingPlatform.h movingPlatformVector.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
platform.o: platform.cpp platform.h player.h gameObject.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
player.o: player.cpp player.h gameObject.h eventManager.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
timeline.o: timeline.cpp timeline.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
gameObject.o: gameObject.cpp gameObject.h v8helpers.h ScriptManager.h typedefs.h timeline.h player.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
eventManager.o: eventManager.cpp eventManager.h gameObject.h timeline.h typedefs.h v8helpers.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
timeListener.o: timeListener.cpp timeListener.h gameObject.h timeline.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
ScriptManager.o: ScriptManager.cpp ScriptManager.h v8helpers.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
v8helpers.o: v8helpers.cpp v8helpers.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
scriptListener.o: scriptListener.cpp scriptListener.h gameObject.h ScriptManager.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ $(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
movingPlatform.o: movingPlatform.cpp movingPlatform.h platform.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
movingPlatformVector.o: movingPlatformVector.cpp movingPlatformVector.h platform.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
killPlane.o: killPlane.cpp killPlane.h gameObject.h player.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
movingCamera.o: movingCamera.cpp movingCamera.h movingCameraBounds.h gameObject.h player.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(UBUNTU_INCLUDE)
endif

uname_s := $(shell uname -s)
movingCameraBounds.o: movingCameraBounds.cpp movingCameraBounds.h movingCamera.h platform.h gameObject.h player.h
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -c $(@:.o=.cpp) -o $@ -I$(UBUNTU_INCLUDE)
endif

.PHONY: clean
clean:
	rm -f $(patsubst %.cpp,%.o,$(src)) client server