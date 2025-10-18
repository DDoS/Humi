if(DEFINED ENV{OPENOCD_PATH})
    message(STATUS "Using openocd from \"$ENV{OPENOCD_PATH}\"")
    set(openocd_path $ENV{OPENOCD_PATH})
else()
    find_program(openocd_path NAMES openocd)
endif()

if(openocd_path)
    if(DEFINED ENV{OPENOCD_SEARCH_PATH})
        message(STATUS "Using openocd search path \"$ENV{OPENOCD_SEARCH_PATH}\"")
        set(openocd_search_path_arg "-s $ENV{OPENOCD_SEARCH_PATH}")
    else()
        set(openocd_search_path_arg "")
    endif()
endif()

function(picoprobe_add_flash_target target)
    if(openocd_path)
        add_custom_target(flash-${target} COMMAND "${openocd_path}" ${openocd_search_path_arg}
            -f interface/cmsis-dap.cfg -c "adapter speed 5000" -f target/rp2350.cfg
            -c "program $<TARGET_FILE:${target}> verify; reset halt; rp2350.dap.core1 arp_reset assert 0; rp2350.dap.core0 arp_reset assert 0; exit"
            DEPENDS ${target}
            VERBATIM)
    endif()
endfunction()

function(picoprobe_add_reset_target)
    if(openocd_path)
        add_custom_target(reset COMMAND "${openocd_path}" ${openocd_search_path_arg}
            -f interface/cmsis-dap.cfg -c "adapter speed 5000" -f target/rp2350.cfg
            -c "init; reset halt; rp2350.dap.core1 arp_reset assert 0; rp2350.dap.core0 arp_reset assert 0; exit")
    endif()
endfunction()

if(DEFINED ENV{PICOTOOL_PATH})
    message(STATUS "Using picotool from \"$ENV{PICOTOOL_PATH}\"")
    set(picotool_path $ENV{PICOTOOL_PATH})
else()
    find_program(picotool_path NAMES picotool)
endif()

function(picotool_add_load_target target)
    add_custom_target(load-${target} COMMAND "${picotool_path}" load -f "$<TARGET_FILE:${target}>"
        DEPENDS ${target}
        VERBATIM)
endfunction()
