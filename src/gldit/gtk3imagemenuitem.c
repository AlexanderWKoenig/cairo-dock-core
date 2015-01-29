/* GTK - The GIMP Toolkit
 * Copyright (C) 2001 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

// Imported from gtk+-3.8.4
// removed stock features
// GtkImageMenuItem has been renamed to Gtk3ImageMenuItem (and all corresponding functions and macros) so that we can test it even when using GTK < 3.10, and also because GtkImageMenuItem is still present in GTK even if deprecated.


#include "gtk3imagemenuitem.h"

#include <string.h>  // strcmp
#define GTK_PARAM_READWRITE G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB  // from gtkprivate.h
#define GTK_PARAM_WRITABLE G_PARAM_WRITABLE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB  // from gtkprivate.h
#define P_(String) (String)  // from gtkintl.h


/**
 * SECTION:gtkimagemenuitem
 * @Short_description: A menu item with an icon
 * @Title: Gtk3ImageMenuItem
 *
 * A Gtk3ImageMenuItem is a menu item which has an icon next to the text label.
 *
 * Note that the user can disable display of menu icons, so make sure to still
 * fill in the text label.
 */


struct _Gtk3ImageMenuItemPrivate
{
  GtkWidget     *image;

  gchar         *label;
  guint          always_show_image : 1;
};

enum {
  PROP_0,
  PROP_IMAGE,
  PROP_ALWAYS_SHOW_IMAGE
};

static GtkActivatableIface *parent_activatable_iface;

static void gtk3_image_menu_item_destroy              (GtkWidget        *widget);
static void gtk3_image_menu_item_get_preferred_width  (GtkWidget        *widget,
                                                      gint             *minimum,
                                                      gint             *natural);
static void gtk3_image_menu_item_get_preferred_height (GtkWidget        *widget,
                                                      gint             *minimum,
                                                      gint             *natural);
static void gtk3_image_menu_item_get_preferred_height_for_width (GtkWidget *widget,
                                                                gint       width,
                                                                gint      *minimum,
                                                                gint      *natural);
static void gtk3_image_menu_item_size_allocate        (GtkWidget        *widget,
                                                      GtkAllocation    *allocation);
static void gtk3_image_menu_item_map                  (GtkWidget        *widget);
static void gtk3_image_menu_item_remove               (GtkContainer     *container,
                                                      GtkWidget        *child);
static void gtk3_image_menu_item_toggle_size_request  (GtkMenuItem      *menu_item,
                                                      gint             *requisition);
static void gtk3_image_menu_item_set_label            (GtkMenuItem      *menu_item,
                                                      const gchar      *label);
static const gchar * gtk3_image_menu_item_get_label   (GtkMenuItem *menu_item);

static void gtk3_image_menu_item_forall               (GtkContainer    *container,
                                                      gboolean         include_internals,
                                                      GtkCallback      callback,
                                                      gpointer         callback_data);

static void gtk3_image_menu_item_finalize             (GObject         *object);
static void gtk3_image_menu_item_set_property         (GObject         *object,
                                                      guint            prop_id,
                                                      const GValue    *value,
                                                      GParamSpec      *pspec);
static void gtk3_image_menu_item_get_property         (GObject         *object,
                                                      guint            prop_id,
                                                      GValue          *value,
                                                      GParamSpec      *pspec);

static void gtk3_image_menu_item_recalculate          (Gtk3ImageMenuItem *image_menu_item);

static void gtk3_image_menu_item_activatable_interface_init (GtkActivatableIface  *iface);
static void gtk3_image_menu_item_update                     (GtkActivatable       *activatable,
                                                            GtkAction            *action,
                                                            const gchar          *property_name);
static void gtk3_image_menu_item_sync_action_properties     (GtkActivatable       *activatable,
                                                            GtkAction            *action);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
G_DEFINE_TYPE_WITH_CODE (Gtk3ImageMenuItem, gtk3_image_menu_item, GTK_TYPE_MENU_ITEM,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_ACTIVATABLE,
                                                gtk3_image_menu_item_activatable_interface_init))
G_GNUC_END_IGNORE_DEPRECATIONS;

static void
gtk3_image_menu_item_class_init (Gtk3ImageMenuItemClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GtkWidgetClass *widget_class = (GtkWidgetClass*) klass;
  GtkMenuItemClass *menu_item_class = (GtkMenuItemClass*) klass;
  GtkContainerClass *container_class = (GtkContainerClass*) klass;

  widget_class->destroy = gtk3_image_menu_item_destroy;
  widget_class->screen_changed = NULL;
  widget_class->get_preferred_width = gtk3_image_menu_item_get_preferred_width;
  widget_class->get_preferred_height = gtk3_image_menu_item_get_preferred_height;
  widget_class->get_preferred_height_for_width = gtk3_image_menu_item_get_preferred_height_for_width;
  widget_class->size_allocate = gtk3_image_menu_item_size_allocate;
  widget_class->map = gtk3_image_menu_item_map;

  container_class->forall = gtk3_image_menu_item_forall;
  container_class->remove = gtk3_image_menu_item_remove;

  menu_item_class->toggle_size_request = gtk3_image_menu_item_toggle_size_request;
  menu_item_class->set_label           = gtk3_image_menu_item_set_label;
  menu_item_class->get_label           = gtk3_image_menu_item_get_label;

  gobject_class->finalize     = gtk3_image_menu_item_finalize;
  gobject_class->set_property = gtk3_image_menu_item_set_property;
  gobject_class->get_property = gtk3_image_menu_item_get_property;

  g_object_class_install_property (gobject_class,
                                   PROP_IMAGE,
                                   g_param_spec_object ("image",
                                                        P_("Image widget"),
                                                        P_("Child widget to appear next to the menu text"),
                                                        GTK_TYPE_WIDGET,
                                                        GTK_PARAM_READWRITE));

  /**
   * Gtk3ImageMenuItem:always-show-image:
   *
   * If %TRUE, the menu item will ignore the #GtkSettings:gtk-menu-images
   * setting and always show the image, if available.
   *
   * Use this property if the menuitem would be useless or hard to use
   * without the image.
   *
   * Since: 2.16
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ALWAYS_SHOW_IMAGE,
                                   g_param_spec_boolean ("always-show-image",
                                                         P_("Always show image"),
                                                         P_("Whether the image will always be shown"),
                                                         FALSE,
                                                         GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_type_class_add_private (klass, sizeof (Gtk3ImageMenuItemPrivate));
}

static void
gtk3_image_menu_item_init (Gtk3ImageMenuItem *image_menu_item)
{
  Gtk3ImageMenuItemPrivate *priv;

  image_menu_item->priv = G_TYPE_INSTANCE_GET_PRIVATE (image_menu_item,
                                                       GTK3_TYPE_IMAGE_MENU_ITEM,
                                                       Gtk3ImageMenuItemPrivate);
  priv = image_menu_item->priv;

  priv->image = NULL;
  priv->label  = NULL;
}

static void
gtk3_image_menu_item_finalize (GObject *object)
{
  Gtk3ImageMenuItemPrivate *priv = GTK3_IMAGE_MENU_ITEM (object)->priv;

  g_free (priv->label);
  priv->label  = NULL;

  G_OBJECT_CLASS (gtk3_image_menu_item_parent_class)->finalize (object);
}

static void
gtk3_image_menu_item_set_property (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (object);

  switch (prop_id)
    {
    case PROP_IMAGE:
      gtk3_image_menu_item_set_image (image_menu_item, (GtkWidget *) g_value_get_object (value));
      break;
    case PROP_ALWAYS_SHOW_IMAGE:
      gtk3_image_menu_item_set_always_show_image (image_menu_item, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk3_image_menu_item_get_property (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (object);

  switch (prop_id)
    {
    case PROP_IMAGE:
      g_value_set_object (value, gtk3_image_menu_item_get_image (image_menu_item));
      break;
    case PROP_ALWAYS_SHOW_IMAGE:
      g_value_set_boolean (value, gtk3_image_menu_item_get_always_show_image (image_menu_item));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

#if (CAIRO_DOCK_FORCE_ICON_IN_MENUS == 1)
#define show_image(image_menu_item) TRUE
#else
static gboolean
show_image (Gtk3ImageMenuItem *image_menu_item)
{
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;
  return priv->always_show_image;
}
#endif

static void
gtk3_image_menu_item_map (GtkWidget *widget)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (widget);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;

  GTK_WIDGET_CLASS (gtk3_image_menu_item_parent_class)->map (widget);

  if (priv->image)
    g_object_set (priv->image,
                  "visible", show_image (image_menu_item),
                  NULL);
}

static void
gtk3_image_menu_item_destroy (GtkWidget *widget)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (widget);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;

  if (priv->image)
    gtk_container_remove (GTK_CONTAINER (image_menu_item),
                          priv->image);

  GTK_WIDGET_CLASS (gtk3_image_menu_item_parent_class)->destroy (widget);
}

static void
gtk3_image_menu_item_toggle_size_request (GtkMenuItem *menu_item,
                                         gint        *requisition)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (menu_item);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkPackDirection pack_dir;
  GtkWidget *parent;
  GtkWidget *widget = GTK_WIDGET (menu_item);

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  *requisition = 0;

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      GtkRequisition image_requisition;
      guint toggle_spacing;

      gtk_widget_get_preferred_size (priv->image, &image_requisition, NULL);

      gtk_widget_style_get (GTK_WIDGET (menu_item),
                            "toggle-spacing", &toggle_spacing,
                            NULL);

      if (pack_dir == GTK_PACK_DIRECTION_LTR || pack_dir == GTK_PACK_DIRECTION_RTL)
        {
          if (image_requisition.width > 0)
            *requisition = image_requisition.width + toggle_spacing;
        }
      else
        {
          if (image_requisition.height > 0)
            *requisition = image_requisition.height + toggle_spacing;
        }
    }
}

static void
gtk3_image_menu_item_recalculate (Gtk3ImageMenuItem *image_menu_item)
{
  Gtk3ImageMenuItemPrivate    *priv = image_menu_item->priv;
  const gchar             *resolved_label = priv->label;

  GTK_MENU_ITEM_CLASS
    (gtk3_image_menu_item_parent_class)->set_label (GTK_MENU_ITEM (image_menu_item), resolved_label);

}

static void
gtk3_image_menu_item_set_label (GtkMenuItem      *menu_item,
                               const gchar      *label)
{
  Gtk3ImageMenuItemPrivate *priv = GTK3_IMAGE_MENU_ITEM (menu_item)->priv;

  if (priv->label != label)
    {
      g_free (priv->label);
      priv->label = g_strdup (label);

      gtk3_image_menu_item_recalculate (GTK3_IMAGE_MENU_ITEM (menu_item));

      g_object_notify (G_OBJECT (menu_item), "label");

    }
}

static const gchar *
gtk3_image_menu_item_get_label (GtkMenuItem *menu_item)
{
  Gtk3ImageMenuItemPrivate *priv = GTK3_IMAGE_MENU_ITEM (menu_item)->priv;

  return priv->label;
}

static void
gtk3_image_menu_item_get_preferred_width (GtkWidget        *widget,
                                         gint             *minimum,
                                         gint             *natural)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (widget);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  GTK_WIDGET_CLASS (gtk3_image_menu_item_parent_class)->get_preferred_width (widget, minimum, natural);

  if ((pack_dir == GTK_PACK_DIRECTION_TTB || pack_dir == GTK_PACK_DIRECTION_BTT) &&
      priv->image &&
      gtk_widget_get_visible (priv->image))
    {
      gint child_minimum, child_natural;

      gtk_widget_get_preferred_width (priv->image, &child_minimum, &child_natural);

      *minimum = MAX (*minimum, child_minimum);
      *natural = MAX (*natural, child_natural);
    }
}

static void
gtk3_image_menu_item_get_preferred_height (GtkWidget        *widget,
                                          gint             *minimum,
                                          gint             *natural)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (widget);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;
  gint child_height = 0;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      GtkRequisition child_requisition;

      gtk_widget_get_preferred_size (priv->image, &child_requisition, NULL);

      child_height = child_requisition.height;
    }

  GTK_WIDGET_CLASS (gtk3_image_menu_item_parent_class)->get_preferred_height (widget, minimum, natural);

  if (pack_dir == GTK_PACK_DIRECTION_RTL || pack_dir == GTK_PACK_DIRECTION_LTR)
    {
      *minimum = MAX (*minimum, child_height);
      *natural = MAX (*natural, child_height);
    }
}

static void
gtk3_image_menu_item_get_preferred_height_for_width (GtkWidget        *widget,
                                                    gint              width,
                                                    gint             *minimum,
                                                    gint             *natural)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (widget);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;
  gint child_height = 0;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      GtkRequisition child_requisition;

      gtk_widget_get_preferred_size (priv->image, &child_requisition, NULL);

      child_height = child_requisition.height;
    }

  GTK_WIDGET_CLASS
    (gtk3_image_menu_item_parent_class)->get_preferred_height_for_width (widget, width, minimum, natural);

  if (pack_dir == GTK_PACK_DIRECTION_RTL || pack_dir == GTK_PACK_DIRECTION_LTR)
    {
      *minimum = MAX (*minimum, child_height);
      *natural = MAX (*natural, child_height);
    }
}


static void
gtk3_image_menu_item_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (widget);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkAllocation widget_allocation;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  GTK_WIDGET_CLASS (gtk3_image_menu_item_parent_class)->size_allocate (widget, allocation);

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      gint x, y, offset;
      GtkStyleContext *context;
      GtkStateFlags state;
      GtkBorder padding;
      GtkRequisition child_requisition;
      GtkAllocation child_allocation;
      guint horizontal_padding, toggle_spacing;
      gint toggle_size;

      ///toggle_size = GTK_MENU_ITEM (image_menu_item)->priv->toggle_size;
      gtk_menu_item_toggle_size_request (GTK_MENU_ITEM (image_menu_item), &toggle_size);
      gtk_widget_style_get (widget,
                            "horizontal-padding", &horizontal_padding,
                            "toggle-spacing", &toggle_spacing,
                            NULL);

      /* Man this is lame hardcoding action, but I can't
       * come up with a solution that's really better.
       */

      gtk_widget_get_preferred_size (priv->image, &child_requisition, NULL);

      gtk_widget_get_allocation (widget, &widget_allocation);

      context = gtk_widget_get_style_context (widget);
      state = gtk_widget_get_state_flags (widget);
      gtk_style_context_get_padding (context, state, &padding);
      offset = gtk_container_get_border_width (GTK_CONTAINER (image_menu_item));

      if (pack_dir == GTK_PACK_DIRECTION_LTR ||
          pack_dir == GTK_PACK_DIRECTION_RTL)
        {
          if ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) ==
              (pack_dir == GTK_PACK_DIRECTION_LTR))
            x = offset + horizontal_padding + padding.left +
               (toggle_size - toggle_spacing - child_requisition.width) / 2;
          else
            x = widget_allocation.width - offset - horizontal_padding - padding.right -
              toggle_size + toggle_spacing +
              (toggle_size - toggle_spacing - child_requisition.width) / 2;

          y = (widget_allocation.height - child_requisition.height) / 2;
        }
      else
        {
          if ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) ==
              (pack_dir == GTK_PACK_DIRECTION_TTB))
            y = offset + horizontal_padding + padding.top +
              (toggle_size - toggle_spacing - child_requisition.height) / 2;
          else
            y = widget_allocation.height - offset - horizontal_padding - padding.bottom -
              toggle_size + toggle_spacing +
              (toggle_size - toggle_spacing - child_requisition.height) / 2;

          x = (widget_allocation.width - child_requisition.width) / 2;
        }

      child_allocation.width = child_requisition.width;
      child_allocation.height = child_requisition.height;
      child_allocation.x = widget_allocation.x + MAX (x, 0);
      child_allocation.y = widget_allocation.y + MAX (y, 0);

      gtk_widget_size_allocate (priv->image, &child_allocation);
    }
}

static void
gtk3_image_menu_item_forall (GtkContainer   *container,
                            gboolean        include_internals,
                            GtkCallback     callback,
                            gpointer        callback_data)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (container);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;

  GTK_CONTAINER_CLASS (gtk3_image_menu_item_parent_class)->forall (container,
                                                                  include_internals,
                                                                  callback,
                                                                  callback_data);

  if (include_internals && priv->image)
    (* callback) (priv->image, callback_data);
}


static void
gtk3_image_menu_item_activatable_interface_init (GtkActivatableIface  *iface)
{
  parent_activatable_iface = g_type_interface_peek_parent (iface);
  iface->update = gtk3_image_menu_item_update;
  iface->sync_action_properties = gtk3_image_menu_item_sync_action_properties;
}

static gboolean
activatable_update_gicon (Gtk3ImageMenuItem *image_menu_item, GtkAction *action)
{

  GtkWidget   *image;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  GIcon       *icon = gtk_action_get_gicon (action);
G_GNUC_END_IGNORE_DEPRECATIONS;
  image = gtk3_image_menu_item_get_image (image_menu_item);

  if (icon && GTK_IS_IMAGE (image))
    {
      gtk_image_set_from_gicon (GTK_IMAGE (image), icon, GTK_ICON_SIZE_MENU);
      gtk_image_set_pixel_size (GTK_IMAGE (image), GTK_ICON_SIZE_MENU); // force size
      return TRUE;
    }

  return FALSE;
}

static void
activatable_update_icon_name (Gtk3ImageMenuItem *image_menu_item, GtkAction *action)
{
  GtkWidget   *image;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  const gchar *icon_name = gtk_action_get_icon_name (action);
G_GNUC_END_IGNORE_DEPRECATIONS;

  image = gtk3_image_menu_item_get_image (image_menu_item);

  if (GTK_IS_IMAGE (image) &&
      (gtk_image_get_storage_type (GTK_IMAGE (image)) == GTK_IMAGE_EMPTY ||
       gtk_image_get_storage_type (GTK_IMAGE (image)) == GTK_IMAGE_ICON_NAME))
    {
      gtk_image_set_from_icon_name (GTK_IMAGE (image), icon_name, GTK_ICON_SIZE_MENU);
    }
}

static void
gtk3_image_menu_item_update (GtkActivatable *activatable,
                            GtkAction      *action,
                            const gchar    *property_name)
{
  Gtk3ImageMenuItem *image_menu_item;
  gboolean   use_appearance;
  image_menu_item = GTK3_IMAGE_MENU_ITEM (activatable);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  parent_activatable_iface->update (activatable, action, property_name);

  use_appearance = gtk_activatable_get_use_action_appearance (activatable);
G_GNUC_END_IGNORE_DEPRECATIONS;
  if (!use_appearance)
    return;

G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  if (strcmp (property_name, "gicon") == 0)
    activatable_update_gicon (image_menu_item, action);
  else if (strcmp (property_name, "icon-name") == 0)
    activatable_update_icon_name (image_menu_item, action);
G_GNUC_END_IGNORE_DEPRECATIONS;
}

static void
gtk3_image_menu_item_sync_action_properties (GtkActivatable *activatable,
                                            GtkAction      *action)
{
  Gtk3ImageMenuItem *image_menu_item;
  GtkWidget *image;
  gboolean   use_appearance;

  image_menu_item = GTK3_IMAGE_MENU_ITEM (activatable);

  parent_activatable_iface->sync_action_properties (activatable, action);

  if (!action)
    return;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  use_appearance = gtk_activatable_get_use_action_appearance (activatable);
G_GNUC_END_IGNORE_DEPRECATIONS;
  if (!use_appearance)
    return;

  image = gtk3_image_menu_item_get_image (image_menu_item);
  if (image && !GTK_IS_IMAGE (image))
    {
      gtk3_image_menu_item_set_image (image_menu_item, NULL);
      image = NULL;
    }

  if (!image)
    {
      image = gtk_image_new ();
      gtk_widget_show (image);
      gtk3_image_menu_item_set_image (GTK3_IMAGE_MENU_ITEM (activatable),
                                     image);
    }
G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
  if (!activatable_update_gicon (image_menu_item, action))
    activatable_update_icon_name (image_menu_item, action);

  gtk3_image_menu_item_set_always_show_image (image_menu_item,
                                             gtk_action_get_always_show_image (action));
G_GNUC_END_IGNORE_DEPRECATIONS;
}


/**
 * gtk3_image_menu_item_new:
 *
 * Creates a new #Gtk3ImageMenuItem with an empty label.
 *
 * Returns: a new #Gtk3ImageMenuItem
 */
GtkWidget*
gtk3_image_menu_item_new (void)
{
  return g_object_new (GTK3_TYPE_IMAGE_MENU_ITEM, NULL);
}

/**
 * gtk3_image_menu_item_new_with_label:
 * @label: the text of the menu item.
 *
 * Creates a new #Gtk3ImageMenuItem containing a label.
 *
 * Returns: a new #Gtk3ImageMenuItem.
 */
GtkWidget*
gtk3_image_menu_item_new_with_label (const gchar *label)
{
  return g_object_new (GTK3_TYPE_IMAGE_MENU_ITEM,
                       "label", label,
                       NULL);
}

/**
 * gtk3_image_menu_item_new_with_mnemonic:
 * @label: the text of the menu item, with an underscore in front of the
 *         mnemonic character
 *
 * Creates a new #Gtk3ImageMenuItem containing a label. The label
 * will be created using gtk_label_new_with_mnemonic(), so underscores
 * in @label indicate the mnemonic for the menu item.
 *
 * Returns: a new #Gtk3ImageMenuItem
 */
GtkWidget*
gtk3_image_menu_item_new_with_mnemonic (const gchar *label)
{
  return g_object_new (GTK3_TYPE_IMAGE_MENU_ITEM,
                       "use-underline", TRUE,
                       "label", label,
                       NULL);
}

/**
 * gtk3_image_menu_item_set_always_show_image:
 * @image_menu_item: a #Gtk3ImageMenuItem
 * @always_show: %TRUE if the menuitem should always show the image
 *
 * If %TRUE, the menu item will ignore the #GtkSettings:gtk-menu-images
 * setting and always show the image, if available.
 *
 * Use this property if the menuitem would be useless or hard to use
 * without the image.
 *
 * Since: 2.16
 */
void
gtk3_image_menu_item_set_always_show_image (Gtk3ImageMenuItem *image_menu_item,
                                           gboolean          always_show)
{
  Gtk3ImageMenuItemPrivate *priv;

  g_return_if_fail (GTK3_IS_IMAGE_MENU_ITEM (image_menu_item));

  priv = image_menu_item->priv;

  if (priv->always_show_image != always_show)
    {
      priv->always_show_image = always_show;

      if (priv->image)
        {
          if (show_image (image_menu_item))
            gtk_widget_show (priv->image);
          else
            gtk_widget_hide (priv->image);
        }

      g_object_notify (G_OBJECT (image_menu_item), "always-show-image");
    }
}

/**
 * gtk3_image_menu_item_get_always_show_image:
 * @image_menu_item: a #Gtk3ImageMenuItem
 *
 * Returns whether the menu item will ignore the #GtkSettings:gtk-menu-images
 * setting and always show the image, if available.
 *
 * Returns: %TRUE if the menu item will always show the image
 *
 * Since: 2.16
 */
gboolean
gtk3_image_menu_item_get_always_show_image (Gtk3ImageMenuItem *image_menu_item)
{
  g_return_val_if_fail (GTK3_IS_IMAGE_MENU_ITEM (image_menu_item), FALSE);

  return image_menu_item->priv->always_show_image;
}


/**
 * gtk3_image_menu_item_set_image:
 * @image_menu_item: a #Gtk3ImageMenuItem.
 * @image: (allow-none): a widget to set as the image for the menu item.
 *
 * Sets the image of @image_menu_item to the given widget.
 * Note that it depends on the show-menu-images setting whether
 * the image will be displayed or not.
 */
void
gtk3_image_menu_item_set_image (Gtk3ImageMenuItem *image_menu_item,
                               GtkWidget        *image)
{
  Gtk3ImageMenuItemPrivate *priv;

  g_return_if_fail (GTK3_IS_IMAGE_MENU_ITEM (image_menu_item));

  priv = image_menu_item->priv;

  if (image == priv->image)
    return;

  if (priv->image)
    gtk_container_remove (GTK_CONTAINER (image_menu_item),
                          priv->image);

  priv->image = image;

  if (image == NULL)
    return;

  gtk_widget_set_parent (image, GTK_WIDGET (image_menu_item));
  g_object_set (image,
                "visible", show_image (image_menu_item),
                "no-show-all", TRUE,
                NULL);

  g_object_notify (G_OBJECT (image_menu_item), "image");
}

/**
 * gtk3_image_menu_item_get_image:
 * @image_menu_item: a #Gtk3ImageMenuItem
 *
 * Gets the widget that is currently set as the image of @image_menu_item.
 * See gtk3_image_menu_item_set_image().
 *
 * Return value: (transfer none): the widget set as image of @image_menu_item
 **/
GtkWidget*
gtk3_image_menu_item_get_image (Gtk3ImageMenuItem *image_menu_item)
{
  g_return_val_if_fail (GTK3_IS_IMAGE_MENU_ITEM (image_menu_item), NULL);

  return image_menu_item->priv->image;
}

static void
gtk3_image_menu_item_remove (GtkContainer *container,
                            GtkWidget    *child)
{
  Gtk3ImageMenuItem *image_menu_item = GTK3_IMAGE_MENU_ITEM (container);
  Gtk3ImageMenuItemPrivate *priv = image_menu_item->priv;

  if (child == priv->image)
    {
      gboolean widget_was_visible;

      widget_was_visible = gtk_widget_get_visible (child);

      gtk_widget_unparent (child);
      priv->image = NULL;

      if (widget_was_visible &&
          gtk_widget_get_visible (GTK_WIDGET (container)))
        gtk_widget_queue_resize (GTK_WIDGET (container));

      g_object_notify (G_OBJECT (image_menu_item), "image");
    }
  else
    {
      GTK_CONTAINER_CLASS (gtk3_image_menu_item_parent_class)->remove (container, child);
    }
}
