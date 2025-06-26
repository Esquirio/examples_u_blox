# idf_component_register(SRCS "itwt.c"
#                     INCLUDE_DIRS ".")


idf_component_register(SRCS "itwt.c"
                            "tcp_client.c"
                            "timer.c"
                    INCLUDE_DIRS "include")