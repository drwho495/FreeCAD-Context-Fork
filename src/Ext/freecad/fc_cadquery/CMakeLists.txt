# WARNING!!! DO NOT include cqgi.py, as it allows running arbitrary python code!
SET(FC_CADQUERY_SRCS
    __init__.py
    cq.py
    selectors.py
)

ADD_CUSTOM_TARGET(fc_cadquery_COPY_SOURCE ALL
    SOURCES ${FC_CADQUERY_SRCS})

fc_copy_sources(fc_cadquery_COPY_SOURCE
    "${CMAKE_BINARY_DIR}/Ext/freecad/fc_cadquery" ${FC_CADQUERY_SRCS})

INSTALL(FILES ${FC_CADQUERY_SRCS} DESTINATION
    "${SITE_PACKAGE_DIR}/fc_cadquery")

SET(FC_CADQUERY_IMPL_SRCS
    freecad_impl/__init__.py
    freecad_impl/exporters.py
    freecad_impl/console_logging.py
    freecad_impl/geom.py
    freecad_impl/importers.py
    freecad_impl/shapes.py
)

ADD_CUSTOM_TARGET(fc_cadquery_impl_COPY_SOURCE ALL
    SOURCES ${FC_CADQUERY_IMPL_SRCS})

fc_copy_sources(fc_cadquery_impl_COPY_SOURCE
    "${CMAKE_BINARY_DIR}/Ext/freecad/fc_cadquery" ${FC_CADQUERY_IMPL_SRCS})

INSTALL(FILES ${FC_CADQUERY_IMPL_SRCS} DESTINATION
    "${SITE_PACKAGE_DIR}/fc_cadquery/freecad_impl")
