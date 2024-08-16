/*
 * Copyright Eugenio Paolantonio (g7) <me@medesimo.eu>
 * Copyright Cedric Bellegarde <cedric.bellegarde@adishatz.org>
 * Copyright Deepak Kumar <notwho53@gmail.com>
 */

#define G_LOG_DOMAIN "gestures-generic"

#include <unistd.h>

#include "gestures.h"
#include "gestures-generic.h"

#include "../common/utils.h"

struct _GesturesGeneric
{
    GObject parent_instance;

    gboolean double_tap_supported;
    gboolean double_tap_enabled;
    char *touchpanel_dt2w_node;
};

typedef enum {
    GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED = 1,
    GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_ENABLED,
    GESTURES_GENERIC_PROP_DOUBLE_TAP_NODE_PATH,
    GESTURES_GENERIC_PROP_LAST
} GesturesGenericProperty;

static void gestures_generic_interface_init (GesturesInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GesturesGeneric, gestures_generic, G_TYPE_OBJECT,
                            G_IMPLEMENT_INTERFACE (TYPE_GESTURES,
                                gestures_generic_interface_init))

static void
set_double_tap (GesturesGeneric *self, gboolean double_tap)
{
    g_autofree gchar *value = NULL;

    value = g_strdup_printf ("%d", double_tap);
    g_debug ("Changing double_tap setting: %d", double_tap);

    if (self->touchpanel_dt2w_node) {
        write_to_file (self->touchpanel_dt2w_node, value);
    }
}

static void
gestures_generic_constructed (GObject *obj)
{
    GesturesGeneric *self = GESTURES_GENERIC (obj);

    G_OBJECT_CLASS (gestures_generic_parent_class)->constructed (obj);

    self->double_tap_supported = (self->touchpanel_dt2w_node != NULL);
    self->double_tap_enabled = FALSE;
}

static void
gestures_generic_dispose (GObject *obj)
{
    GesturesGeneric *self = GESTURES_GENERIC (obj);

    g_free(self->touchpanel_dt2w_node);
    self->touchpanel_dt2w_node = NULL;

    G_OBJECT_CLASS (gestures_generic_parent_class)->dispose (obj);
}

static void
gestures_generic_set_property (GObject      *obj,
                              uint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    GesturesGeneric *self = GESTURES_GENERIC (obj);
    gboolean bool_value;

    switch ((GesturesGenericProperty) property_id)
        {
        case GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_ENABLED:
            bool_value =  g_value_get_boolean (value);
            set_double_tap (self, bool_value);
            self->double_tap_enabled = bool_value;
            break;

        case GESTURES_GENERIC_PROP_DOUBLE_TAP_NODE_PATH:
            g_free(self->touchpanel_dt2w_node);
            self->touchpanel_dt2w_node = g_strdup(g_value_get_string (value));
            self->double_tap_supported = (self->touchpanel_dt2w_node != NULL);
            break;

        case GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED:
        case GESTURES_GENERIC_PROP_LAST:
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, property_id, pspec);
            break;
        }

}

static void
gestures_generic_get_property (GObject    *obj,
                              uint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
    GesturesGeneric *self = GESTURES_GENERIC (obj);

    switch ((GesturesGenericProperty) property_id)
        {
        case GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED:
            g_value_set_boolean (value, self->double_tap_supported);
            break;

        case GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_ENABLED:
            g_value_set_boolean (value, self->double_tap_enabled);
            break;

        case GESTURES_GENERIC_PROP_DOUBLE_TAP_NODE_PATH:
            g_value_set_string (value, self->touchpanel_dt2w_node);
            break;

        case GESTURES_GENERIC_PROP_LAST:
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, property_id, pspec);
            break;
        }

}

static void
gestures_generic_class_init (GesturesGenericClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->constructed  = gestures_generic_constructed;
    object_class->dispose      = gestures_generic_dispose;
    object_class->set_property = gestures_generic_set_property;
    object_class->get_property = gestures_generic_get_property;

    g_object_class_override_property (object_class, GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED,
                                                                        "double-tap-to-wake-supported");
    g_object_class_override_property (object_class, GESTURES_GENERIC_PROP_DOUBLE_TAP_TO_WAKE_ENABLED,
                                                                        "double-tap-to-wake-enabled");
    g_object_class_override_property (object_class, GESTURES_GENERIC_PROP_DOUBLE_TAP_NODE_PATH,
                                                                        "touchpanel-dt2w-node");
}

static void
gestures_generic_interface_init (GesturesInterface *iface)
{}

static void
gestures_generic_init (GesturesGeneric *self)
{
    self->touchpanel_dt2w_node = NULL;
}

GesturesGeneric *
gestures_generic_new (void)
{
    return g_object_new (TYPE_GESTURES_GENERIC, NULL);
}

gboolean
gestures_generic_supported (void)
{
    GesturesGeneric *self = gestures_generic_new();
    gboolean supported = (self->touchpanel_dt2w_node != NULL);
    g_object_unref(self);
    return supported;
}
