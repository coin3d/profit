TARGET             = profit
TEMPLATE	   = lib
win32:INCLUDEPATH += .;..\include;
DEFINES           += HAVE_CONFIG_H
win32:CONFIG      += win_crt_mt

HEADERS            = ../include/profit/callback.h \
                     ../include/profit/debug.h \
                     ../include/profit/material.h \
                     ../include/profit/messages.h \
                     ../include/profit/model.h \
                     ../include/profit/node.h \
                     ../include/profit/nodeinfo.h \
                     ../include/profit/nodes/color_palette.h \
                     ../include/profit/nodes/face.h \
                     ../include/profit/nodes/group.h \
                     ../include/profit/nodes/header.h \
                     ../include/profit/nodes/instance_definition.h \
                     ../include/profit/nodes/instance_reference.h \
                     ../include/profit/nodes/material_palette.h \
                     ../include/profit/nodes/matrix.h \
                     ../include/profit/nodes/morph_vertex_list.h \
                     ../include/profit/nodes/object.h \
                     ../include/profit/nodes/obsolete_vertex_with_color.h \
                     ../include/profit/nodes/obsolete_vertex_with_normal.h \
                     ../include/profit/nodes/pop_attribute.h \
                     ../include/profit/nodes/pop_extension.h \
                     ../include/profit/nodes/pop_level.h \
                     ../include/profit/nodes/pop_subface.h \
                     ../include/profit/nodes/push_attribute.h \
                     ../include/profit/nodes/push_extension.h \
                     ../include/profit/nodes/push_level.h \
                     ../include/profit/nodes/push_subface.h \
                     ../include/profit/nodes/vertex_list.h \
                     ../include/profit/nodes/vertex_palette.h \
                     ../include/profit/nodes/vertex_with_color.h \
                     ../include/profit/nodes/vertex_with_normal.h \
                     ../include/profit/nodes/vertex_with_normal_and_texture.h \
                     ../include/profit/nodes/vertex_with_texture.h \
                     ../include/profit/profit.h \
                     ../include/profit/state.h \
                     ../include/profit/texture.h \
                     ../include/profit/util/array.h \
                     ../include/profit/util/bfile.h \
                     ../include/profit/util/mempool.h \
                     ../include/profit/vertex.h

SOURCES            = ../src/basics.c \ 
                     ../src/cruft.c \ 
                     ../src/debug.c \
                     ../src/messages.c \
                     ../src/model.c \
                     ../src/node.c \
                     ../src/nodeinfo.c \
                     ../src/nodes/color_palette.c \
                     ../src/nodes/face.c \
                     ../src/nodes/group.c \
                     ../src/nodes/header.c \
                     ../src/nodes/instance_definition.c \
                     ../src/nodes/instance_reference.c \
                     ../src/nodes/material.c \
                     ../src/nodes/material_palette.c \
                     ../src/nodes/matrix.c \
                     ../src/nodes/morph_vertex_list.c \
                     ../src/nodes/object.c \
                     ../src/nodes/obsolete_vertex_with_color.c \
                     ../src/nodes/obsolete_vertex_with_normal.c \
                     ../src/nodes/pop_attribute.c \
                     ../src/nodes/pop_extension.c \
                     ../src/nodes/pop_level.c \
                     ../src/nodes/pop_subface.c \
                     ../src/nodes/push_attribute.c \
                     ../src/nodes/push_extension.c \
                     ../src/nodes/push_level.c \
                     ../src/nodes/push_subface.c \
                     ../src/nodes/texture.c \
                     ../src/nodes/vertex_list.c \
                     ../src/nodes/vertex_palette.c \
                     ../src/nodes/vertex_with_color.c \
                     ../src/nodes/vertex_with_normal.c \
                     ../src/nodes/vertex_with_normal_and_texture.c \
                     ../src/nodes/vertex_with_texture.c \
                     ../src/profit.c \
                     ../src/state.c \
                     ../src/util/array.c \
                     ../src/util/bfile.c \
                     ../src/util/mempool.c \
                     ../src/vertex.c
