#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <unordered_map>

namespace api
{
    enum class Event
    {
        NO_EVENT,

        FINISHED,
        FAILURE,
        TIME_OUT,

        BUTTON_STANDSTILL,
        BUTTON_WALK,
        BUTTON_WALK_ASSIST, 
        BUTTON_TURN_AROUND,
        BUTTON_REPOSITIONING,

        BUTTON_STAND_TO_SIT, 
        BUTTON_SITTING,      
        BUTTON_TRANSFER,     
        BUTTON_PAUSE,           
        
        TURNAROUND_LEFT,     
        TURNAROUND_RIGHT,    
        REPOSITIONING_RIGHT, 
        REPOSITIONING_LEFT,  
        REPOSITIONING_BACK,  

        WALK_FAILURE,        
        UNRECOVERABLE_ERROR, 
        ALERT_ERROR,         
        EMERGENCY_ERROR,     

        PAUSE_SITTING,       
        PAUSE_STANDING,      
        PAUSE,               

        N_EVENTS
    };


    /// \brief Convert the Event enum to a string (for display purpose).
    std::string const& toString(Event event);

    struct KeyMap
    {
        std::string keyString;
        Event event;
    };

    std::unordered_map<std::string, Event> const KEY_MAPPING =
    {
        { "hlc_finished",                  Event::FINISHED                     },
        { "hlc_failure",                   Event::FAILURE                      },

        { "time_out",                      Event::TIME_OUT                         },

        { "remote_standstill",             Event::BUTTON_STANDSTILL                },
        { "remote_repositioning",          Event::BUTTON_REPOSITIONING              },
        { "remote_turn_around",            Event::BUTTON_TURN_AROUND               },
        { "remote_walk",                   Event::BUTTON_WALK                      },
        { "physio_stand_to_sit",           Event::BUTTON_STAND_TO_SIT              },
        { "physio_sitting",                Event::BUTTON_SITTING                   },
        { "physio_pause",                  Event::BUTTON_PAUSE                     },
        { "physio_transfer",               Event::BUTTON_TRANSFER                  },

        { "turnaround_left",               Event::TURNAROUND_LEFT                  },
        { "turnaround_right",              Event::TURNAROUND_RIGHT                 },

        { "repositioning_left",            Event::REPOSITIONING_LEFT               },
        { "repositioning_right",           Event::REPOSITIONING_RIGHT              },
        { "repositioning_back",            Event::REPOSITIONING_BACK               },

        { "unrecoverable_error",           Event::UNRECOVERABLE_ERROR              },
        { "alert_error",                   Event::ALERT_ERROR                      },
        { "emergency_error",               Event::EMERGENCY_ERROR                  },
        { "walk_failure",                  Event::WALK_FAILURE                     },

        { "pause_sitting",                 Event::PAUSE_SITTING                    },
        { "pause_standing",                Event::PAUSE_STANDING                   },

        { "trigger_assisted_walk",         Event::BUTTON_WALK_ASSIST               },
        
    };
}

#endif
