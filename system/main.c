/*
 * Copyright Cedric Bellegarde <cedric.bellegarde@adishatz.org>
 */

#include "config.h"

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <stdlib.h>

#include "bus.h"

static GMainLoop *loop;

static void
sigint_handler(int dummy) {
    g_main_loop_quit (loop);
}

gint
main (gint argc, gchar * argv[])
{
    GObject *bus;
    GResource *resource;
    g_autoptr (GOptionContext) context = NULL;
    g_autoptr (GError) error = NULL;
    gboolean version = FALSE;
    GOptionEntry main_entries[] = {
        {"version", 0, 0, G_OPTION_ARG_NONE, &version, "Show version"},
        {NULL}
    };

    signal(SIGINT, sigint_handler);

    context = g_option_context_new ("Mobile Power Saver");
    g_option_context_add_main_entries (context, main_entries, NULL);

    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_printerr ("%s\n", error->message);
        return EXIT_FAILURE;
    }

    if (version) {
        g_printerr ("%s\n", PACKAGE_VERSION);
        return EXIT_SUCCESS;
    }

    resource = g_resource_load (DROIDIAN_SETTINGSRESOURCES, NULL);
    g_resources_register (resource);

    bus = bus_new ();

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

    g_clear_pointer (&loop, g_main_loop_unref);
    g_clear_object (&bus);

    return EXIT_SUCCESS;
}
