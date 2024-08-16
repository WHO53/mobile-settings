/*
 * Copyright Eugenio Paolantonio (g7) <me@medesimo.eu>
 * Copyright Cedric Bellegarde <cedric.bellegarde@adishatz.org>
 * Copyright Deepak Kumar <notwho53@gmail.com>
 */

#define G_LOG_DOMAIN "gestures-focaltech"

#include <unistd.h>
#include <stdio.h>
#include <libudev.h>
#include <limits.h>

#include "gestures.h"
#include "gestures-focaltech.h"

#include "../common/utils.h"

static char* find_fts_gesture_mode_node(void) {
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    char *path = NULL;
    char resolved_path[PATH_MAX];
    const char *syspath;
    const char *sysattr;

    udev = udev_new();
    if (!udev) {
        return NULL;
    }

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "i2c");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        syspath = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, syspath);

        sysattr = udev_device_get_sysattr_value(dev, "fts_gesture_mde");
        if (sysattr) {
            if (realpath(syspath, resolved_path) != NULL) {
                size_t full_path_len = strlen(resolved_path) + strlen("/fts_gesture_mode") + 1;
                path = malloc(full_path_len);
                if (path) {
                    snprintf(path, full_path_len, "%s/fts_gesture_mode", resolved_path);
                }
                udev_device_unref(dev);
                break;
            }
        }

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return path;
}

struct _GesturesFocaltech
{
    GObject parent_instance;

    gboolean double_tap_supported;
    gboolean double_tap_enabled;
    char *touchpanel_dt2w_node;
};

typedef enum {
    GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED = 1,
    GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_ENABLED,
    GESTURES_FOCALTECH_PROP_LAST
} GesturesFocaltechProperty;

static void gestures_focaltech_interface_init (GesturesInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GesturesFocaltech, gestures_focaltech, G_TYPE_OBJECT,
                            G_IMPLEMENT_INTERFACE (TYPE_GESTURES,
                                gestures_focaltech_interface_init))

static void
set_double_tap (GesturesFocaltech *self, gboolean double_tap)
{
    g_autofree gchar *value = NULL;

    value = g_strdup_printf ("%d", double_tap);
    g_debug ("Changing double_tap setting: %d", double_tap);

    if (self->touchpanel_dt2w_node) {
        write_to_file (self->touchpanel_dt2w_node, value);
    }
}

static void
gestures_focaltech_constructed (GObject *obj)
{
    GesturesFocaltech *self = GESTURES_FOCALTECH (obj);

    G_OBJECT_CLASS (gestures_focaltech_parent_class)->constructed (obj);

    self->touchpanel_dt2w_node = find_fts_gesture_mode_node();
    self->double_tap_supported = (self->touchpanel_dt2w_node != NULL);
    self->double_tap_enabled = FALSE;
}

static void
gestures_focaltech_dispose (GObject *obj)
{
    GesturesFocaltech *self = GESTURES_FOCALTECH (obj);

    g_free(self->touchpanel_dt2w_node);
    self->touchpanel_dt2w_node = NULL;

    G_OBJECT_CLASS (gestures_focaltech_parent_class)->dispose (obj);
}

static void
gestures_focaltech_set_property (GObject      *obj,
                              uint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    GesturesFocaltech *self = GESTURES_FOCALTECH (obj);
    gboolean bool_value;

    switch ((GesturesFocaltechProperty) property_id)
        {
        case GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_ENABLED:
            bool_value =  g_value_get_boolean (value);
            set_double_tap (self, bool_value);
            self->double_tap_enabled = bool_value;
            break;

        case GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED:
        case GESTURES_FOCALTECH_PROP_LAST:
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, property_id, pspec);
            break;
        }

}

static void
gestures_focaltech_get_property (GObject    *obj,
                              uint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
    GesturesFocaltech *self = GESTURES_FOCALTECH (obj);

    switch ((GesturesFocaltechProperty) property_id)
        {
        case GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED:
            g_value_set_boolean (value, self->double_tap_supported);
            break;

        case GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_ENABLED:
            g_value_set_boolean (value, self->double_tap_enabled);
            break;

        case GESTURES_FOCALTECH_PROP_LAST:
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, property_id, pspec);
            break;
        }

}

static void
gestures_focaltech_class_init (GesturesFocaltechClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->constructed  = gestures_focaltech_constructed;
    object_class->dispose      = gestures_focaltech_dispose;
    object_class->set_property = gestures_focaltech_set_property;
    object_class->get_property = gestures_focaltech_get_property;

    g_object_class_override_property (object_class, GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_SUPPORTED,
                                                                        "double-tap-to-wake-supported");
    g_object_class_override_property (object_class, GESTURES_FOCALTECH_PROP_DOUBLE_TAP_TO_WAKE_ENABLED,
                                                                        "double-tap-to-wake-enabled");
}

static void
gestures_focaltech_interface_init (GesturesInterface *iface)
{}

static void
gestures_focaltech_init (GesturesFocaltech *self)
{
    self->touchpanel_dt2w_node = NULL;
}

GesturesFocaltech *
gestures_focaltech_new (void)
{
    return g_object_new (TYPE_GESTURES_FOCALTECH, NULL);
}

gboolean
gestures_focaltech_supported (void)
{
    char *node_path = find_fts_gesture_mode_node();
    gboolean supported = (node_path != NULL);
    free(node_path);
    return supported;
}
