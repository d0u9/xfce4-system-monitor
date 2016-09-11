#ifndef __CONFIG_H
#define __CONFIG_H


#define VERSION_MAJOR           0
#define VERSION_MINOR           1

#define TEXT_VERSION            TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR)

#define TEXT_PROGRAM_NAME       "xfce4-system-monitor-plugin"
#define TEXT_WEBSITE            "https://www.github.com/d0u9/xfce4-system-monitor"

#define TEXT_COMMENT            "Monitor CPU usage, link speed, CPU temperature, etc."
#define TEXT_COPYRIGHT          "Copyright (c) 2016-2016\n"

#define AUTHOR1                 "Douglas Su, <d0u9.su@outlook.com>"

#define TEXT_LICENSE            "This program is licensed under a Creative Commons"     \
                                "Attribution-ShareAlike 4.0 International License.\n"   \
                                "\n"                                                    \
                                "You are free to:\n"                                    \
                                "Share - copy and redistribute the material in any"     \
                                "medium or format.\n"                                   \
                                "Adapt - remix, transform, and build upon the material" \
                                "for any purpose, even commercially.\n"                 \
                                "\n"                                                    \
                                "For more details about the license, please check: "    \
                                "https://creativecommons.org/licenses/by-sa/4.0/\n"     \
                                "\n"


/* auxiliary macros */
#define STRINGFY(x)     #x
#define TOSTRING(x)     STRINGFY(x)


#endif

