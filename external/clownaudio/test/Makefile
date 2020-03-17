ALL_CXXFLAGS = $(CXXFLAGS)
ALL_LIBS = $(LIBS)

ifeq ($(STATIC), 1)
  ALL_CXXFLAGS += $(shell pkg-config clownaudio --cflags --static) -static -DCLOWNAUDIO_STATIC_DEFINE
  ALL_LIBS += $(shell pkg-config clownaudio --libs --static)
else
  ALL_CXXFLAGS += $(shell pkg-config clownaudio --cflags)
  ALL_LIBS += $(shell pkg-config clownaudio --libs)
endif

test: test.cpp
	$(CXX) $(ALL_CXXFLAGS) -o $@ $^ $(LDFLAGS) $(ALL_LIBS)
