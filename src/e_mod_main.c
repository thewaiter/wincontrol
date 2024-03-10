#include "e_mod_main.h"

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon * gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown(E_Gadcon_Client * gcc);
static void             _gc_orient(E_Gadcon_Client * gcc, E_Gadcon_Orient orient __UNUSED__);
static const char      *_gc_label(const E_Gadcon_Client_Class *client_class);
static                  Evas_Object *_gc_icon(const E_Gadcon_Client_Class *client_class, Evas * evas);
static const char      *_gc_id_new(const E_Gadcon_Client_Class *client_class);
static Eina_Bool        _clip_cb_changed_icon_set();

/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class = {
   GADCON_CLIENT_CLASS_VERSION,
   "wincontrol",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL,
      e_gadcon_site_is_not_toolbar
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

/* actual module specifics */
typedef struct _Instance Instance;
struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_button;
};

static void _button_cb_mouse_down(void *data, Evas * e, Evas_Object * obj, void *event_info);
static void _button_cb_mouse_wheel(void *data, Evas * e, Evas_Object * obj, void *event_info);

static E_Module *wincontrol_module = NULL;
Eina_List *handlers;

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;

   Instance *inst = NULL;
   inst = E_NEW(Instance, 1);

   o = e_icon_add(gc->evas);
   e_icon_fdo_icon_set(o, "window-close");

   evas_object_show(o);

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_button = o;

   e_gadcon_client_util_menu_attach(gcc);

   evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
                                   _button_cb_mouse_down, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_WHEEL,
                                  _button_cb_mouse_wheel, inst);
   E_LIST_HANDLER_APPEND(handlers, E_EVENT_CONFIG_ICON_THEME,
                                  _clip_cb_changed_icon_set, NULL);
   E_LIST_HANDLER_APPEND(handlers, EFREET_EVENT_ICON_CACHE_UPDATE,
                                  _clip_cb_changed_icon_set, NULL);

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;

   E_FREE_LIST(handlers, ecore_event_handler_del);
   evas_object_del(inst->o_button);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient __UNUSED__)
{
   e_gadcon_client_aspect_set (gcc, 16, 16);
   e_gadcon_client_min_size_set (gcc, 16, 16);
}

static const char *
_gc_label (const E_Gadcon_Client_Class *client_class __UNUSED__)
{
   return "Wincontrol";
}

static Evas_Object *
_gc_icon (const E_Gadcon_Client_Class *client_class __UNUSED__, Evas * evas)
{
   Evas_Object *o;
   char buf[PATH_MAX];

   o = edje_object_add(evas);
   snprintf (buf, sizeof(buf), "%s/e-module-wincontrol.edj", e_module_dir_get(wincontrol_module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new (const E_Gadcon_Client_Class *client_class __UNUSED__)
{
   return _gadcon_class.name;
}

static void
_button_cb_mouse_down (void *data __UNUSED__, Evas *e __UNUSED__,
                       Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Mouse_Down *ev;

   ev = event_info;
   if (ev->button == 1)
    {
      E_Action *a;

      a = e_action_find("window_close");
      if ((a) && (a->func.go)) a->func.go(NULL, NULL);
    }
}

static void
_button_cb_mouse_wheel (void *data __UNUSED__, Evas *e __UNUSED__, 
                        Evas_Object *obj __UNUSED__, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev = event_info;
   E_Action *a;

   if (ev->z > 0)
     {
       a = e_action_find("window_iconic_toggle");
       if ((a) && (a->func.go)) a->func.go(NULL, NULL);
     }
   else if (ev->z < 0)
     {
       a = e_action_find("window_maximized_toggle");
       if ((a) && (a->func.go)) a->func.go(NULL, NULL);
     }
}

static Eina_Bool
_clip_cb_changed_icon_set()
{
   e_gadcon_provider_unregister(&_gadcon_class);
   e_gadcon_provider_register(&_gadcon_class);
   return ECORE_CALLBACK_PASS_ON;
}

/* module setup */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "wincontrol"
};

EAPI void *
e_modapi_init (E_Module * m)
{
   wincontrol_module = m;
   e_gadcon_provider_register(&_gadcon_class);
   return wincontrol_module;
}

EAPI int
e_modapi_shutdown (E_Module * m __UNUSED__)
{
   wincontrol_module = NULL;
   e_gadcon_provider_unregister(&_gadcon_class);
   return 1;
}

EAPI int
e_modapi_save(E_Module * m __UNUSED__)
{
   return 1;
}
