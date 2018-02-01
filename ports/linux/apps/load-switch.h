#ifndef __LOAD_SWITCH__
#define __LOAD_SWITCH__

#define DEV_NUM_CHANNELS 16

#define MC_IP        "239.0.0.1"
#define MC_PORT      2183

#define TOPIC_STATUS "load/switch/status"
#define TOPIC_SET    "load/switch/set"
#define TOPIC_GET    "load/switch/get"

struct s_channels {
        float level;
        float current;
        float sense;
        char label[32];
};

#endif
