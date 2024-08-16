/*
 * Copyright Eugenio Paolantonio (g7) <me@medesimo.eu>
 * Copyright Deepak Kumar <notwho53@gmail.com>
 */

#ifndef GESTURESFOCALTECH_H
#define GESTURESFOCALTECH_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define TYPE_GESTURES_FOCALTECH gestures_focaltech_get_type ()
G_DECLARE_FINAL_TYPE (GesturesFocaltech, gestures_focaltech, GESTURES, FOCALTECH, GObject)

GesturesFocaltech *gestures_focaltech_new (void);
gboolean gestures_focaltech_supported (void);

G_END_DECLS

#endif /* GESTURESFOCALTECH_H */
