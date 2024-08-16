/*
 * Copyright Eugenio Paolantonio (g7) <me@medesimo.eu>
 * Copyright Deepak Kumar <notwho53@gmail.com>
 */

#ifndef GESTURESGENERIC_H
#define GESTURESGENERIC_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_GESTURES_GENERIC gestures_generic_get_type ()
G_DECLARE_FINAL_TYPE (GesturesGeneric, gestures_generic, GESTURES, GENERIC, GObject)

GesturesGeneric *gestures_generic_new (void);
gboolean gestures_generic_supported (void);

G_END_DECLS

#endif /* GESTURESGENERIC_H */
