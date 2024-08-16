/*
 * Copyright Eugenio Paolantonio (g7) <me@medesimo.eu>
 */

#define G_LOG_DOMAIN "gestures-interface"

#include "gestures.h"

G_DEFINE_INTERFACE (Gestures, gestures, G_TYPE_OBJECT)

static void
gestures_default_init (GesturesInterface *iface)
{
    g_object_interface_install_property (iface,
                                         g_param_spec_boolean ("double-tap-to-wake-supported",
                                                               "double-tap-to-wake-supported",
                                                               "Whether the device supports double tap to wake",
                                                               FALSE,
                                                               G_PARAM_READABLE));
    g_object_interface_install_property (iface,
                                         g_param_spec_boolean ("double-tap-to-wake-enabled",
                                                               "double-tap-to-wake-enabled",
                                                               "Whether double tap to wake is enabled",
                                                               FALSE,
                                                               G_PARAM_READABLE | G_PARAM_WRITABLE));
    g_object_interface_install_property (iface,
                                         g_param_spec_string ("touchpanel-dt2w-node",
                                                              "touchpanel-dt2w-node",
                                                              "Node Path",
                                                              NULL,
                                                              G_PARAM_READABLE | G_PARAM_WRITABLE));
}
